#include <minix/cfm_server.h>
#include <string.h>

#include "syslib.h"

static int do_invoke_cfm_server(message *m, int type)
{
	int r;

	r = _taskcall(CFM_PROC_NR, type, m);

	return r;
}

int cfm_verify_hash(int address)
{
	message m;

	memset(&m, 0, sizeof(m));
	return do_invoke_cfm_server(&m, CFM_VERIFY_HASH);
}
