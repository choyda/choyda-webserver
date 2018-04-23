#include <sys/socket.h>
#include <arpa/inet.h>

#ifndef __CD_HTTP_WRAP_H_
#define __CD_HTTP_WRAP_H_

#define GFL_SIZE 100
#define FIRST_LEN 1024                     //第一行文件大小

//解析http起始行，获取请求方法，uri路径，参数
struct st_http_parsing{
    char *method;
    char *uri;
    char *params;
    char *version;
};

//定义一个结构体, 将地址结构跟cfd捆绑
struct st_cli_info{
    struct sockaddr_in cli_addr;
    char *cli_accept_info;
    int cfd;
};

//解析http协议头的第一行，获取 方法，路径 协议
//void cd_get_first_line(struct st_gfl_muv *gfl_muv, char *buf);

//获取文件的类型
void cd_get_file_type(char *file_name, char *file_type);

/*处理get方法*/
//void cd_act_get(struct st_gfl_muv *);

/*http解析返回值，往客户端发送的数据*/
int cd_act_http_parsing(struct st_cli_info *cli_info, char *buf);

/*获得uri的路径和参数*/
void cd_get_uri(char *uri, char *file_name, char *params, int *act_type);

/*处理php文件*/
void execute_cgi(int client, const char *path, const char *method, const char *query_string);
#endif