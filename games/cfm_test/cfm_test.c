#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <lib.h>

// CHANGED: using PM_FOWRD_CFM syscall now
int main(int argc, char *argv[])
{
  message m;
  memset(&m, 0, sizeof(m));

  //syssafecopyto(CFM_PROC_NR, )

  int ret = _syscall(PM_PROC_NR, PM_FORWARD_CFM, &m);;
  printf("syscall PM_FORWARD_CFM returned with: %d\n", ret);
  return 0;
}
