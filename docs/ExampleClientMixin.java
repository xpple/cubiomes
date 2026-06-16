package lithodora.biomes262.client.mixin;

import com.mojang.datafixers.util.Either;
import lithodora.biomes262.client.NPTreeC;
import net.minecraft.core.Holder;
import net.minecraft.world.level.biome.Biome;
import net.minecraft.world.level.biome.Climate;
import net.minecraft.world.level.biome.MultiNoiseBiomeSource;
import net.minecraft.world.level.biome.MultiNoiseBiomeSourceParameterList;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.Shadow;
import org.spongepowered.asm.mixin.Final;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfoReturnable;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;

import static lithodora.biomes262.client.NPTreeC.getField;

@Mixin(MultiNoiseBiomeSource.class)
public class ExampleClientMixin{
	private int call_num = 0;
	@Shadow
	@Final
	private Either<
				Climate.ParameterList<Holder<Biome>>,
				Holder<MultiNoiseBiomeSourceParameterList>
				> parameters;

	@Inject(method = "parameters", at = @At("HEAD"))
	private void onParameters(
			CallbackInfoReturnable<Climate.ParameterList<Holder<Biome>>> cir
	) {
		// nether biomes seem to call first?
		// perhaps there is a better place to inject the mixin,
		// but this works. will produce ~3-4 files called dump_x.txt at the path specified below
		// (I think you need to create the folder manually before running as well - not familiar with Java file stuff)
		if(call_num < 2) {
			Object params = this.parameters.right().get().value().parameters();
			Object tree = getField(params, "index");
			Object root = getField(tree, "root");

			NPTreeC.CNode pytree = new NPTreeC.CNode();
			int[] idx = new int[] {0,0,0,0,0};

			NPTreeC.CNode.grow(pytree, root, idx, -1, 0, true);
			pytree.gen_ids(0);

			StringBuilder sb = new StringBuilder();
			pytree.dump_c(sb);
			String fileContents = sb.toString();

            try {
                Files.writeString(Path.of("D:\\Documents\\Fabric\\biomes262\\output\\dump_"+String.valueOf(call_num)+".txt"), fileContents);
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
            call_num++;
		}
	}
}