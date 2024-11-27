/* SPDX-License-Identifier: GPL-2.0+ */

#include <cpu_func.h>
#include <dm.h>
#include <asm/sections.h>
#include <asm/io.h>

int misc_init_r(void)
{
	return 0;
}

int board_fdt_blob_setup(void **fdtp)
{
	if (gd->arch.firmware_fdt_addr) {
		*fdtp = (ulong *)map_physmem(gd->arch.firmware_fdt_addr, 0, MAP_DATA);

		return 0;
	}

	return -EEXIST;
}

int board_init(void)
{
	return 0;
}
