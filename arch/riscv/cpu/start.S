/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Startup Code for RISC-V Core
 *
 * Copyright (c) 2017 Microsemi Corporation.
 * Copyright (c) 2017 Padmarao Begari <Padmarao.Begari@microsemi.com>
 *
 * Copyright (C) 2017 Andes Technology Corporation
 * Rick Chen, Andes Technology Corporation <rick@andestech.com>
 */

#include <asm-offsets.h>
#include <config.h>
#include <elf.h>
#include <system-constants.h>
#include <asm/asm.h>
#include <asm/encoding.h>
#ifdef __CHERI__
#include <elf_cheri.h>
#include <asm/cheri.h>
#endif /* __CHERI__ */
#include <generated/asm-offsets.h>

#ifdef CONFIG_32BIT
#define RELOC_TYPE		R_RISCV_32
#define SYM_INDEX		0x8
#define SYM_SIZE		0x10
#else
#define RELOC_TYPE		R_RISCV_64
#define SYM_INDEX		0x20
#define SYM_SIZE		0x18
#endif

.section .data
secondary_harts_relocation_error:
	.ascii "Relocation of secondary harts has failed, error %d\n"

.section .text
.globl _start
_start:
#if CONFIG_IS_ENABLED(RISCV_MMODE)
	csrr	a0, CSR_MHARTID
#endif

#ifdef __CHERI__
	/* Run u-boot in Capability pointer mode */
	ymodeswy
	auipc s6, 0	/* s6 -> Use pcc as the Infinite Capability */
	yaddrw	s6, s6, zero
#endif /* __CHERI__ */

	/*
	 * Save hart id and dtb pointer. The thread pointer register is not
	 * modified by C code. It is used by secondary_hart_loop.
	 */
	mv	tp, a0
	mv	s1, a1

	/*
	 * Set the global data pointer to a known value in case we get a very
	 * early trap. The global data pointer will be set its actual value only
	 * after it has been initialized.
	 */
	mv	gp, zero

	/*
	 * Set the trap handler. This must happen after initializing gp because
	 * the handler may use it.
	 */
	PC_PTR_L	t0, trap_entry
	csrw	MODE_PREFIX(tvec), t0

	/*
	 * Mask all interrupts. Interrupts are disabled globally (in m/sstatus)
	 * for U-Boot, but we will need to read m/sip to determine if we get an
	 * IPI
	 */
	csrw	MODE_PREFIX(ie), zero

#if CONFIG_IS_ENABLED(SMP)
	/* check if hart is within range */
	/* tp: hart id */
	li	t0, CONFIG_NR_CPUS
	bge	tp, t0, hart_out_of_bounds_loop

	/* set xSIE bit to receive IPIs */
#if CONFIG_IS_ENABLED(RISCV_MMODE)
	li	t0, MIE_MSIE
#else
	li	t0, SIE_SSIE
#endif
	csrs	MODE_PREFIX(ie), t0
#endif

/*
 * Set stackpointer in internal/ex RAM to call board_init_f
 */
call_board_init_f:
#if CONFIG_IS_ENABLED(HAVE_INIT_STACK)
	li	t0, CONFIG_VAL(STACK)
#else
	li	t0, SYS_INIT_SP_ADDR
#endif
	and	t0, t0, -16		/* force 16 byte alignment */

	/* setup stack */
#ifdef __CHERI__
#if CONFIG_IS_ENABLED(SMP)
	/* tp: hart id */
	slli	t1, tp, CONFIG_STACK_SIZE_SHIFT
	sub		t2, t0, t1
#endif
	/* Initialize csp from sp with an almighty cap for initialization */
	li	t1, 1
	slli	t2, t1, CONFIG_STACK_SIZE_SHIFT
	sub 	t1, t0, t2
	yaddrw	t3, s6, t1
	ybndsrw	t1, t3, t2
	yaddrw	sp, t1, t0
	li t1,	~CHERI_PERM_STACK
	ypermc	sp, sp, t1
#else  /* !__CHERI__ */
#if CONFIG_IS_ENABLED(SMP)
	/* tp: hart id */
	slli	t1, tp, CONFIG_STACK_SIZE_SHIFT
	sub	sp, t0, t1
