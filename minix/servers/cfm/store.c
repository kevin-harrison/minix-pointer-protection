#include "inc.h"
#include <minix/aes256.h>

/*===========================================================================*
 *		            sef_cb_init_fresh                                *
 *===========================================================================*/
int sef_cb_init_fresh(int UNUSED(type), sef_init_info_t* UNUSED(info))
{
	return(OK);
}

/*===========================================================================*
 *				do_cfm_get_hash				     *
 *===========================================================================*/
int do_cfm_get_hash(message *m)
{
  // Get the data to be hashed from the message
  unsigned int dataLen = m->m_cfm_sendrecv.dataLen;
  uint8_t data[dataLen];
  memcpy(data, m->m_cfm_sendrecv.data, dataLen);

  printf("do_cfm_get_hash received %c\n", data[0]);

  // Hash here and update data and dataLen respectively.
  uint8_t hash[] = {'Y'};
  unsigned int hashLen = 1;

  // Update the message
  memset(m, 0, sizeof(message));
  memcpy(m->m_cfm_sendrecv.data, hash, hashLen);
  m->m_cfm_sendrecv.dataLen = hashLen;

  return (OK);
}
