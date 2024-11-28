// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2024 Codasip
 */

#include <asm/csr.h>
#include <asm/cheri.h>
#include <asm/global_data.h>
#include <asm/setjmp.h>
#include <test/suites.h>
#include <test/test.h>
#include <test/ut.h>
#include <env_internal.h>
#include <linker_lists.h>
#include <interrupt.h>

#if CONFIG_IS_ENABLED(RISCV_MMODE)
#define XEPCC	"mepcc"
#define XRET	"mret"
#elif CONFIG_IS_ENABLED(RISCV_SMODE)
#define XEPCC	"sepcc"
#define XRET	"sret"
#endif

#define ZCHERI_TEST(_name, _flags)		UNIT_TEST(_name, _flags, zcheri_test)

void (*default_handler_ptr)(void);

__attribute__((optnone))
static void cheri_jmp_test(void)
{
}

static inline void zcheri_ut_inst_tag_violation_trigger(void)
{
	__asm__ __volatile__(
		"llc ct0, 1f\n\t"
		"addi t0, t0, 4\n\t"
		"csrw " XEPCC ", ct0\n\t"
		XRET "\n\t"
		"1:\n\t"
	);
}

static inline void zcheri_ut_instr_permission_violation_trigger(void)
{
	__asm__ __volatile__(
		"llc ct0, 1f\n\t"
		"add ct0, ct0, 4\n\t"
		"li t1, %0\n\t"
		"acperm ct0, ct0, t1\n\t"
		"csrw " XEPCC ", ct0\n\t"
		XRET "\n\t"
		"1:\n\t" ::
		"i"(~CHERI_PERM_EXECUTE)
	);
}

static inline void zcheri_ut_instr_length_violation_trigger(void)
{
	__asm__ __volatile__(
		"llc ct0, 1f\n\t"
		"add ct0, ct0, 4\n\t"
		"li t1, 1\n\t"
		"scbnds ct0, ct0, t1\n\t"
		"csrw " XEPCC ", ct0\n\t"
		XRET "\n\t"
		"1:\n\t"
	);
}

__attribute__((optnone))
static int zcheri_exce_data_tag_violation(struct unit_test_state *uts)
{
	struct resume_data resume;
	int ret;

	int data;
	unsigned long data_addr = (unsigned long)&data;
	int *ptr = (int *)(uintptr_t)data_addr;

	set_resume(&resume);
	ret = setjmp(resume.jump);
	if (ret) {
		ut_asserteq(1, ret);
		ut_asserteq(EXC_CHERI_FAULT, resume.code);
		ut_asserteq(EXC_CHERI_TYPE_DATA, XTVAL2_CHERI_TYPE(resume.tval2));
		ut_asserteq(EXC_CHERI_CAUSE_TAG, XTVAL2_CHERI_CAUSE(resume.tval2));

	} else {
		*ptr = 1;

		ut_reportf("No expected exception");
	}
	set_resume(NULL);

	return 0;
}
ZCHERI_TEST(zcheri_exce_data_tag_violation, 0);

__attribute__((optnone))
static int zcheri_exce_data_seal_violation(struct unit_test_state *uts)
{
	struct resume_data resume;
	int ret;

	int data;
	int *ptr = cheri_sentry_create(&data);

	set_resume(&resume);
	ret = setjmp(resume.jump);
	if (ret) {
		ut_asserteq(1, ret);
		ut_asserteq(EXC_CHERI_FAULT, resume.code);
		ut_asserteq(EXC_CHERI_TYPE_DATA, XTVAL2_CHERI_TYPE(resume.tval2));
		ut_asserteq(EXC_CHERI_CAUSE_SEAL, XTVAL2_CHERI_CAUSE(resume.tval2));

	} else {
		*ptr = 1;

		ut_reportf("No expected exception");
	}
	set_resume(NULL);

	return 0;
}
ZCHERI_TEST(zcheri_exce_data_seal_violation, 0);

