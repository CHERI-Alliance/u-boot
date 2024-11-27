// SPDX-License-Identifier: GPL-2.0+
/*
 * Pre process the permission and the bonds on RISC-V CHERI ELF
 * images to improve the runtime capability initiazation by
 * cbld instructions
 *
 * This source code is based from tools/prelink-riscv.inc
 */

#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)
#define CONCAT3(x, y, z) CONCAT(CONCAT(x, y), z)

#define cheri_relocs_bonn    CONCAT3(cheri_relocs_, RELOCS_BYTEORDER, RELOCS_INC_BITS)
#define uintnn_t        CONCAT3(uint, RELOCS_INC_BITS, _t)
#define get_offset_bonn CONCAT3(get_offset_, RELOCS_BYTEORDER, RELOCS_INC_BITS)
#define Elf_Ehdr        CONCAT3(Elf, RELOCS_INC_BITS, _Ehdr)
#define Elf_Phdr        CONCAT3(Elf, RELOCS_INC_BITS, _Phdr)
#define Elf_Dyn         CONCAT3(Elf, RELOCS_INC_BITS, _Dyn)
#define Elf_Addr        CONCAT3(Elf, RELOCS_INC_BITS, _Addr)
#define Elf_cap_reloc   CONCAT3(Elf, RELOCS_INC_BITS, _cap_reloc)

#define ELF_CAP_RELOC_CR_FLAG_FUNC		CONCAT3(ELF, RELOCS_INC_BITS, _CAP_RELOC_CR_FLAG_FUNC)
#define ELF_CAP_RELOC_CR_FLAG_CONST		CONCAT3(ELF, RELOCS_INC_BITS, _CAP_RELOC_CR_FLAG_CONST)

#define CC_MAX_TOP	CONCAT3(CC, RELOCS_INC_CAP_BITS, R_MAX_TOP)
#define cc_cap_t		CONCAT3(cc, RELOCS_INC_CAP_BITS, r_cap_t)
#define cc_addr_t	CONCAT3(cc, RELOCS_INC_CAP_BITS, r_addr_t)
#define cc_length_t	CONCAT3(cc, RELOCS_INC_CAP_BITS, r_length_t)
#define cc_get_representable_length		CONCAT3(cc, RELOCS_INC_CAP_BITS, r_get_representable_length)
#define cc_get_alignment_mask			CONCAT3(cc, RELOCS_INC_CAP_BITS, r_get_alignment_mask)
#define cc_get_required_alignment		CONCAT3(cc, RELOCS_INC_CAP_BITS, r_get_required_alignment)
#define cc_make_max_perms_cap			CONCAT3(cc, RELOCS_INC_CAP_BITS, r_make_max_perms_cap)
#define cc_update_ap					CONCAT3(cc, RELOCS_INC_CAP_BITS, r_update_ap)
#define cc_update_m						CONCAT3(cc, RELOCS_INC_CAP_BITS, r_update_m)
#define cc_update_ct					CONCAT3(cc, RELOCS_INC_CAP_BITS, r_update_ct)
#define cc_compress_mem					CONCAT3(cc, RELOCS_INC_CAP_BITS, r_compress_mem)

#define target16_to_cpu CONCAT(RELOCS_BYTEORDER, 16_to_cpu)
#define target32_to_cpu CONCAT(RELOCS_BYTEORDER, 32_to_cpu)
#define target64_to_cpu CONCAT(RELOCS_BYTEORDER, 64_to_cpu)
#define targetnn_to_cpu CONCAT3(RELOCS_BYTEORDER, RELOCS_INC_BITS, _to_cpu)
#define cpu_to_target32 CONCAT3(cpu_to_, RELOCS_BYTEORDER, 32)
#define cpu_to_target64 CONCAT3(cpu_to_, RELOCS_BYTEORDER, 64)

static void *get_offset_bonn(void *data, Elf_Phdr *phdrs, size_t phnum, Elf_Addr addr)
{
	Elf_Phdr *p;

	for (p = phdrs; p < phdrs + phnum; ++p)
		if (targetnn_to_cpu(p->p_vaddr) <= addr && targetnn_to_cpu(p->p_vaddr) + targetnn_to_cpu(p->p_memsz) > addr)
			return data + targetnn_to_cpu(p->p_offset) + (addr - targetnn_to_cpu(p->p_vaddr));

	return NULL;
}

