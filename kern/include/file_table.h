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
};

int  file_table_create (struct file_table **ft);
void file_table_destroy (struct file_table *ft);
int  file_table_get_fd (struct file_table *ft, int *fd);


#endif