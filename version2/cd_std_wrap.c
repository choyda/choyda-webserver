#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/*常用功能函数封装*/

void unix_error(const char *msg)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(0);
}

pid_t Fork(){

    pid_t pid;
    if((pid = fork()) < 0){
        unix_error("fork error:");
    }
    return pid;

}