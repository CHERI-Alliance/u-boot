/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2015 Regents of the University of California
 */

#ifndef _ASM_RISCV_ASM_H
#define _ASM_RISCV_ASM_H

#ifdef __ASSEMBLY__
#define __ASM_STR(x)	x
#else
#define __ASM_STR(x)	#x
#endif

#ifdef __CHERI__
#ifdef __ASSEMBLY__
# define lla lgy
# define mv ymv
# define add yadd
# define addi yaddi
#endif
#endif /* !__CHERI__ */

#if __riscv_xlen == 64
#define __REG_SEL(a, b)	__ASM_STR(a)
#elif __riscv_xlen == 32
#define __REG_SEL(a, b)	__ASM_STR(b)
#else
#error "Unexpected __riscv_xlen"
#endif

#ifdef __CHERI__
#define REG_L		__REG_SEL(ly, ly)
#define REG_S		__REG_SEL(sy, sy)
#define SZREG		__REG_SEL(16, 8)
#define LGREG		__REG_SEL(4, 3)

#define XREG_L		__REG_SEL(ld, lw)
#define XREG_S		__REG_SEL(sd, sw)
#define SZXREG		__REG_SEL(8, 4)
#define LGXREG		__REG_SEL(3, 2)

#define PREG_L		__REG_SEL(ly, ly)
#define PREG_S		__REG_SEL(sy, sy)
#define SZPREG		__REG_SEL(16, 8)
#define LGPREG		__REG_SEL(4, 3)

#define PC_PTR_L	__ASM_STR(lly)
#define PTR_L		__ASM_STR(lgy)

#if __SIZEOF_POINTER__ == 16
#ifdef __ASSEMBLY__
#define RISCV_PTR		.chericap
#define RISCV_SZPTR		16
#define RISCV_LGPTR		4
#else
#define RISCV_PTR		".chericap"
#define RISCV_SZPTR		"16"
#define RISCV_LGPTR		"4"
#endif
#elif __SIZEOF_POINTER__ == 8
#ifdef __ASSEMBLY__
#define RISCV_PTR		.chericap
#define RISCV_SZPTR		8
#define RISCV_LGPTR		3
#else
#define RISCV_PTR		".chericap"
#define RISCV_SZPTR		"8"
#define RISCV_LGPTR		"3"
#endif
#else
#error "Unexpected __SIZEOF_POINTER__"
#endif
#else /* !__CHERI__ */
#define REG_L		__REG_SEL(ld, lw)
#define REG_S		__REG_SEL(sd, sw)
#define SZREG		__REG_SEL(8, 4)
#define LGREG		__REG_SEL(3, 2)

#define XREG_L		__REG_SEL(ld, lw)
#define XREG_S		__REG_SEL(sd, sw)
#define SZXREG		__REG_SEL(8, 4)
#define LGXREG		__REG_SEL(3, 2)

#define PREG_L		__REG_SEL(ld, lw)
#define PREG_S		__REG_SEL(sd, sw)
#define SZPREG		__REG_SEL(8, 4)
#define LGPREG		__REG_SEL(3, 2)

#define PC_PTR_L	__ASM_STR(la)
#define PTR_L		__ASM_STR(la)

#if __SIZEOF_POINTER__ == 8
#ifdef __ASSEMBLY__
#define RISCV_PTR		.dword
#define RISCV_SZPTR		8
#define RISCV_LGPTR		3
#else
#define RISCV_PTR		".dword"
#define RISCV_SZPTR		"8"
#define RISCV_LGPTR		"3"
#endif
#elif __SIZEOF_POINTER__ == 4
#ifdef __ASSEMBLY__
#define RISCV_PTR		.word
#define RISCV_SZPTR		4
#define RISCV_LGPTR		2
#else
#define RISCV_PTR		".word"
#define RISCV_SZPTR		"4"
#define RISCV_LGPTR		"2"
#endif
#else
#error "Unexpected __SIZEOF_POINTER__"
#endif
#endif /* !__CHERI__ */

#if (__SIZEOF_INT__ == 4)
#define RISCV_INT		__ASM_STR(.word)
#define RISCV_SZINT		__ASM_STR(4)
#define RISCV_LGINT		__ASM_STR(2)
#else
#error "Unexpected __SIZEOF_INT__"
#endif

#if (__SIZEOF_SHORT__ == 2)
#define RISCV_SHORT		__ASM_STR(.half)
#define RISCV_SZSHORT		__ASM_STR(2)
#define RISCV_LGSHORT		__ASM_STR(1)
#else
#error "Unexpected __SIZEOF_SHORT__"
#endif

#endif /* _ASM_RISCV_ASM_H */