#else
	mv	sp, t0
#endif
#endif /* !__CHERI__ */

/*
 * Now sp points to the right stack belonging to current CPU.
 * It's essential before any function call, otherwise, we get data-race.
 */

/* clear stack if necessary */
#if CONFIG_IS_ENABLED(ZERO_MEM_BEFORE_USE)
clear_stack:
	li	t1, 1
	slli	t1, t1, CONFIG_STACK_SIZE_SHIFT
#ifdef __CHERI__
	sub	t1, sp, t1
	yaddrw	t1, sp, t1
#else /* !__CHERI__ */
	sub	t1, sp, t1
#endif /* !__CHERI__ */
clear_stack_loop:
	REG_S	zero, 0(t1)		/* t1 is always 16 byte aligned */
	add		t1, t1, SZREG
	blt	t1, sp, clear_stack_loop
#endif

#ifdef __CHERI__
	/* initialize cap table before executing c functions */
cheri_captable_init:
	PC_PTR_L	t1, __cap_relocs_start
	PC_PTR_L	t2, __cap_relocs_end
	beq	t1, t2, call_board_init_f_0

2:	XREG_L	t3, 0(t1)	/* t3 <-- capreloc:capability_location */
	yaddrw	t3, s6, t3 	/* t3 <-- W permission capability points to capability */
#ifdef CONFIG_RISCV_Y_PRE_CAP_RELOCS_INIT
	PREG_L	t5, 0(t3)	/* t5 <-- pre initialized capability */
	yeq 	t4, t5, zero
	bnez	t4, 10f
	ybld	t5, s6, t5 /* set tag for pre initialized capability by ybld */
#else /* !CONFIG_RISCV_Y_PRE_CAP_RELOCS_INIT */
	XREG_L	t4, SZXREG(t1)	/* t4 <-- capreloc:capability_base */
	bnez	t4, 3f		/* write cnull if capreloc:capability_base is zero */

	j	10f		/* Initialize next capability */

3:	/* Set address/base */
	yaddrw	t5, s6, t4 	/* t5 <-- Set base from a Infinite capability */

	/* Do not set tight bounds for functions for default setting -cheri-cap-table-abi=pcrel  */
	XREG_L	t4, (SZXREG * 4)(t1)	/* t4 <-- capreloc:capability_flags */
	li		t6,	ELFXX_CAP_RELOC_CR_FLAG_FUNC
	and		t6, t6, t4
	bnez	t6, 4f

	/* Set bounds */
	XREG_L	t4, (SZXREG * 3)(t1)	/* t4 <-- capreloc:capability_length */
	beqz	t4,	4f		/* unlimited bounds if the capability_length is zero */
	ybndsrw	t5, t5, t4

4:	/* Set offset */
	XREG_L	t4, (SZXREG * 2)(t1)	/* t4 <-- capreloc:capability_offset */
	add		t5, t5, t4

5:	/* Set permission */
	XREG_L	t4, (SZXREG * 4)(t1)	/* t4 <-- capreloc:capability_flags */
	bnez	t4, 6f
	li t6,	~CHERI_PERM_DATA			/* read-write data */
	ypermc	t5, t5, t6
	j	9f
6:	li		t6,	ELFXX_CAP_RELOC_CR_FLAG_CONST
	and		t6, t6, t4
	beqz	t6, 7f
	li t6,	~CHERI_PERM_R_DATA		/* read only data */
	ypermc	t5, t5, t6
	j	9f

7:	li		t6,	ELFXX_CAP_RELOC_CR_FLAG_FUNC
	and		t6, t6, t4
	beqz	t6, 9f
	li t6,	~CHERI_PERM_EXECUTABLE		/* function pointer */
	ypermc	t5, t5, t6

	/* Seal entry */
	yseale	t5, t5
9:
#endif /* !CONFIG_RISCV_Y_PRE_CAP_RELOCS_INIT */
	PREG_S	t5, 0(t3)
10:
	add	t1, t1, (SZXREG * 5)
	blt	t1, t2, 2b
#endif /* __CHERI__ */

call_board_init_f_0:
	/* find top of reserve space */
