/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2013 Regents of the University of California
 */

#include <linux/linkage.h>
#include <asm/asm.h>

/* void *memcpy(void *, const void *, size_t) */
ENTRY(__memcpy)
WEAK(memcpy)
	beq	a0, a1, .copy_end
	/* Save for return value */
	mv	PREG(t6), PREG(a0)

	/*
	 * Register allocation for code below:
	 * a0 - start of uncopied dst
	 * a1 - start of uncopied src
	 * t0 - end of uncopied dst
	 */
	add	t0, a0, a2

	/*
	 * Use bytewise copy if too small.
	 *
	 * For non-CHERI, This threshold must be at least 2*SZREG to ensure at
	 * least one wordwise copy is performed. It is chosen to be 16 because
	 * it will save at least 7 iterations of bytewise copy, which pays off
	 * the fixed overhead. For CHERI, it is chosen to be capability aligned
	 * for tag bit preserved.
	 */
#ifdef CONFIG_RISCV_ISA_ZCHERIPURECAP_ABI
	li	a3, SZPREG
#else /* !CONFIG_RISCV_ISA_ZCHERIPURECAP_ABI */
	li	a3, 16
#endif /* !CONFIG_RISCV_ISA_ZCHERIPURECAP_ABI */
	bltu	a2, a3, .Lbyte_copy_tail

	/*
	 * Bytewise copy first to align a0 to word boundary.
	 */
	addi	a2, a0, SZREG-1
	andi	a2, a2, ~(SZREG-1)
	beq	a0, a2, 2f
1:
	lb	a5, 0(PREG(a1))
	add	PREG(a1), PREG(a1), 1
	sb	a5, 0(PREG(a0))
	add	PREG(a0), PREG(a0), 1
	bne	a0, a2, 1b
2:

	/*
	 * Now a0 is word-aligned. If a1 is also word aligned, we could perform
	 * aligned word-wise copy. Otherwise we need to perform misaligned
	 * word-wise copy.
	 */
	andi	a3, a1, SZREG-1
#ifdef CONFIG_RISCV_ISA_ZCHERIPURECAP_ABI
	bnez	a3, .Lbyte_copy_tail
#else
	bnez	a3, .Lmisaligned_word_copy
#endif

	/* Unrolled wordwise copy */
	addi	t0, t0, -(16*SZREG-1)
	bgeu	a0, t0, 2f
1:
	REG_L	REG(a2),        0(PREG(a1))
	REG_L	REG(a3),    SZREG(PREG(a1))
	REG_L	REG(a4),  2*SZREG(PREG(a1))
	REG_L	REG(a5),  3*SZREG(PREG(a1))
	REG_L	REG(a6),  4*SZREG(PREG(a1))
	REG_L	REG(a7),  5*SZREG(PREG(a1))
	REG_L	REG(t1),  6*SZREG(PREG(a1))
	REG_L	REG(t2),  7*SZREG(PREG(a1))
	REG_L	REG(t3),  8*SZREG(PREG(a1))
	REG_L	REG(t4),  9*SZREG(PREG(a1))
	REG_L	REG(t5), 10*SZREG(PREG(a1))
	REG_S	REG(a2),        0(PREG(a0))
	REG_S	REG(a3),    SZREG(PREG(a0))
	REG_S	REG(a4),  2*SZREG(PREG(a0))
	REG_S	REG(a5),  3*SZREG(PREG(a0))
	REG_S	REG(a6),  4*SZREG(PREG(a0))
	REG_S	REG(a7),  5*SZREG(PREG(a0))
	REG_S	REG(t1),  6*SZREG(PREG(a0))
	REG_S	REG(t2),  7*SZREG(PREG(a0))
	REG_S	REG(t3),  8*SZREG(PREG(a0))
	REG_S	REG(t4),  9*SZREG(PREG(a0))
	REG_S	REG(t5), 10*SZREG(PREG(a0))
	REG_L	REG(a2), 11*SZREG(PREG(a1))
	REG_L	REG(a3), 12*SZREG(PREG(a1))
	REG_L	REG(a4), 13*SZREG(PREG(a1))
	REG_L	REG(a5), 14*SZREG(PREG(a1))
	REG_L	REG(a6), 15*SZREG(PREG(a1))
	add		PREG(a1), PREG(a1), 16*SZREG
	REG_S	REG(a2), 11*SZREG(PREG(a0))
	REG_S	REG(a3), 12*SZREG(PREG(a0))
	REG_S	REG(a4), 13*SZREG(PREG(a0))
	REG_S	REG(a5), 14*SZREG(PREG(a0))
	REG_S	REG(a6), 15*SZREG(PREG(a0))
	add		PREG(a0), PREG(a0), 16*SZREG
	bltu	a0, t0, 1b
2:
	/* Post-loop increment by 16*SZREG-1 and pre-loop decrement by SZREG-1 */
	addi	t0, t0, 15*SZREG

	/* Wordwise copy */
	bgeu	a0, t0, 2f
1:
	REG_L	REG(a5), 0(PREG(a1))
	add		PREG(a1), PREG(a1), SZREG
	REG_S	REG(a5), 0(PREG(a0))
	add		PREG(a0), PREG(a0), SZREG
	bltu	a0, t0, 1b
2:
	addi	t0, t0, SZREG-1

.Lbyte_copy_tail:
	/*
	 * Bytewise copy anything left.
	 */
	beq	a0, t0, 2f
1:
	lb	a5, 0(PREG(a1))
	add	PREG(a1), PREG(a1), 1
	sb	a5, 0(PREG(a0))
	add	PREG(a0), PREG(a0), 1
	bne	a0, t0, 1b
2:

	mv	PREG(a0), PREG(t6)
.copy_end:
	ret

.Lmisaligned_word_copy:
	/*
	 * Misaligned word-wise copy.
	 * For misaligned copy we still perform word-wise copy, but we need to
	 * use the value fetched from the previous iteration and do some shifts.
	 * This is safe because we wouldn't access more words than necessary.
	 */

	/* Calculate shifts */
	slli	t3, a3, 3
	sub	t4, x0, t3 /* negate is okay as shift will only look at LSBs */

	/* Load the initial value and align a1 */
	andi	a1, a1, ~(SZXREG-1)
	XREG_L	a5, 0(PREG(a1))

	addi	t0, t0, -(SZXREG-1)
	/* At least one iteration will be executed here, no check */
1:
	srl	a4, a5, t3
	XREG_L	a5, SZXREG(PREG(a1))
	add		PREG(a1), PREG(a1), SZXREG
	sll	a2, a5, t4
	or	a2, a2, a4
	XREG_S	a2, 0(PREG(a0))
	add		PREG(a0), PREG(a0), SZXREG
	bltu	a0, t0, 1b

	/* Update pointers to correct value */
	addi	t0, t0, SZXREG-1
	add		PREG(a1), PREG(a1), a3

	j	.Lbyte_copy_tail
ENDPROC(__memcpy)
ENDPROC(memcpy)

