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
#include <proc.h>
#include <file_handle.h>
#include <file_table.h>

#include <vfs.h>
#include <vnode.h>





int
sys_close(int fd, int32_t *result) {
	struct file_table *ft;

	if (!(fd > 2 && fd < 60)) {
		return EBADF;
	}

	ft = curproc->ft;
	file_handle_destroy(ft->fd_array[fd]);
	ft->fd_array[fd] = NULL;
	*result = 0;
	return 0;
}