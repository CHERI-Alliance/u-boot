#ifndef CODASIP_A70X_H
#define CODASIP_A70X_H

#include <linux/sizes.h>

#define CFG_EXTRA_ENV_SETTINGS \
	"fdt_high=0xffffffffffffffff\0" \
	"kernel_comp_addr_r=0x84000000\0" \
	"kernel_comp_size=0x4000000\0" \

#endif /* CODASIP_A70X_H */
