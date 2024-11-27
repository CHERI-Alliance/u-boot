// SPDX-License-Identifier: GPL-2.0-only
/*
 * SBI initialilization and all extension implementation.
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Taken from Linux arch/riscv/kernel/sbi.c
 */

#include <errno.h>
#include <asm/asm.h>
#include <asm/encoding.h>
#include <asm/sbi.h>

struct sbiret sbi_ecall(int ext, int fid, uintptr_t arg0,
			uintptr_t arg1, uintptr_t arg2,
			uintptr_t arg3, uintptr_t arg4,
			uintptr_t arg5)
{
	struct sbiret ret;

	register uintptr_t a0 asm (REG(a0)) = (uintptr_t)(arg0);
	register uintptr_t a1 asm (REG(a1)) = (uintptr_t)(arg1);
	register uintptr_t a2 asm (REG(a2)) = (uintptr_t)(arg2);
	register uintptr_t a3 asm (REG(a3)) = (uintptr_t)(arg3);
	register uintptr_t a4 asm (REG(a4)) = (uintptr_t)(arg4);
	register uintptr_t a5 asm (REG(a5)) = (uintptr_t)(arg5);
	register uintptr_t a6 asm (REG(a6)) = (uintptr_t)(fid);
	register uintptr_t a7 asm (REG(a7)) = (uintptr_t)(ext);
	asm volatile ("ecall"
		      : "+" PTR_REG(a0), "+" PTR_REG(a1)
		      : PTR_REG(a2), PTR_REG(a3),
			PTR_REG(a4), PTR_REG(a5),
			PTR_REG(a6), PTR_REG(a7)
		      : "memory");
	ret.error = (long)a0;
	ret.value = (long)a1;

	return ret;
}

/**
 * sbi_set_timer() - Program the timer for next timer event.
 * @stime_value: The value after which next timer event should fire.
 *
 * Return: None
 */
void sbi_set_timer(uint64_t stime_value)
{
#if __riscv_xlen == 32
	sbi_ecall(SBI_EXT_SET_TIMER, SBI_FID_SET_TIMER, stime_value,
		  stime_value >> 32, 0, 0, 0, 0);
#else
	sbi_ecall(SBI_EXT_SET_TIMER, SBI_FID_SET_TIMER, stime_value,
		  0, 0, 0, 0, 0);
#endif
}

/**
 * sbi_get_spec_version() - get current SBI specification version
 *
 * Return: version id
 */
long sbi_get_spec_version(void)
{
	struct sbiret ret;

	ret = sbi_ecall(SBI_EXT_BASE, SBI_EXT_BASE_GET_SPEC_VERSION,
			0, 0, 0, 0, 0, 0);
	if (!ret.error)
		if (ret.value)
			return ret.value;

	return -ENOTSUPP;
}

/**
 * sbi_get_impl_id() - get SBI implementation ID
 *
 * Return: implementation ID
 */
int sbi_get_impl_id(void)
{
	struct sbiret ret;

	ret = sbi_ecall(SBI_EXT_BASE, SBI_EXT_BASE_GET_IMP_ID,
			0, 0, 0, 0, 0, 0);
	if (!ret.error)
		if (ret.value)
			return ret.value;

	return -ENOTSUPP;
}

/**
 * sbi_get_impl_version() - get SBI implementation version
 *
 * @version:	pointer to receive version
 * Return:	0 on success, -ENOTSUPP otherwise
 */
int sbi_get_impl_version(long *version)
{
	struct sbiret ret;

	ret = sbi_ecall(SBI_EXT_BASE, SBI_EXT_BASE_GET_IMP_VERSION,
			0, 0, 0, 0, 0, 0);
	if (ret.error)
		return -ENOTSUPP;
	if (version)
		*version = ret.value;
	return 0;
}

/**
 * sbi_probe_extension() - Check if an SBI extension ID is supported or not.
 * @extid: The extension ID to be probed.
 *
 * Return: Extension specific nonzero value f yes, -ENOTSUPP otherwise.
 */
int sbi_probe_extension(int extid)
{
	struct sbiret ret;

	ret = sbi_ecall(SBI_EXT_BASE, SBI_EXT_BASE_PROBE_EXT, extid,
			0, 0, 0, 0, 0);
	if (!ret.error)
		if (ret.value)
			return ret.value;

	return -ENOTSUPP;
}

/**
 * sbi_get_mvendorid() - get machine vendor ID
 *
 * @mimpid:	on return machine vendor ID
 * Return:	0 on success
 */
int sbi_get_mvendorid(long *mvendorid)
{
	struct sbiret ret;

	ret = sbi_ecall(SBI_EXT_BASE, SBI_EXT_BASE_GET_MVENDORID,
			0, 0, 0, 0, 0, 0);
	if (ret.error)
		return -ENOTSUPP;

	if (mvendorid)
		*mvendorid = ret.value;

	return 0;
}

/**
 * sbi_get_marchid() - get machine architecture ID
 *
 * @mimpid:	on return machine architecture ID
 * Return:	0 on success
 */
int sbi_get_marchid(long *marchid)
{
	struct sbiret ret;

	ret = sbi_ecall(SBI_EXT_BASE, SBI_EXT_BASE_GET_MARCHID,
			0, 0, 0, 0, 0, 0);

	if (ret.error)
		return -ENOTSUPP;

	if (marchid)
		*marchid = ret.value;

	return 0;
}

