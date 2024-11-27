/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Based on arch/riscv/cpu/u-boot.lds, which is
 * Copyright (C) 2017 Andes Technology Corporation
 * Rick Chen, Andes Technology Corporation <rick@andestech.com>
 *
 * and arch/mips/cpu/u-boot-spl.lds.
 */
MEMORY { .spl_mem : ORIGIN = IMAGE_TEXT_BASE, LENGTH = IMAGE_MAX_SIZE }
MEMORY { .bss_mem : ORIGIN = CONFIG_SPL_BSS_START_ADDR, \
		    LENGTH = CONFIG_SPL_BSS_MAX_SIZE }

OUTPUT_ARCH("riscv")
ENTRY(_start)

SECTIONS
{
	. = ALIGN(4);
	.text : {
		arch/riscv/cpu/start.o	(.text)
		*(.text*)
	} > .spl_mem

#ifdef CONFIG_RISCV_ISA_ZCHERIPURECAP_ABI
	. = ALIGN(16);
#else
	. = ALIGN(4);
#endif
	.rodata : {
		*(SORT_BY_ALIGNMENT(SORT_BY_NAME(.rodata*)))
	} > .spl_mem

#ifdef CONFIG_RISCV_ISA_ZCHERIPURECAP_ABI
	. = ALIGN(16);
#else
	. = ALIGN(4);
#endif
	.data : {
		*(.data*)
	} > .spl_mem
#ifdef CONFIG_RISCV_ISA_ZCHERIPURECAP_ABI
	. = ALIGN(16);
#else
	. = ALIGN(4);
#endif

	__u_boot_list : {
		KEEP(*(SORT(__u_boot_list*)));
	} > .spl_mem

#ifdef CONFIG_RISCV_ISA_ZCHERIPURECAP_ABI
	. = ALIGN(16);
#else
	. = ALIGN(4);
#endif

#ifdef CONFIG_RISCV_ISA_ZCHERIPURECAP_ABI
	__cap_relocs : {
		__cap_relocs_start = .;
		*(__cap_relocs)
		__cap_relocs_end = .;
	} > .spl_mem

	. = ALIGN(16);
	.captable : {
		__captable_start = .;
		*(.captable)
		__captable_end = .;
	} > .spl_mem
	. = ALIGN(16);
#endif

	.binman_sym_table : {
		__binman_sym_start = .;
		KEEP(*(SORT(.binman_sym*)));
		__binman_sym_end = .;
	} > .spl_mem

#ifdef CONFIG_RISCV_ISA_ZCHERIPURECAP_ABI
	. = ALIGN(16);
#else
	. = ALIGN(8);
#endif

	_end = .;
	_image_binary_end = .;

	.bss : {
		__bss_start = .;
		*(.bss*)
#ifdef CONFIG_RISCV_ISA_ZCHERIPURECAP_ABI
		. = ALIGN(16);
#else
		. = ALIGN(8);
#endif

		__bss_end = .;
	} > .bss_mem
}
