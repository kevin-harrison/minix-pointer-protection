#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char pwd[16] = "admin=admin";
char buffer[16];
int * x;

void print_pwd(void);

void print_pwd(void) {
  printf("My password is %s\n", pwd);
}

int main(int argc, char *argv[])
{
  int pwd_diff = strcmp(argv[1], pwd);
  x = malloc(sizeof(int) * 1);
  printf("addresses %p %p %p %p\n", &pwd_diff, &x, x, buffer);

  strcpy(buffer, argv[2]);
  *x = atoi(argv[3]);

  if (pwd_diff != 0)
    return 1;

  print_pwd();
  return 0;
}
