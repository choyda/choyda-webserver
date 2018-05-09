#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

//自己定义的函数库头文件
#include "cd_sock_wrap.h"
#include "cd_io_wrap.h"
#include "cd_std_wrap.h"

/*
 * DESCRIPTION:
 * 单进程的客户端交互，不能开启多个客户端，下个版本将添加多进程
 * 添加系统函数出错包裹函数，统一用大写开头表示，如：socket()-->Socket()
 * 自定义函数统一用rio_开头。Robust io 健壮的io操作
 */

#define SERV_PORT 9999 //定义服务器端口

void act_sin_fun(int sin_num)
{
    perror("child wait");
    while (waitpid(0, NULL, WNOHANG) > 0)
        ;
}

int main(int argc, char *argv[]){

    /*定义使用中的变量*/
    int sfd, cfd;                               //服务器端和客户端的socket描述符
    char buf[BUFSIZ];                           //读取写入buf的大小
    int len, i;                                 //读取长度和循环因子
    struct sockaddr_in serv_addr, cli_addr;     //客户端和服务器端bind结构体
    char cli_ip[BUFSIZ], serv_ip[BUFSIZ];       //保存打印信息ip的字符数组
    struct sigaction signal_act;                //处理信号的结构体
    pid_t pid;                                    //进程id

    /*创建socket描述符*/
    sfd = Socket(AF_INET, SOCK_STREAM, 0);      //最后一个参数0，内核会自动推演出使用的协议

    rio_port_reuse(sfd);                        //端口复用，TIME_WAIT等待问题

    /*初始化号结构体，用于子进程回收发送信号，也可以用signal函数代替，但是不建议。*/
    signal_act.sa_handler = act_sin_fun;        //注册信号处理函数
    sigemptyset(&signal_act.sa_mask);           //清空信号的mask表
    signal_act.sa_flags = 0;                    //通常设置为0，表使用默认属性。
    sigaction(SIGCHLD, &signal_act, NULL);      //要处理的信号,SIGCHLD 处理子进程退出的

    /*绑定服务器地址结构*/
    socklen_t serv_len, cli_len;                //结构体长度
    serv_len = sizeof(serv_addr);               //获取结构体长度
    memset(&serv_addr, 0, serv_len);            //清空结构体
    serv_addr.sin_family = AF_INET;             //使用的协议族
    serv_addr.sin_addr.s_addr = INADDR_ANY;     //本机的任何网卡
    serv_addr.sin_port = htons(SERV_PORT);      //程序端口号，本地转网络字节序【为0，则系统自动分配，使用getsockname函数配合】

    /*初始化一个地址结构 man 7 ip 查看对应信息*/
    Bind(sfd, (struct sockaddr *)&serv_addr, serv_len);

    /*设定链接上限,注意此处不阻塞*/
    Listen(sfd, 10);                            //同一时刻允许向服务器发起链接请求的数量


    /*打印一些交互信息*/
    printf("client IP:%s\tport:%d\t%d\n",
           inet_ntop(AF_INET, &cli_addr.sin_addr.s_addr, cli_ip, sizeof(cli_ip)),
           ntohs(cli_addr.sin_port), cfd);
    printf("server IP:%s\tport:%d\t%d\n",
           inet_ntop(AF_INET, &serv_addr.sin_addr.s_addr, serv_ip, sizeof(serv_ip)),
           ntohs(serv_addr.sin_port), sfd);

    while (1) {
        cfd = Accept(sfd, (struct sockaddr *)&cli_addr, &cli_len); //ctrl+c 注意ECONNABORTED这个错误
        pid = Fork(); //创建子进程
        if (pid == 0) {
            Close(sfd);//如果是子进程，关闭监听描述符，因为子进程不需要
            while (1) {
                /*读取客户端发送数据*/
                len = rio_read(cfd, buf, sizeof(BUFSIZ));
                if(len == 0){ //如果是0 表示客户端关闭socket
                    break;
                }
                rio_write(STDOUT_FILENO, buf, len);

                /*处理客户端数据*/
                for (i = 0; i < len; i++) {
                    buf[i] = toupper(buf[i]);
                }

                /*处理完数据回写给客户端*/
                rio_write(cfd, buf, len);
            }

            Close(cfd); //读写完毕关闭客户端描述符
            return 0;
        }else if(pid > 0){ //如果是父进程，关闭客户端描述符，继续阻塞accept,获取客户端描述符
            Cclose(cfd);
        }else{
            exit(1); //如果是fork失败直接退出
        }
    }
    return 0;
}