#if CONFIG_IS_ENABLED(SMP)
	li	t1, CONFIG_NR_CPUS
#else
	li	t1, 1
#endif
	slli	t1, t1, CONFIG_STACK_SIZE_SHIFT
	sub	a0, t0, t1		/* t1 -> size of all CPU stacks */
	jal	board_init_f_alloc_reserve

	/*
	 * Save global data pointer for later. We don't set it here because it
	 * is not initialized yet.
	 */
#ifdef __CHERI__
	/* Setup global data pointer with an almighty cap */
	yaddrw	s0, s6, a0
#else /* !__CHERI__ */
	mv	s0, a0
#endif /* !__CHERI__ */

	/* Configure proprietary settings and customized CSRs of harts */
call_harts_early_init:
	jal	harts_early_init

#if !CONFIG_IS_ENABLED(XIP)
	/*
	 * Pick hart to initialize global data and run U-Boot. The other harts
	 * wait for initialization to complete.
	 */
	PC_PTR_L	t0, hart_lottery
	li	t1, 1
	amoswap.w s2, t1, 0(t0)
	bnez	s2, wait_for_gd_init
#else
	/*
	 * FIXME: gp is set before it is initialized. If an XIP U-Boot ever
	 * encounters a pending IPI on boot it is liable to jump to whatever
	 * memory happens to be in ipi_data.addr on boot. It may also run into
	 * problems if it encounters an exception too early (because printf/puts
	 * accesses gd).
	 */
	mv gp, s0
#if CONFIG_IS_ENABLED(RISCV_MMODE)
	bnez	tp, secondary_hart_loop
#endif
#endif

	mv a0, s0
	jal	board_init_f_init_reserve

#ifdef __CHERI__
	PREG_S	s6, GD_CHERI_INFINITE_CAP(gp)
#endif

	XREG_S	s1, GD_FIRMWARE_FDT_ADDR(gp)
	/* save the boot hart id to global_data */
	XREG_S	tp, GD_BOOT_HART(gp)

#if !CONFIG_IS_ENABLED(XIP)
#ifdef CONFIG_AVAILABLE_HARTS
	PC_PTR_L	t0, available_harts_lock
	amoswap.w.rl zero, zero, 0(t0)
#endif

wait_for_gd_init:
	/*
	 * Set the global data pointer only when gd_t has been initialized.
	 * This was already set by arch_setup_gd on the boot hart, but all other
	 * harts' global data pointers gets set here.
	 */
	mv gp, s0
#ifdef CONFIG_AVAILABLE_HARTS
	PC_PTR_L	t0, available_harts_lock
	li	t1, 1
1:	amoswap.w.aq t1, t1, 0(t0)
	bnez	t1, 1b

	/* register available harts in the available_harts mask */
	li	t1, 1
	sll	t1, t1, tp
	XREG_L	t2, GD_AVAILABLE_HARTS(gp)
	or	t2, t2, t1
	XREG_S	t2, GD_AVAILABLE_HARTS(gp)

	amoswap.w.rl zero, zero, 0(t0)
#endif

	/*
	 * Continue on hart lottery winner, others branch to
	 * secondary_hart_loop.
	 */
	bnez	s2, secondary_hart_loop
#endif

#ifdef CONFIG_DEBUG_UART
	jal	debug_uart_init
#endif

	mv	a0, zero		/* a0 <-- boot_flags = 0 */
	PC_PTR_L	t5, board_init_f
	jalr	t5		/* jump to board_init_f() */

#ifdef CONFIG_XPL_BUILD
spl_clear_bss:
	PC_PTR_L	t0, __bss_start
	PC_PTR_L	t1, __bss_end
	beq	t0, t1, spl_stack_gd_setup

spl_clear_bss_loop:
	REG_S	zero, 0(t0)
	add	t0, t0, SZREG
	blt	t0, t1, spl_clear_bss_loop

spl_stack_gd_setup:
	jal	spl_relocate_stack_gd

	/* skip setup if we did not relocate */
	beqz	a0, spl_call_board_init_r
#ifdef __CHERI__
#if CONFIG_IS_ENABLED(SMP)
	li	t1, CONFIG_NR_CPUS
