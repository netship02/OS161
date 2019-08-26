/*
 * files_table.c
 * Definitions for files_table functions
 *
 */

#include <types.h>
#include <lib.h>
#include <kern/errno.h>
#include <kern/unistd.h>
#include <spinlock.h>
#include <vfs.h>
#include <file_handle.h>
#include <file_table.h>
#include <current.h>


int
file_table_create (struct file_table **ft, char name) {
	struct file_table *this_ft = kmalloc(sizeof(*this_ft));

	if (this_ft == NULL) {
		return -1;
	}
	this_ft->name = name;

	for (int i = 3; i < 60; ++i) {
		this_ft->fd_array[i] = NULL;
	}

	spinlock_init(&this_ft->ft_lock);


	*ft = this_ft;
	return 0;
}



void 
file_table_destroy (struct file_table *ft) {
	KASSERT(ft != NULL);
	spinlock_cleanup(&ft->ft_lock);
	kfree(ft);
}


int  
file_table_get_fd (struct file_table *ft, int *fd) {
	KASSERT(ft != NULL);

	for (int i = 3; i < 60; ++i) {
		if (ft->fd_array[i] == NULL) {
			*fd = i;
			return 0;
		}
	}
	*fd = -1;
	return 1;
}

int  file_table_init_std(struct file_table *ft) {
	struct file_table *this_ft;
	int result;

	

	struct vnode *this_file_in;
	struct vnode *this_file_out;
	struct vnode *this_file_err;

	struct file_handle *this_fh_in;
	struct file_handle *this_fh_out;
	struct file_handle *this_fh_err;

	this_ft = ft;

	char file_in[] = {'c', 'o', 'n', ':', '\0'};
	result = vfs_open(file_in, O_RDONLY, 0644, &this_file_in); /*StdIN */
	//kprintf("%d", result);
	KASSERT(result == 0);
	if (result) {
		return result;
	}

	char file_out[] = {'c', 'o', 'n', ':', '\0'};
	result = vfs_open(file_out, O_WRONLY, 0644, &this_file_out); /*StdIN */
	//kprintf("%d", result);
	KASSERT(result == 0);
	if (result) {
		return result;
	}

	char file_err[] = {'c', 'o', 'n', ':', '\0'};
	result = vfs_open(file_err, O_WRONLY, 0644, &this_file_err); /*StdIN */
	//kprintf("%d", result);
	KASSERT(result == 0);
	if (result) {
		return result;
	}

	result = file_handle_create(&this_fh_in, this_file_in, O_RDONLY, 0);
	//kprintf("%d", result);
	KASSERT(result == 0);
	if (result) {
		return result;
	}
	result = file_handle_create(&this_fh_out, this_file_out, O_WRONLY, 0);
	//kprintf("%d", result);
	KASSERT(result == 0);
	if (result) {
		return result;
	}
	result = file_handle_create(&this_fh_err, this_file_err, O_WRONLY, 0);
	//kprintf("%d", result);
	KASSERT(result == 0);
	if (result) {
		return result;
	}


	/* Finally Setting curproc ft to their file_habdle */
	this_ft->fd_array[STDIN_FILENO] = this_fh_in;
	this_ft->fd_array[STDOUT_FILENO] = this_fh_out;
	this_ft->fd_array[STDERR_FILENO] = this_fh_err;
	return 0;
}