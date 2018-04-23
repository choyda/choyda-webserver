#ifndef __CD_SOCK_WRAP_H_
#define __CD_SOCK_WRAP_H_

#define SERV_PORT 80
int cd_sock_init();
int cd_accept(int fd, struct sockaddr *sa, socklen_t *salenptr);
int cd_bind(int fd, const struct sockaddr *sa, socklen_t salen);
int cd_onnect(int fd, const struct sockaddr *sa, socklen_t salen);
int cd_listen(int fd, int backlog);
int cd_socket(int family, int type, int protocol);

#endif
