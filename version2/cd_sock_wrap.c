#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
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