#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

/*常用功能函数封装*/

void cd_perr_exit(const char *s)
{
    perror(s);
    exit(-1);
}