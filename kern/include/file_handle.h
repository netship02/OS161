#ifndef _FILE_HANDLE_H_
#define _FILE_HANDLE_H_

/*
 * Definition of a File Handle.
 *
 */

#include <kern/fcntl.h>
#include <kern/unistd.h>

#include <spinlock.h>
#include <synch.h>

#include <vnode.h>


struct file_handle {
	struct vnode *file; 
	int fh_flags;                    	
    off_t fh_pos; 
    volatile unsigned int refcount;     	
    struct spinlock fh_lock;            	                  	
    struct lock *file_lock;          	/* Decide which synch primitive to use */   	
};


int  file_handle_create(struct file_handle **fh, struct vnode *file, int fh_flags, off_t fh_pos);
void file_handle_destroy(struct file_handle *);
bool file_handle_permission(struct file_handle *fh, int flag);

#endif