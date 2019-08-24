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

int  file_handle_create(struct file_handle *fh, struct vnode *file, int fh_flags, off_t fh_pos) {
	(void)fh;
	(void)file;
	(void)fh_flags;
	(void)fh_pos;
	return 0;
} 


void file_handle_destroy(struct file_handle *fh) {
	(void)fh;
}


bool file_handle_permission(struct file_handle *fh, int flag) {
	(void)fh;
	(void)flag;
	return true;
}
