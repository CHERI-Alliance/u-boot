// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2024 Codasip
 */

#include <asm/cheri.h>
#include <asm/setjmp.h>
#include <test/suites.h>
#include <test/test.h>
#include <test/ut.h>
#include <env_internal.h>
#include <sort.h>

#define ZCHERI_PC_ABI_TEST(_name, _flags)		UNIT_TEST(_name, _flags, zcheri_pc_abi_test)

#define EXPECTED_CHERI_DATA_PERM (0x403e3)

static int zcheri_pc_abi_memcpy_cap(struct unit_test_state *uts)
{
	/* Test memcpy with capability pointer */
	static const char *cap_ptr_array_src[] = {
		"t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7"
	};
	char **cap_ptr_array_dst = malloc(sizeof(cap_ptr_array_src));

	memcpy(cap_ptr_array_dst, cap_ptr_array_src, sizeof(cap_ptr_array_src));

	for (size_t i = 0; i < ARRAY_SIZE(cap_ptr_array_src); i++) {
		ut_asserteq_ptr(cap_ptr_array_src[i], cap_ptr_array_dst[i]);
		ut_asserteq_strn(cap_ptr_array_src[i], cap_ptr_array_dst[i]);
	}

	free(cap_ptr_array_dst);

	return 0;
}
ZCHERI_PC_ABI_TEST(zcheri_pc_abi_memcpy_cap, 0);

static int zcheri_pc_abi_memmove_cap(struct unit_test_state *uts)
{
	/* Test memmove with capability pointer */
	static const char *cap_ptr_array_ref[] = {
		"t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7"
	};

	size_t elements = ARRAY_SIZE(cap_ptr_array_ref);
	size_t element_size = sizeof(cap_ptr_array_ref[0]);

	for (size_t overlap = 0; overlap < elements; overlap++) {
		char **cap_ptr_array_src =
			(char **)malloc(element_size * (elements + (elements - overlap)));
		char **cap_ptr_array_dst = &cap_ptr_array_src[elements - overlap];

		memcpy(cap_ptr_array_src, cap_ptr_array_ref, sizeof(cap_ptr_array_ref));

		for (size_t i = 0; i < elements; i++) {
			ut_asserteq_ptr(cap_ptr_array_ref[i], cap_ptr_array_src[i]);
			ut_asserteq_strn(cap_ptr_array_ref[i], cap_ptr_array_src[i]);
		}

		memmove(cap_ptr_array_dst, cap_ptr_array_src, sizeof(cap_ptr_array_ref));

		for (size_t i = 0; i < elements; i++) {
			ut_asserteq_ptr(cap_ptr_array_ref[i], cap_ptr_array_dst[i]);
			ut_asserteq_strn(cap_ptr_array_ref[i], cap_ptr_array_dst[i]);
		}
		free(cap_ptr_array_src);
	}

	return 0;
}
ZCHERI_PC_ABI_TEST(zcheri_pc_abi_memmove_cap, 0);

static int zcheri_pc_abi_stack_permission(struct unit_test_state *uts)
{
	char array[10];

	array[0] = 'a';

	ut_asserteq_64(cheri_perms_get(array) & CHERI_PERM_STACK,
			EXPECTED_CHERI_DATA_PERM);

	return 0;
}
ZCHERI_PC_ABI_TEST(zcheri_pc_abi_stack_permission, 0);

static int zcheri_pc_abi_malloc_smallbin(struct unit_test_state *uts)
{
	/* Test small bin, i.e. size < 512 */
	void *ptr = malloc(300UL);

	ut_assertnonnull(ptr);
	ut_assert(malloc_usable_size(ptr) >= 300UL);

	free(ptr);

	return 0;
}
ZCHERI_PC_ABI_TEST(zcheri_pc_abi_malloc_smallbin, 0);