static void cheri_relocs_bonn(void *data)
{
	Elf_Ehdr *ehdr = data;
	Elf_Phdr *p;
	Elf_Dyn *dyn;
	Elf_cap_reloc *c;

	if (target16_to_cpu(ehdr->e_machine) != EM_RISCV)
		die("Machine type is not RISC-V");

	Elf_Phdr *phdrs = data + targetnn_to_cpu(ehdr->e_phoff);

	Elf_Dyn *dyns = NULL;
	for (p = phdrs; p < phdrs + target16_to_cpu(ehdr->e_phnum); ++p) {
		if (target32_to_cpu(p->p_type) == PT_DYNAMIC) {
			dyns = data + targetnn_to_cpu(p->p_offset);
			break;
		}
	}

	if (dyns == NULL)
		die("No dynamic section found");

	Elf_cap_reloc *cap_reloc = NULL;
	size_t cap_reloc_sz = 0;
	for (dyn = dyns; ; ++dyn) {
		if (targetnn_to_cpu(dyn->d_tag) == DT_NULL)
			break;
		else if (targetnn_to_cpu(dyn->d_tag) == DT_RISCV_CHERI___CAPRELOCS)
			cap_reloc = get_offset_bonn(data, phdrs, target16_to_cpu(ehdr->e_phnum), + targetnn_to_cpu(dyn->d_un.d_ptr));
		else if (targetnn_to_cpu(dyn->d_tag) == DT_RISCV_CHERI___CAPRELOCSSZ)
			cap_reloc_sz = targetnn_to_cpu(dyn->d_un.d_val) / sizeof(Elf_cap_reloc);
	}

	if (cap_reloc == NULL)
		die("No __cap_relocs found");

	for (c = cap_reloc; c < cap_reloc + cap_reloc_sz; ++c) {
		cc_cap_t cap = {0};
		cc_addr_t pesbt = 0;
		cc_addr_t base = 0;
		cc_addr_t cursor = 0;
		cc_length_t len = 0;
		cc_length_t top = 0;

		uint16_t ap = 0;
		uint8_t m = 0;
		uint8_t ct = 0;

		if(targetnn_to_cpu(c->cr_base)) {
			/* Set Address */
			cursor = targetnn_to_cpu(c->cr_base) + targetnn_to_cpu(c->cr_offset);

			/* Set Bounds */
			if (targetnn_to_cpu(c->cr_flags) == ELF_CAP_RELOC_CR_FLAG_FUNC ||
				targetnn_to_cpu(c->cr_length) == 0) {
				base = 0;
				top = CC_MAX_TOP;
			} else {
				len = cc_get_representable_length(targetnn_to_cpu(c->cr_length));
				base = targetnn_to_cpu(c->cr_base) & cc_get_alignment_mask(len);
				if (base != targetnn_to_cpu(c->cr_base) &&
						len == targetnn_to_cpu(c->cr_length))
					len += cc_get_required_alignment(len);

				top = base + len;
			}

			/* Set AP, M and CT */
			if (targetnn_to_cpu(c->cr_flags) == ELF_CAP_RELOC_CR_FLAG_FUNC) {
				ct = 1;
				m = 0;
				ap = CAP_AP_C | CAP_AP_R | CAP_AP_X | CAP_AP_ASR | CAP_AP_LM;
			} else if (targetnn_to_cpu(c->cr_flags) == ELF_CAP_RELOC_CR_FLAG_CONST) {
				ct = 0;
				m = 0;
				ap = CAP_AP_C | CAP_AP_R | CAP_AP_LM;
			} else {
				ct = 0;
				m = 0;
				ap = CAP_AP_C | CAP_AP_W | CAP_AP_R | CAP_AP_LM;
			}

			cap = cc_make_max_perms_cap(base, cursor, top);
			cc_update_ap(&cap, ap);
			cc_update_m(&cap, m);
			cc_update_ct(&cap, ct);
			pesbt = cc_compress_mem(&cap);
		}

		void* buf = get_offset_bonn(data, phdrs, target16_to_cpu(ehdr->e_phnum), targetnn_to_cpu(c->cr_location));

		if (buf == NULL)
			continue;

		*((uintnn_t *)buf) = cpu_to_target64(cursor);
		*((uintnn_t *)buf+1) = cpu_to_target64(pesbt);
	}
}

#undef cheri_relocs_bonn
#undef uintnn_t
#undef get_offset_bonn
#undef Elf_Ehdr
#undef Elf_Phdr
#undef Elf_Dyn
#undef Elf_Addr
#undef target16_to_cpu
#undef target32_to_cpu
#undef target64_to_cpu
#undef targetnn_to_cpu
#undef cpu_to_target32
#undef cpu_to_target64

#undef CONCAT_IMPL
#undef CONCAT
#undef CONCAT3