#else
	li	t1, 1
#endif
	slli	t0, t1, CONFIG_STACK_SIZE_SHIFT
	sub 	t1, a0, t0
	yaddrw	t2, s6, t1
	ybndsw	t1, t2, t0
	yaddrw	s0, t1, a0
#else /* __CHERI__ */
	mv	s0, a0
#endif /* __CHERI__ */

	/* setup stack on main hart */
#if CONFIG_IS_ENABLED(SMP)
	/* tp: hart id */
#ifdef __CHERI__
	li		t0, 1
	slli	t1, t0, CONFIG_STACK_SIZE_SHIFT
	slli	t0, tp, CONFIG_STACK_SIZE_SHIFT

	sub 	t2, s0, t0
	sub 	t0, t2, t1

	yaddrw	t3, s0, t0
	ybndsw	t0, t3, t1
	yaddrw	sp, t0, t2
	li t0,	~CHERI_PERM_STACK
	ypermc	sp, sp, t0
#else /* !__CHERI__ */
	slli	t0, tp, CONFIG_STACK_SIZE_SHIFT
	sub	sp, s0, t0
#endif /* !__CHERI__ */
#else
	mv	sp, s0
#endif

#if CONFIG_IS_ENABLED(SMP)
	/* set new stack and global data pointer on secondary harts */
spl_secondary_hart_stack_gd_setup:
	PC_PTR_L	a0, secondary_hart_relocate
	mv	a1, s0
	mv	a2, s0
	mv	a3, zero
	jal	smp_call_function

	/* hang if relocation of secondary harts has failed */
	beqz	a0, 1f
	mv	a1, a0
	PC_PTR_L	a0, secondary_harts_relocation_error
	jal	printf
	jal	hang
#endif

	/* set new global data pointer on main hart */
1:	mv	gp, s0

spl_call_board_init_r:
	mv	a0, zero
	mv	a1, zero
	j	board_init_r
#endif

#if !defined(CONFIG_XPL_BUILD)
/*
 * void relocate_code(addr_sp, gd, addr_moni)
 *
 * This "function" does not return, instead it continues in RAM
 * after relocating the monitor code.
 *
 */
.globl relocate_code
relocate_code:
	mv	s2, a0			/* save addr_sp */
	mv	s3, a1	/* save pointer of gd */
	mv	s4, a2			/* save addr of destination */

/*
 *Set up the stack
 */
stack_setup:
#ifdef __CHERI__
#if CONFIG_IS_ENABLED(SMP)
	/* tp: hart id */
	slli	t1, tp, CONFIG_STACK_SIZE_SHIFT
	sub		t0, s2, t1
#else
	mv	t0, s2
#endif
	/* Initialize csp from sp with an almighty cap for initialization */
	li	t1, 1
	slli	t2, t1, CONFIG_STACK_SIZE_SHIFT
	sub 	t1, t0, t2
	yaddrw	t3, s6, t1
	ybndsrw	t2, t3, t2
	yaddrw	sp, t2, t0
	li t0,	~CHERI_PERM_STACK
	ypermc	sp, sp, t0
#else /* __CHERI__ */
#if CONFIG_IS_ENABLED(SMP)
	/* tp: hart id */
	slli	t0, tp, CONFIG_STACK_SIZE_SHIFT
	sub	sp, s2, t0
#else
	mv	sp, s2
#endif
#endif /* !__CHERI__ */

	PC_PTR_L	t0, _start
	sub	t6, s4, t0		/* t6 <- relocation offset */
	beq	t0, s4, clear_bss	/* skip relocation */

#ifdef __CHERI__
	yaddrw	t1, s6, s4 /* t1 <- scratch for copy_loop */
#else /* !__CHERI__ */
	mv	t1, s4			/* t1 <- scratch for copy_loop */
#endif /* !__CHERI__ */
	PC_PTR_L	t2, __bss_start		/* t2 <- source end address */

copy_loop:
	REG_L	t5, 0(t0)
	add		t0, t0, SZREG
	REG_S	t5, 0(t1)
	add		t1, t1, SZREG
	blt	t0, t2, copy_loop

/*
 * Update dynamic relocations after board_init_f
 */
