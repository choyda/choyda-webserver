#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "cd_std_wrap.h"

ssize_t Read(int fd, void *ptr, size_t nbytes)
{
    ssize_t n;
again:
    if ( (n = read(fd, ptr, nbytes)) == -1) {
        if (errno == EINTR) { //以下均是慢速系统调用的EINTR判断
            goto again;
        }else{
            cd_perr_exit("cd_read error");
        }
    }
    return n;
}

ssize_t Write(int fd, const void *ptr, size_t nbytes)
{
    ssize_t n;
again:
    if ( (n = write(fd, ptr, nbytes)) == -1) {
        if (errno == EINTR) {
            goto again;
        }else {
            cd_perr_exit("cd_write error");
        }
    }
    return n;
}

int Close(int fd)
{
    int n;
    if ((n = close(fd)) == -1) {
        cd_perr_exit("cd_close error");
    }
    return n;
}

