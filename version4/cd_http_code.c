#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "cd_http_code.h"
#include <unistd.h>
#include <stdio.h>

/*成功的协议头，但是每包含长度，需要读入文件的长度，如果没有content-length 浏览器不会显示*/
void cd_http_code_200(char *buf, char *file_type, int *con_len){

//    char buff[]="HTTP/1.1 200 OK\r\nServer: CD_Server <0.1>\r\nAccept-Ranges: bytes\r\nContent-Length: \r\nConnection: close\r\nContent-Type:text/html\r\n\r\nbody:asdfasfasdf\r\n\r\n";
    char *http_code="HTTP/1.1 200 OK\r\nServer: CdServe\r\nConnection: close\r\n";
    sprintf(buf, "%s", http_code);
    sprintf(buf, "%sContent-Type: %s;charset=utf-8\r\n", buf, file_type);
    sprintf(buf, "%sContent-Length: %d\r\n\r\n", buf, con_len);
//    sprintf(buf, http_res_hdr_tmpl, file_len, type);
//    sprintf(buf, "%sServer:cd_server\r\n", buf);
//    sprintf(buf, "%sContent-Type: text/html\r\n", buf);
}

void http_code_500(char *buf)
{
    /* HTTP method 不被支持*/
    sprintf(buf, "HTTP/1.0 501 Method Not Implemented\r\n");
    /*服务器信息*/
//    sprintf(buf, SERVER_STRING);
    sprintf(buf, "Content-Type: text/html\r\n");
    sprintf(buf, "\r\n");
    sprintf(buf, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
    sprintf(buf, "</TITLE></HEAD>\r\n");
    sprintf(buf, "<BODY><P>HTTP request method not supported.\r\n");
    sprintf(buf, "</BODY></HTML>\r\n");
}

/**
 * @desc 404页面的response拼接
 *
 */
void http_code_404(char *response) {
    struct stat sbuf;
    int filefd;
    char *srcp;
//    char error_body[MAXLINE],response[MAXLINE];
//    char filename[] = "./404.html";

//    stat(filename,&sbuf);
//    filefd = open(filename,O_RDONLY);

    sprintf(response, "HTTP/1.1 404 Not found\r\n");
    sprintf(response, "%sServer: Pengge Web Server\r\n",response);
    sprintf(response, "%sConnection: close\r\n",response);
    sprintf(response, "%sContent-length: %lld\r\n",response,sbuf.st_size);
    sprintf(response, "%sContent-type: text/html\r\n\r\n",response);
//    cd_write(connfd, response, strlen(response));
//    printf("Response headers:\n");
//    srcp = mmap(0, sbuf.st_size, PROT_READ, MAP_PRIVATE, filefd, 0);
//    cd_close(filefd);
    //清空srcp空间
//    cd_write(connfd, srcp, sbuf.st_size);
//    munmap(srcp, sbuf.st_size);

}