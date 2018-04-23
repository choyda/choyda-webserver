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
/*���httpͷ�ײ�*/
void cd_get_first_line(struct st_http_parsing *gfl_muv, char *buf){

    //�����ܶ࣬strtok����ѡ��sscanf
    char method[GFL_SIZE], uri[GFL_SIZE], version[GFL_SIZE];
    sscanf(buf, "%s %s %s", method, uri, version);
    gfl_muv->method = method;
    gfl_muv->uri = uri;
    gfl_muv->version = version;
}


/*����get����*/
void cd_act_get(struct st_http_parsing * gfl_muv){
    if(strcasecmp(gfl_muv->method, "GET") == 0){

    }

}

/*��ȡ�ļ�������*/
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

/*���������ļ�������Ĳ���*/
void cd_get_uri(char *uri, char *file_name, char *params, int *act_type){

    char *ptr;
    int act_t;
    char temp1[BUFSIZ];
    char temp2[BUFSIZ];

    //˵���ߵ��Ǿ�̬��ַ, �����1�������� 2��ָ�����ļ���3�������ڵ��ļ�
//    printf("%s\n", uri);
    if(!strstr(uri, ".php")) {
        sprintf(file_name, "./htdocs%s", uri); //�������htdocsҪ�������ļ���ȡ
        if (uri[strlen(uri)-1] == '/') {
            sprintf(file_name, "/htdocs%s", "/index.html");  //Ĭ�ϵ�index.htmlҲҪ�������ļ�
        }
        *act_type = 1;
    }else{
        //�����p
        // hp�ļ�, �����1���в�����2��û������3���������ļ�
        // uri===/index.php?name=22&age=32?aaa=vbb

//        ptr = index(uri, '?');
//        printf("%s\n", ptr);
//        printf("%s\n", uri);
        sscanf(uri, "%[^?]?%s",temp1,temp2);//�ֽ��ļ����Ͳ���
//        printf("%s\n", temp1);// /index.php

//        printf("%s\n", temp2); //name=22&age=32?aaa=vbb

        if(temp2[0] != '\0'){
            strcpy(params, temp2); //���������0 û�в������Ͳ���ӵ�params�У���ֹ\0��ӽ�ȥ
        }
        strcpy(file_name, ".");
        strcat(file_name, temp1);
        *act_type = 2;
    }
//    printf("%s---%s----\n", file_name, params);
}



/*http����*/
int cd_act_http_parsing(struct st_cli_info *cli_info, char *buf){

    //1,��õ�һ��http����.
    int n, l=1;
    char first_buf[FIRST_LEN],test_buf[BUFSIZ];
    cd_read(cli_info->cfd, test_buf, BUFSIZ);
    printf("%s--\n", test_buf);

    n = cd_get_line(cli_info->cfd, first_buf, FIRST_LEN);//���ͻ���
    if(n < 0){
        cd_perr_exit("cd_get_line err:");
    }

    //2,���get,uri. ������gfl_muv �ṹ����
    struct st_http_parsing gfl_muv;
    cd_get_first_line(&gfl_muv, first_buf);

    //3,����uri,����ļ������������Ƕ�̬���Ǿ�̬
    char file_name[BUFSIZ], params[BUFSIZ], file_type[BUFSIZ];
    int act_type = 0; //1��̬�� 2��̬
    cd_get_uri(gfl_muv.uri, file_name, params, &act_type);
    int file_fd; //���ļ���fd

    //4,�ж��ļ��Ƿ���ڣ����������ֱ����ִ��,ֱ�ӷ���404
    //��ȡ�ļ�״̬���鿴�ļ��ڲ���
//
//    struct stat stat_buf;
//    if (stat(file_name, &stat_buf) < 0) {
////        cd_http_code_404();
//    }else{
//        cd_get_file_type(file_name, file_type);
//        // ����ļ����ڣ��ж���get����post
//
//        if(strcasecmp(gfl_muv.method, "GET") == 0) {
//            //get ����ʽ
//            //��ȡ�ļ�״̬���鿴�ļ��ڲ���
//            if (act_type == 1) {    //��̬·��
//                //1,��ȡ�ļ�
//                char strbuf[2048];//�ڴ�ӳ��Ŀռ�
//                file_fd = open(file_name,O_RDONLY);
//                read(file_fd,strbuf,sizeof(strbuf));
////                printf("%s\n",head_buf);
////                srcp = mmap(0, stat_buf.st_size, PROT_READ, MAP_PRIVATE, file_fd, 0); //�ڴ�ӳ��
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
//                //���srcp�ռ�
////                munmap(srcp, stat_buf.st_size);
//            } else if (act_type == 2) {
//                //��̬
////                php_cgi(cli_info->cfd, file_name, gfl_muv.method, params);
//                return 0;
//            } else {
//                return 0;
//            }
//        }else if (strcasecmp(gfl_muv.method, "POST") == 0) {
//            //post����ʽ
//        }
//    }

    return 0;

}