static int zcheri_pc_abi_malloc_bin(struct unit_test_state *uts)
{
	/* Test large bin, i.e. size >= 512 */
	void *ptr = malloc(3000UL);

	ut_assertnonnull(ptr);
	ut_assert(malloc_usable_size(ptr) >= 3000UL);

	free(ptr);

	return 0;
}
ZCHERI_PC_ABI_TEST(zcheri_pc_abi_malloc_bin, 0);

static int zcheri_pc_abi_malloc_non_aligned_bound(struct unit_test_state *uts)
{
	/* Test malloc a non-aligned bonnd address >= 16M */
	for (size_t i = 14; i < 20; i++) {
		size_t alloc_len = (1 << i) + 5;
		void *ptr = malloc(alloc_len);

		ut_assertnonnull(ptr);
		ut_assert(alloc_len <= malloc_usable_size(ptr));
		ut_asserteq(cheri_offset_get(ptr), 0UL);

		free(ptr);
	}

	return 0;
}
ZCHERI_PC_ABI_TEST(zcheri_pc_abi_malloc_non_aligned_bound, 0);

static int zcheri_pc_abi_realloc_non_aligned_bound(struct unit_test_state *uts)
{
	/* Realloc to a larger size from a aligned bound size */
	/* to and non-aligned bound.                          */
	char *ptr = (char *)malloc(3000UL);

	ut_assertnonnull(ptr);
	ut_assert(malloc_usable_size(ptr) >= 3000UL);

	memset(ptr, 106, 200);
	ptr = (char *)realloc(ptr, 16384UL);
	ut_assertnonnull(ptr);
	ut_assert(malloc_usable_size(ptr) >= 16384UL);
	ut_asserteq(cheri_offset_get(ptr), 0UL);

	for (size_t i = 0; i < 200; i++)
		ut_asserteq(106, ptr[i]);
	free(ptr);

	/* Realloc to a larger size from a aligned bound size */
	/* to and non-aligned bound.                          */
	ptr = (char *)malloc(3000UL);
	ut_assertnonnull(ptr);
	ut_assert(malloc_usable_size(ptr) >= 3000UL);

	memset(ptr, 61, 200);
	for (size_t i = 0; i < 200; i++)
		ut_asserteq(61, ptr[i]);

	ptr = (char *)realloc(ptr, 16384UL);
	ut_assertnonnull(ptr);
	ut_assert(malloc_usable_size(ptr) >= 16384UL);
	ut_asserteq(cheri_offset_get(ptr), 0UL);

	for (size_t i = 0; i < 200; i++)
		ut_asserteq(61, ptr[i]);
	free(ptr);

	return 0;
}
ZCHERI_PC_ABI_TEST(zcheri_pc_abi_realloc_non_aligned_bound, 0);

static int zcheri_pc_abi_realloc_non_aligned_bound_backward(struct unit_test_state *uts)
{
	/* Allocate 4K pages as much as possible          */
	/* Assume the latest pages are on the top of heap */
#define MAX_NO_OF_PAGE		2500
	char **malloc_ptr = (char **)calloc(sizeof(char **), MAX_NO_OF_PAGE);
	unsigned int no_of_malloc = 0;

	for (unsigned int i = 0; i < MAX_NO_OF_PAGE; i++) {
		malloc_ptr[i] = (char *)malloc(PAGE_SIZE);
		if (!malloc_ptr[i])
			break;
		no_of_malloc++;
	}

	if (no_of_malloc < 2) {
		printf("cheri_malloc_realloc_non_aligned_bound_backward_forward can not be run as not enough memory\n");
		for (unsigned int i = 0; i < no_of_malloc; i++)	{
			free(malloc_ptr[i]);
			malloc_ptr[i] = NULL;
		}

		free(malloc_ptr);

		return 0;
	}

	/* Intentionally make free chunk before the tested malloc */
	for (unsigned int i = 0; i < no_of_malloc - 2; i++)	{
		free(malloc_ptr[i]);
		malloc_ptr[i] = NULL;
	}

	memset(malloc_ptr[no_of_malloc - 2], 61, 200);
	for (size_t i = 0; i < 200; i++)
		ut_asserteq(61, malloc_ptr[no_of_malloc - 2][i]);

	malloc_ptr[no_of_malloc - 2] = (char *)realloc(malloc_ptr[no_of_malloc - 2], 1048576UL);
	ut_assertnonnull(malloc_ptr[no_of_malloc - 2]);
	ut_assert(malloc_usable_size(malloc_ptr[no_of_malloc - 2]) >= 1048576UL);
	ut_asserteq(cheri_offset_get(malloc_ptr[no_of_malloc - 2]), 0UL);

	for (size_t i = 0; i < 200; i++)
		ut_asserteq(61, malloc_ptr[no_of_malloc - 2][i]);

	for (unsigned int i = no_of_malloc - 2; i < no_of_malloc; i++) {
		free(malloc_ptr[i]);
		malloc_ptr[i] = NULL;
	}

	free(malloc_ptr);

	return 0;
}
ZCHERI_PC_ABI_TEST(zcheri_pc_abi_realloc_non_aligned_bound_backward, 0);

