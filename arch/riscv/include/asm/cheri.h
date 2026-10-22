/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _ASM_RISCV_CHERI_H
#define _ASM_RISCV_CHERI_H

#ifndef __ASSEMBLY__
#include <cheriintrin.h>
#include <asm/global_data.h>
#include <asm/csr.h>
#endif /* __ASSEMBLY__ */

/* Capability permissions definition for assembly */
#if defined(__CHERI__)
#define CHERI_PERM_CAP			__CHERI_CAP_PERMISSION_CAPABILITY__
#define CHERI_PERM_WRITE		__CHERI_CAP_PERMISSION_WRITE__
#define CHERI_PERM_READ			__CHERI_CAP_PERMISSION_READ__
#define CHERI_PERM_EXECUTE		__CHERI_CAP_PERMISSION_EXECUTE__
#define CHERI_PERM_SYSTEM_REGS		__CHERI_CAP_PERMISSION_ACCESS_SYSTEM_REGISTERS__
#define CHERI_PERM_LOAD_MUTABLE		__CHERI_CAP_PERMISSION_LOAD_MUTABLE__
#if defined(__riscv_zylg)
#define CHERI_PERM_LOAD_GLOBAL    __CHERI_CAP_PERMISSION_LOAD_GLOBAL__
#define CHERI_PERM_STORE_LOCAL    __CHERI_CAP_PERMISSION_STORE_LOCAL__
#define CHERI_PERM_GLOBAL   __CHERI_CAP_PERMISSION_GLOBAL__
#endif /* defined(__riscv_zylg) */
#endif /* defined(__CHERI__) */

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

#ifdef __CHERI__
enum cheri_execution_mode {
	RISCV_CHERI_CAP_PTR_EXE_MODE = 0,
	RISCV_CHERI_INT_PTR_EXE_MODE,
};

static inline enum cheri_execution_mode cheri_execution_mode_get(const void *cap)
{
	enum cheri_execution_mode mode = RISCV_CHERI_CAP_PTR_EXE_MODE;
#ifdef __CHERI__
	__asm__ __volatile__("ymoder %0, %1\n" : "+r" (mode) : "C" (cap));
#endif
	return mode;
}

static inline void *cheri_execution_mode_set(const void *cap, enum cheri_execution_mode mode)
{
#ifdef __CHERI__
	__asm__ __volatile__("ymodew %0, %0, %1\n" : "+C" (cap) : "r" (mode));
#endif
	return (void *)cap;
}

static inline void *cheri_cap_reloc(const void *cap, unsigned long offset)
{
	if (cap) {
		unsigned long cap_base = cheri_base_get(cap);
		unsigned long cap_offset = cheri_offset_get(cap);
		unsigned long cap_addr = cheri_address_get(cap);
		unsigned long cap_len = cheri_length_get(cap);

		if (cap_base == 0x0) {
			/* Cap with unlimited bounds */
			cap = cheri_address_set(cap, cap_addr + offset);
		} else {
			cap = cheri_address_set(cap, cap_base + offset);
			cap = cheri_bounds_set(cap, cap_len);
			cap += cap_offset;
		}

		cap = cheri_cap_build(gd->arch.infinite_cap, (uintptr_t)cap);
	}

	return (void *)cap;
}

static inline void *cheri_infinite_cap_get(void)
{
	return gd->arch.infinite_cap;
}

static inline void *cheri_build_infinite_cap(unsigned long offset)
{
	return (void *)cheri_address_set(cheri_infinite_cap_get(), offset);
}

static inline void cheri_reset_cap_csr(void)
{
	cap_csr_write(CSR_STVEC, (uintptr_t)cheri_infinite_cap_get());
	cap_csr_write(CSR_SSCRATCH, 0);

	if (!CONFIG_IS_ENABLED(RISCV_SMODE)) {
		cap_csr_write(CSR_MTVEC, (uintptr_t)cheri_infinite_cap_get());
		cap_csr_write(CSR_MSCRATCH, 0);
	}
}
#endif /* __CHERI__ */
#endif /* __ASSEMBLY__ */
#endif /* _ASM_RISCV_CHERI_H */
