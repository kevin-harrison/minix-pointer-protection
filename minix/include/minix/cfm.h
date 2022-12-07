#ifndef _MINIX_CFM_SERVER_H
#define _MINIX_CFM_SERVER_H

#include <sys/types.h>
#include <minix/endpoint.h>

/* monitor_server.c */

/* U32 */
int cfm_get_hash(uint8_t* data, unsigned int dataLen, uint8_t* hash);

#endif /* _MINIX_CFM_SERVER_H */
