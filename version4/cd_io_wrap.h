#ifndef __CD_IO_WRAP_H_
#define __CD_IO_WRAP_H_
ssize_t cd_read(int fd, void *ptr, size_t nbytes);
ssize_t cd_write(int fd, const void *ptr, size_t nbytes);
ssize_t cd_readn(int fd, void *vptr, size_t n);
ssize_t cd_writen(int fd, const void *vptr, size_t n);
ssize_t cd_my_read(int fd, char *ptr);
ssize_t cd_read_line(int fd, void *vptr, size_t maxlen);
ssize_t cd_get_line(int sock, char *buf, int size);
int cd_close(int fd);
int cd_port_reuse(int fd);
#endif
