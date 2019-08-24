/*
 * files_table.c
 * Definitions for files_table functions
 *
 */

#include <types.h>
#include <lib.h>
#include <kern/errno.h>
#include <kern/unistd.h>
#include <kern/fcntl.h>
#include <spinlock.h>
#include <synch.h>
#include <vfs.h>
#include <vnode.h>
#include <file_table.h>
#include <file_handle.h>

int  file_handle_create(struct file_handle **fh, struct vnode *file, int fh_flags, off_t fh_pos) {

	KASSERT(file != NULL);

	struct file_handle *this_fh;
	this_fh = kmalloc(sizeof(this_fh));
	if (this_fh == NULL) {
		return -1;
	}

	spinlock_init(&this_fh->fh_lock);
	this_fh->file_lock = lock_create("file_handle_lock");

	this_fh->file = file;
	this_fh->fh_flags = fh_flags;
	this_fh->fh_pos   = fh_pos;
	this_fh->refcount = 1;


	*fh = this_fh;
	return 0;
} 


void file_handle_destroy(struct file_handle *fh) {
	KASSERT(fh != NULL);
	lock_destroy(fh->file_lock);
	spinlock_cleanup(&fh->fh_lock);
	kfree(fh);
}


bool file_handle_permission(struct file_handle *fh, int flag) {

	KASSERT(fh != NULL);

	if (fh->fh_flags == O_RDWR) {
		return true;
	}else return fh->fh_flags == flag;
	return false;
}