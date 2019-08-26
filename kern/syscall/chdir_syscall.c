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
sys_chdir(const char *pathname, int32_t *result) {
	(void)*pathname;
	(void)*result;
	return 0;
}