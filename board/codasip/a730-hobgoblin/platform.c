/* SPDX-License-Identifier: GPL-2.0+ */

#include <cpu_func.h>
#include <dm.h>
#include <asm/sections.h>
#include <asm/io.h>

int misc_init_r(void)
{
	return 0;
}

void *board_fdt_blob_setup(int *err)
{
	*err = 0;
	if (IS_ENABLED(CONFIG_OF_SEPARATE) || IS_ENABLED(CONFIG_OF_BOARD)) {
		if (gd->arch.firmware_fdt_addr)
			return map_physmem(gd->arch.firmware_fdt_addr, 0, MAP_DATA);
	}

	return map_physmem((ulong)&_end, 0, MAP_DATA);
}

int board_init(void)
{
	return 0;
}
