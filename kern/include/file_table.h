#ifndef _FILES_TABLE_H_
#define _FILES_TABLE_H_

/*
 * Definition of a Files Table.
 *
 */

#include <spinlock.h>
#include <file_handle.h>

struct file_table {
    struct spinlock ft_lock;                      
    struct file_handle *fd_array[60];
    char name;  
};

// return 0 on success and -1 or other values on failing


int  file_table_create (struct file_table **ft, char name);
void file_table_destroy (struct file_table *ft);
int  file_table_get_fd (struct file_table *ft, int *fd);
int  file_table_init_std(struct file_table *ft);

#endif