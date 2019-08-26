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
sys___getcwd(char *buf, size_t buflen, int32_t *result) {
	(void)*buf;
	(void)buflen;
	(void)*result;
	return 0;
}