static int zcheri_pc_abi_realloc_non_aligned_bound_backward_forward(struct unit_test_state *uts)
{
	/* Allocate 4K pages as much as possible          */
	/* Assume the latest pages are on the top of heap */
#define MAX_NO_OF_PAGE		2500
	char **malloc_ptr = (char **)calloc(sizeof(char **), MAX_NO_OF_PAGE);

	unsigned int no_of_malloc = 0;

	if (!malloc_ptr)
		return 0;

	for (unsigned int i = 0; i < MAX_NO_OF_PAGE; i++) {
		malloc_ptr[i] = (char *)malloc(PAGE_SIZE);
		if (!malloc_ptr[i])
			break;
		no_of_malloc++;
	}

	if (no_of_malloc < 3) {
		for (unsigned int i = 0; i < no_of_malloc; i++)	{
			free(malloc_ptr[i]);
			malloc_ptr[i] = NULL;
		}

		free(malloc_ptr);

		return 0;
	}

	/* Intentionally make free chunk before and after the tested malloc */
	for (unsigned int i = 0; i < no_of_malloc; i++)	{
		if (i == no_of_malloc - 1 || i == no_of_malloc - 3)
			continue;
		free(malloc_ptr[i]);
		malloc_ptr[i] = NULL;
	}

	memset(malloc_ptr[no_of_malloc - 3], 61, 200);
	for (size_t i = 0; i < 200; i++)
		ut_asserteq(61, malloc_ptr[no_of_malloc - 3][i]);

	malloc_ptr[no_of_malloc - 3] = (char *)realloc(malloc_ptr[no_of_malloc - 3], 1048576UL);
	ut_assertnonnull(malloc_ptr[no_of_malloc - 3]);
	ut_assert(malloc_usable_size(malloc_ptr[no_of_malloc - 3]) >= 1048576UL);
	ut_asserteq(cheri_offset_get(malloc_ptr[no_of_malloc - 3]), 0UL);

	for (size_t i = 0; i < 200; i++)
		ut_asserteq(61, malloc_ptr[no_of_malloc - 3][i]);

	free(malloc_ptr[no_of_malloc - 3]);
	malloc_ptr[no_of_malloc - 3] = NULL;

	free(malloc_ptr[no_of_malloc - 1]);
	malloc_ptr[no_of_malloc - 1] = NULL;

	free(malloc_ptr);

	return 0;
}
ZCHERI_PC_ABI_TEST(zcheri_pc_abi_realloc_non_aligned_bound_backward_forward, 0);

