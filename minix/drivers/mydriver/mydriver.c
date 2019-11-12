#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <minix/syslib.h>
#include <minix/chardriver.h>
#include <minix/myserver.h>

#include <minix/timers.h>
#include <include/arch/i386/include/archtypes.h>
#include "kernel/proc.h"
#include <minix/sysinfo.h>
#include "servers/pm/mproc.h"

#include "mydriver.h"
 
/* SEF functions and variables. */
static void sef_local_startup(void);
static int sef_cb_init(int type, sef_init_info_t *info);
static int sef_cb_lu_state_save(int, int);
static int lu_state_restore(void);

 
/** State variable to count the number of times the device has been opened.
 * Note that this is not the regular type of open counter: it never decreases.
 */
static int open_counter;
static int received_pos;
 
static int sef_cb_lu_state_save(int UNUSED(state), int UNUSED(flags)) {
  return OK;
}
 
static int lu_state_restore() {
  return OK;
}
  
static int sef_cb_init(int type, sef_init_info_t *UNUSED(info))
{
  /* Initialize the hello driver. */
  int do_announce_driver = TRUE;
 
  open_counter = 0;
  switch(type) {
  case SEF_INIT_FRESH:
    printf("%s", HELLO_MESSAGE);
    break;
 
  case SEF_INIT_LU:
    /* Restore the state. */
    lu_state_restore();
    do_announce_driver = FALSE;
 
    printf("%sHey, I'm a new version!\n", HELLO_MESSAGE);
    break;
 
  case SEF_INIT_RESTART:
    printf("%sHey, I've just been restarted!\n", HELLO_MESSAGE);
    break;
  }

  /* Announce we are up when necessary. */
  if (do_announce_driver) {
    chardriver_announce();
  }
 
  /* Initialization completed successfully. */
  return OK;
}

static void sef_local_startup()
{
  /*
   * Register init callbacks. Use the same function for all event types
   */
  sef_setcb_init_fresh(sef_cb_init);
  sef_setcb_init_lu(sef_cb_init);
  sef_setcb_init_restart(sef_cb_init);
 
  /*
   * Register live update callbacks.
   */
  sef_setcb_lu_state_save(sef_cb_lu_state_save);
 
  /* Let SEF perform startup. */
  sef_startup();
}


/*
 * Function prototypes for the hello driver.
 */
static int mydriver_open(devminor_t minor, int access, endpoint_t user_endpt);
static int mydriver_close(devminor_t minor);
static ssize_t mydriver_read(devminor_t minor, u64_t position, endpoint_t endpt,
    cp_grant_id_t grant, size_t size, int flags, cdev_id_t id);
static ssize_t mydriver_write(devminor_t UNUSED(minor), u64_t position,
                           endpoint_t endpt, cp_grant_id_t grant, size_t size, int UNUSED(flags),
                           cdev_id_t UNUSED(id));


/* Entry points to the hello driver. */
static struct chardriver mydriver_tab =
{
 .cdr_open	= mydriver_open,
 .cdr_close	= mydriver_close,
 .cdr_read	= mydriver_read,
 .cdr_write	= mydriver_write,
};

static int mydriver_open(devminor_t UNUSED(minor), int UNUSED(access),
                      endpoint_t UNUSED(user_endpt))
{
  printf("mydriver_open(). Called %d time(s).\n", ++open_counter);
  received_pos = 0;
  return OK;
}
 
static int mydriver_close(devminor_t UNUSED(minor))
{
  printf("mydriver_close()\n");
  return OK;
}
 
static ssize_t mydriver_read(devminor_t UNUSED(minor), u64_t position,
                          endpoint_t endpt, cp_grant_id_t grant, size_t size, int UNUSED(flags),
                          cdev_id_t UNUSED(id))
{
  u64_t dev_size;
  char *ptr;
  int ret;
  char *buf = HELLO_MESSAGE;
 
  printf("mydriver_read()\n");
 
  /* This is the total size of our device. */
  dev_size = (u64_t) strlen(buf);
 
  /* Check for EOF, and possibly limit the read size. */
  if (position >= dev_size) return 0;		/* EOF */
  if (position + size > dev_size)
    size = (size_t)(dev_size - position);	/* limit size */
 
  /* Copy the requested part to the caller. */
  ptr = buf + (size_t)position;
  if ((ret = sys_safecopyto(endpt, grant, 0, (vir_bytes) ptr, size)) != OK)
    return ret;
 
  /* Return the number of bytes read. */
  return size;
}

struct proc proc[NR_TASKS + NR_PROCS];
struct mproc mproc[NR_PROCS];
char received_msg[1024];

static ssize_t mydriver_write(devminor_t UNUSED(minor), u64_t position,
                           endpoint_t endpt, cp_grant_id_t grant, size_t size, int UNUSED(flags),
                           cdev_id_t UNUSED(id))
{
  int ret;
 
  printf("mydriver_write(position=%llu, size=%zu)\n", position, size);
 
  if (size > 1023 - received_pos)
    size = (size_t)(1023 - received_pos);	/* limit size */

  ret = sys_safecopyfrom(endpt, grant, 0, (vir_bytes) (received_msg+received_pos), size);
  printf("ret=%d\n", ret);

  if (ret != OK) {
    printf("MYSERVERDRIVER: warning: couldn't copy data: %d\n", ret);
    return OK;
  }
  received_pos += size;
  received_msg[received_pos] = '\0';
 
  printf("received=%s\n", received_msg);

  if (received_msg[received_pos-1] != '\n')
    return size;


  /* char * token = strtok(buf, ":"); */
  /* if (token == NULL) */
  /*   return 0; */
  /* int pid = atoi(token); */
  /* token = strtok(NULL, ":"); */
  /* if (token == NULL) */
  /*   return 0; */
  /* void * address = (void *)strtoul(token, NULL, 0); */

  received_msg[4] = 0;
  int pid = atoi(received_msg);
  void * address = (void *)strtoul(received_msg+5, NULL, 0);

  printf("Process %d address %p\n", pid, address);
  
  /* Retrieve and check the PM process table. */
  ret = getsysinfo(PM_PROC_NR, SI_PROC_TAB, mproc, sizeof(mproc));
  if (ret != OK) {
    printf("MYDRIVER: warning: couldn't get copy of PM process table: %d\n", ret);
    return OK;
  }
  endpoint_t end_p = 0;
  for (int mslot = 0; mslot < NR_PROCS; mslot++) {
    if (mproc[mslot].mp_flags & IN_USE) {
      if (mproc[mslot].mp_pid == pid)
	end_p = mproc[mslot].mp_endpoint;
    }
  }

  if (end_p == 0) {
      printf("Endpoint not found\n");
      return size;
  }
  printf("Endpoint %d\n", end_p);
  
  message msg;
  memset(&msg, 0, sizeof(msg));
  msg.MYSERVER_CHECK_PROC = end_p;
  msg.MYSERVER_CHECK_CODE_BASE = (long)address;
  msg.MYSERVER_CHECK_CODE_SIZE = 1024;
  msg.m_type = MYSERVER_CHECK_CODE;
  int status = ipc_sendrec(MYSERVER_PROC_NR, &msg);
  
  return size;
}

int main(void)
{
  /*
   * Perform initialization.
   */
  sef_local_startup();
 
  /*
   * Run the main loop.
   */
  chardriver_task(&mydriver_tab);
  return OK;
}
