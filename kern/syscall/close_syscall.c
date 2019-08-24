/*
 * Definition for open syscall
 *
 */
#include <types.h>
#include <lib.h>
#include <kern/errno.h>
#include <kern/unistd.h>

#include <current.h>
#include <copyinout.h>


#include <syscall.h>
#include <file_handle.h>
#include <file_table.h>

#include <vfs.h>
#include <vnode.h>





int
sys_close(int fd, int32_t *result) {
	(void)fd;
	(void)result;
	return 0;
}