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

#include <proc.h>
#include <vfs.h>
#include <vnode.h>



int 
sys_write(int fd, const void *buf, size_t buflen, int32_t *wrote) {

	KASSERT(buf != NULL);
	KASSERT(buflen > 0);
	//KASSERT(fd > 3 && fd < 60);

	struct iovec iov;
	struct uio u;
	int len = buflen;

	struct file_table *this_ft;
	struct file_handle *this_fh;
	struct vnode *this_file;

	int result;



	this_ft = curproc->ft;
	//kprintf("%d", this_ft->name);
	//kprintf("%d", fd);

	this_fh = this_ft->fd_array[fd];

	this_file = this_fh->file;

	int offset = this_fh->fh_pos;

	//  Sync Data using file handle locks

	//lock_acquire(this_fh->file_lock);
	iov.iov_ubase = (userptr_t)buf;
	iov.iov_len = len;

	u.uio_iov = &iov;
	u.uio_iovcnt = 1;
	u.uio_offset = offset;
	u.uio_resid = len;
	u.uio_segflg = UIO_USERSPACE;
	u.uio_rw = UIO_WRITE;
	u.uio_space = proc_getas();

	result = VOP_WRITE(this_file, &u);
	if (result) {
		return result;
	}

	*wrote = len - u.uio_resid;
	//lock_release(this_fh->file_lock);
	return 0;
}