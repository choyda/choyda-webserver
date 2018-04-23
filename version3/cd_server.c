#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

//自己定义的函数库头文件
#include "cd_sock_wrap.h"
#include "cd_io_wrap.h"
#include "cd_std_wrap.h"

/**** 多线程 ****/
/*多线程客户端交互，可以同时支持多个客户端连接*/

#define SERV_PORT 9999 //定义服务器端口
#define SIZEBUF 1024 //读取buf大小

struct pth_st_info{                     //定义一个结构体, 将地址结构跟cfd捆绑
    struct sockaddr_in cli_addr;
    int cfd;
};

void *pth_act(void *args)
{
    int n,i;
    struct pth_st_info *pt_st = (struct pth_st_info*)args;
    char buf[SIZEBUF];              //读取缓冲区大小
    char inet_len[INET_ADDRSTRLEN];      //#define INET_ADDRSTRLEN 16

    while (1) {
        n = cd_read(pt_st->cfd, buf, SIZEBUF);                     //读客户端
        if (n == 0) {
            printf("client %d closed...\n", pt_st->cfd);
            break;                                              //跳出循环,关闭cfd
        }
        printf("received from %s at PORT %d\n",
               inet_ntop(AF_INET, &(*pt_st).cli_addr.sin_addr, inet_len, sizeof(inet_len)),
               ntohs((*pt_st).cli_addr.sin_port));                 //打印客户端信息(IP/PORT)

        for (i = 0; i < n; i++)
            buf[i] = toupper(buf[i]);                           //小写-->大写

        cd_write(STDOUT_FILENO, buf, n);                           //写出至屏幕
        cd_write(pt_st->cfd, buf, n);                              //回写给客户端
    }
    cd_close(pt_st->cfd);

    return (void *)0;
}

int main(int argc, char *argv[]){

    /*定义使用中的变量*/
    int sfd, cfd;                               //服务器端和客户端的socket描述符
    int i=0;                                    //i创建线程的个数
    struct sockaddr_in serv_addr, cli_addr;     //客户端和服务器端bind结构体
    char clie_ip[SIZEBUF], serv_ip[SIZEBUF];    //保存打印信息ip的字符数组
    pthread_t tid;                              //线程id
    struct pth_st_info pth_st[256];                      //根据最大线程数创建结构体数组.

    /*创建socket描述符*/
    sfd = cd_socket(AF_INET, SOCK_STREAM, 0);      //最后一个参数0，内核会自动推演出使用的协议

    /*端口复用，TIME_WAIT等待问题*/
    cd_port_reuse(sfd);

    /*绑定服务器地址结构*/
    socklen_t serv_len, cli_len;                //结构体长度
    serv_len = sizeof(serv_addr);               //获取结构体长度
    memset(&serv_addr, 0, serv_len);            //清空结构体
    serv_addr.sin_family = AF_INET;             //使用的协议族
    serv_addr.sin_addr.s_addr = INADDR_ANY;     //本机的任何网卡
    serv_addr.sin_port = htons(SERV_PORT);      //程序端口号，本地转网络字节序【为0，则系统自动分配，使用getsockname函数配合】

    /*初始化一个地址结构 man 7 ip 查看对应信息*/
    cd_bind(sfd, (struct sockaddr *)&serv_addr, serv_len);

    /*设定链接上限,注意此处不阻塞*/
    cd_listen(sfd, 10);                         //同一时刻允许向服务器发起链接请求的数量


    /*打印一些交互信息*/
    printf("client IP:%s\tport:%d\t%d\n",
           inet_ntop(AF_INET, &cli_addr.sin_addr.s_addr, clie_ip, sizeof(clie_ip)),
           ntohs(cli_addr.sin_port), cfd);
    printf("server IP:%s\tport:%d\t%d\n",
           inet_ntop(AF_INET, &serv_addr.sin_addr.s_addr, serv_ip, sizeof(serv_ip)),
           ntohs(serv_addr.sin_port), sfd);

    while (1) {
        cfd = cd_accept(sfd, (struct sockaddr *)&cli_addr, &cli_len); //ctrl+c 注意ECONNABORTED这个错误
        pth_st[i].cli_addr = cli_addr;              //客户端信息和描述符绑定到一个自定义的结构体，当作参数传给线程
        pth_st[i].cfd = cfd;

        /* 达到线程最大数时，pthread_create出错处理, 增加服务器稳定性*/
        pthread_create(&tid, NULL, pth_act, (void*)&pth_st[i]);
        pthread_detach(tid);                      //子线程分离,防止僵线程产生,也可以用pthread_join回收
        i++;
        if(255 == i){ //256个是自己设置的，也可以使用系统默认值， 也可以用ulimit -s 指定
            cd_perr_exit("pthread max error");
        }
    }

    return 0;
}
