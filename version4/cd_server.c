#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <netinet/in.h>

//自己定义的函数库头文件
#include "cd_sock_wrap.h"
#include "cd_io_wrap.h"
#include "cd_std_wrap.h"
#include "cd_http_wrap.h"
#include "cd_http_code.h"

/**** 多线程 ****/
/*多线程客户端交互，可以同时支持多个客户端连接*/

#define SIZEBUF 1024 //读取buf大小

/*每一个线程处理一个客户端请求*/
void *pth_act(void *args)
{
    struct st_cli_info *cli_info = (struct st_cli_info*)args;
    char buf[SIZEBUF], serv_log_cli_info[SIZEBUF]; //服务器记录客户端的信息
    struct st_http_parsing http_parsing;        //保存解析http的结构体
    int n = 1, i=0;
    //1, 解析http行首
    while(1) {

        n = cd_get_line(cli_info->cfd, buf, sizeof(buf));
        printf("%s\n", buf);
        printf("%d\n", n);
        i++;
        if(i == 2) break;
    }
//    while(1){
        //解析http
//        n = cd_act_http_parsing(cli_info, buf);                    //buf保存返回信息
//        printf("%s\n", buf);
//        printf("%d\n", cli_info->cfd);
                         //回写给客户端
//        printf("%d\n", n);
//        printf("%s\n", buf);
//        n = cd_read(cli_info->cfd, buf, SIZEBUF);
//        printf("%s\n", buf);

//        char buff[]="HTTP/1.1 200 OK\r\nServer: CD_Server <0.1>\r\nAccept-Ranges:bytes\r\nContent-Length: \r\nConnection: close\r\nContent-Type:text/html\r\n\r\nbody:asdfasfasdf\r\n\r\n";
//        cd_write(cli_info->cfd, "asdfasfasdfsd\n", strlen(buf));                           //回写给客户端

//        cd_write(STDOUT_FILENO, buf, strlen(buf));                           //回写给客户端

        //服务器端显示
//        sprintf(serv_log_cli_info, "%s %s", cli_info->cli_accept_info, buf);
//        cd_write(STDOUT_FILENO, serv_log_cli_info, strlen(serv_log_cli_info));                           //服务器显示
//
//        if (n == 0) {
//            printf("client %d closed...\n", cli_info->cfd);
////           break; //一定要跳出循环，要不然会read错误
//        }
//        break;
//    }
    cd_close(cli_info->cfd);


    return (void *)0;
}

int main(int argc, char *argv[]){

    int sfd, cfd;                                   //服务器端和客户端的socket描述符
    int i = 0;                                      //i创建线程的个数, ulimit -u查看最大数
    struct sockaddr_in cli_addr;                    //accept客户端结构体
    socklen_t cli_len;
    char cli_ip[INET_ADDRSTRLEN];                   //保存打印信息ip的字符数组
    char cli_accept_info[SIZEBUF];           //保存客户端信息
    pthread_t tid;                                  //线程id
    struct st_cli_info cli_info[256];                //根据最大线程数创建结构体数组.
    cli_len = sizeof(cli_addr);                     //客户端结构体长度，用于保存客户端信息
    sfd = cd_sock_init();                           //初始化服务器socket

    printf("Accepting connections ...\n");
    while (1) {
        /*有客户端请求read，调用accept，否则阻塞。*/
        cfd = cd_accept(sfd, (struct sockaddr *)&cli_addr, &cli_len);

        /*构建客户端连接信息*/
        sprintf(cli_accept_info, "%s %d %d",
                inet_ntop(AF_INET, &cli_addr.sin_addr.s_addr,
                          cli_ip, sizeof(cli_ip)), ntohs(cli_addr.sin_port), cfd);

        /*线程要处理的客户端sock信息，作参数传给线程 i是线程数*/
        cli_info[i].cli_addr = cli_addr;
        cli_info[i].cfd = cfd;
        cli_info[i].cli_accept_info = cli_accept_info;

        /*达到线程最大数时，pthread_create出错处理, 增加服务器稳定性*/
        pthread_create(&tid, NULL, pth_act, (void*)&cli_info[i]);

        /*子线程分离,防止僵线程产生,也可以用pthread_join回收*/
        pthread_detach(tid);

        /*256个是自己设置的，也可以使用系统默认值， 也可以用ulimit -s 指定*/
        i++;
        if(255 == i){
            cd_perr_exit("pthread max error");
        }
    }
    return 0;
}
