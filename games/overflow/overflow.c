#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <minix/merkle.h>

typedef struct _state {
  char buffer[16];
  int * x;
} State;

State state;

void print_pwd(void);

void print_pwd(void) {
  printf("My password is admin\n");
}

int main(int argc, char *argv[])
{
  int pwd_diff = strcmp(argv[1], "admin");
  state.x = malloc(sizeof(int) * 1);
  STORE_POINTER(state.x)
  printf("addresses %p %p %p %p \n", &pwd_diff, &(state.x), state.x, state.buffer);

  strcpy(state.buffer, argv[2]);
  printf("addresses %p %p %p %p \n", &pwd_diff, &(state.x), state.x, state.buffer);
  VALIDATE_POINTER(state.x)
  *(state.x) = atoi(argv[3]);

  if (pwd_diff != 0)
    return 1;

  print_pwd();
  return 0;
}
