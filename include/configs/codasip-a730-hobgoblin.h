#ifndef CODASIP_A730_HOBGOBLIN_H
#define CODASIP_A730_HOBGOBLIN_H

#include <linux/sizes.h>

#define BOOT_TARGET_DEVICES(func) \
	func(MMC, mmc, 0)

#include <config_distro_bootcmd.h>

#define CFG_EXTRA_ENV_SETTINGS \
	"scriptaddr=0x88000000\0" \
	"fdt_high=0xffffffffffffffff\0" \
	"kernel_comp_addr_r=0x84000000\0" \
	"kernel_comp_size=0x4000000\0" \
	BOOTENV

#endif /* CCODASIP_A730_HOBGOBLIN_H */
