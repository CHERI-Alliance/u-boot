#ifndef CODASIP_A70X_H
#define CODASIP_A70X_H

#include <linux/sizes.h>

#define CONFIG_SYS_SDRAM_BASE		0x80000000
#define CONFIG_EXTRA_ENV_SETTINGS \
	"fdt_high=0xffffffffffffffff\0" \
	"mmcdev=0\0" \
	"bootenvfile=uEnv.txt\0" \
	"linuxfile=Image\0" \
	"linuxaddr=0x81000000\0" \
	"dtbaddr=0x86000000\0" \
	"envboot=fatload mmc 0 ${loadaddr} ${bootenvfile} && " \
		"env import -t ${loadaddr} ${filesize};\0" \
	"downloaddtb=${netcmd} ${dtbaddr} ${serverip}:${dtbfile};\0" \
	"downloadlinux=${netcmd} ${linuxaddr} ${serverip}:${linuxfile};\0" \
	"netboot=if test -z \"${serverip}\"; then " \
			"echo No TFTP server IP address specified, set the variable serverip.; " \
		"else " \
			"if test -n ${ipaddr}; then " \
				"setenv netcmd tftpboot; " \
			"else " \
				"setenv netcmd dhcp; " \
			"fi; " \
			"if test -n ${dtbfile}; then " \
				"run downloaddtb && " \
				"run downloadlinux && " \
				"booti ${linuxaddr} - ${dtbaddr}; " \
			"else " \
				"run downloadlinux && " \
				"boot; " \
			"fi; " \
		"fi;\0" \

#endif /* CODASIP_A70X_H */
