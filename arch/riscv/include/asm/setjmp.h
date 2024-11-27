/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2018 Alexander Graf <agraf@suse.de>
 */

#ifndef _SETJMP_H_
#define _SETJMP_H_	1

#include <linux/types.h>

/*
 * This really should be opaque, but the EFI implementation wrongly
 * assumes that a 'struct jmp_buf_data' is defined.
 */
struct jmp_buf_data {
	/* Non-CHERI: x2, x8, x9, x18, x19, x20, x21, x22, x23, x24, x25, x26, x27, sp */
	/* CHERI: c2, c8, c9, c18, c19, c20, c21, c22, c23, c24, c25, c26, c27, csp */
	uintptr_t s_regs[12];	/* s0 - s11 / cs0 - cs11 */
	uintptr_t ra;
	uintptr_t sp;
};

typedef struct jmp_buf_data jmp_buf[1];

int setjmp(jmp_buf jmp);
void longjmp(jmp_buf jmp, int ret);

#endif /* _SETJMP_H_ */
