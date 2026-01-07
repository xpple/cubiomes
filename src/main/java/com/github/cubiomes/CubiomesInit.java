package com.github.cubiomes;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardCopyOption;
import java.util.Objects;

public final class CubiomesInit {

    private CubiomesInit() {
    }

    public static void load() {
        String libraryName = System.mapLibraryName("cubiomes");
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
