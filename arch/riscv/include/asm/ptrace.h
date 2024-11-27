/*
 * Copyright (c) 2017 Microsemi Corporation.
 * Copyright (c) 2017 Padmarao Begari <Padmarao.Begari@microsemi.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef __ASM_RISCV_PTRACE_H
#define __ASM_RISCV_PTRACE_H

struct pt_regs {
	uintptr_t sepc;
	uintptr_t ra;
	uintptr_t sp;
	uintptr_t gp;
	uintptr_t tp;
	uintptr_t t0;
	uintptr_t t1;
	uintptr_t t2;
	uintptr_t s0;
	uintptr_t s1;
	uintptr_t a0;
	uintptr_t a1;
	uintptr_t a2;
	uintptr_t a3;
	uintptr_t a4;
	uintptr_t a5;
	uintptr_t a6;
	uintptr_t a7;
	uintptr_t s2;
	uintptr_t s3;
	uintptr_t s4;
	uintptr_t s5;
	uintptr_t s6;
	uintptr_t s7;
	uintptr_t s8;
	uintptr_t s9;
	uintptr_t s10;
	uintptr_t s11;
	uintptr_t t3;
	uintptr_t t4;
	uintptr_t t5;
	uintptr_t t6;
	/* Supervisor CSRs */
	uintptr_t sstatus;
	uintptr_t sbadaddr;
	uintptr_t scause;
};

#ifdef CONFIG_64BIT
#define REG_FMT "%016lx"
#else
#define REG_FMT "%08lx"
#endif

#define user_mode(regs) (((regs)->sstatus & SR_PS) == 0)

/* Helpers for working with the instruction pointer */
#define GET_IP(regs) ((regs)->sepc)
#define SET_IP(regs, val) (GET_IP(regs) = (val))

static inline unsigned long instruction_pointer(struct pt_regs *regs)
{
	return GET_IP(regs);
}

static inline void instruction_pointer_set(struct pt_regs *regs, uintptr_t val)
{
	SET_IP(regs, val);
}

#define profile_pc(regs) instruction_pointer(regs)

/* Helpers for working with the user stack pointer */
#define GET_USP(regs) ((regs)->sp)
#define SET_USP(regs, val) (GET_USP(regs) = (val))

static inline unsigned long user_stack_pointer(struct pt_regs *regs)
{
	return GET_USP(regs);
}

static inline void user_stack_pointer_set(struct pt_regs *regs, uintptr_t val)
{
	SET_USP(regs, val);
}

/* Helpers for working with the frame pointer */
#define GET_FP(regs) ((regs)->s0)
#define SET_FP(regs, val) (GET_FP(regs) = (val))

static inline unsigned long frame_pointer(struct pt_regs *regs)
{
	return GET_FP(regs);
}

static inline void frame_pointer_set(struct pt_regs *regs, uintptr_t val)
{
	SET_FP(regs, val);
}

#endif /* __ASM_RISCV_PTRACE_H */
