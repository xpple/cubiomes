# Cubiomes for Java
This branch contains Java bindings that are automatically kept up to date with [master](https://github.com/xpple/cubiomes). The bindings use [The Foreign Function and Memory API](https://dev.java/learn/ffm/) that was introduced in JDK 22. The library itself uses JDK 23. You must therefore use at least Java 23 in your project.

## Importing the bindings
There are two ways you can import the bindings.

1. By importing the bindings as Maven dependency.

   ```groovy
   implementation "com.github:cubiomes:${project.cubiomes_version}"
   ```

   You can either add my own Maven server, or use GitHub Packages. Note that GitHub Packages requires authentication with GitHub.

   ```groovy
   repositories {
       // My own
       maven {
           name = 'xpple'
           url = 'https://maven.xpple.dev/maven2'
       }
       // GitHub Packages
       maven {
           url 'https://maven.pkg.github.com/xpple/cubiomes'
           credentials {
               username = System.getenv("GITHUB_USERNAME")
               password = System.getenv("GITHUB_TOKEN")
           }
       }
   }
   ```
   
   Since the C library is unversioned, the version string for the Java bindings is determined by time the commit is pushed. That is, the version string will look something like:

   ```
   <major>.<minor>.<patch>-<YYYY>.<MM>.<DD>.<seconds since Unix epoch>+<master commit SHA>
   ```
   
   This ensures the version is SemVer compliant, without needing to adopt a versioning scheme for the C library. You can always view the latest version on GitHub [here](https://github.com/xpple/cubiomes/packages).

2. By cloning the repository locally.

   ```shell
   git clone --recurse-submodules -b java-bindings https://github.com/xpple/cubiomes
   ```
   
   Note that you will have to build the shared library yourself. The following is for Windows:

   ```shell
   cd src/main/c/cubiomes
   cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
   cmake --build build --config Release
   cp build/cubiomes.dll ../../resources/cubiomes.dll
   cd ../../../../
   ```

## Using the bindings
Before you can use the bindings, you must call `CubiomesInit.load()`. This will load the native library, so that the native calls are delegated to the binary. The program will crash if you do not do this.

Since the bindings use the FFM API, it is helpful to get yourself familiar with how it works. See [this tutorial](https://dev.java/learn/ffm/) for a brief introduction. I have also written a few exemplary tests in the [`test` source set](https://github.com/xpple/cubiomes/tree/java-bindings/src/test/java/com/github/cubiomes). Should you have any further questions on how to use the bindings, feel free to contact me on Discord.