static int zcheri_pc_abi_realloc_non_aligned_bound_forward(struct unit_test_state *uts)
{
	/* Allocate 4K pages as much as possible          */
	/* Assume the latest pages are on the top of heap */
#define MAX_NO_OF_PAGE		2500
	char **malloc_ptr = (char **)calloc(sizeof(char **), MAX_NO_OF_PAGE);
	unsigned int no_of_malloc = 0;

	if (!malloc_ptr)
		return 0;

	for (unsigned int i = 0; i < MAX_NO_OF_PAGE; i++) {
		malloc_ptr[i] = (char *)malloc(PAGE_SIZE);
		if (!malloc_ptr[i])
			break;
		no_of_malloc++;
	}

	if (no_of_malloc < 257) {
		for (unsigned int i = 0; i < no_of_malloc; i++)	{
			free(malloc_ptr[i]);
			malloc_ptr[i] = NULL;
		}

		free(malloc_ptr);

		return 0;
	}

	/* Intentionally make free chunk after the tested malloc */
	for (unsigned int i = 0; i < no_of_malloc; i++)	{
		if (i == no_of_malloc - 1 || i == no_of_malloc - 257)
			continue;
		free(malloc_ptr[i]);
		malloc_ptr[i] = NULL;
	}

	memset(malloc_ptr[no_of_malloc - 257], 61, 200);
	for (size_t i = 0; i < 200; i++)
		ut_asserteq(61, malloc_ptr[no_of_malloc - 257][i]);

	malloc_ptr[no_of_malloc - 257] = (char *)realloc(malloc_ptr[no_of_malloc - 257], 1048576UL);
	ut_assertnonnull(malloc_ptr[no_of_malloc - 257]);
	ut_assert(malloc_usable_size(malloc_ptr[no_of_malloc - 257]) >= 1048576UL);
	ut_asserteq(cheri_offset_get(malloc_ptr[no_of_malloc - 257]), 0UL);

	for (size_t i = 0; i < 200; i++)
		ut_asserteq(61, malloc_ptr[no_of_malloc - 257][i]);

	free(malloc_ptr[no_of_malloc - 257]);
	malloc_ptr[no_of_malloc - 257] = NULL;

	free(malloc_ptr[no_of_malloc - 1]);
	malloc_ptr[no_of_malloc - 1] = NULL;

	free(malloc_ptr);

	return 0;
}
ZCHERI_PC_ABI_TEST(zcheri_pc_abi_realloc_non_aligned_bound_forward, 0);

static int zcheri_pc_abi_realloc_non_aligned_bound_top(struct unit_test_state *uts)
{
	/* Allocate 4K pages as much as possible          */
	/* Assume the latest pages are on the top of heap */
#define MAX_NO_OF_PAGE		2500
	char **malloc_ptr = (char **)calloc(sizeof(char **), MAX_NO_OF_PAGE);
	unsigned int no_of_malloc = 0;

	if (!malloc_ptr)
		return 0;

	for (unsigned int i = 0; i < MAX_NO_OF_PAGE; i++) {
		malloc_ptr[i] = (char *)malloc(PAGE_SIZE);
		if (!malloc_ptr[i])
			break;
		no_of_malloc++;
	}

	if (no_of_malloc < 256) {
		for (unsigned int i = 0; i < no_of_malloc; i++)	{
			free(malloc_ptr[i]);
			malloc_ptr[i] = NULL;
		}

		free(malloc_ptr);

		return 0;
	}

	/* Intentionally make free chunk after the tested malloc */
	for (unsigned int i = 0; i < no_of_malloc; i++) {
		if (i == no_of_malloc - 256)
			continue;
		free(malloc_ptr[i]);
		malloc_ptr[i] = NULL;
	}

	memset(malloc_ptr[no_of_malloc - 256], 61, 200);
	for (size_t i = 0; i < 200; i++)
		ut_asserteq(61, malloc_ptr[no_of_malloc - 256][i]);

	malloc_ptr[no_of_malloc - 256] = (char *)realloc(malloc_ptr[no_of_malloc - 256], 1048576UL);
	ut_assertnonnull(malloc_ptr[no_of_malloc - 256]);
	ut_assert(malloc_usable_size(malloc_ptr[no_of_malloc - 256]) >= 1048576UL);
	ut_asserteq(cheri_offset_get(malloc_ptr[no_of_malloc - 256]), 0UL);

	for (size_t i = 0; i < 200; i++)
		ut_asserteq(61, malloc_ptr[no_of_malloc - 256][i]);

	free(malloc_ptr[no_of_malloc - 256]);
	malloc_ptr[no_of_malloc - 256] = NULL;

	free(malloc_ptr);

	return 0;
}
ZCHERI_PC_ABI_TEST(zcheri_pc_abi_realloc_non_aligned_bound_top, 0);