/**
 * sbi_get_mimpid() - get machine implementation ID
 *
 * @mimpid:	on return machine implementation ID
 * Return:	0 on success
 */
int sbi_get_mimpid(long *mimpid)
{
	struct sbiret ret;

	ret = sbi_ecall(SBI_EXT_BASE, SBI_EXT_BASE_GET_MIMPID,
			0, 0, 0, 0, 0, 0);

	if (ret.error)
		return -ENOTSUPP;

	if (mimpid)
		*mimpid = ret.value;

	return 0;
}

/**
 * sbi_srst_reset() - invoke system reset extension
 *
 * @type:	type of reset
 * @reason:	reason for reset
 */
void sbi_srst_reset(unsigned long type, unsigned long reason)
{
	sbi_ecall(SBI_EXT_SRST, SBI_EXT_SRST_RESET, type, reason,
		  0, 0, 0, 0);
}

/**
 * sbi_dbcn_write_byte() - write byte to debug console
 *
 * @ch:		byte to be written
 * Return:	SBI error code (SBI_SUCCESS = 0 on success)
 */
int sbi_dbcn_write_byte(unsigned char ch)
{
	struct sbiret ret;

	ret = sbi_ecall(SBI_EXT_DBCN,
			SBI_EXT_DBCN_CONSOLE_WRITE_BYTE,
			ch, 0, 0, 0, 0, 0);
	return ret.error;
}

#ifdef CONFIG_SBI_V01

/**
 * sbi_console_putchar() - Writes given character to the console device.
 * @ch: The data to be written to the console.
 *
 * Return: None
 */
void sbi_console_putchar(int ch)
{
	sbi_ecall(SBI_EXT_0_1_CONSOLE_PUTCHAR, 0, ch, 0, 0, 0, 0, 0);
}

/**
 * sbi_console_getchar() - Reads a byte from console device.
 *
 * Returns the value read from console.
 */
int sbi_console_getchar(void)
{
	struct sbiret ret;

	ret = sbi_ecall(SBI_EXT_0_1_CONSOLE_GETCHAR, 0, 0, 0, 0, 0, 0, 0);

	return ret.error;
}

/**
 * sbi_clear_ipi() - Clear any pending IPIs for the calling hart.
 *
 * Return: None
 */
void sbi_clear_ipi(void)
{
	sbi_ecall(SBI_EXT_0_1_CLEAR_IPI, 0, 0, 0, 0, 0, 0, 0);
}

/**
 * sbi_shutdown() - Remove all the harts from executing supervisor code.
 *
 * Return: None
 */
void sbi_shutdown(void)
{
	sbi_ecall(SBI_EXT_0_1_SHUTDOWN, 0, 0, 0, 0, 0, 0, 0);
}

/**
 * sbi_send_ipi() - Send an IPI to any hart.
 * @hart_mask: A cpu mask containing all the target harts.
 *
 * Return: None
 */
void sbi_send_ipi(const unsigned long *hart_mask)
{
	sbi_ecall(SBI_EXT_SEND_IPI, SBI_FID_SEND_IPI, (uintptr_t)hart_mask,
		  0, 0, 0, 0, 0);
}

/**
 * sbi_remote_fence_i() - Execute FENCE.I instruction on given remote harts.
 * @hart_mask: A cpu mask containing all the target harts.
 *
 * Return: None
 */
void sbi_remote_fence_i(const unsigned long *hart_mask)
{
	sbi_ecall(SBI_EXT_REMOTE_FENCE_I, SBI_FID_REMOTE_FENCE_I,
		  (uintptr_t)hart_mask, 0, 0, 0, 0, 0);
}

/**
 * sbi_remote_sfence_vma() - Execute SFENCE.VMA instructions on given remote
 *			     harts for the specified virtual address range.
 * @hart_mask: A cpu mask containing all the target harts.
 * @start: Start of the virtual address
 * @size: Total size of the virtual address range.
 *
 * Return: None
 */
void sbi_remote_sfence_vma(const unsigned long *hart_mask,
			   unsigned long start,
			   unsigned long size)
{
	sbi_ecall(SBI_EXT_REMOTE_SFENCE_VMA, SBI_FID_REMOTE_SFENCE_VMA,
		  (uintptr_t)hart_mask, start, size, 0, 0, 0);
}

/**
 * sbi_remote_sfence_vma_asid() - Execute SFENCE.VMA instructions on given
 * remote harts for a virtual address range belonging to a specific ASID.
 *
 * @hart_mask: A cpu mask containing all the target harts.
 * @start: Start of the virtual address
 * @size: Total size of the virtual address range.
 * @asid: The value of address space identifier (ASID).
 *
 * Return: None
 */
void sbi_remote_sfence_vma_asid(const unsigned long *hart_mask,
				unsigned long start,
				unsigned long size,
				unsigned long asid)
{
	sbi_ecall(SBI_EXT_REMOTE_SFENCE_VMA_ASID,
		  SBI_FID_REMOTE_SFENCE_VMA_ASID,
		  (uintptr_t)hart_mask, start, size, asid, 0, 0);
}

#endif /* CONFIG_SBI_V01 */
