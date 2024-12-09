/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _ASM_RISCV_CHERI_H
#define _ASM_RISCV_CHERI_H

#ifndef __ASSEMBLY__
#include <cheriintrin.h>
#include <asm/global_data.h>
#else /* __ASSEMBLY__ */
/* Capability permissions definition for assembly */
#if defined(__riscv_zcheripurecap)
#define CHERI_PERM_CAP			__CHERI_CAP_PERMISSION_CAPABILITY__
#define CHERI_PERM_WRITE		__CHERI_CAP_PERMISSION_WRITE__
#define CHERI_PERM_READ			__CHERI_CAP_PERMISSION_READ__
#define CHERI_PERM_EXECUTE		__CHERI_CAP_PERMISSION_EXECUTE__
#define CHERI_PERM_SYSTEM_REGS		__CHERI_CAP_PERMISSION_ACCESS_SYSTEM_REGISTERS__
#define CHERI_PERM_LOAD_MUTABLE		__CHERI_CAP_PERMISSION_LOAD_MUTABLE__
#if defined(__riscv_zcherilevels)
#define CHERI_PERM_ELEVATE_LEVEL	__CHERI_CAP_PERMISSION_ELEVATE_LEVEL__
#define CHERI_PERM_STORE_LEVEL		__CHERI_CAP_PERMISSION_STORE_LEVEL__
#define CHERI_PERM_CAPABILITY_LEVEL	__CHERI_CAP_PERMISSION_CAPABILITY_LEVEL__
#endif /* defined(__riscv_zcherilevels) */
#endif /* defined(__riscv_zcheripurecap) */
#endif /* __ASSEMBLY__ */

/* Capability permissions for Stack */
#define CHERI_PERM_STACK	(~CHERI_PERM_EXECUTE)
/* Capability permissions for Heap */
#define CHERI_PERM_HEAP		(~CHERI_PERM_EXECUTE)
/* Capability permissions for Executable */
#define CHERI_PERM_EXECUTABLE	(~CHERI_PERM_WRITE)
/* Capability permissions for Read Only Data */
#define CHERI_PERM_R_DATA	(~(CHERI_PERM_WRITE | CHERI_PERM_EXECUTE))
/* Capability permissions for Write Only Data */
#define CHERI_PERM_W_DATA	(~(CHERI_PERM_READ | CHERI_PERM_EXECUTE))
/* Capability permissions for Read Write Data */
#define CHERI_PERM_DATA		(~CHERI_PERM_EXECUTE)
/* Capability permissions for IO*/
#define CHERI_PERM_IO		(~(CHERI_PERM_EXECUTE | CHERI_PERM_CAP))

#ifndef __ASSEMBLY__
/* Get capability permission helper */
#define	CHERI_PERM_GET_SDP(cap)		(cheri_perms_get(cap) >> 6 & 0xF)
#define	CHERI_PERM_GET_CAP_LVL(cap)	(cheri_perms_get(cap) >> 4 & 0x1)

/* Getting the alignment for a length (x) which a base address
 * must be aligned to for a capability representable bounds.
 */
#define cheri_representable_alignment(x)		\
	(1 + ~cheri_representable_alignment_mask(x))
/* Check if an base address (base) is aligned for a representable
 * length (x).
 */
#define cheri_is_representable_aligned(x, base)		\
	(!(~cheri_representable_alignment_mask(x) & (base)))

#ifdef CONFIG_RISCV_ISA_ZCHERIPURECAP_ABI
enum cheri_execution_mode {
	RISCV_CHERI_CAP_PTR_EXE_MODE = 0,
	RISCV_CHERI_INT_PTR_EXE_MODE,
};

static inline enum cheri_execution_mode cheri_execution_mode_get(const void *cap)
{
	enum cheri_execution_mode mode = RISCV_CHERI_CAP_PTR_EXE_MODE;
#ifdef CONFIG_RISCV_ISA_ZCHERIHYBRID
	__asm__ __volatile__("gcmode %0, %1\n" : "+r" (mode) : "C" (cap));
#endif
	return mode;
}

static inline void *cheri_infinite_cap_get(void)
{
	return gd->arch.infinite_cap;
}
#endif /* CONFIG_RISCV_ISA_ZCHERIPURECAP_ABI */
#endif /* __ASSEMBLY__ */
#endif /* _ASM_RISCV_CHERI_H */
