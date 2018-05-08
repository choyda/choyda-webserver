#ifndef __CD_SOCK_WRAP_H_
#define __CD_SOCK_WRAP_H_

int Accept(int fd, struct sockaddr *cli_addr, socklen_t *cli_addr_len);
int Bind(int fd, const struct sockaddr *serv_addr, socklen_t serv_addr_len);
int Connect(int fd, const struct sockaddr *sa, socklen_t salen);
int Listen(int fd, int backlog);
int Socket(int family, int type, int protocol);
void rio_port_reuse(int sfd);
#endif
