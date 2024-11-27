/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * M-mode Trap Handler Code for RISC-V Core
 *
 * Copyright (c) 2017 Microsemi Corporation.
 * Copyright (c) 2017 Padmarao Begari <Padmarao.Begari@microsemi.com>
 *
 * Copyright (C) 2017 Andes Technology Corporation
 * Rick Chen, Andes Technology Corporation <rick@andestech.com>
 *
 * Copyright (C) 2018, Bin Meng <bmeng.cn@gmail.com>
 */

#include <asm/asm.h>
#include <asm/encoding.h>

#ifdef CONFIG_RISCV_ISA_ZCHERIPURECAP
/* TODO: Remove when CHERI toolchain support stval2 naming  */
#define stval2		0x14b
#endif

	.text

	/* trap entry */
	.align 6
	.global trap_entry
trap_entry:
	add PREG(sp), PREG(sp), -32 * SZREG
	REG_S REG_NUM(1),   1 * SZREG(PREG(sp))
	REG_S REG_NUM(2),   2 * SZREG(PREG(sp))
	REG_S REG_NUM(3),   3 * SZREG(PREG(sp))
	REG_S REG_NUM(4),   4 * SZREG(PREG(sp))
	REG_S REG_NUM(5),   5 * SZREG(PREG(sp))
	REG_S REG_NUM(6),   6 * SZREG(PREG(sp))
	REG_S REG_NUM(7),   7 * SZREG(PREG(sp))
	REG_S REG_NUM(8),   8 * SZREG(PREG(sp))
	REG_S REG_NUM(9),   9 * SZREG(PREG(sp))
	REG_S REG_NUM(10), 10 * SZREG(PREG(sp))
	REG_S REG_NUM(11), 11 * SZREG(PREG(sp))
	REG_S REG_NUM(12), 12 * SZREG(PREG(sp))
	REG_S REG_NUM(13), 13 * SZREG(PREG(sp))
	REG_S REG_NUM(14), 14 * SZREG(PREG(sp))
	REG_S REG_NUM(15), 15 * SZREG(PREG(sp))
	REG_S REG_NUM(16), 16 * SZREG(PREG(sp))
	REG_S REG_NUM(17), 17 * SZREG(PREG(sp))
	REG_S REG_NUM(18), 18 * SZREG(PREG(sp))
	REG_S REG_NUM(19), 19 * SZREG(PREG(sp))
	REG_S REG_NUM(20), 20 * SZREG(PREG(sp))
	REG_S REG_NUM(21), 21 * SZREG(PREG(sp))
	REG_S REG_NUM(22), 22 * SZREG(PREG(sp))
	REG_S REG_NUM(23), 23 * SZREG(PREG(sp))
	REG_S REG_NUM(24), 24 * SZREG(PREG(sp))
	REG_S REG_NUM(25), 25 * SZREG(PREG(sp))
	REG_S REG_NUM(26), 26 * SZREG(PREG(sp))
	REG_S REG_NUM(27), 27 * SZREG(PREG(sp))
	REG_S REG_NUM(28), 28 * SZREG(PREG(sp))
	REG_S REG_NUM(29), 29 * SZREG(PREG(sp))
	REG_S REG_NUM(30), 30 * SZREG(PREG(sp))
	REG_S REG_NUM(31), 31 * SZREG(PREG(sp))
	csrr a0, MODE_PREFIX(cause)

	csrr PREG(a1), PCSR(MODE_PREFIX(epc))

	csrr a2, MODE_PREFIX(tval)
#ifdef CONFIG_RISCV_ISA_ZCHERIPURECAP
	csrr a3, MODE_PREFIX(tval2)
#else
	mv a3, zero
#endif
	mv PREG(a4), PREG(sp)

	jal handle_trap

	csrw PCSR(MODE_PREFIX(epc)), PREG(a0)

	REG_L REG_NUM(1),   1 * SZREG(PREG(sp))
	REG_L REG_NUM(3),   3 * SZREG(PREG(sp))
	REG_L REG_NUM(4),   4 * SZREG(PREG(sp))
	REG_L REG_NUM(5),   5 * SZREG(PREG(sp))
	REG_L REG_NUM(6),   6 * SZREG(PREG(sp))
	REG_L REG_NUM(7),   7 * SZREG(PREG(sp))
	REG_L REG_NUM(8),   8 * SZREG(PREG(sp))
	REG_L REG_NUM(9),   9 * SZREG(PREG(sp))
	REG_L REG_NUM(10), 10 * SZREG(PREG(sp))
	REG_L REG_NUM(11), 11 * SZREG(PREG(sp))
	REG_L REG_NUM(12), 12 * SZREG(PREG(sp))
	REG_L REG_NUM(13), 13 * SZREG(PREG(sp))
	REG_L REG_NUM(14), 14 * SZREG(PREG(sp))
	REG_L REG_NUM(15), 15 * SZREG(PREG(sp))
	REG_L REG_NUM(16), 16 * SZREG(PREG(sp))
	REG_L REG_NUM(17), 17 * SZREG(PREG(sp))
	REG_L REG_NUM(18), 18 * SZREG(PREG(sp))
	REG_L REG_NUM(19), 19 * SZREG(PREG(sp))
	REG_L REG_NUM(20), 20 * SZREG(PREG(sp))
	REG_L REG_NUM(21), 21 * SZREG(PREG(sp))
	REG_L REG_NUM(22), 22 * SZREG(PREG(sp))
	REG_L REG_NUM(23), 23 * SZREG(PREG(sp))
	REG_L REG_NUM(24), 24 * SZREG(PREG(sp))
	REG_L REG_NUM(25), 25 * SZREG(PREG(sp))
	REG_L REG_NUM(26), 26 * SZREG(PREG(sp))
	REG_L REG_NUM(27), 27 * SZREG(PREG(sp))
	REG_L REG_NUM(28), 28 * SZREG(PREG(sp))
	REG_L REG_NUM(29), 29 * SZREG(PREG(sp))
	REG_L REG_NUM(30), 30 * SZREG(PREG(sp))
	REG_L REG_NUM(31), 31 * SZREG(PREG(sp))
	REG_L REG_NUM(2),   2 * SZREG(PREG(sp))
	add PREG(sp), PREG(sp), 32 * SZREG
	MODE_PREFIX(ret)
