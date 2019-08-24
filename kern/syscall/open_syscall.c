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
#include <proc.h>

#include <vfs.h>
#include <vnode.h>

int
sys_open(const char *filename, int flags, int32_t *fd) {
	
	int result, len;	
	int this_fd;
	struct vnode *file;
	char *this_filename;
	len = sizeof(filename) + 1;
	struct file_handle *this_fh;
	struct file_table *this_ft;

	this_ft = curproc->ft;

	this_filename = kmalloc(sizeof(len));
	if (this_filename == NULL) {
		return -1;
	}

	result = copyinstr((const_userptr_t)filename, this_filename, len, NULL);
	if (result != 0){
		return EFAULT;
	}


	result = vfs_open(this_filename, flags, 0644, &file);
	if (result != 0) {
		return -1;
	}


	result = file_handle_create(&this_fh, file, flags, 0);
	if (result != 0) {
		return -1;
	}

	result = file_table_get_fd(this_ft, fd);
	if (result != 0) {
		return -1;
	}
	this_fd = *fd;

	this_ft->fd_array[this_fd] = this_fh;    /* Set curproc one fd to point to created fh */

	return 0;
}



