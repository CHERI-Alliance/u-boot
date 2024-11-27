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

	.text

	/* trap entry */
	.align 6
	.global trap_entry
trap_entry:
	addi sp, sp, -32 * SZREG
	REG_S x1,   1 * SZREG(sp)
	REG_S x2,   2 * SZREG(sp)
	REG_S x3,   3 * SZREG(sp)
	REG_S x4,   4 * SZREG(sp)
	REG_S x5,   5 * SZREG(sp)
	REG_S x6,   6 * SZREG(sp)
	REG_S x7,   7 * SZREG(sp)
	REG_S x8,   8 * SZREG(sp)
	REG_S x9,   9 * SZREG(sp)
	REG_S x10, 10 * SZREG(sp)
	REG_S x11, 11 * SZREG(sp)
	REG_S x12, 12 * SZREG(sp)
	REG_S x13, 13 * SZREG(sp)
	REG_S x14, 14 * SZREG(sp)
	REG_S x15, 15 * SZREG(sp)
	REG_S x16, 16 * SZREG(sp)
	REG_S x17, 17 * SZREG(sp)
	REG_S x18, 18 * SZREG(sp)
	REG_S x19, 19 * SZREG(sp)
	REG_S x20, 20 * SZREG(sp)
	REG_S x21, 21 * SZREG(sp)
	REG_S x22, 22 * SZREG(sp)
	REG_S x23, 23 * SZREG(sp)
	REG_S x24, 24 * SZREG(sp)
	REG_S x25, 25 * SZREG(sp)
	REG_S x26, 26 * SZREG(sp)
	REG_S x27, 27 * SZREG(sp)
	REG_S x28, 28 * SZREG(sp)
	REG_S x29, 29 * SZREG(sp)
	REG_S x30, 30 * SZREG(sp)
	REG_S x31, 31 * SZREG(sp)
	csrr a0, MODE_PREFIX(cause)
	csrr a1, MODE_PREFIX(epc)
	csrr a2, MODE_PREFIX(tval)
	mv a3, sp
	jal handle_trap
	csrw MODE_PREFIX(epc), a0

	REG_L x1,   1 * SZREG(sp)
	REG_L x3,   3 * SZREG(sp)
	REG_L x4,   4 * SZREG(sp)
	REG_L x5,   5 * SZREG(sp)
	REG_L x6,   6 * SZREG(sp)
	REG_L x7,   7 * SZREG(sp)
	REG_L x8,   8 * SZREG(sp)
	REG_L x9,   9 * SZREG(sp)
	REG_L x10, 10 * SZREG(sp)
	REG_L x11, 11 * SZREG(sp)
	REG_L x12, 12 * SZREG(sp)
	REG_L x13, 13 * SZREG(sp)
	REG_L x14, 14 * SZREG(sp)
	REG_L x15, 15 * SZREG(sp)
	REG_L x16, 16 * SZREG(sp)
	REG_L x17, 17 * SZREG(sp)
	REG_L x18, 18 * SZREG(sp)
	REG_L x19, 19 * SZREG(sp)
	REG_L x20, 20 * SZREG(sp)
	REG_L x21, 21 * SZREG(sp)
	REG_L x22, 22 * SZREG(sp)
	REG_L x23, 23 * SZREG(sp)
	REG_L x24, 24 * SZREG(sp)
	REG_L x25, 25 * SZREG(sp)
	REG_L x26, 26 * SZREG(sp)
	REG_L x27, 27 * SZREG(sp)
	REG_L x28, 28 * SZREG(sp)
	REG_L x29, 29 * SZREG(sp)
	REG_L x30, 30 * SZREG(sp)
	REG_L x31, 31 * SZREG(sp)
	REG_L x2,   2 * SZREG(sp)
	addi sp, sp, 32 * SZREG
	MODE_PREFIX(ret)
