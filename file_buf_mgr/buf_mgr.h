//
//  buf_mgr.h
//
//  Created by xichen on 2013-1-8.
//  Copyright (c) 2013 ccteam. All rights reserved.
//

#ifndef BUF_MGR_H
#define BUF_MGR_H

// default buffer size
#define DEFAULT_PAGE_SIZE   (4 * 1024)
// this is a test size
//#define DEFAULT_PAGE_SIZE   (4)

// buffer manager which contains file's descriptor, seek info and buf pointer, size
// and so on
typedef struct
{
	int		file_fd;        // file descriptor
	long	seek_begin;     // seek beginning pos, not the current seek state, but the previous seek pos
    long    true_data_size; // the true data size that the buffer contains
    
	char	*buf;           // buf pointer
	long	size;           // buf size
}BUF_MGR;

// create a buffer manager
// file_path: like "/test"
BUF_MGR *create_buf_mgr(const char *file_path);

// get data from the buffer manager
// offset:  the offset of the file's data
// size:    the data size which you want to get
// buf:     the user buffer that contains the data(you must make sure the buf's size is too big to store the needed file's data)
// true_size: the true data size that returned

// return value: the pointer that points to the needed data; if NULL, it means error occurs, then don't use the buf's data 
char *	buf_mgr_get_data(BUF_MGR *buf_mgr, int offset, int size, char *buf, long *true_size);

// close the buffer manager
void	close_buf_mgr(BUF_MGR *buf_mgr);


#endif

