#ifndef CODASIP_A730_HOBGOBLIN_H
#define CODASIP_A730_HOBGOBLIN_H

#include <linux/sizes.h>

#define BOOT_TARGET_DEVICES(func) \
	func(MMC, mmc, 0)

#include <config_distro_bootcmd.h>

#if defined(CONFIG_TARGET_CODASIP_A730_HOBGOBLIN_PROFPGA)
#define CFG_EXTRA_ENV_SETTINGS \
	"scriptaddr=0x2008000000\0" \
	"kernel_comp_addr_r=0x2004000000\0" \
	"kernel_comp_size=0x4000000\0" \
	BOOTENV
#elif defined(CONFIG_TARGET_CODASIP_A730_HOBGOBLIN)
#define CFG_EXTRA_ENV_SETTINGS \
	"scriptaddr=0x88000000\0" \
	"kernel_comp_addr_r=0x84000000\0" \
	"kernel_comp_size=0x4000000\0" \
	BOOTENV
#else
#error "No codasip target defined!!"
#endif

#endif /* CCODASIP_A730_HOBGOBLIN_H */
