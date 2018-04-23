#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

/*
 * DESCRIPTION:
 * 单进程的客户端交互，不能开启多个客户端。下个版本将添加多进程
 */

#define SERV_PORT 9999 //定义服务器端口
#define SIZEBUF 1024 //buf大小 最大1500

int main(int argc, char *argv[]){

    /*定义使用中的变量*/
    int sfd, cfd;                               //服务器端和客户端的socket描述符
    char buf[SIZEBUF];                          //读取写入buf的大小
    int len, i;                                 //读取长度和循环因子
    struct sockaddr_in serv_addr, cli_addr;     //客户端和服务器端bind结构体
    char clie_ip[SIZEBUF], serv_ip[SIZEBUF];    //保存打印信息ip的字符数组

    /*创建socket描述符*/
    sfd = socket(AF_INET, SOCK_STREAM, 0);      //最后一个参数0，内核会自动推演出使用的协议
    if(sfd == -1){
        perror("fail to create socket");
        exit(1);
    }

    /*端口复用，TIME_WAIT等待问题*/
    int opt = 1;
    if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt)) == -1){
        perror("fail to setsockopt socket");
        exit(1);
    }

    /*绑定服务器地址结构*/
    socklen_t serv_len, cli_len;                //结构体长度
    serv_len = sizeof(serv_addr);               //获取结构体长度
    memset(&serv_addr, 0, serv_len);            //清空结构体
    serv_addr.sin_family = AF_INET;             //使用的协议族
    serv_addr.sin_addr.s_addr = INADDR_ANY;     //本机的任何网卡
    serv_addr.sin_port = htons(SERV_PORT);      //程序端口号，本地转网络字节序【为0，则系统自动分配，使用getsockname函数配合】

    /*初始化一个地址结构 man 7 ip 查看对应信息*/
    if(bind(sfd, (struct sockaddr *)&serv_addr, serv_len) == -1){
        perror("fail to bind socket");
        exit(1);
    }

    /*设定链接上限,注意此处不阻塞*/
    if(listen(sfd, 10) == -1){                   //同一时刻允许向服务器发起链接请求的数量
        perror("fail to listen socket");
        exit(1);
    }

    /*获取已经三次握手成功的请求，参数1是sfd; 参2传出参数, 参3传入传入参数, 全部是client端的参数*/
    cli_len = sizeof(cli_addr);                  //获取客户端结构体大小
    if((cfd = accept(sfd, (struct sockaddr *)&cli_addr, &cli_len)) == -1){  //监听客户端链接, 会阻塞
        perror("fail to bind socket");
        exit(1);
    }

    /*打印一些交互信息*/
    printf("client IP:%s\tport:%d\t%d\n",
           inet_ntop(AF_INET, &cli_addr.sin_addr.s_addr, clie_ip, sizeof(clie_ip)),
           ntohs(cli_addr.sin_port), cfd);
    printf("server IP:%s\tport:%d\t%d\n",
           inet_ntop(AF_INET, &serv_addr.sin_addr.s_addr, serv_ip, sizeof(serv_ip)),
           ntohs(serv_addr.sin_port), sfd);

    /*读写文件描述符*/
    while(1){
        /*读取客户端发送数据*/
        len = read(cfd, buf, sizeof(SIZEBUF));
        write(STDOUT_FILENO, buf, len);

        /*处理客户端数据*/
        for(i=0; i<len; i++){
            buf[i] = toupper(buf[i]);
        }

        /*处理完数据回写给客户端*/
        write(cfd, buf, len);
    }

    /*关闭链接*/
    close(sfd);
    close(cfd);
    return 0;
}