__attribute__((optnone))
static int zcheri_exce_data_permission_violation(struct unit_test_state *uts)
{
	struct resume_data resume;
	int ret;

	int data;
	int *ptr = cheri_perms_and(&data, ~CHERI_PERM_WRITE);

	set_resume(&resume);
	ret = setjmp(resume.jump);
	if (ret) {
		ut_asserteq(1, ret);
		ut_asserteq(EXC_CHERI_FAULT, resume.code);
		ut_asserteq(EXC_CHERI_TYPE_DATA, XTVAL2_CHERI_TYPE(resume.tval2));
		ut_asserteq(EXC_CHERI_CAUSE_PERM, XTVAL2_CHERI_CAUSE(resume.tval2));

	} else {
		*ptr = 1;

		ut_reportf("No expected exception");
	}
	set_resume(NULL);

	return 0;
}
ZCHERI_TEST(zcheri_exce_data_permission_violation, 0);

__attribute__((optnone))
static int zcheri_exce_data_length_violation(struct unit_test_state *uts)
{
	struct resume_data resume;
	int ret;

	int array[10];
	int *ptr = array;

	set_resume(&resume);
	ret = setjmp(resume.jump);
	if (ret) {
		ut_asserteq(1, ret);
		ut_asserteq(EXC_CHERI_FAULT, resume.code);
		ut_asserteq(EXC_CHERI_TYPE_DATA, XTVAL2_CHERI_TYPE(resume.tval2));
		ut_asserteq(EXC_CHERI_CAUSE_BOUNDS, XTVAL2_CHERI_CAUSE(resume.tval2));

	} else {
		for (int i = 0; i <= ARRAY_SIZE(array); i++)
			*ptr++ = i;

		ut_reportf("No expected exception");
	}
	set_resume(NULL);

	return 0;
}
ZCHERI_TEST(zcheri_exce_data_length_violation, 0);

__attribute__((optnone))
static int zcheri_exce_jmp_tag_violation(struct unit_test_state *uts)
{
	struct resume_data resume;
	int ret;

	set_resume(&resume);
	ret = setjmp(resume.jump);
	if (ret) {
		ut_asserteq(1, ret);
		ut_asserteq(EXC_CHERI_FAULT, resume.code);
		ut_asserteq(EXC_CHERI_TYPE_JMP_BRANCH, XTVAL2_CHERI_TYPE(resume.tval2));
		ut_asserteq(EXC_CHERI_CAUSE_TAG, XTVAL2_CHERI_CAUSE(resume.tval2));

	} else {
		unsigned long fun_addr = cheri_address_get(&cheri_jmp_test);
		void (*fun_ptr)(void) = (void (*)(void))(uintptr_t)fun_addr;

		fun_ptr();

		ut_reportf("No expected exception");
	}
	set_resume(NULL);

	return 0;
}
ZCHERI_TEST(zcheri_exce_jmp_tag_violation, 0);

__attribute__((optnone))
static int zcheri_exce_jmp_seal_violation(struct unit_test_state *uts)
{
	struct resume_data resume;
	int ret;

	set_resume(&resume);
	ret = setjmp(resume.jump);
	if (ret) {
		ut_asserteq(1, ret);
		ut_asserteq(EXC_CHERI_FAULT, resume.code);
		ut_asserteq(EXC_CHERI_TYPE_JMP_BRANCH, XTVAL2_CHERI_TYPE(resume.tval2));
		ut_asserteq(EXC_CHERI_CAUSE_SEAL, XTVAL2_CHERI_CAUSE(resume.tval2));

	} else {
		void (*fun_ptr)(void) =
			(void (*)(void))cheri_address_set(gd->arch.infinite_cap,
							  cheri_address_get(&cheri_jmp_test));

		fun_ptr = cheri_sentry_create(fun_ptr);
		__asm__ __volatile__("jalr 4(%0)\n" :  : "C" (fun_ptr));

		ut_reportf("No expected exception");
	}
	set_resume(NULL);

	return 0;
}
ZCHERI_TEST(zcheri_exce_jmp_seal_violation, 0);

__attribute__((optnone))
static int zcheri_exce_jmp_permission_violation(struct unit_test_state *uts)
{
	struct resume_data resume;
	int ret;

	set_resume(&resume);
	ret = setjmp(resume.jump);
	if (ret) {
		ut_asserteq(1, ret);
		ut_asserteq(EXC_CHERI_FAULT, resume.code);
		ut_asserteq(EXC_CHERI_TYPE_JMP_BRANCH, XTVAL2_CHERI_TYPE(resume.tval2));
		ut_asserteq(EXC_CHERI_CAUSE_PERM, XTVAL2_CHERI_CAUSE(resume.tval2));

	} else {
		void (*fun_ptr)(void) =
			(void (*)(void))cheri_address_set(gd->arch.infinite_cap,
							  cheri_address_get(&cheri_jmp_test));

		fun_ptr = cheri_perms_and(fun_ptr, ~CHERI_PERM_EXECUTE);
		fun_ptr();

		ut_reportf("No expected exception");
	}
	set_resume(NULL);

	return 0;
}
ZCHERI_TEST(zcheri_exce_jmp_permission_violation, 0);

