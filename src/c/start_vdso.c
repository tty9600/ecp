/* @file: start_vdso.c
 * #desc:
 *    The implementations of overview of the virtual ELF dynamic shared object.
 *
 * #copy:
 *    Copyright (C) 2026 - Alex Chen <chenalex.cy@gmail.com>
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 2.1 of the License, or (at your option) any later version.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not,
 *    see <https://www.gnu.org/licenses/>.
 */

#include <ecp/config.h>
#include <ecp/c/stddef.h>
#include <ecp/c/stdint.h>
#include <ecp/c/string.h>
#include <ecp/c/start.h>
#include <ecp/elf.h>

/* @func: ecp_vdso_sym
 * #desc:
 *    get the vdso symbol address.
 *
 * #1: sym [in]  symbol name
 * #r:     [ret] symbol address
 */
void *ecp_vdso_sym(const char *sym)
{
#if defined(ECP_MARCH_BITS_32)
# define __elf_auxv elf32_auxv
# define __elf_ehdr elf32_ehdr
# define __elf_shdr elf32_shdr
# define __elf_sym elf32_sym
#elif defined(ECP_MARCH_BITS_64)
# define __elf_auxv elf64_auxv
# define __elf_ehdr elf64_ehdr
# define __elf_shdr elf64_shdr
# define __elf_sym elf64_sym
#else
# error "!!!unknown machine bits!!!"
#endif

	if (!__ecp_auxp)
		return NULL;

	struct __elf_auxv *aux = (struct __elf_auxv *)__ecp_auxp;
	char *p;

	for (; aux->a_type != AT_SYSINFO_EHDR; aux++) {
		if (aux->a_type == AT_NULL)
			return NULL;
	}
	p = (char *)aux->a_un.a_val;

	struct __elf_ehdr *ehdr = (struct __elf_ehdr *)p;
	struct __elf_shdr *shdr = (struct __elf_shdr *)(p + ehdr->e_shoff);
	struct __elf_shdr *shdr_str = shdr + ehdr->e_shstrndx;
	char *shstr = p + shdr_str->sh_offset;

	struct __elf_sym *dynsym = NULL;
	size_t dynsym_size, dynsym_entsize;
	char *dynstr = NULL;

	for (uint16_t i = 0; i < ehdr->e_shnum; i++) {
		if (!ecp_strcmp(shstr + shdr->sh_name, ".dynsym")) {
			dynsym = (struct __elf_sym *)(p + shdr->sh_offset);
			dynsym_size = shdr->sh_size;
			dynsym_entsize = shdr->sh_entsize;
		} else if (!ecp_strcmp(shstr + shdr->sh_name, ".dynstr")) {
			dynstr = p + shdr->sh_offset;
		}
		shdr++;
	}
	if (!(dynsym && dynstr))
		return NULL;

	for (size_t i = 0; i < dynsym_size; i += dynsym_entsize) {
		if (!ecp_strcmp(dynstr + dynsym->st_name, sym))
			return (void *)(p + dynsym->st_value);
		dynsym++;
	}

	return NULL;
}
