#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <minix/cfm_server.h>

int main(int argc, char *argv[])
{
  int ret = cfm_verify_hash(123);
  printf("cfm_verify_hash returned %d\n", ret);
  return 0;
}
