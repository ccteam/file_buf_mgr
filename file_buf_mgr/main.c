//
//  main.c
//  file_buf_mgr
//
//  Created by xichen on 13-1-8.
//  Copyright (c) 2013 ccteam. All rights reserved.
//

#include <stdio.h>
#include "buf_mgr.h"

int main (int argc, const char * argv[])
{
    BUF_MGR *mgr = create_buf_mgr("test");
    char *ret;
    char buf[128] = {0};
    long true_size;
    
    if(!mgr)
    {
        perror("create_buf_mgr failed");
        return -1;
    }
    
    memset(buf, 0, sizeof(buf));
    ret = buf_mgr_get_data(mgr, 0, 5, buf, &true_size);
    if(ret)
    {
        buf[true_size] = '\0';
        printf("0-5|%s|\n", buf);
    }
    
    memset(buf, 0, sizeof(buf));
    ret = buf_mgr_get_data(mgr, 0, 1, buf, &true_size);
    if(ret)
    {
        buf[true_size] = '\0';
        printf("0-1|%s|\n", buf);
    }
    
    memset(buf, 0, sizeof(buf));
    ret = buf_mgr_get_data(mgr, 0, 200, buf, &true_size);
    if(ret)
    {
        buf[true_size] = '\0';
        printf("0-200|%s|\n", buf);
    }
    
    
    
    memset(buf, 0, sizeof(buf));
    ret = buf_mgr_get_data(mgr, 3, 10, buf, &true_size);
    if(ret)
    {
        buf[true_size] = '\0';
        printf("3-10|%s|\n", buf);
    }
    
    close_buf_mgr(mgr);
    
    return 0;
}

