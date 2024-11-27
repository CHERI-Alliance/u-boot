/* SPDX-License-Identifier: GPL-2.0 */

#include <linux/linkage.h>
#include <asm/asm.h>

ENTRY(__memmove)
WEAK(memmove)
	/*
	 * Here we determine if forward copy is possible. Forward copy is
	 * preferred to backward copy as it is more cache friendly.
	 *
	 * If a0 >= a1, t0 gives their distance, if t0 >= a2 then we can
	 *   copy forward.
	 * If a0 < a1, we can always copy forward. This will make t0 negative,
	 *   so a *unsigned* comparison will always have t0 >= a2.
	 *
	 * For forward copy we just delegate the task to memcpy.
	 */
	sub	t0, a0, a1
	bltu	t0, a2, 1f
#ifdef CONFIG_RISCV_ISA_ZCHERIPURECAP_ABI
	PTR_L	PREG(t0), __memcpy
	jr	PREG(t0)
#else /* !CONFIG_RISCV_ISA_ZCHERIPURECAP_ABI */
	tail	__memcpy
#endif /* !CONFIG_RISCV_ISA_ZCHERIPURECAP_ABI */
1:

	/*
	 * Register allocation for code below:
	 * a0 - end of uncopied dst
	 * a1 - end of uncopied src
	 * t0 - start of uncopied dst
	 */
	mv	PREG(t0), PREG(a0)
	add	PREG(a0), PREG(a0), a2
	add	PREG(a1), PREG(a1), a2

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
	 * Bytewise copy first to align t0 to word boundary.
	 */
	andi	a2, a0, ~(SZREG-1)
	beq	a0, a2, 2f
1:
	add	PREG(a1), PREG(a1), -1
	lb	a5, 0(PREG(a1))
	add	PREG(a0), PREG(a0), -1
	sb	a5, 0(PREG(a0))
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

	/* Wordwise copy */
	add	PREG(t0), PREG(t0), SZREG-1
	bleu	a0, t0, 2f
1:
	add	PREG(a1), PREG(a1), -SZREG
	REG_L	PREG(a5), 0(PREG(a1))
	add	PREG(a0), PREG(a0), -SZREG
	REG_S	PREG(a5), 0(PREG(a0))
	bgtu	a0, t0, 1b
2:
	add	PREG(t0), PREG(t0), -(SZREG-1)

.Lbyte_copy_tail:
	/*
	 * Bytewise copy anything left.
	 */
	beq	a0, t0, 2f
1:
	add	PREG(a1), PREG(a1), -1
	lb	a5, 0(PREG(a1))
	add	PREG(a0), PREG(a0), -1
	sb	a5, 0(PREG(a0))
	bne	a0, t0, 1b
2:

	mv	PREG(a0), PREG(t0)
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

	add	PREG(t0), PREG(t0), SZXREG-1
	/* At least one iteration will be executed here, no check */
1:
	sll	a4, a5, t4
	add	PREG(a1), PREG(a1), -SZXREG
	XREG_L	a5, 0(PREG(a1))
	srl	a2, a5, t3
	or	a2, a2, a4
	add	PREG(a0), PREG(a0), -SZXREG
	XREG_S	a2, 0(PREG(a0))
	bgtu	a0, t0, 1b

	/* Update pointers to correct value */
	add	PREG(t0), PREG(t0), -(SZXREG-1)
	add	PREG(a1), PREG(a1), a3

	j	.Lbyte_copy_tail

ENDPROC(__memmove)
ENDPROC(memmove)
