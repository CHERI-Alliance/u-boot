/* SPDX-License-Identifier: GPL-2.0+ */

#include <cpu_func.h>
#include <dm.h>
#include <image.h>
#include <memtop.h>
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

#ifndef MMU_SECTION_SIZE
#define MMU_SECTION_SIZE        (1 * 1024 * 1024)
#endif

/* Get the top of usable RAM */
phys_addr_t board_get_usable_ram_top(phys_size_t total_size)
{
	phys_size_t size;
	phys_addr_t reg;

	if (!total_size)
		return gd->ram_top;

	/* Find enough non-reserved memory to relocate U-Boot
	 * Add 8M for other U-Boot reserved memory: display,
	 * fdt, gd,...
	 */
	size = ALIGN(SZ_8M + CONFIG_SYS_MALLOC_LEN + total_size, MMU_SECTION_SIZE);
	reg = get_mem_top(gd->ram_base, gd->ram_size, size,
			  (void *)gd->fdt_blob);

	if (!reg)
		reg = gd->ram_top - size;

	return reg + size;
}
