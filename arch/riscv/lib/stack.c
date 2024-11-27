// SPDX-License-Identifier: GPL-2.0+

#include <asm/cheri.h>
#include <asm/global_data.h>
#include <init.h>

int arch_reserve_stacks(void)
{
	/* Initialize the root stack capability */
	/* Aligned the stack to avoid bounds imprecision */
#if CONFIG_IS_ENABLED(SMP)
	size_t len = CONFIG_NR_CPUS << CONFIG_STACK_SIZE_SHIFT;
#else
	size_t len = 1 << CONFIG_STACK_SIZE_SHIFT;
#endif
	size_t rounded_len = cheri_representable_length(len);
	unsigned long end_addr_sp = gd->start_addr_sp - rounded_len;

	end_addr_sp = end_addr_sp & cheri_representable_alignment_mask(rounded_len);
	gd->start_addr_sp = end_addr_sp + len;

	return 0;
}
