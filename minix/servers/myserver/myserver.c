#include "inc.h"
#include "myserver.h"


/*===========================================================================*
 *			    sef_cb_init_fresh				     *
 *===========================================================================*/
int sef_cb_init_fresh(int UNUSED(type), sef_init_info_t *info)
{
  return(OK);
}

/*===========================================================================*
 *				do_*				     *
 *===========================================================================*/
int do_sys1(message *m_ptr)
{
  printf("invoked the syscall 01\n");
  return(OK);
}


int do_check_memory(message *m_ptr)
{
  printf("Checking code from %p to %p for endpoint %d\n", (void*)(m_ptr->MYSERVER_CHECK_CODE_BASE), (void*)(m_ptr->MYSERVER_CHECK_CODE_SIZE), m_ptr->MYSERVER_CHECK_PROC);

  printf("Request magic grant from VFS\n");

  message m_out;
  memset(&m_out, 0, sizeof(m_out));

  m_out.VFS_MYSERVER_BASE = m_ptr->MYSERVER_CHECK_CODE_BASE;
  m_out.VFS_MYSERVER_SIZE = m_ptr->MYSERVER_CHECK_CODE_SIZE;
  m_out.VFS_MYSERVER_ENDPT = m_ptr->MYSERVER_CHECK_PROC;

  char buffer[1024];
  _taskcall(VFS_PROC_NR, VFS_MYSERVER_ACQUIRE_MEM, &m_out);
  printf("MYSERVER magic grant received %x\n", m_out.VFS_MYSERVER_GRANT_ID);

  int size = (m_ptr->MYSERVER_CHECK_CODE_SIZE > 1024)?1024:m_ptr->MYSERVER_CHECK_CODE_SIZE; 
  cp_grant_id_t grant_id = m_out.VFS_MYSERVER_GRANT_ID;
  int r = sys_safecopyfrom(VFS_PROC_NR,
                           grant_id, 0,
                           (vir_bytes)buffer, size);
  printf("MYSERVER copy performed %d\n", r);
  buffer[1023] = 0;
  printf("MYSERVER buffer %s\n", buffer);
  
  return(OK);
}

