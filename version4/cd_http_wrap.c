#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "cd_std_wrap.h"
#include "cd_io_wrap.h"
#include "cd_http_wrap.h"
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "cd_std_wrap.h"
/*获得http头首部*/
void cd_get_first_line(struct st_http_parsing *gfl_muv, char *buf){

    //方法很多，strtok，我选择sscanf
    char method[GFL_SIZE], uri[GFL_SIZE], version[GFL_SIZE];
    sscanf(buf, "%s %s %s", method, uri, version);
    gfl_muv->method = method;
    gfl_muv->uri = uri;
    gfl_muv->version = version;
}


/*处理get方法*/
void cd_act_get(struct st_http_parsing * gfl_muv){
    if(strcasecmp(gfl_muv->method, "GET") == 0){

    }

}

/*获取文件的类型*/
void cd_get_file_type(char *file_name, char *file_type){

    if (strstr(file_name, ".html") || strstr(file_name, ".php")){
        strcpy(file_type, "text/html");
    }else if(strstr(file_name, ".gif")){
        strcpy(file_type, "image/gif");
    }else if(strstr(file_name, ".png")){
        strcpy(file_type, "image/png");
    }else if(strstr(file_name, ".jpg")){
        strcpy(file_type, "image/jpeg");
    }else if(strstr(file_name, ".js")){
        strcpy(file_type, "application/x-javascript");
    }else if(strstr(file_name, ".css")){
        strcpy(file_type, "text/css");
    }else{
        strcpy(file_type, "text/plain");
    }
}

/*处理请求文件和请求的参数*/
void cd_get_uri(char *uri, char *file_name, char *params, int *act_type){

    char *ptr;
    int act_t;
    char temp1[BUFSIZ];
    char temp2[BUFSIZ];

    //说明走的是静态网址, 情况：1，不传， 2，指定的文件，3，不存在的文件
//    printf("%s\n", uri);
    if(!strstr(uri, ".php")) {
        sprintf(file_name, "./htdocs%s", uri); //将来这个htdocs要用配置文件读取
        if (uri[strlen(uri)-1] == '/') {
            sprintf(file_name, "/htdocs%s", "/index.html");  //默认的index.html也要走配置文件
        }
        *act_type = 1;
    }else{
        //如果是p
        // hp文件, 情况：1，有参数，2，没参数，3，不存在文件
        // uri===/index.php?name=22&age=32?aaa=vbb

//        ptr = index(uri, '?');
//        printf("%s\n", ptr);
//        printf("%s\n", uri);
        sscanf(uri, "%[^?]?%s",temp1,temp2);//分解文件名和参数
//        printf("%s\n", temp1);// /index.php

//        printf("%s\n", temp2); //name=22&age=32?aaa=vbb

        if(temp2[0] != '\0'){
            strcpy(params, temp2); //如果参数是0 没有参数，就不添加到params中，防止\0添加进去
        }
        strcpy(file_name, ".");
        strcat(file_name, temp1);
        *act_type = 2;
    }
//    printf("%s---%s----\n", file_name, params);
}



/*http解析*/
int cd_act_http_parsing(struct st_cli_info *cli_info, char *buf){

    //1,获得第一行http请求.
    int n, l=1;
    char first_buf[FIRST_LEN],test_buf[BUFSIZ];
    cd_read(cli_info->cfd, test_buf, BUFSIZ);
    printf("%s--\n", test_buf);

    n = cd_get_line(cli_info->cfd, first_buf, FIRST_LEN);//读客户端
    if(n < 0){
        cd_perr_exit("cd_get_line err:");
    }

    //2,获得get,uri. 保存在gfl_muv 结构体中
    struct st_http_parsing gfl_muv;
    cd_get_first_line(&gfl_muv, first_buf);

    //3,根据uri,获得文件名，参数，是动态还是静态
    char file_name[BUFSIZ], params[BUFSIZ], file_type[BUFSIZ];
    int act_type = 0; //1静态， 2动态
    cd_get_uri(gfl_muv.uri, file_name, params, &act_type);
    int file_fd; //打开文件的fd

    //4,判断文件是否存在，如果不存在直往下执行,直接返回404
    //获取文件状态，查看文件在不在
//
//    struct stat stat_buf;
//    if (stat(file_name, &stat_buf) < 0) {
////        cd_http_code_404();
//    }else{
//        cd_get_file_type(file_name, file_type);
//        // 如果文件存在，判断是get还是post
//
//        if(strcasecmp(gfl_muv.method, "GET") == 0) {
//            //get 处理方式
//            //获取文件状态，查看文件在不在
//            if (act_type == 1) {    //静态路径
//                //1,读取文件
//                char strbuf[2048];//内存映射的空间
//                file_fd = open(file_name,O_RDONLY);
//                read(file_fd,strbuf,sizeof(strbuf));
////                printf("%s\n",head_buf);
////                srcp = mmap(0, stat_buf.st_size, PROT_READ, MAP_PRIVATE, file_fd, 0); //内存映射
//
////                cd_close(file_fd);
////                printf("%s\n", head_buf);
////                char strbuf[1025];
////                strcpy(strbuf, "hello");
//                cd_http_code_200(buf, file_type, stat_buf.st_size);
//                sprintf(buf, "%s%s", buf,strbuf);
////                printf("%s\n", buf);
//                cd_write(cli_info->cfd, buf, strlen(buf));
//                return 0;
//                //清空srcp空间
////                munmap(srcp, stat_buf.st_size);
//            } else if (act_type == 2) {
//                //动态
////                php_cgi(cli_info->cfd, file_name, gfl_muv.method, params);
//                return 0;
//            } else {
//                return 0;
//            }
//        }else if (strcasecmp(gfl_muv.method, "POST") == 0) {
//            //post处理方式
//        }
//    }

    return 0;

}