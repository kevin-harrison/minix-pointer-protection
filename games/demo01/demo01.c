#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
  char message[1024];

  printf("Hello I'm demo 01\n");
  printf("Stack address %p\n", message);
  printf("Code address %p\n", &main);

  strcpy(message, "I'm a nice message\n");
  
  while (1) {
  }
  
  return 0;
}
