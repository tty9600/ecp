/* @file: setjmp.h
 * #desc:
 *    The definitions of non-local jump.
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

#ifndef _ECP_C_SETJMP_H
#define _ECP_C_SETJMP_H

#include <ecp/config.h>
#include <ecp/c/stdint.h>

struct jmpbuf {
	unsigned long reg[64];
};

#define JMPBUF_X86_32_EBX 0
#define JMPBUF_X86_32_ESI 1
#define JMPBUF_X86_32_EDI 2
#define JMPBUF_X86_32_EBP 3
#define JMPBUF_X86_32_ESP 4
#define JMPBUF_X86_32_RET 5

#define JMPBUF_X86_64_RBX 0
#define JMPBUF_X86_64_RBP 1
#define JMPBUF_X86_64_R12 2
#define JMPBUF_X86_64_R13 3
#define JMPBUF_X86_64_R14 4
#define JMPBUF_X86_64_R15 5
#define JMPBUF_X86_64_RSP 6
#define JMPBUF_X86_64_RET 7

#define JMPBUF_ARM_32_R4 0
#define JMPBUF_ARM_32_R5 1
#define JMPBUF_ARM_32_R6 2
#define JMPBUF_ARM_32_R7 3
#define JMPBUF_ARM_32_R8 4
#define JMPBUF_ARM_32_R9 5
#define JMPBUF_ARM_32_R10 6
#define JMPBUF_ARM_32_R11 7
#define JMPBUF_ARM_32_SP 8
#define JMPBUF_ARM_32_LR 9

#define JMPBUF_ARM_64_X19 0
#define JMPBUF_ARM_64_X20 1
#define JMPBUF_ARM_64_X21 2
#define JMPBUF_ARM_64_X22 3
#define JMPBUF_ARM_64_X23 4
#define JMPBUF_ARM_64_X24 5
#define JMPBUF_ARM_64_X25 6
#define JMPBUF_ARM_64_X26 7
#define JMPBUF_ARM_64_X27 8
#define JMPBUF_ARM_64_X28 9
#define JMPBUF_ARM_64_X29 10
#define JMPBUF_ARM_64_X30 11 /* ret */
#define JMPBUF_ARM_64_SP 13
#define JMPBUF_ARM_64_D8 14
#define JMPBUF_ARM_64_D9 15
#define JMPBUF_ARM_64_D10 16
#define JMPBUF_ARM_64_D11 17
#define JMPBUF_ARM_64_D12 18
#define JMPBUF_ARM_64_D13 19
#define JMPBUF_ARM_64_D14 20
#define JMPBUF_ARM_64_D15 21

#define JMPBUF_RISCV_32_S0 0
#define JMPBUF_RISCV_32_S1 1
#define JMPBUF_RISCV_32_S2 2
#define JMPBUF_RISCV_32_S3 3
#define JMPBUF_RISCV_32_S4 4
#define JMPBUF_RISCV_32_S5 5
#define JMPBUF_RISCV_32_S6 6
#define JMPBUF_RISCV_32_S7 7
#define JMPBUF_RISCV_32_S8 8
#define JMPBUF_RISCV_32_S9 9
#define JMPBUF_RISCV_32_S10 10
#define JMPBUF_RISCV_32_S11 11
#define JMPBUF_RISCV_32_SP 12
#define JMPBUF_RISCV_32_RA 13

#define JMPBUF_RISCV_64_S0 0
#define JMPBUF_RISCV_64_S1 1
#define JMPBUF_RISCV_64_S2 2
#define JMPBUF_RISCV_64_S3 3
#define JMPBUF_RISCV_64_S4 4
#define JMPBUF_RISCV_64_S5 5
#define JMPBUF_RISCV_64_S6 6
#define JMPBUF_RISCV_64_S7 7
#define JMPBUF_RISCV_64_S8 8
#define JMPBUF_RISCV_64_S9 9
#define JMPBUF_RISCV_64_S10 10
#define JMPBUF_RISCV_64_S11 11
#define JMPBUF_RISCV_64_SP 12
#define JMPBUF_RISCV_64_RA 13

#ifdef __cplusplus
extern "C" {
#endif

/* c/setjmp.S */
extern
int32_t ecp_setjmp(struct jmpbuf *env)
;
extern
void ecp_longjmp(struct jmpbuf *env, int32_t val)
;

#ifdef __cplusplus
}
#endif

#endif
