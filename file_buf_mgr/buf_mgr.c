//
//  buf_mgr.c
//
//  Created by xichen on 2013-1-8.
//  Copyright (c) 2013 ccteam. All rights reserved.
//

#include "buf_mgr.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

// the internal function which read more data from the buffer manager,
// for satisfying the needed data.
// if buffer manager's buf has the data needed, then it will give it;
// if not, buffer manager will load more data from the file
// buf:         the user's buf pointer
// size:        the remaining data's size that is needed
// true_size:   the pointer that points to the current effective size that buf contains
static char * buf_mgr_read_loop(BUF_MGR *buf_mgr, char *buf, long size, long *true_size);

BUF_MGR *create_buf_mgr(const char *file_path)
{
	BUF_MGR *buf_mgr;
	int filefd;
    char *buf;
    long read_size;
    
    buf_mgr = (BUF_MGR *)malloc(sizeof(BUF_MGR));
	if(!buf_mgr)
		return NULL;

	filefd = open(file_path, O_RDONLY);
	if(filefd < 0)
		goto error_open_file;	
    lseek(filefd, 0, SEEK_SET); // seek to the beginning
    
    buf = (char *)malloc(DEFAULT_PAGE_SIZE);
    if(!buf)
        goto error_malloc_buf;
    
    // init the buffer manager's data
    buf_mgr->file_fd = filefd;
    buf_mgr->seek_begin = 0;
    buf_mgr->buf = buf;
    buf_mgr->size = DEFAULT_PAGE_SIZE;
    
    // read DEFAULT_PAGE_SIZE data
    read_size = read(filefd, buf, DEFAULT_PAGE_SIZE);       
    if(read_size < 0)
        goto error_read_file;
    buf_mgr->true_data_size = read_size;    // set the true data
    
	return buf_mgr;
    
error_read_file:
    free(buf);
error_malloc_buf:
    close(filefd);
error_open_file:
    free(buf_mgr);
    
	return NULL;
}

char *	buf_mgr_get_data(BUF_MGR *buf_mgr, int offset, int size, char *buf, long *true_size)
{
    long ret;
    char *buf_backup = buf;
    if(offset < 0 || size <= 0)
        return NULL;
    
    // if the needed offset is more than buf mgr's seek begin
    if(offset >= buf_mgr->seek_begin)   
    {
        // the buf mgr has the data which is needed, just copy it
        if(size + offset <= buf_mgr->true_data_size + buf_mgr->seek_begin) 
        {
            long offset1 = offset - buf_mgr->seek_begin;
            memcpy(buf, buf_mgr->buf + offset1, size);
            if(true_size)
                *true_size = size;
            return buf_backup;
        }
        else
        {       // the needed data is more than the buf mgr's buffer
            long fill_size = 0;
            long offset1 = offset - buf_mgr->seek_begin;
            if(buf_mgr->true_data_size - offset1 > 0)   // if possible, copy something
            {
                memcpy(buf, buf_mgr->buf + offset1, buf_mgr->true_data_size - offset1); // 先拷贝一批数据
                fill_size = buf_mgr->true_data_size - offset1;
                *true_size = fill_size;
            }
            // seek to the next needed data
            ret = lseek(buf_mgr->file_fd, offset + fill_size, SEEK_SET);
            if(ret == -1)
                return NULL;
            buf_mgr->seek_begin = offset + fill_size;   // set the seek_begin value
            
            // read more data from file, then return the final buffer
            return buf_mgr_read_loop(buf_mgr, buf, size - fill_size, true_size);
        }
    }
    else    // the needed offset is smaller than the buf mgr' seek_pos
    {
        // seek to the new position
        ret = lseek(buf_mgr->file_fd, offset, SEEK_SET);
        if(ret == -1)
            return NULL;
        buf_mgr->seek_begin = offset;   // reset the seek_begin value
        *true_size = 0;

        // read more data from file, then return the final buffer
        return buf_mgr_read_loop(buf_mgr, buf_backup, size, true_size);
    }
    return NULL;
    
}


static char * buf_mgr_read_loop(BUF_MGR *buf_mgr, char *buf, long size, long *true_size)
{
    long fill_size = 0;
    long ret;
    char *buf_backup = buf;
    buf += (*true_size);
    
    while(fill_size < size)
    {
        ret = read(buf_mgr->file_fd, buf_mgr->buf, buf_mgr->size);
        if(ret < 0)
            return NULL;
        else if(ret == 0)
        {
            if(fill_size == 0 && *true_size == 0)
                return NULL;
            
            return buf_backup;
        }
        else
        {
            buf_mgr->seek_begin += buf_mgr->true_data_size;
            buf_mgr->true_data_size = ret;
            
            long true_copy_size = ret;
            if(ret > size - fill_size)
            {
                true_copy_size = size - fill_size;
            }
            memcpy(buf, buf_mgr->buf, true_copy_size);
            buf += true_copy_size;
            fill_size += true_copy_size;
            *true_size += true_copy_size;
        }
    }
    
    if(fill_size == 0 && *true_size == 0)
        return NULL;
    
    return buf_backup;
}


void		close_buf_mgr(BUF_MGR *buf_mgr)
{
    free(buf_mgr->buf);
    free(buf_mgr);
}




