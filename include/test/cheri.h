/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2025 Codasip
 */

#ifndef __TEST_CHERI_H__
#define __TEST_CHERI_H__

#include <test/test.h>

/* Declare cheri tests */
#define CHERI_TEST(_name, _flags)		UNIT_TEST(_name, _flags, cheri)

#endif /* __TEST_CMD_H__ */
