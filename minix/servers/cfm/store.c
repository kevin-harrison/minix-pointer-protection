#include "inc.h"

/*===========================================================================*
 *		            sef_cb_init_fresh                                *
 *===========================================================================*/
int sef_cb_init_fresh(int UNUSED(type), sef_init_info_t* UNUSED(info))
{
	return(OK);
}

/*===========================================================================*
 *				do_publish				     *
 *===========================================================================*/
int do_verify_hash(message *m_ptr)
{
  /* int flags = m_ptr->m_ds_req.flags; */
  printf("Called do_verify_hash\n");
  return 321;
}
