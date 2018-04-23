#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "cd_std_wrap.h"

ssize_t cd_read(int fd, void *ptr, size_t nbytes)
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

ssize_t cd_write(int fd, const void *ptr, size_t nbytes)
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

int cd_close(int fd)
{
    int n;
    if ((n = close(fd)) == -1) {
        cd_perr_exit("cd_close error");
    }

    return n;
}

/*
 * 读取指定字节数，linux没有， 自己封装
 * 参数三 size_t: 应该读取的字节数
 */
ssize_t cd_readn(int fd, void *vptr, size_t n) {

    size_t  nleft;                  //usigned int 剩余未读取的字节数
    ssize_t nread;                  //int 实际读到的字节数
    char   *ptr = (char *)vptr;     //指向缓冲区的指针

    nleft = n;                      //指定要读的字节数 比如5000

    while (nleft > 0) { //5000大于0, 第二次循环nleft=4000
        if ((nread = read(fd, ptr, nleft)) < 0) { //ptr += nread表示下次读的时候按着+完的位置放
            if (errno == EINTR) {   //出错逻辑
                nread = 0;          //EINTR错误，重读，表示一个自己没读到
            }else {
                return -1;
            }
        }else if(nread == 0){       //如果读完了，就返回
            break;
        }
        nleft -= nread;             //1，如果是EINTR的0，相当于没读到，重新读
                                    //2，比如读到1000，用5000-1000，还剩4000没读
        ptr += nread;               //后移指针1000的位置， 下次从这个位置继续存储字节
    }
    return n - nleft;               //1，一次读完直接就是5000 -5000 返回0，在调用方表示读完。
                                    //2,如果5000，一次1000，最后2次，一次是800，一次是600，多出4000
                                    //传进来的vpte大小是5000，会自动截取后面的。
}

ssize_t cd_writen(int fd, const void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;

    ptr = vptr;
    nleft = n;
    while (nleft > 0) {
        if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
            if (nwritten < 0 && errno == EINTR)
                nwritten = 0;
            else
                return -1;
        }

        nleft -= nwritten;
        ptr += nwritten;
    }
    return n;
}

static ssize_t cd_my_read(int fd, char *ptr)
{
    static int read_cnt;
    static char *read_ptr;
    static char read_buf[100];

    if (read_cnt <= 0) {
        again:
        if ( (read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
            if (errno == EINTR)
                goto again;
            return -1;
        } else if (read_cnt == 0)
            return 0;
        read_ptr = read_buf;
    }
    read_cnt--;
    *ptr = *read_ptr++;

    return 1;
}

ssize_t cd_read_line(int fd, void *vptr, size_t maxlen)
{
    ssize_t n, rc;
    char    c, *ptr;

    ptr = vptr;
    for (n = 1; n < maxlen; n++) {
        if ( (rc = cd_my_read(fd, &c)) == 1) {
            *ptr++ = c;
            if (c  == '\n')
                break;
        } else if (rc == 0) {
            *ptr = 0;
            return n - 1;
        } else
            return -1;
    }
    *ptr  = 0;

    return n;
}
