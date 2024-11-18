/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2017 Andes Technology Corporation
 * Rick Chen, Andes Technology Corporation <rick@andestech.com>
 */

OUTPUT_ARCH("riscv")
ENTRY(_start)

SECTIONS
{
	. = ALIGN(4);
	.text : {
		arch/riscv/cpu/start.o	(.text)
	}

	/* This needs to come before *(.text*) */
	.efi_runtime : {
		__efi_runtime_start = .;
		*(.text.efi_runtime*)
		*(.rodata.efi_runtime*)
		*(.data.efi_runtime*)
		__efi_runtime_stop = .;
	}

	.text_rest : {
		*(.text*)
	}

#ifdef CONFIG_RISCV_ISA_ZCHERIPURECAP_ABI
	. = ALIGN(16);
#else
	. = ALIGN(4);
#endif
	.rodata : { *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.rodata*))) }

#ifdef CONFIG_RISCV_ISA_ZCHERIPURECAP_ABI
	. = ALIGN(16);
#else
	. = ALIGN(4);
#endif
	.data : {
		*(.data*)
	}
#ifdef CONFIG_RISCV_ISA_ZCHERIPURECAP_ABI
	. = ALIGN(16);
#else
	. = ALIGN(4);
#endif

	.got : {
		__got_start = .;
		*(.got.plt) *(.got)
		__got_end = .;
	}

#ifdef CONFIG_RISCV_ISA_ZCHERIPURECAP_ABI
	. = ALIGN(16);
#else
	. = ALIGN(4);
#endif

	__u_boot_list : {
		KEEP(*(SORT(__u_boot_list*)));
	}

#ifdef CONFIG_RISCV_ISA_ZCHERIPURECAP_ABI
	. = ALIGN(16);
#else
	. = ALIGN(8);
#endif

	.efi_runtime_rel : {
		__efi_runtime_rel_start = .;
		*(.rel*.efi_runtime)
		*(.rel*.efi_runtime.*)
		__efi_runtime_rel_stop = .;
	}

#ifdef CONFIG_RISCV_ISA_ZCHERIPURECAP_ABI
	. = ALIGN(16);
	__cap_relocs : {
		__cap_relocs_start = .;
		*(__cap_relocs)
		__cap_relocs_end = .;
	}

	. = ALIGN(16);
	.captable : {
		__captable_start = .;
		*(.captable)
		__captable_end = .;
	}
	. = ALIGN(16);
#else
	/DISCARD/ : { *(.rela.plt*) }
	.rela.dyn : {
		__rel_dyn_start = .;
		*(.rela*)
		__rel_dyn_end = .;
	}

	. = ALIGN(8);

	.dynsym : {
		__dyn_sym_start = .;
		*(.dynsym)
		__dyn_sym_end = .;
	}

	. = ALIGN(8);
#endif

	_end = .;

	.bss : {
		__bss_start = .;
		*(.bss*)
		. = ALIGN(8);
		__bss_end = .;
	}
}
