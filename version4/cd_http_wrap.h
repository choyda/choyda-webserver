#include <sys/socket.h>
#include <arpa/inet.h>

#ifndef __CD_HTTP_WRAP_H_
#define __CD_HTTP_WRAP_H_

#define GFL_SIZE 100
#define FIRST_LEN 1024                     //��һ���ļ���С

//����http��ʼ�У���ȡ���󷽷���uri·��������
struct st_http_parsing{
    char *method;
    char *uri;
    char *params;
    char *version;
};

//����һ���ṹ��, ����ַ�ṹ��cfd����
struct st_cli_info{
    struct sockaddr_in cli_addr;
    char *cli_accept_info;
    int cfd;
};

//����httpЭ��ͷ�ĵ�һ�У���ȡ ������·�� Э��
//void cd_get_first_line(struct st_gfl_muv *gfl_muv, char *buf);

//��ȡ�ļ�������
void cd_get_file_type(char *file_name, char *file_type);

/*����get����*/
//void cd_act_get(struct st_gfl_muv *);

/*http��������ֵ�����ͻ��˷��͵�����*/
int cd_act_http_parsing(struct st_cli_info *cli_info, char *buf);

/*���uri��·���Ͳ���*/
void cd_get_uri(char *uri, char *file_name, char *params, int *act_type);

/*����php�ļ�*/
void execute_cgi(int client, const char *path, const char *method, const char *query_string);
#endif