/*
 * Definition for read syscall
 *
 */

#include <types.h>
#include <lib.h>
#include <kern/errno.h>
#include <kern/unistd.h>

#include <current.h>
#include <copyinout.h>

#include <kern/iovec.h>
#include <uio.h>


#include <syscall.h>
#include <file_handle.h>
#include <file_table.h>

#include <vfs.h>
#include <vnode.h>



int
sys_read(int fd, void *buf, size_t buflen, int32_t *read) {
	(void)fd;
	(void)buf;
	(void)buflen;
	(void)read;
	return 0;
}