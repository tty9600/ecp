/* @file: fcntl.h
 * #desc:
 *    The definitions of file control options.
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

#ifndef _ECP_C_FCNTL_H
#define _ECP_C_FCNTL_H

#include <ecp/config.h>
#include <ecp/c/stdint.h>
#include <ecp/c/sys_types.h>

#if defined(ECP_PLATFORM_LINUX)

#define X_F_DUPFD  0 /* duplicate file descriptor */
#define X_F_GETFD  1 /* get file descriptor flags */
#define X_F_SETFD  2 /* set file descriptor flags */
#define X_F_GETFL  3 /* get file status flags and file access modes */
#define X_F_SETFL  4 /* set file status flags */

#if defined(ECP_MARCH_BITS_32)
# define X_F_GETLK  12
# define X_F_SETLK  13
# define X_F_SETLKW 14
#elif defined(ECP_MARCH_BITS_64)
# define X_F_GETLK  5 /* get record locking info */
# define X_F_SETLK  6 /* get record locking info */
# define X_F_SETLKW 7 /* set record locking info(wait if blocked) */
#else
# error "!!!unknown machine bits!!!"
#endif

/* set process or process group id to receive SIGIO/SIGURG signal */
#define X_F_SETOWN 8
/* get process or process group id to receive SIGIO/SIGURG signal */
#define X_F_GETOWN 9

/* duplicate file descriptor with the close-on-exec flag */
#define X_F_DUPFD_CLOEXEC (1024 + 6)

/* close file descriptors when executing the exec */
#define X_FD_CLOEXEC 1

struct xflock {
	int16_t l_type;   /* type of lock F_*LCK */
	int16_t l_whence; /* offset type (LSEEK_*) */
	int64_t l_start;  /* relative offset */
	int64_t l_len;    /* size (if 0 then until EOF) */
	int32_t l_pid;    /* process id of the process holding the lock */
};

#define X_F_UNLCK 0 /* unlock */
#define X_F_RDLCK 1 /* read lock */
#define X_F_WRLCK 2 /* write lock */

#define X_O_RDONLY   0x00 /* read only */
#define X_O_WRONLY   0x01 /* write only */
#define X_O_RDWR     0x02 /* read and write */
#define X_O_ACCMODE  0x03 /* mask of read and write */

#if (defined(ECP_MARCH_ARM_32) || defined(ECP_MARCH_ARM_64))
# define X_O_SEARCH    0x4000
# define X_O_NOFOLLOW  0x8000
# define X_O_DIRECT    0x10000
# define X_O_LARGEFILE 0x20000
#endif

#define X_O_CREAT    0x40 /* create file if it does not exist */
#define X_O_EXCL     0x80 /* exclusive flag */

/* do not assign controlling terminal (SIGTTOU/SIGTTIN signal) */
#define X_O_NOCTTY   0x100

#define X_O_TRUNC    0x200 /* truncate flag */
#define X_O_APPEND   0x400 /* set append mode */

#define X_O_NONBLOCK 0x800  /* non-blocking mode */
#define X_O_DSYNC    0x1000 /* synchronized writing */

/* write according to synchronized I/O data integrity */
#define X_O_SYNC     (X_O_DSYNC | 0x100000)

/* direct disk access hint */
#ifndef X_O_DIRECT
# define X_O_DIRECT    0x4000
#endif
/* large file support (>4GiB) */
#ifndef X_O_LARGEFILE
# define X_O_LARGEFILE 0x8000
#endif
/* open directory for search only (*at() functions) */
#ifndef X_O_SEARCH
# define X_O_SEARCH    0x10000
#endif
/* do not follow symbolic links */
#ifndef X_O_NOFOLLOW
# define X_O_NOFOLLOW  0x20000
#endif

/* close file descriptors when executing the exec */
#define X_O_CLOEXEC  0x80000

/* current working directory */
#define X_AT_FDCWD            (-100)
/* do not follow symbolic links */
#define X_AT_SYMLINK_NOFOLLOW 0x100
/* check access using effective user and group id */
#define X_AT_EACCESS          0x200
/* remove directory instead of unlinking file */
#define X_AT_REMOVEDIR        0x200
/* follow symbolic link */
#define X_AT_SYMLINK_FOLLOW   0x400

#else
# error "!!!unknown platform!!!"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* c_fcntl.c */
extern
int32_t ecp_fcntl(int32_t fd, int32_t cmd, ...)
;
extern
int32_t ecp_openat(int32_t fd, const char *path, int32_t flags, ...)
;
extern
int32_t ecp_open(const char *path, int32_t flags, ...)
;
extern
int32_t ecp_creat(const char *path, uint32_t mode)
;

#ifdef __cplusplus
}
#endif

#endif
