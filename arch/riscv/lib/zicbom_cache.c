// SPDX-License-Identifier: GPL-2.0+
/*
 */

#include <asm/asm.h>
#ifdef CONFIG_RISCV_ISA_ZCHERIPURECAP_ABI
#include <asm/cheri.h>
#endif
#include <cpu_func.h>

#ifndef CONFIG_SYS_CACHELINE_SIZE
#error "CONFIG_SYS_CACHELINE_SIZE must be defined"
#endif

void flush_dcache_range(unsigned long start, unsigned long end)
{
	void *addr, *end_addr;

	start &= ~(CONFIG_SYS_CACHELINE_SIZE - 1);

#ifdef CONFIG_RISCV_ISA_ZCHERIPURECAP_ABI
	addr = cheri_build_infinite_cap(start);
	end_addr = cheri_build_infinite_cap(end);
#else
	addr = (void*)start;
	end_addr = (void*)end;
#endif

	for ( ; addr < end_addr; addr += CONFIG_SYS_CACHELINE_SIZE)
		 __asm__ __volatile__ ("cbo.flush 0(%[addr])\n" :: [addr] PTR_REG(addr));
}

void invalidate_dcache_range(unsigned long start, unsigned long end)
{
	flush_dcache_range(start, end);
}

