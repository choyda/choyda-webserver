#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <netinet/in.h>

//�Լ�����ĺ�����ͷ�ļ�
#include "cd_sock_wrap.h"
#include "cd_io_wrap.h"
#include "cd_std_wrap.h"
#include "cd_http_wrap.h"
#include "cd_http_code.h"

/**** ���߳� ****/
/*���߳̿ͻ��˽���������ͬʱ֧�ֶ���ͻ�������*/

#define SIZEBUF 1024 //��ȡbuf��С

/*ÿһ���̴߳���һ���ͻ�������*/
void *pth_act(void *args)
{
    struct st_cli_info *cli_info = (struct st_cli_info*)args;
    char buf[SIZEBUF], serv_log_cli_info[SIZEBUF]; //��������¼�ͻ��˵���Ϣ
    struct st_http_parsing http_parsing;        //�������http�Ľṹ��
    int n = 1, i=0;
    //1, ����http����
    while(1) {

        n = cd_get_line(cli_info->cfd, buf, sizeof(buf));
        printf("%s\n", buf);
        printf("%d\n", n);
        i++;
        if(i == 2) break;
    }
//    while(1){
        //����http
//        n = cd_act_http_parsing(cli_info, buf);                    //buf���淵����Ϣ
//        printf("%s\n", buf);
//        printf("%d\n", cli_info->cfd);
                         //��д���ͻ���
//        printf("%d\n", n);
//        printf("%s\n", buf);
//        n = cd_read(cli_info->cfd, buf, SIZEBUF);
//        printf("%s\n", buf);

//        char buff[]="HTTP/1.1 200 OK\r\nServer: CD_Server <0.1>\r\nAccept-Ranges:bytes\r\nContent-Length: \r\nConnection: close\r\nContent-Type:text/html\r\n\r\nbody:asdfasfasdf\r\n\r\n";
//        cd_write(cli_info->cfd, "asdfasfasdfsd\n", strlen(buf));                           //��д���ͻ���

//        cd_write(STDOUT_FILENO, buf, strlen(buf));                           //��д���ͻ���

        //����������ʾ
//        sprintf(serv_log_cli_info, "%s %s", cli_info->cli_accept_info, buf);
//        cd_write(STDOUT_FILENO, serv_log_cli_info, strlen(serv_log_cli_info));                           //��������ʾ
//
//        if (n == 0) {
//            printf("client %d closed...\n", cli_info->cfd);
////           break; //һ��Ҫ����ѭ����Ҫ��Ȼ��read����
//        }
//        break;
//    }
    cd_close(cli_info->cfd);


    return (void *)0;
}

int main(int argc, char *argv[]){

    int sfd, cfd;                                   //�������˺Ϳͻ��˵�socket������
    int i = 0;                                      //i�����̵߳ĸ���, ulimit -u�鿴�����
    struct sockaddr_in cli_addr;                    //accept�ͻ��˽ṹ��
    socklen_t cli_len;
    char cli_ip[INET_ADDRSTRLEN];                   //�����ӡ��Ϣip���ַ�����
    char cli_accept_info[SIZEBUF];           //����ͻ�����Ϣ
    pthread_t tid;                                  //�߳�id
    struct st_cli_info cli_info[256];                //��������߳��������ṹ������.
    cli_len = sizeof(cli_addr);                     //�ͻ��˽ṹ�峤�ȣ����ڱ���ͻ�����Ϣ
    sfd = cd_sock_init();                           //��ʼ��������socket

    printf("Accepting connections ...\n");
    while (1) {
        /*�пͻ�������read������accept������������*/
        cfd = cd_accept(sfd, (struct sockaddr *)&cli_addr, &cli_len);

        /*�����ͻ���������Ϣ*/
        sprintf(cli_accept_info, "%s %d %d",
                inet_ntop(AF_INET, &cli_addr.sin_addr.s_addr,
                          cli_ip, sizeof(cli_ip)), ntohs(cli_addr.sin_port), cfd);

        /*�߳�Ҫ����Ŀͻ���sock��Ϣ�������������߳� i���߳���*/
        cli_info[i].cli_addr = cli_addr;
        cli_info[i].cfd = cfd;
        cli_info[i].cli_accept_info = cli_accept_info;

        /*�ﵽ�߳������ʱ��pthread_create������, ���ӷ������ȶ���*/
        pthread_create(&tid, NULL, pth_act, (void*)&cli_info[i]);

        /*���̷߳���,��ֹ���̲߳���,Ҳ������pthread_join����*/
        pthread_detach(tid);

        /*256�����Լ����õģ�Ҳ����ʹ��ϵͳĬ��ֵ�� Ҳ������ulimit -s ָ��*/
        i++;
        if(255 == i){
            cd_perr_exit("pthread max error");
        }
    }
    return 0;
}
