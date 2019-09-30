#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  int x = 0;
  int * y = malloc(sizeof(int));
  printf("Hello I'm demo 01\n");
  printf("Heap address  %p\n", y);
  printf("Stack address %p\n", &x);
  printf("Code address %p\n", &main);

  return 0;
}
