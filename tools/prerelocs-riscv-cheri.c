// SPDX-License-Identifier: GPL-2.0+
/*
 * Pre process the permission and the bonds on RISC-V CHERI ELF
 * images to improve the runtime capability initiazation by
 * cbld instructions
 *
 * This source code is based from tools/prelink-riscv.c
 */

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <elf.h>
#include <elf_cheri.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <compiler.h>

#include "cheri/cheri_compressed_cap.h"

#ifndef EM_RISCV
#define EM_RISCV 243
#endif

#ifndef R_RISCV_32
#define R_RISCV_32 1
#endif

#ifndef R_RISCV_64
#define R_RISCV_64 2
#endif

const char *argv0;

#define die(fmt, ...) \
	do { \
		fprintf(stderr, "%s: " fmt "\n", argv0, ## __VA_ARGS__); \
		exit(EXIT_FAILURE); \
	} while (0)

#define RELOCS_BYTEORDER le
#define RELOCS_INC_BITS 32
#define RELOCS_INC_CAP_BITS 64
#include "prerelocs-riscv-cheri.inc"
#undef RELOCS_BYTEORDER
#undef RELOCS_INC_BITS
#undef RELOCS_INC_CAP_BITS

#define RELOCS_BYTEORDER le
#define RELOCS_INC_BITS 64
#define RELOCS_INC_CAP_BITS 128
#include "prerelocs-riscv-cheri.inc"
#undef RELOCS_BYTEORDER
#undef RELOCS_INC_BITS
#undef RELOCS_INC_CAP_BITS

#define RELOCS_BYTEORDER be
#define RELOCS_INC_BITS 32
#define RELOCS_INC_CAP_BITS 64
#include "prerelocs-riscv-cheri.inc"
#undef RELOCS_BYTEORDER
#undef RELOCS_INC_BITS
#undef RELOCS_INC_CAP_BITS

#define RELOCS_BYTEORDER be
#define RELOCS_INC_BITS 64
#define RELOCS_INC_CAP_BITS 128
#include "prerelocs-riscv-cheri.inc"
#undef RELOCS_BYTEORDER
#undef RELOCS_INC_BITS
#undef RELOCS_INC_CAP_BITS

int main(int argc, const char *const *argv)
{
	argv0 = argv[0];

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <u-boot>\n", argv0);
		exit(EXIT_FAILURE);
	}

	int fd = open(argv[1], O_RDWR, 0);

	if (fd < 0)
		die("Cannot open %s: %s", argv[1], strerror(errno));

	struct stat st;

	if (fstat(fd, &st) < 0)
		die("Cannot stat %s: %s", argv[1], strerror(errno));

	void *data =
		mmap(0, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (data == MAP_FAILED)
		die("Cannot mmap %s: %s", argv[1], strerror(errno));

	close(fd);

	unsigned char *e_ident = (unsigned char *)data;

	if (memcmp(e_ident, ELFMAG, SELFMAG) != 0)
		die("Invalid ELF file %s", argv[1]);

	bool is64 = e_ident[EI_CLASS] == ELFCLASS64;
	bool isbe = e_ident[EI_DATA] == ELFDATA2MSB;

	if (is64) {
		if (isbe)
			cheri_relocs_be64(data);
		else
			cheri_relocs_le64(data);
	} else {
		if (isbe)
			cheri_relocs_be32(data);
		else
			cheri_relocs_le32(data);
	}

	munmap(data, st.st_size);

	return 0;
}
