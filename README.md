## Before cloning warning
The compilation process creates files in the directory that is the parent of the cloned repo. So set up your directory structure with that in mind.

## Finalize linker fix
You may need to add the file `<REPO-DIRECTORY>/external/gpl3/binutils/patches/0011-gcc10fix.patch` with contents
```diff
diff -u --recursive dist.orig/gold/errors.h dist/gold/errors.h
--- dist.orig/gold/errors.h	2011-06-08 08:43:28.000000000 +0400
+++ dist/gold/errors.h	2021-03-25 13:07:03.148266532 +0300
@@ -24,6 +24,7 @@
 #define GOLD_ERRORS_H
 
 #include <cstdarg>
+#include <string>
 
 #include "gold-threads.h"

```

## Cross-compiling notes
To compile just run `bash <REPO-DIRECTORY>/releasetools/x86_hdimage.sh`. A command line for running the result in a KVM virtual machine is printed at the end of the process.

## Toolset
After you have compiled the toolchain for the first time, you can run:

    . set_path

This will add the toolchain bin directory to your path, giving you direct access to the following tools:

- `qqmake` - Make for cross-compilation.
- `qqmake-game` - Reberto's faster compliation announcement (only works for programs in /usr/games/)
- `qqrun` - Run using qemu.
- `qqrebuild` - Rebuilds using the `x86_hdimage.sh` script without removing previous build.
- `qqrebuild-slow` - Like `qqrebuild`, but removes everything of the previous build except for the toolchain. This should always work, but typically recompiles a lot more than necessary.
- `qqgrep` - A grep that functions on all `.include "` and `SUBDIR[+]=` lines in all Makefiles (except for those in ./external). Useful in finding where to run `qqmake clean` to allow for faster rebuilding (see below).


## Faster rebuilding
Sometimes it's enough to run:

    qqrebuild

Other times, you need to remove files from the build directory before running `qqrebuild`. This can be achieved by running `qqmake clean` in the appropriate directory, or by manually removing the appropriate files from `../obj.i386`.

It can be tricky to find the correct Makefile, but `qqgrep` can help. Moreover, setting `MAKEVERBOSE=3` (like `MAKEVERBOSE=3 qqmake`) can help you see what `qqmake` is actually doing.

Helpful `qqmake` flags:

    -w      Print a message containing the working directory before and after other processing.
            Can help you see where built files end up.

    -n      Dry run. Just print the commands that would be executed.

## Contributors

Christopher Gauffin\
Jesper Lagnelöv\
Joseph Johansson\
Kevin Harrison