#ifdef __CHERI__
fix_cap_relocs:
	PC_PTR_L	t1, __cap_relocs_start
	PC_PTR_L	t2, __cap_relocs_end
	add		t1, t1, t6		/* t1 <- __cap_relocs_start in RAM */
	add		t2, t2, t6		/* t2 <- __cap_relocs_end in RAM */
	beq	t1, t2, clear_bss

	yaddrw	t1, s6, t1 	/* t1 <-- capability pointer with unlimited bound */
2:	XREG_L	t3, 0(t1)				/* t3 <-- capreloc:capability_location */
	yaddrw	t3, s6, t3 	/* t3 <-- W permission capability points to capability */
	add		t3, t3, t6	/* t3 <- capreloc:capability_location in RAM */

	/* Fix base and bounds */
	PREG_L	t0, 0(t3)		/* t0 <- capability to be fixed */
	yeq 	t4, t0, zero
	bnez	t4, 10f		/* fix next capability */
	ybaser 	t4, t0		/* t4 <-- capability base */
	bnez	t4, 3f
	add		t4, t0, t6 	/* fix the address for infinite cap */
	j 4f
3:
	sub		t5, t0, t4			/* t5 <-- capability offset */
	add		t4, t4, t6
	yaddrw	t4, t0, t4 	/* Set fixed base address */
	ylenr	t0, t0		/* t0 <-- capability len */
	ybndsrw	t4, t4, t0	/* Set bounds */
	add		t4, t4, t5	/* Set offset */
4:
	ybld	t4, s6, t4 /* set tag by cbld from a infinite capability */

	PREG_S	t4, 0(t3)
10:
	add	t1, t1, (SZXREG * 5)
	blt	t1, t2, 2b
#else /* !__CHERI__ */
fix_rela_dyn:
	PC_PTR_L	t1, __rel_dyn_start
	PC_PTR_L	t2, __rel_dyn_end
	beq	t1, t2, clear_bss
	add	t1, t1, t6		/* t1 <- rela_dyn_start in RAM */
	add	t2, t2, t6		/* t2 <- rela_dyn_end in RAM */

6:
	XREG_L	t5, SZXREG(t1)	/* t5 <-- relocation info:type */
	li	t3, R_RISCV_RELATIVE	/* reloc type R_RISCV_RELATIVE */
	bne	t5, t3, 8f		/* skip non-RISCV_RELOC entries */
	PREG_L	t3, 0(t1)
	PREG_L	t5, (SZXREG * 2)(t1)	/* t5 <-- addend */
	add	t5, t5, t6		/* t5 <-- location to fix up in RAM */
	add	t3, t3, t6		/* t3 <-- location to fix up in RAM */
	PREG_S	t5, 0(t3)
	j	10f

8:
	PC_PTR_L	t4, __dyn_sym_start
	add	t4, t4, t6

9:
	srli	t0, t5, SYM_INDEX	/* t0 <--- sym table index */
	andi	t5, t5, 0xFF		/* t5 <--- relocation type */
	li	t3, RELOC_TYPE
	bne	t5, t3, 10f		/* skip non-addned entries */

	PREG_L	t3, 0(t1)
	li	t5, SYM_SIZE
	mul	t0, t0, t5
	add	s5, t4, t0
	XREG_L	t0, (SZXREG * 2)(t1)	/* t0 <-- addend */
	PREG_L	t5, SZXREG(s5)
	add	t5, t5, t0
	add	t5, t5, t6		/* t5 <-- location to fix up in RAM */
	add	t3, t3, t6		/* t3 <-- location to fix up in RAM */
	PREG_S	t5, 0(t3)
10:
	add	t1, t1, (SZXREG * 3)
	blt	t1, t2, 6b
#endif /* !__CHERI__ */

/*
 * trap update
*/
	PC_PTR_L	t0, trap_entry
#ifdef __CHERI__
	add	t1, t0, t6
	yaddrw	t0, s6, t1
	li t1,	~CHERI_PERM_EXECUTABLE
	ypermc	t0, t0, t1
	add	t0, t0, t6
#endif /* !__CHERI__ */
	csrw	MODE_PREFIX(tvec), t0

