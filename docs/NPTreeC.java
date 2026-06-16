package lithodora.biomes262.client;

import com.mojang.datafixers.util.Pair;
import net.minecraft.core.Holder;
import net.minecraft.world.level.biome.Biome;
import net.minecraft.world.level.biome.Climate;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.stream.Collectors;

public class NPTreeC {

    public static Object getFieldFromSuper(Object obj, String name) {
        try {
            java.lang.reflect.Field f = obj.getClass().getSuperclass().getDeclaredField(name);
            f.setAccessible(true);
            return f.get(obj);
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }
    public static Object getField(Object obj, String name) {
        try {
            java.lang.reflect.Field f = obj.getClass().getDeclaredField(name);
            f.setAccessible(true);
            return f.get(obj);
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    //copied from nptree_c.py
    public static class CNode{
        int id=0;
        List<Pair<Integer, Integer>> range;
        String biome;
        List<CNode> inner = new ArrayList<>();
        public void set(List<Pair<Integer, Integer>> r, String b, int[] idx){
            if(idx.length > 0){
                int i = idx[0];
                while(i >= inner.size()){
                    inner.add(new CNode());
                }
                inner.get(i).set(r, b, Arrays.copyOfRange(idx, 1, idx.length));
            }else{
                if(r != null){
                    this.range = r;
                }
                if(b!=null && !b.isEmpty()){
                    this.biome = b;
                }

            }
        }

        public int gen_ids(int num){
            this.id = num;
            num++;
            for(CNode n : this.inner){
                num = n.gen_ids(num);
            }
            return num;
        }

        public static void grow(CNode root, Object searchTreeNode, int[] idx, int dim, int i, boolean isRoot){
            boolean isLeaf = searchTreeNode.getClass().getSimpleName().equals("Leaf");
            if(!isRoot) {

                Climate.Parameter[] params = (Climate.Parameter[]) getFieldFromSuper(searchTreeNode, "parameterSpace");

                List<Pair<Integer, Integer>> ranges = Arrays.stream(params)
                        .limit(params.length - 1)
                        .map(p -> new Pair<>((int) p.min(), (int) p.max()))
                        .toList();

                String biome = "";
                if (isLeaf) {
                    biome = ((Holder.Reference<Biome>)getField(searchTreeNode, "value"))
                            .key().identifier().getPath();
                }


                idx[dim] = i;
                int end = Math.min(idx.length, dim + 1);
                root.set(ranges, biome, Arrays.copyOfRange(idx, 0, end));
            }
            if(!isLeaf) {
                Object[] children = (Object[])getField(searchTreeNode, "children");
                for (int j = 0; j < children.length; j++){
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