__attribute__((optnone))
static int zcheri_exce_jmp_length_violation(struct unit_test_state *uts)
{
	struct resume_data resume;
	int ret;

	set_resume(&resume);
	ret = setjmp(resume.jump);
	if (ret) {
		ut_asserteq(1, ret);
		ut_asserteq(EXC_CHERI_FAULT, resume.code);
		ut_asserteq(EXC_CHERI_TYPE_JMP_BRANCH, XTVAL2_CHERI_TYPE(resume.tval2));
		ut_asserteq(EXC_CHERI_CAUSE_BOUNDS, XTVAL2_CHERI_CAUSE(resume.tval2));

	} else {
		void (*fun_ptr)(void) =
			(void (*)(void))cheri_address_set(gd->arch.infinite_cap,
						      cheri_address_get(&cheri_jmp_test));

		fun_ptr = cheri_bounds_set(fun_ptr, 1);
		fun_ptr();

		ut_reportf("No expected exception");
	}
	set_resume(NULL);

	return 0;
}
ZCHERI_TEST(zcheri_exce_jmp_length_violation, 0);

__attribute__((optnone))
static int zcheri_exce_inst_tag_violation(struct unit_test_state *uts)
{
	struct resume_data resume;
	int ret;

	set_resume(&resume);
	ret = setjmp(resume.jump);
	if (ret) {
		ut_asserteq(1, ret);
		ut_asserteq(EXC_CHERI_FAULT, resume.code);
		ut_asserteq(EXC_CHERI_TYPE_INST, XTVAL2_CHERI_TYPE(resume.tval2));
		ut_asserteq(EXC_CHERI_CAUSE_TAG, XTVAL2_CHERI_CAUSE(resume.tval2));
	} else {
		zcheri_ut_inst_tag_violation_trigger();

		ut_reportf("No expected exception");
	}
	set_resume(NULL);

	return 0;
}
ZCHERI_TEST(zcheri_exce_inst_tag_violation, 0);

__attribute__((optnone))
static int zcheri_exce_inst_permission_violation(struct unit_test_state *uts)
{
	struct resume_data resume;
	int ret;

	set_resume(&resume);
	ret = setjmp(resume.jump);
	if (ret) {
		ut_asserteq(1, ret);
		ut_asserteq(EXC_CHERI_FAULT, resume.code);
		ut_asserteq(EXC_CHERI_TYPE_INST, XTVAL2_CHERI_TYPE(resume.tval2));
		ut_asserteq(EXC_CHERI_CAUSE_PERM, XTVAL2_CHERI_CAUSE(resume.tval2));
	} else {
		zcheri_ut_instr_permission_violation_trigger();

		ut_reportf("No expected exception");
	}
	set_resume(NULL);

	return 0;
}
ZCHERI_TEST(zcheri_exce_inst_permission_violation, 0);

__attribute__((optnone))
static int zcheri_exce_inst_length_violation(struct unit_test_state *uts)
{
	struct resume_data resume;
	int ret;

	set_resume(&resume);
	ret = setjmp(resume.jump);
	if (ret) {
		ut_asserteq(1, ret);
		ut_asserteq(EXC_CHERI_FAULT, resume.code);
		ut_asserteq(EXC_CHERI_TYPE_INST, XTVAL2_CHERI_TYPE(resume.tval2));
		ut_asserteq(EXC_CHERI_CAUSE_BOUNDS, XTVAL2_CHERI_CAUSE(resume.tval2));
	} else {
		zcheri_ut_instr_length_violation_trigger();

		ut_reportf("No expected exception");
	}
	set_resume(NULL);

	return 0;
}
ZCHERI_TEST(zcheri_exce_inst_length_violation, 0);

int do_ut_zcheri(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	struct unit_test *tests = UNIT_TEST_SUITE_START(zcheri_test);
	const int n_ents = UNIT_TEST_SUITE_COUNT(zcheri_test);

	return cmd_ut_category("zcheri", "zcheri_", tests, n_ents, argc, argv);
}
