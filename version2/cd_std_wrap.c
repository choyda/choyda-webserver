#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

/*���ù��ܺ�����װ*/

void cd_perr_exit(const char *s)
{
    perror(s);
    exit(-1);
}