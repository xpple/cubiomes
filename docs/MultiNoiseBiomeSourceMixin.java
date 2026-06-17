package dev.xpple.ctfmod.mixin;

import com.mojang.datafixers.util.Either;
import com.mojang.datafixers.util.Pair;
import net.minecraft.core.Holder;
import net.minecraft.util.Util;
import net.minecraft.world.level.biome.Biome;
import net.minecraft.world.level.biome.BiomeSource;
import net.minecraft.world.level.biome.Climate;
import net.minecraft.world.level.biome.MultiNoiseBiomeSource;
import net.minecraft.world.level.biome.MultiNoiseBiomeSourceParameterList;
import org.spongepowered.asm.mixin.Final;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.Shadow;
import org.spongepowered.asm.mixin.Unique;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfoReturnable;

import java.io.IOException;
import java.lang.reflect.Field;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.stream.Collectors;

/// @author [Lithodora](https://github.com/lithodoradiffusa)
@Mixin(MultiNoiseBiomeSource.class)
public abstract class MultiNoiseBiomeSourceMixin extends BiomeSource {

    @Unique
    private volatile boolean collected = false;

    @Shadow
    @Final
    private Either<Climate.ParameterList<Holder<Biome>>, Holder<MultiNoiseBiomeSourceParameterList>> parameters;

    @Inject(method = "parameters", at = @At("HEAD"))
    private synchronized void onParameters(CallbackInfoReturnable<Climate.ParameterList<Holder<Biome>>> cir) {
        if (this.collected) {
            return;
        }
        if (!this.parameters.right().get().unwrapKey().get().identifier().equals(MultiNoiseBiomeSourceParameterList.Preset.OVERWORLD.id())) {
            return;
        }

        Climate.ParameterList<Holder<Biome>> params = this.parameters.right().get().value().parameters();
        Object tree = getField(params, "index");
        Object root = getField(tree, "root");

        NPTreeC.CNode pytree = new NPTreeC.CNode();
        int[] idx = new int[]{0, 0, 0, 0, 0};

        NPTreeC.CNode.grow(pytree, root, idx, -1, 0, true);
        pytree.gen_ids(0);

        StringBuilder sb = new StringBuilder();
        pytree.dump_c(sb);
        String fileContents = sb.toString();

        try {
            Path path = Path.of("biome_dump");
            Files.createDirectories(path);
            Files.writeString(path.resolve("dump_" + Util.getEpochMillis() + ".txt"), fileContents);
            this.collected = true;
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    private static class NPTreeC {
        //copied from nptree_c.py
        private static class CNode {
            private int id = 0;
            private List<Pair<Integer, Integer>> range;
            private String biome;
            private final List<CNode> inner = new ArrayList<>();
            public void set(List<Pair<Integer, Integer>> r, String b, int[] idx) {
                if (idx.length > 0) {
                    int i = idx[0];
                    while (i >= inner.size()) {
                        inner.add(new CNode());
                    }
                    inner.get(i).set(r, b, Arrays.copyOfRange(idx, 1, idx.length));
                } else {
                    if (r != null) {
                        this.range = r;
                    }
                    if (b != null && !b.isEmpty()) {
                        this.biome = b;
                    }
                }
            }

            public int gen_ids(int num) {
                this.id = num;
                num++;
                for (CNode n : this.inner) {
                    num = n.gen_ids(num);
                }
                return num;
            }

            public static void grow(CNode root, Object searchTreeNode, int[] idx, int dim, int i, boolean isRoot) {
                boolean isLeaf = searchTreeNode.getClass().getSimpleName().equals("Leaf");
                if (!isRoot) {
                    Climate.Parameter[] params = (Climate.Parameter[]) getFieldFromSuper(searchTreeNode, "parameterSpace");

                    List<Pair<Integer, Integer>> ranges = Arrays.stream(params)
                        .limit(params.length - 1)
                        .map(p -> new Pair<>((int) p.min(), (int) p.max()))
                        .toList();

                    String biome = "";
                    if (isLeaf) {
                        biome = ((Holder.Reference<Biome>)getField(searchTreeNode, "value")).key().identifier().getPath();
                    }

                    idx[dim] = i;
                    int end = Math.min(idx.length, dim + 1);
                    root.set(ranges, biome, Arrays.copyOfRange(idx, 0, end));
                }
                if (!isLeaf) {
                    Object[] children = (Object[])getField(searchTreeNode, "children");
                    for (int j = 0; j < children.length; j++) {
                        Object child = children[j];
                        grow(root, child, idx, dim + 1, j, false);
                    }
                }
            }

            public void dump_c(StringBuilder sb_global) {
                StringBuilder sb = new StringBuilder();
                sb.append(String.format("/*%d*/{", this.id));
                if (this.range != null) {
                    String rstring = this.range.stream()
                        .map(r -> r.getFirst() + "," + r.getSecond())
                        .collect(Collectors.joining(","));
                    sb.append("{")
                        .append(rstring)
                        .append("},");
                } else {
                    sb.append("{},");
                }
                String idstring = this.inner.stream()
                    .map(n -> String.valueOf(n.id))
                    .collect(Collectors.joining(","));
                sb.append("{")
                    .append(idstring)
                    .append("},");
                if (this.biome != null && !this.biome.isEmpty()) {
                    sb.append(this.biome)
                        .append("},");
                } else {
                    sb.append("none},");
                }
                sb.append("\n");
                sb_global.append(sb);
                for (CNode n : this.inner) {
                    n.dump_c(sb_global);
                }
            }
        }

    }

    @Unique
    private static Object getFieldFromSuper(Object obj, String name) {
        try {
            Field f = obj.getClass().getSuperclass().getDeclaredField(name);
            f.setAccessible(true);
            return f.get(obj);
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }
    @Unique
    private static Object getField(Object obj, String name) {
        try {
            Field f = obj.getClass().getDeclaredField(name);
            f.setAccessible(true);
            return f.get(obj);
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }
}
