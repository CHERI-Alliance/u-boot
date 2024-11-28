/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2000-2004
 * DENX Software Engineering
 * Wolfgang Denk, wd@denx.de
 */

#ifndef _MKIIMAGE_H_
#define _MKIIMAGE_H_

#include "os_support.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>
#include <u-boot/sha1.h>
#include "fdt_host.h"
#include "imagetool.h"

#undef MKIMAGE_DEBUG

#ifdef MKIMAGE_DEBUG
#define debug(fmt,args...)	printf (fmt ,##args)
#else
#define debug(fmt,args...)
#endif /* MKIMAGE_DEBUG */

#define log_debug(fmt, args...)	debug(fmt, ##args)

typedef unsigned long phys_addr_t;

static inline void *map_sysmem(ulong paddr, unsigned long len)
{
	return (void *)(uintptr_t)paddr;
}

static inline ulong map_to_sysmem(void *ptr)
{
	return (ulong)(uintptr_t)ptr;
}

# define MAP_NOCACHE	0	/* Produce an uncached mapping */
# define MAP_WRCOMBINE	0	/* Allow write-combining on the mapping */
# define MAP_WRBACK	0	/* Map using write-back caching */
# define MAP_WRTHROUGH	0	/* Map using write-through caching */
# define MAP_IO		0	/* Map for memory mapped IO */
# define MAP_DATA	0	/* Map for data */
# define MAP_RO_DATA	0	/* Map for read only data */
# define MAP_EXE	0	/* Map for executable */

static inline void *map_physmem(phys_addr_t paddr, unsigned long len,
				unsigned long flags)
{
	return (void *)(uintptr_t)paddr;
}

static inline void unmap_physmem(void *vaddr, unsigned long flags)
{
}

#define ALLOC_CACHE_ALIGN_BUFFER(type, name, size) type name[size]

#define MKIMAGE_TMPFILE_SUFFIX		".tmp"
#define MKIMAGE_MAX_TMPFILE_LEN		PATH_MAX
#define MKIMAGE_DEFAULT_DTC_OPTIONS	"-I dts -O dtb -p 500"
#define MKIMAGE_MAX_DTC_CMDLINE_LEN	2 * MKIMAGE_MAX_TMPFILE_LEN + 35

#endif /* _MKIIMAGE_H_ */
