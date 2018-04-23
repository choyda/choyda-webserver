#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>

#include "cd_sock_wrap.h"
#include "cd_std_wrap.h"


/*accept函数处理，错误处理*/
int cd_accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	int n;
again:
	if ((n = accept(fd, sa, salenptr)) < 0) {
		if ((errno == ECONNABORTED) || (errno == EINTR)) //意外中断重启
			goto again;
		else
			cd_perr_exit("accept error");
	}
	return n;
}

int cd_bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
    int n;

	if ((n = bind(fd, sa, salen)) < 0)
		cd_perr_exit("bind error");

    return n;
}

int cd_onnect(int fd, const struct sockaddr *sa, socklen_t salen)
{
    int n;

	if ((n = connect(fd, sa, salen)) < 0)
		cd_perr_exit("connect error");

    return n;
}

int cd_listen(int fd, int backlog)
{
    int n;

	if ((n = listen(fd, backlog)) < 0)
		cd_perr_exit("listen error");

    return n;
}

int cd_socket(int family, int type, int protocol)
{
	int n;

	if ((n = socket(family, type, protocol)) < 0)
		cd_perr_exit("socket error");

	return n;
}

/*端口复用，TIME_WAIT等待问题*/
int cd_port_reuse(int fd){
	int opt = 1;
	if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt)) == -1){
		cd_perr_exit("setsockopt error");
	}
}

/*服务器端socket初始化*/
int cd_sock_init(){

	int sfd;	                                //服务器端scket描述符
	struct sockaddr_in serv_addr;               //客户端和服务器端bind结构体

	/*创建socket描述符*/
	sfd = cd_socket(AF_INET, SOCK_STREAM, 0);      //最后一个参数0，内核会自动推演出使用的协议

	/*端口复用，TIME_WAIT等待问题*/
	cd_port_reuse(sfd);

	/*绑定服务器地址结构*/
	socklen_t serv_len;			                //结构体长度
	serv_len = sizeof(serv_addr);               //获取结构体长度
	memset(&serv_addr, 0, serv_len);            //清空结构体
	serv_addr.sin_family = AF_INET;             //使用的协议族
	serv_addr.sin_addr.s_addr = INADDR_ANY;     //本机的任何网卡
	serv_addr.sin_port = htons(SERV_PORT);      //程序端口号，本地转网络字节序【为0，则系统自动分配，使用getsockname函数配合】

	/*初始化一个地址结构 man 7 ip 查看对应信息*/
	cd_bind(sfd, (struct sockaddr *)&serv_addr, serv_len);

	/*设定链接上限,注意此处不阻塞*/
	cd_listen(sfd, 10);                         //同一时刻允许向服务器发起链接请求的数量

	return sfd;
}
