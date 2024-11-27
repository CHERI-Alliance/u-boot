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

#ifdef CONFIG_RISCV_ISA_ZCHERIPURECAP_ABI
#ifdef __ASSEMBLY__
# define REG_NUM(r)			c ## r

# define REG_ZERO			cnull
# define REG(r)				c ## r

# define PREG_NULL			cnull
# define PREG(r)			c ## r

# define __PCSR(r)			r ## c
# define PCSR(r)			__PCSR(r)
#else
# define REG_NUM(r)			"c" #r

# define REG_ZERO			"cnull"
# define REG(r)				"c" #r

# define PREG_NULL			"cnull"
# define PREG(r)			"c" #r

# define PCSR(r)			#r "c"
#endif
#else /* !CONFIG_RISCV_ISA_ZCHERIPURECAP_ABI */
#ifdef __ASSEMBLY__
# define REG_NUM(r)			x ## r

# define REG_ZERO			zero
# define REG(r)				r

# define PREG_NULL			zero
# define PREG(r)			r

# define PCSR(r)			r
#else
# define REG_NUM(r)			"x" #r

# define REG_ZERO			"zero"
# define REG(r)				#r

# define PREG_NULL			"zero"
# define PREG(r)			 #r

# define PCSR(r)			#r
#endif
#endif /* !CONFIG_RISCV_ISA_ZCHERIPURECAP_ABI */

#if __riscv_xlen == 64
#define __REG_SEL(a, b)	__ASM_STR(a)
#elif __riscv_xlen == 32
#define __REG_SEL(a, b)	__ASM_STR(b)
#else
#error "Unexpected __riscv_xlen"
#endif

#ifdef CONFIG_RISCV_ISA_ZCHERIPURECAP_ABI
#define REG_L		__REG_SEL(lc, lc)
#define REG_S		__REG_SEL(sc, sc)
#define SZREG		__REG_SEL(16, 8)
#define LGREG		__REG_SEL(4, 3)

#define XREG_L		__REG_SEL(ld, lw)
#define XREG_S		__REG_SEL(sd, sw)
#define SZXREG		__REG_SEL(8, 4)
#define LGXREG		__REG_SEL(3, 2)

#define PREG_L		__REG_SEL(lc, lc)
#define PREG_S		__REG_SEL(sc, sc)
#define SZPREG		__REG_SEL(16, 8)
#define LGPREG		__REG_SEL(4, 3)

#define PC_PTR_L	__ASM_STR(llc)
#define PTR_L		__ASM_STR(lgc)

#define PTR_REG	    "C"

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
#else /* !CONFIG_RISCV_ISA_ZCHERIPURECAP_ABI */
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

#define PTR_REG	    "r"

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
#endif /* !CONFIG_RISCV_ISA_ZCHERIPURECAP_ABI */

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
