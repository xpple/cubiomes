package dev.xpple.cubiomes;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardCopyOption;
import java.util.Locale;
import java.util.Objects;

public final class CubiomesInit {

    private CubiomesInit() {
    }

    private static final String OS = System.getProperty("os.name", "").toLowerCase(Locale.ROOT);
    private static final String ARCH = System.getProperty("os.arch", "").toLowerCase(Locale.ROOT);

    private static boolean isMacOS() {
        return OS.contains("mac");
    }

    private static boolean isX86() {
        return ARCH.equals("x86_64") || ARCH.equals("amd64");
    }

    private static boolean isArm() {
        return ARCH.equals("aarch64") || ARCH.equals("arm64");
    }

    /// Load the shared library. This is required for the bindings to function. This should work on
    /// all operating systems and CPU architectures, but in the case that it did not, an
    /// [UnsupportedOperationException] will be thrown. If this is the case, please open a bug report
    /// that includes information about your operating system and CPU architecture.
    /// @throws UnsupportedOperationException when the CPU architecture is not supported
    public static void load() {
        String name = "cubiomes";

        if (!isMacOS()) {
            if (isX86()) {
                name += "_x86";
            } else if (isArm()) {
                name += "_arm";
            } else {
                throw new UnsupportedOperationException("Unsupported architecture: " + ARCH);
            }
        }

        String libraryName = System.mapLibraryName(name);

        Path tempFile;
        try {
            tempFile = Files.createTempFile(libraryName, "");
            Files.copy(Objects.requireNonNull(CubiomesInit.class.getResourceAsStream("/" + libraryName)), tempFile, StandardCopyOption.REPLACE_EXISTING);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
        System.load(tempFile.toAbsolutePath().toString());
    }
}
