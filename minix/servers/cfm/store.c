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


/*===========================================================================*
 *				do_cfm_get_merkle_root				     *
 *===========================================================================*/
static char merkle_root_hash[16] = {0};

int do_cfm_get_merkle_root(message *m)
{
  printf("do_cfm_get_merkle_root called\n");

  // Update the message with reply
  memset(m, 0, sizeof(message));
  memcpy(m->m_cfm_sendrecv.data, merkle_root_hash, 16);
  m->m_cfm_sendrecv.dataLen = 16;

  return (OK);
}

/*===========================================================================*
 *				do_cfm_update_merkle_root				     *
 *===========================================================================*/
int do_cfm_update_merkle_root(message *m)
{
  printf("do_cfm_update_merkle_root called\n");

  // Get the new merkle root hash from the message
  //unsigned int dataLen = m->m_cfm_sendrecv.dataLen;
  //uint8_t data[dataLen];
  //if (dataLen != 16) return (EINVAL)

  // Update cached merkle root hash
  memcpy(merkle_root_hash, m->m_cfm_sendrecv.data, 16);

  // Update the message with reply
  memset(m, 0, sizeof(message));

  return (OK);
}