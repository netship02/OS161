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
file_table_create (struct file_table *ft) {
	(void)ft;
	return 0;
}



void 
file_table_destroy (struct file_table *ft) {
	(void)ft;
}


int  
file_table_get_fd (struct file_table *ft, int *fd) {
	(void)ft;
	(void)fd;
	return 0;
}