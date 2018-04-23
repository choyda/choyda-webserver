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
        if (errno == EINTR) { //���¾�������ϵͳ���õ�EINTR�ж�
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
 * ��ȡָ���ֽ�����linuxû�У� �Լ���װ
 * ������ size_t: Ӧ�ö�ȡ���ֽ���
 */
ssize_t cd_readn(int fd, void *vptr, size_t n) {

    size_t  nleft;                  //usigned int ʣ��δ��ȡ���ֽ���
    ssize_t nread;                  //int ʵ�ʶ������ֽ���
    char   *ptr = (char *)vptr;     //ָ�򻺳�����ָ��

    nleft = n;                      //ָ��Ҫ�����ֽ��� ����5000

    while (nleft > 0) { //5000����0, �ڶ���ѭ��nleft=4000
        if ((nread = read(fd, ptr, nleft)) < 0) { //ptr += nread��ʾ�´ζ���ʱ����+���λ�÷�
            if (errno == EINTR) {   //�����߼�
                nread = 0;          //EINTR�����ض�����ʾһ���Լ�û����
            }else {
                return -1;
            }
        }else if(nread == 0){       //��������ˣ��ͷ���
            break;
        }
        nleft -= nread;             //1�������EINTR��0���൱��û���������¶�
                                    //2���������1000����5000-1000����ʣ4000û��
        ptr += nread;               //����ָ��1000��λ�ã� �´δ����λ�ü����洢�ֽ�
    }
    return n - nleft;               //1��һ�ζ���ֱ�Ӿ���5000 -5000 ����0���ڵ��÷���ʾ���ꡣ
                                    //2,���5000��һ��1000�����2�Σ�һ����800��һ����600�����4000
                                    //��������vpte��С��5000�����Զ���ȡ����ġ�
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