static int zcheri_pc_abi_calloc_simple(struct unit_test_state *uts)
{
	/* Test calloc */
	size_t alloc_len = 300UL;
	char *ptr = (char *)calloc(1, alloc_len);

	ut_assertnonnull(ptr);
	ut_assert(alloc_len <= malloc_usable_size(ptr));
	for (size_t i = 0; i < alloc_len; i++)
		ut_asserteq(0, ptr[i]);

	free(ptr);

	return 0;
}
ZCHERI_PC_ABI_TEST(zcheri_pc_abi_calloc_simple, 0);

static int zcheri_pc_abi_memalign_simple(struct unit_test_state *uts)
{
	/* Test memalign */
	size_t alloc_len = 300UL;
	char *ptr = (char *)memalign(4096, alloc_len);

	ut_assertnonnull(ptr);
	ut_assert(IS_ALIGNED(ptr, 4096));
	ut_asserteq(cheri_offset_get(ptr), 0UL);
	ut_assert(alloc_len <= malloc_usable_size(ptr));
	memset(ptr, 23, alloc_len);
	for (size_t i = 0; i < alloc_len; i++)
		ut_asserteq(23, ptr[i]);

	free(ptr);

	return 0;
}
ZCHERI_PC_ABI_TEST(zcheri_pc_abi_memalign_simple, 0);

static int zcheri_pc_abi_valloc_simple(struct unit_test_state *uts)
{
	/* Test valloc */
	size_t alloc_len = 300UL;
	char *ptr = (char *)valloc(alloc_len);

	ut_assertnonnull(ptr);
	ut_assert(IS_ALIGNED(ptr, malloc_getpagesize));
	ut_asserteq(cheri_offset_get(ptr), 0UL);
	ut_assert(alloc_len <= malloc_usable_size(ptr));
	memset(ptr, 45, alloc_len);
	for (size_t i = 0; i < alloc_len; i++)
		ut_asserteq(45, ptr[i]);

	free(ptr);

	return 0;
}
ZCHERI_PC_ABI_TEST(zcheri_pc_abi_valloc_simple, 0);

static int zcheri_pc_abi_pvalloc_simple(struct unit_test_state *uts)
{
	/* Test pvalloc */
	size_t alloc_len = 300UL;
	char *ptr = (char *)pvalloc(alloc_len);

	ut_assertnonnull(ptr);
	ut_assert(IS_ALIGNED(ptr, malloc_getpagesize));
	ut_asserteq(cheri_offset_get(ptr), 0UL);
	ut_assert(alloc_len < malloc_getpagesize);
	ut_assert(malloc_getpagesize <= malloc_usable_size(ptr));
	memset(ptr, 24, alloc_len);
	for (size_t i = 0; i < alloc_len; i++)
		ut_asserteq(24, ptr[i]);

	free(ptr);

	return 0;
}
ZCHERI_PC_ABI_TEST(zcheri_pc_abi_pvalloc_simple, 0);

