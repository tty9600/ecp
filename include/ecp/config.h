/* @file: config.h
 * #desc:
 *    The definitions of ECP config.
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

#ifndef _ECP_CONFIG_H
#define _ECP_CONFIG_H

#ifndef ECP_MARCH_BITS
# ifdef __SIZEOF_POINTER__
#  if (__SIZEOF_POINTER__ == 4)
#   define ECP_MARCH_BITS_32
#  elif (__SIZEOF_POINTER__ == 8)
#   define ECP_MARCH_BITS_64
#  else
#   error "!!!unknown __SIZEOF_POINTER__!!!"
#  endif
# else
#  error "!!!undefined __SIZEOF_POINTER__!!!"
# endif
#endif

#ifndef ECP_MARCH_TYPE
# if defined(__i386__)
#  define ECP_MARCH_X86_32
# elif defined(__x86_64__)
#  define ECP_MARCH_X86_64
# elif (defined(__arm__) || defined(__thumb__) || defined(__ARM_EABI__))
#  if (defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || __ARM_ARCH >= 7)
#   define ECP_MARCH_ARM_32
#  else
#   define ECP_MARCH_ARM_32
#   warning "!!!not is armv7-a!!!"
#  endif
# elif defined(__aarch64__)
#  define ECP_MARCH_ARM_64
# elif (defined(__riscv) || defined(__riscv_xlen))
#  if (__riscv_xlen == 32)
#   define ECP_MARCH_RISCV_32
#  else
#   define ECP_MARCH_RISCV_64
#  endif
# else
#  error "!!!unknown architecture!!!"
# endif
#endif

/* default linux platform */
#ifndef ECP_PLATFORM_TYPE
# define ECP_PLATFORM_LINUX
#endif

#if 0
# define ECP_STDDEF_TYPEDEF
# define ECP_STDINT_TYPEDEF
#endif

#endif
