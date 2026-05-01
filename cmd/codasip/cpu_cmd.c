// SPDX-License-Identifier: GPL-2.0+
/*
 * The 'codasip' command for Codasip CPU.
 *
 * Copyright (c) 2026 Codasip GmbH.
 */

#include <command.h>
#include <console.h>
#include <asm/sbi.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <vsprintf.h>

#define SBI_EXT_CODASIP		0x09000503

enum sbi_ext_codasip_fid {
	SBI_EXT_CODASIP_CACHE_STATUS = 0,
	SBI_EXT_CODASIP_DCACHE_CTRL,
	SBI_EXT_CODASIP_ICACHE_CTRL,
	SBI_EXT_CODASIP_L2CACHE_CTRL,

	SBI_EXT_CODASIP_CFGCTRL_READ,
	SBI_EXT_CODASIP_CFGCTRL_WRITE,
};

static int sbi_get_mcachestatus(unsigned long *status)
{
	struct sbiret ret;

	ret = sbi_ecall(SBI_EXT_CODASIP, SBI_EXT_CODASIP_CACHE_STATUS, 0,
		0, 0, 0, 0, 0);

	if (ret.error)
		return -ENOTSUPP;

	if (status)
		*status = ret.value;

	return 0;
}

static int sbi_set_mdcachectrl(unsigned long action)
{
	struct sbiret ret;

	ret = sbi_ecall(SBI_EXT_CODASIP, SBI_EXT_CODASIP_DCACHE_CTRL, action,
		0, 0, 0, 0, 0);

	if (ret.error)
		return -ENOTSUPP;

	return 0;
}

static int sbi_set_micachectrl(unsigned long action)
{
	struct sbiret ret;

	ret = sbi_ecall(SBI_EXT_CODASIP, SBI_EXT_CODASIP_ICACHE_CTRL, action,
		0, 0, 0, 0, 0);

	if (ret.error)
		return -ENOTSUPP;

	return 0;
}

static int sbi_set_ml2cachectrl(unsigned long action)
{
	struct sbiret ret;

	ret = sbi_ecall(SBI_EXT_CODASIP, SBI_EXT_CODASIP_L2CACHE_CTRL, action,
		0, 0, 0, 0, 0);

	if (ret.error)
		return -ENOTSUPP;

	return 0;
}

static int sbi_get_mcfgctrl(unsigned long *mcfgctrl)
{
	struct sbiret ret;

	ret = sbi_ecall(SBI_EXT_CODASIP, SBI_EXT_CODASIP_CFGCTRL_READ, 0,
		0, 0, 0, 0, 0);

	if (ret.error)
		return -ENOTSUPP;

	if (mcfgctrl)
		*mcfgctrl = ret.value;

	return 0;
}

static int sbi_set_mcfgctrl(unsigned long mcfgctrl)
{
	struct sbiret ret;

	ret = sbi_ecall(SBI_EXT_CODASIP, SBI_EXT_CODASIP_CFGCTRL_WRITE, mcfgctrl,
		0, 0, 0, 0, 0);

	if (ret.error)
		return -ENOTSUPP;

	return 0;
}

static int do_codasip_sbi_detect(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	long ret;

	if (argc != 1)
		return CMD_RET_USAGE;

	ret = sbi_probe_extension(SBI_EXT_CODASIP);
	if (ret > 0)
		printf("Codasip vendor-specific SBI found.\n");
	else
		printf("Codasip vendor-specific SBI not found!\n");

	return CMD_RET_SUCCESS;
}

static int do_codasip_sbi_cache(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	if (sbi_probe_extension(SBI_EXT_CODASIP) < 1) {
		printf("Codasip vendor-specific SBI not found!\n");
		return CMD_RET_FAILURE;
	}

	if (argc == 2) {
		unsigned long status;
		if (strcmp(argv[1], "status") != 0)
			return CMD_RET_USAGE;
		if(sbi_get_mcachestatus(&status))
			return CMD_RET_FAILURE;

		printf("  MCACHESTATUS: 0x%lx\n", status);
		return CMD_RET_SUCCESS;
	} else if (argc == 3) {
		long ret = 0;
		unsigned long action = simple_strtoul(argv[2], NULL, 0);
		if (!strcmp(argv[1], "dcache")) {
			ret = sbi_set_mdcachectrl(action);
		} else if (!strcmp(argv[1], "icache")) {
			ret = sbi_set_micachectrl(action);
		} else if (!strcmp(argv[1], "l2cache")) {
			ret = sbi_set_ml2cachectrl(action);
		} else {
			return CMD_RET_USAGE;
		}
		if (ret)
			return CMD_RET_FAILURE;

		return CMD_RET_SUCCESS;
	}

	return CMD_RET_USAGE;
}

static int do_codasip_sbi_cpu(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	unsigned long cfgctrl;

	if (argc < 2 || argc > 3)
		return CMD_RET_USAGE;

	if (strcmp(argv[1], "cfgctrl") != 0)
	        return CMD_RET_USAGE;

	if (sbi_probe_extension(SBI_EXT_CODASIP) < 1) {
	    printf("Codasip vendor-specific SBI not found!\n");
	    return CMD_RET_FAILURE;
	}

	if (argc == 2) {
		if(sbi_get_mcfgctrl(&cfgctrl))
			return CMD_RET_FAILURE;

		printf("  MCFGCTRL: 0x%lx\n", cfgctrl);

		return CMD_RET_SUCCESS;
	} else if (argc == 3) {
		cfgctrl = simple_strtoul(argv[2], NULL, 0);

		if(sbi_set_mcfgctrl(cfgctrl))
			return CMD_RET_FAILURE;

	} else {
	    return CMD_RET_USAGE;
	}

	return CMD_RET_SUCCESS;
}

U_BOOT_LONGHELP(codasip,
	"sbi - Detect Codasip SBI vendor-specific extensions\n"
	"codasip cache status - Get mcachestatus\n"
	"codasip cache dcache [action] - Set mdcachectrl action\n"
	"codasip cache icache [action] - Set micachectrl action\n"
	"codasip cache l2cache [action] - Set ml2cachectrl action\n"
	"codasip cpu cfgctrl [value] - Get/Set mcfgctrl\n");

U_BOOT_CMD_WITH_SUBCMDS(codasip, "Codasip CPU commands", codasip_help_text,
	U_BOOT_CMD_MKENT(sbi, 1, 0, do_codasip_sbi_detect, "", ""),
	U_BOOT_CMD_MKENT(cache, 3, 0, do_codasip_sbi_cache, "", ""),
	U_BOOT_CMD_MKENT(cpu, 3, 0, do_codasip_sbi_cpu, "", ""));
