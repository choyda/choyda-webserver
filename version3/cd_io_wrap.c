#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "cd_std_wrap.h"

typedef struct {
    int rio_fd;                //与内部缓冲区关联的描述符
    int rio_cnt;               //缓冲区中剩下的字节数
    char *rio_bufptr;          //指向缓冲区中下一个未读的字节
    char rio_buf[RIO_BUFSIZE];
} rio_t;


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

/*
 * typedef struct {
    int rio_fd;                //与内部缓冲区关联的描述符
    int rio_cnt;               //缓冲区中剩下的字节数
    char *rio_bufptr;          //指向缓冲区中下一个未读的字节
    char rio_buf[RIO_BUFSIZE];
} rio_t;
 *
 */

void rio_readinitb(rio_t *rp, int fd)
{
    rp->rio_fd = fd;
    rp->rio_cnt = 0;
    rp->rio_bufptr = rp->rio_buf; // rio_buf 4096大，内存块的首地址， rio_bufptr下一个没读的地址， 都初始化为首地址，也就是从第一个读
}

static ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n) {

    int cnt;

    while (rp->rio_cnt <= 0) {  /*如果为0 表示缓冲区中没有字节*/

        /*一次性从tcp缓冲区中读4096个字节，文件块的大小。效率更高。有可能返回的字节数少于要读的字节数*/
        rp->rio_cnt = read(rp->rio_fd, rp->rio_buf, sizeof(rp->rio_buf));

        if (rp->rio_cnt < 0) { //如果返回-1表示出错
            if (errno != EINTR) { //如果是EINTR 表示信号终端，重新while，软启动read函数
                return -1;  //如果不是EINTR 表示read系统本身出错返回-1
            }
        } else if (rp->rio_cnt == 0) {  /*EOF 如果是0，在网络终端下，表示客户端关闭了，直接返回0，如果没有数据就会阻塞*/
            return 0;
        } else {
            rp->rio_bufptr = rp->rio_buf; /* 如果都没有问题，把rio_buf的首地址，赋值给第一个要读的字符地址 */
        }
    }

    /* Copy min(n, rp->rio_cnt) bytes from internal buf to user buf */
    cnt = n;
    if (rp->rio_cnt < n) {
        cnt = rp->rio_cnt; //如果实际读的字节数量小于要读的数量，赋值给cnt，便于内存cpy用
    }
    memcpy(usrbuf, rp->rio_bufptr, cnt); // 从第一个字节的地址cpy cnt个字符到usrbuf中。
    rp->rio_bufptr += cnt; //指针后移，如果所有数据都读取完毕，重新while读取4096字节到结构体的那个首地址，这个地址也会初始化为首地址
    rp->rio_cnt -= cnt;
    return cnt;
}