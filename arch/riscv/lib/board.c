// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * RISC-V-specific handling of firmware FDT
 */

#include <asm/global_data.h>
#include <asm/io.h>
#include <linux/errno.h>

DECLARE_GLOBAL_DATA_PTR;

__weak int board_fdt_blob_setup(void **fdtp)
{
	if (!gd->arch.firmware_fdt_addr)
		return -EEXIST;

	*fdtp = (ulong *)map_physmem(gd->arch.firmware_fdt_addr, 0, MAP_DATA);

	return 0;
}
