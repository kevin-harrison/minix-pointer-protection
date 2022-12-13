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


int cfm_get_merkle_root(char* dest_buffer) {

	message m;
	memset(&m, 0, sizeof(m));

	if(cfm_pass_message(&m, CFM_GET_MERKLE_ROOT) != 0) {
		return -1;
	}

	memcpy(dest_buffer, m.m_cfm_sendrecv.data, 16);

	return 0;
}


int cfm_update_merkle_root(char* src_buffer) {

	message m;
	memset(&m, 0, sizeof(m));
	memcpy(m.m_cfm_sendrecv.data, src_buffer, 16);

	if(cfm_pass_message(&m, CFM_UPDATE_MERKLE_ROOT) != 0) {
		return -1;
	}

	return 0;
}