static int zcheri_pc_abi_malloc_permission(struct unit_test_state *uts)
{
	char *ptr = (char *)malloc(300UL);

	ut_assertnonnull(ptr);
	ut_asserteq_64(cheri_offset_get(ptr), 0UL);
	ut_asserteq_64(cheri_perms_get(ptr) & CHERI_PERM_HEAP,
			EXPECTED_CHERI_DATA_PERM);
	free(ptr);

	return 0;
}
ZCHERI_PC_ABI_TEST(zcheri_pc_abi_malloc_permission, 0);

static int zcheri_pc_abi_realloc_permission(struct unit_test_state *uts)
{
	char *ptr = (char *)malloc(300UL);
	u64 malloc_perms = cheri_perms_get(ptr);

	ut_assertnonnull(ptr);
	ut_asserteq_64(cheri_offset_get(ptr), 0UL);
	ut_asserteq_64(malloc_perms & CHERI_PERM_HEAP,
			EXPECTED_CHERI_DATA_PERM);

	ptr = (char *)realloc(ptr, 100UL);
	ut_assertnonnull(ptr);
	ut_asserteq_64(cheri_offset_get(ptr), 0UL);
	ut_asserteq_64(cheri_perms_get(ptr), malloc_perms);

	free(ptr);

	return 0;
}
ZCHERI_PC_ABI_TEST(zcheri_pc_abi_realloc_permission, 0);

struct cap_test {
	char byte0;
	const char *ptr;
	char byte1;
};

static int cap_test_struct_cmp(const void *p1, const void *p2)
{
	const struct cap_test *p1_t = (const struct cap_test *)p1;
	const struct cap_test *p2_t = (const struct cap_test *)p2;

	return (p2_t->byte0 - p1_t->byte0);
}

static int zcheri_pc_abi_qsort_struct(struct unit_test_state *uts)
{
	/* Test qsort with a struct which has a capability pointer */
	struct cap_test test[] = {
		{'1', "t1", 'b'},
		{'0', "t0", 'a'},
		{'2', "t2", 'c'}};

	qsort(test, ARRAY_SIZE(test), sizeof(struct cap_test), cap_test_struct_cmp);

	ut_asserteq(test[0].byte0, '2');
	ut_asserteq_str(test[0].ptr, "t2");
	ut_asserteq(test[0].byte1, 'c');

	ut_asserteq(test[1].byte0, '1');
	ut_asserteq_str(test[1].ptr, "t1");
	ut_asserteq(test[1].byte1, 'b');

	ut_asserteq(test[2].byte0, '0');
	ut_asserteq_str(test[2].ptr, "t0");
	ut_asserteq(test[2].byte1, 'a');

	return 0;
}
ZCHERI_PC_ABI_TEST(zcheri_pc_abi_qsort_struct, 0);

int cap_test_ptr_cmp(const void *p1, const void *p2)
{
	const char **p1_t = (const char **)p1;
	const char **p2_t = (const char **)p2;

	return ((*p2_t)[0] - (*p1_t)[0]);
}

static int zcheri_pc_abi_qsort_ptr(struct unit_test_state *uts)
{
	/* Test qsort with capability pointer */
	static const char *test_ptr[] = { "abc", "hij", "efg" };

	qsort(test_ptr, ARRAY_SIZE(test_ptr), sizeof(char *), cap_test_ptr_cmp);

	ut_asserteq_str(test_ptr[0], "hij");
	ut_asserteq_str(test_ptr[1], "efg");
	ut_asserteq_str(test_ptr[2], "abc");

	return 0;
}
ZCHERI_PC_ABI_TEST(zcheri_pc_abi_qsort_ptr, 0);

int do_ut_zcheri_pc_abi(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	struct unit_test *tests = UNIT_TEST_SUITE_START(zcheri_pc_abi_test);
	const int n_ents = UNIT_TEST_SUITE_COUNT(zcheri_pc_abi_test);

	return cmd_ut_category("zcheri_pc_abi", "zcheri_pc_abi_", tests, n_ents, argc, argv);
}
