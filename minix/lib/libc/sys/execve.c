/* execve() - basic program execution call */

#include <sys/cdefs.h>
#include "namespace.h"
#include <lib.h>

#include <unistd.h>
#include <string.h>
#include <stddef.h>
#include <minix/param.h>
#include <sys/exec_elf.h>
#include <sys/exec.h>
#include <fcntl.h>
#include <stdio.h>

int execve(const char *path, char * const *argv, char * const *envp)
{
	message m;
	size_t frame_size = 0;	/* Size of the new initial stack. */
	int argc = 0;		/* Argument count. */
	int envc = 0;		/* Environment count */
	char overflow = 0;	/* No overflow yet. */
	char *frame;
	struct ps_strings *psp;
	int vsp = 0;	/* (virtual) Stack pointer in new address space. */
	int osp = 0;	/* Stack pointer offset . */

        int random_f = open("/dev/urandom", O_RDONLY);
        if (random_f < 0) {
          printf("EXECVE: Failed to open the random data\n");
          osp = 0;
        }
        else {
          unsigned int random_d;
          ssize_t result = read(random_f, &random_d, sizeof random_d);
          if (result < 0) {
            printf("EXECVE: Failed to read the random data\n");
            osp = 0;
          }
          else {
            osp = (random_d % 10)*4096;
          }
        }
        // osp = 4096*2;
        printf("LIBC: stack offset %d\n", osp);
 

	minix_stack_params(path, argv, envp, &frame_size, &overflow,
		&argc, &envc);

	/* The party is off if there is an overflow. */
	if (overflow) {
		errno = E2BIG;
		return -1;
	}

	/* Allocate space for the stack frame. */
	if ((frame = (char *) sbrk(frame_size)) == (char *) -1) {
		errno = E2BIG;
		return -1;
	}

	minix_stack_fill(path, argc, argv, envc, envp, frame_size, frame,
                         &vsp, &psp, osp);

	/* Clear unused message fields */
	memset(&m, 0, sizeof(m));

	/* We can finally make the system call. */
	m.m_lc_pm_exec.name = (vir_bytes)path;
	m.m_lc_pm_exec.namelen = strlen(path) + 1;
	m.m_lc_pm_exec.frame = (vir_bytes)frame;
	m.m_lc_pm_exec.framelen = frame_size;
	m.m_lc_pm_exec.ps_str = (vir_bytes)(vsp + ((char *)psp - frame));
        m.m_lc_pm_exec.sp_offset = (vir_bytes)osp;

	(void) _syscall(PM_PROC_NR, PM_EXEC, &m);

	/* Failure, return the memory used for the frame and exit. */
	(void) sbrk(-frame_size);

	return -1;
}
