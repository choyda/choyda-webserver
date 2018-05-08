#ifndef __CD_IO_WRAP_H_
#define __CD_IO_WRAP_H_
ssize_t Read(int fd, void *ptr, size_t nbytes);
ssize_t Write(int fd, const void *ptr, size_t nbytes);
int Close(int fd);
#endif
