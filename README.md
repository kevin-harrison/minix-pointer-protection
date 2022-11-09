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

## Contributors

Christopher Gauffin\
Jesper Lagnelöv\
