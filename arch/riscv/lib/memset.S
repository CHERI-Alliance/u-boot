/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2013 Regents of the University of California
 */


#include <linux/linkage.h>
#include <asm/asm.h>

/* void *memset(void *, int, size_t) */
ENTRY(__memset)
WEAK(memset)
	mv PREG(t0), PREG(a0)  /* Preserve return value */

	/* Defer to byte-oriented fill for small sizes */
	sltiu a3, a2, 16
	bnez a3, 4f

	/*
	 * Round to nearest XLEN-aligned address
	 * greater than or equal to start address
	 */
	addi a3, t0, SZXREG-1
	andi a3, a3, ~(SZXREG-1)
	beq a3, t0, 2f  /* Skip if already aligned */
	/* Handle initial misalignment */
	sub a4, a3, t0
1:
	sb a1, 0(PREG(t0))
	add PREG(t0), PREG(t0), 1
	bltu t0, a3, 1b
	sub a2, a2, a4  /* Update count */

2: /* Duff's device with 32 XLEN stores per iteration */
	/* Broadcast value into all bytes */
	andi a1, a1, 0xff
	slli a3, a1, 8
	or a1, a3, a1
	slli a3, a1, 16
	or a1, a3, a1
#ifdef CONFIG_64BIT
	slli a3, a1, 32
	or a1, a3, a1
#endif

	/* Calculate end address */
	andi a4, a2, ~(SZXREG-1)
	add a3, t0, a4

	andi a4, a4, 31*SZXREG  /* Calculate remainder */
	beqz a4, 3f            /* Shortcut if no remainder */
	neg a4, a4
	addi a4, a4, 32*SZXREG  /* Calculate initial offset */

	/* Adjust start address with offset */
#ifdef CONFIG_RISCV_ISA_ZCHERIPURECAP_ABI
	neg t1, a4
	add PREG(t0), PREG(t0), t1
#else
	sub PREG(t0), PREG(t0), a4
#endif

	/* Jump into loop body */
	/* Assumes 32-bit instruction lengths */
	PC_PTR_L PREG(a5), 3f
#ifdef CONFIG_64BIT
	srli a4, a4, 1
#endif
	add PREG(a5), PREG(a5), a4
	jr PREG(a5)
3:
	XREG_S a1,        0(PREG(t0))
	XREG_S a1,    SZXREG(PREG(t0))
	XREG_S a1,  2*SZXREG(PREG(t0))
	XREG_S a1,  3*SZXREG(PREG(t0))
	XREG_S a1,  4*SZXREG(PREG(t0))
	XREG_S a1,  5*SZXREG(PREG(t0))
	XREG_S a1,  6*SZXREG(PREG(t0))
	XREG_S a1,  7*SZXREG(PREG(t0))
	XREG_S a1,  8*SZXREG(PREG(t0))
	XREG_S a1,  9*SZXREG(PREG(t0))
	XREG_S a1, 10*SZXREG(PREG(t0))
	XREG_S a1, 11*SZXREG(PREG(t0))
	XREG_S a1, 12*SZXREG(PREG(t0))
	XREG_S a1, 13*SZXREG(PREG(t0))
	XREG_S a1, 14*SZXREG(PREG(t0))
	XREG_S a1, 15*SZXREG(PREG(t0))
	XREG_S a1, 16*SZXREG(PREG(t0))
	XREG_S a1, 17*SZXREG(PREG(t0))
	XREG_S a1, 18*SZXREG(PREG(t0))
	XREG_S a1, 19*SZXREG(PREG(t0))
	XREG_S a1, 20*SZXREG(PREG(t0))
	XREG_S a1, 21*SZXREG(PREG(t0))
	XREG_S a1, 22*SZXREG(PREG(t0))
	XREG_S a1, 23*SZXREG(PREG(t0))
	XREG_S a1, 24*SZXREG(PREG(t0))
	XREG_S a1, 25*SZXREG(PREG(t0))
	XREG_S a1, 26*SZXREG(PREG(t0))
	XREG_S a1, 27*SZXREG(PREG(t0))
	XREG_S a1, 28*SZXREG(PREG(t0))
	XREG_S a1, 29*SZXREG(PREG(t0))
	XREG_S a1, 30*SZXREG(PREG(t0))
	XREG_S a1, 31*SZXREG(PREG(t0))
	add PREG(t0), PREG(t0), 32*SZXREG
	bltu t0, a3, 3b
	andi a2, a2, SZXREG-1  /* Update count */

4:
	/* Handle trailing misalignment */
	beqz a2, 6f
	add a3, t0, a2
5:
	sb a1, 0(PREG(t0))
	add PREG(t0), PREG(t0), 1
	bltu t0, a3, 5b
6:
	ret
ENDPROC(__memset)
ENDPROC(memset)
