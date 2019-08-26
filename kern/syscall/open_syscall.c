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
	len = strlen(filename) + 1;     /* 1 extra byte for null character  */
	struct file_handle *this_fh;
	struct file_table *this_ft;

	this_ft = curproc->ft;
	KASSERT(this_ft != NULL);

	this_filename = kmalloc(len * sizeof(char));
	if (this_filename == NULL) {
		return -1;
	}

	result = copyinstr((const_userptr_t)filename, this_filename, len, NULL);
	if (result){
		kprintf("copyinstr on open_syscall is failed   %d \n", result);
		return result;
	}

	result = vfs_open(this_filename, flags, 0644, &file);
	if (result) {
		kprintf("vfs_open on open_syscall is failed");
		return result;
	}


	result = file_handle_create(&this_fh, file, flags, 0);
	if (result) {
		kprintf("file_handle_create on open_syscall is failed");
		return result;
	}

	result = file_table_get_fd(this_ft, fd);
	if (result) {
		kprintf("file_table_get_fd on open_syscall is failed");
		return result;
	}
	this_fd = *fd;
	this_ft->fd_array[this_fd] = this_fh;    /* Set curproc one fd to point to created fh */
	
	return 0;
}