clear_bss:
	PC_PTR_L	t0, __bss_start		/* t0 <- rel __bss_start in FLASH */
	add	t0, t0, t6		/* t0 <- rel __bss_start in RAM */
	PC_PTR_L	t1, __bss_end		/* t1 <- rel __bss_end in FLASH */
	add	t1, t1, t6		/* t1 <- rel __bss_end in RAM */
	beq	t0, t1, relocate_secondary_harts

#ifdef __CHERI__
	yaddrw	t0, s6, t0
#endif /* __CHERI__ */

clbss_l:
	REG_S	zero, 0(t0)		/* clear loop... */
	add	t0, t0, SZREG
	blt	t0, t1, clbss_l

relocate_secondary_harts:
#if CONFIG_IS_ENABLED(SMP)
	/* send relocation IPI */
	PC_PTR_L	t0, secondary_hart_relocate
	add	a0, t0, t6

#ifdef __CHERI__
	yaddrw	a0, s6, a0
	li t1,	~CHERI_PERM_EXECUTABLE
	ypermc	a0, a0, t1
	yseale a0, a0
#endif /* __CHERI__ */

	/* store relocation offset */
	mv	s5, t6

	mv	a1, s2
	mv	a2, s3
	mv	a3, zero
	jal	smp_call_function

	/* hang if relocation of secondary harts has failed */
	beqz	a0, 1f
	mv	a1, a0
	PC_PTR_L	a0, secondary_harts_relocation_error
	jal	printf
	jal	hang

	/* restore relocation offset */
1:	mv	t6, s5
#endif

/*
 * We are done. Do not return, instead branch to second part of board
 * initialization, now running from RAM.
 */
call_board_init_r:
	jal	invalidate_icache_all
	jal	flush_dcache_all
	PC_PTR_L	t0, board_init_r        /* offset of board_init_r() */
	add	t4, t0, t6		/* real address of board_init_r() */

#ifdef __CHERI__
	yaddrw	t4, s6, t4
	li t1,	~CHERI_PERM_EXECUTABLE
	ypermc	t4, t4, t1
	yseale t4, t4
#endif /* !__CHERI__ */

/*
 * setup parameters for board_init_r
 */
	mv	a0, s3			/* gd_t */
	mv	a1, s4			/* dest_addr */
	mv	s0, zero		/* fp == NULL */

/*
 * jump to it ...
 */
	jr	t4	/* jump to board_init_r() */
 #endif /* !defined(CONFIG_XPL_BUILD) */

#if CONFIG_IS_ENABLED(SMP)
hart_out_of_bounds_loop:
	/* Harts in this loop are out of bounds, increase CONFIG_NR_CPUS. */
	wfi
	j	hart_out_of_bounds_loop

/* SMP relocation entry */
secondary_hart_relocate:
	/* a1: new sp pointer */
	/* a2: new gd pointer */
	/* tp: hart id */

	/* setup stack */
#ifdef __CHERI__
	li	t0, 1
	slli	t1, t0, CONFIG_STACK_SIZE_SHIFT
	slli	t0, tp, CONFIG_STACK_SIZE_SHIFT
	sub		t2, a1, t0
	sub		t3, t2, t1
	yaddrw	t1, a1, t3
	ybndsw	t3, t1, t0
	yaddrw	sp, t3, t2
#else /* !__CHERI__ */
	slli	t0, tp, CONFIG_STACK_SIZE_SHIFT
	sub	sp, a1, t0
#endif /* !__CHERI__ */

	/* update global data pointer */
	mv	gp, a2
#endif

/*
 * Interrupts are disabled globally, but they can still be read from m/sip. The
 * wfi function will wake us up if we get an IPI, even if we do not trap.
 */
secondary_hart_loop:
	wfi

#if CONFIG_IS_ENABLED(SMP)
	csrr	t0, MODE_PREFIX(ip)
#if CONFIG_IS_ENABLED(RISCV_MMODE)
	andi	t0, t0, MIE_MSIE
#else
	andi	t0, t0, SIE_SSIE
#endif
	beqz	t0, secondary_hart_loop

	mv	a0, tp
	jal	handle_ipi
#endif

	j	secondary_hart_loop
