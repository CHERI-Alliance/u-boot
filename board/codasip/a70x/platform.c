/* SPDX-License-Identifier: GPL-2.0+ */

#include <cpu_func.h>
#include <dm.h>
#include <asm/sections.h>

int misc_init_r(void)
{
	return 0;
}

void *board_fdt_blob_setup(int *err)
{
	*err = 0;
	if (IS_ENABLED(CONFIG_OF_SEPARATE) || IS_ENABLED(CONFIG_OF_BOARD)) {
		if (gd->arch.firmware_fdt_addr)
			return (ulong *)(uintptr_t)gd->arch.firmware_fdt_addr;
	}

	return (ulong *)&_end;
}

int board_init(void)
{
	return 0;
}
