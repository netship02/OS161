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
#include <file_handle.h>
#include <file_table.h>


int
file_table_create (struct file_table **ft) {
	struct file_table *this_ft = NULL;
	this_ft = kmalloc(sizeof(ft));
	if (this_ft == NULL) {
		return 1;
	}

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