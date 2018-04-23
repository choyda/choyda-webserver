#include <stdlib.h>
#include <stdio.h>
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
#define SERV_IP "127.0.0.1"

int main(int argc, char *argv[]){

    /*定义使用中的变量*/
    int sfd;                                    //客户端的socket描述符
    char buf[SIZEBUF];                          //读取写入buf的大小
    int len;                                    //读取长度
    struct sockaddr_in serv_addr, cli_addr;     //客户端和服务器端bind结构体
    char clie_ip[SIZEBUF], serv_ip[SIZEBUF];    //保存打印信息ip的字符数组

    /*创建socket描述符*/
    sfd = socket(AF_INET, SOCK_STREAM, 0);      //最后一个参数0，内核会自动推演出使用的协议
    if(sfd == -1){
        perror("fail to create socket");
        exit(1);
    }

    /*绑定服务器地址结构*/
    socklen_t serv_len;                         //结构体长度
    serv_len = sizeof(serv_addr);               //获取结构体长度
    memset(&serv_addr, 0, serv_len);            //清空结构体
    serv_addr.sin_family = AF_INET;             //使用的协议族
    inet_pton(AF_INET, SERV_IP, &serv_addr.sin_addr.s_addr);  //指定IP 字符串类型转换为网络字节序 参3:传出参数
    serv_addr.sin_port = htons(SERV_PORT);      //指定端口 本地转网络字节序

    /*根据地址结构链接指定服务器进程*/
    if(connect(sfd, (struct sockaddr *) &serv_addr, serv_len) == -1){
        perror("fail to create socket");
        exit(1);
    }

    /*打印一些交互信息*/
    printf("client IP:%s\tport:%d\t%d\n",
           inet_ntop(AF_INET, &cli_addr.sin_addr.s_addr, clie_ip, sizeof(clie_ip)),
           ntohs(cli_addr.sin_port), sfd);
    printf("server IP:%s\tport:%d\t%d\n",
           inet_ntop(AF_INET, &serv_addr.sin_addr.s_addr, serv_ip, sizeof(serv_ip)),
           ntohs(serv_addr.sin_port), sfd);

    /*读写文件描述符*/

    while (1) {
        /*从标准输入获取数据*/
        fgets(buf, sizeof(buf), stdin);
        /*将数据写给服务器*/
        write(sfd, buf, strlen(buf));
        /*从服务器读回转换后数据*/
        len = read(sfd, buf, sizeof(buf));
        /*写至标准输出*/
        write(STDOUT_FILENO, buf, len);
    }
    /*关闭链接*/
    close(sfd);
    return 0;
}
