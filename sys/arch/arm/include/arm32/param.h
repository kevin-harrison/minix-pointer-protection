/*	$NetBSD: param.h,v 1.24 2015/04/02 03:11:01 matt Exp $	*/

/*
 * Copyright (c) 1994,1995 Mark Brinicombe.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the RiscBSD team.
 * 4. The name "RiscBSD" nor the name of the author may be used to
 *    endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY RISCBSD ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL RISCBSD OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef	_ARM_ARM32_PARAM_H_
#define	_ARM_ARM32_PARAM_H_

#ifdef _KERNEL_OPT
# include "opt_arm32_pmap.h"
#endif

/*
 * Machine dependent constants for ARM6+ processors
 */
/* These are defined in the Port File before it includes
 * this file. */

#if defined(__minix)
/* We do not support PAGE_SIZE != 4k */
#define	PGSHIFT		12		/* LOG2(NBPG) */
#endif /* defined(__minix) */
#ifndef PGSHIFT
#if defined(_ARM_ARCH_6)
#define	PGSHIFT		13		/* LOG2(NBPG) */
#else
#define	PGSHIFT		12		/* LOG2(NBPG) */
#endif
#endif
#define	NBPG		(1 << PGSHIFT)	/* bytes/page */
#define	PGOFSET		(NBPG-1)	/* byte offset into page */
#define	NPTEPG		(NBPG/(sizeof (pt_entry_t)))


#define SSIZE		1		/* initial stack size/NBPG */
#define SINCR		1		/* increment of stack/NBPG */
#define USPACE		8192		/* total size of u-area */
#define UPAGES		(USPACE / NBPG)	/* pages of u-area */

#ifndef MSGBUFSIZE
#define MSGBUFSIZE	16384	 	/* default message buffer size */
#endif

/*
 * Minimum and maximum sizes of the kernel malloc arena in PAGE_SIZE-sized
 * logical pages.
 */
#define	NKMEMPAGES_MIN_DEFAULT	((8 * 1024 * 1024) >> PAGE_SHIFT)
#define	NKMEMPAGES_MAX_DEFAULT	((128 * 1024 * 1024) >> PAGE_SHIFT)

/* Constants used to divide the USPACE area */

/*
 * The USPACE area contains :
 * 1. the pcb structure for the process
 * 2. the kernel (svc) stack
 *
 * The layout of the area looks like this
 *
 * | uarea | kernel stack |
 *
 * The size of the uarea is known.
 * The kernel stack should be at least 4K is size.
 *
 * The stack top addresses are used to set the stack pointers. The stack bottom
 * addresses at the addresses monitored by the diagnostic code for stack overflows
 *
 */

#define USPACE_SVC_STACK_TOP		(USPACE)
#define USPACE_SVC_STACK_BOTTOM		(sizeof(struct pcb))

#define arm_btop(x)			((unsigned)(x) >> PGSHIFT)
#define arm_ptob(x)			((unsigned)(x) << PGSHIFT)
#define arm_trunc_page(x)		((unsigned)(x) & ~PGOFSET)
    
#ifdef _KERNEL
#ifndef _LOCORE
void	delay(unsigned);
#define DELAY(x)	delay(x)
#endif
#endif

#include <arm/param.h>

#endif	/* _ARM_ARM32_PARAM_H_ */
