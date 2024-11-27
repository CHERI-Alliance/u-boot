/* SPDX-License-Identifier: GPL-2.0+ */

/* This is the ELF ABI header file for CHERI
 * https://github.com/CTSRD-CHERI/cheri-elf-psabi/blob/master/riscv.md#elf-object-files
 *
 * TODO: Merge to elf.h when the definitions becomes mature.
 */

#ifndef _ELF_CHERI_H
#define _ELF_CHERI_H
#include <elf.h>

/* Dynamic Array Tags - d_tag */
#define DT_RISCV_CHERI___CAPRELOCS	0x7000c000	/* address of cap relocation table */
#define DT_RISCV_CHERI___CAPRELOCSSZ	0x7000c001	/* size of cap relocation table */

#define	ELF32_CAP_RELOC_CR_FLAG_FUNC	(1UL << 31)
#define	ELF32_CAP_RELOC_CR_FLAG_CONST	(1UL << 30)

#define	ELF64_CAP_RELOC_CR_FLAG_FUNC	(1UL << 63)
#define	ELF64_CAP_RELOC_CR_FLAG_CONST	(1UL << 62)

#define	ELFXX_CAP_RELOC_CR_FLAG_FUNC	(1UL << (__SIZE_WIDTH__ - 1))
#define	ELFXX_CAP_RELOC_CR_FLAG_CONST	(1UL << (__SIZE_WIDTH__ - 2))

#ifndef __ASSEMBLY__
/* Relocation entry with explicit addend */
typedef struct {
	Elf32_Addr	cr_location;	/* capability location */
	Elf32_Addr	cr_base;	/* capability base */
	Elf32_Off	cr_offset;	/* capability offset */
	Elf32_Word	cr_length;	/* capability length */
	Elf32_Word	cr_flags;	/* capability flags */
} Elf32_cap_reloc;

typedef struct {
	Elf64_Addr	cr_location;	/* capability location */
	Elf64_Addr	cr_base;	/* capability base */
	Elf64_Off	cr_offset;	/* capability offset */
	Elf64_Xword	cr_length;	/* capability length */
	Elf64_Xword	cr_flags;	/* capability flags */
} Elf64_cap_reloc;
#endif /* __ASSEMBLY__ */
#endif /* _ELF_CHERI_H */
