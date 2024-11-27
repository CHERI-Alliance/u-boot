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
#ifdef CONFIG_RISCV_ISA_ZCHERIPURECAP_ABI
static inline void *cheri_infinite_cap_get(void)
{
	return gd->arch.infinite_cap;
}
#endif /* CONFIG_RISCV_ISA_ZCHERIPURECAP_ABI */
#endif /* __ASSEMBLY__ */
#endif /* _ASM_RISCV_CHERI_H */
