#include <minix/cfm.h>
#include <string.h>

#include "syslib.h"


static int cfm_pass_message(message *m, int type)
{
	int r;

	r = _taskcall(CFM_PROC_NR, type, m);

	return r;
}

int cfm_get_hash(uint8_t* data, unsigned int dataLen, uint8_t *hash) {

	message m;

	memset(&m, 0, sizeof(m));
	memcpy(m.m_cfm_sendrecv.data, data, dataLen);
	m.m_cfm_sendrecv.dataLen = dataLen;

	if(cfm_pass_message(&m, CFM_GET_HASH) != 0) {
		return -1;
	}

	unsigned int hashLen = m.m_cfm_sendrecv.dataLen;
	memcpy(hash, m.m_cfm_sendrecv.data, hashLen);

	return 0;
}

