#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

//�Լ�����ĺ�����ͷ�ļ�
#include "cd_sock_wrap.h"
#include "cd_io_wrap.h"
#include "cd_std_wrap.h"

/**** ���߳� ****/
/*���߳̿ͻ��˽���������ͬʱ֧�ֶ���ͻ�������*/

#define SERV_PORT 9999 //����������˿�
#define SIZEBUF 1024 //��ȡbuf��С

struct pth_st_info{                     //����һ���ṹ��, ����ַ�ṹ��cfd����
    struct sockaddr_in cli_addr;
    int cfd;
};

void *pth_act(void *args)
{
    int n,i;
    struct pth_st_info *pt_st = (struct pth_st_info*)args;
    char buf[SIZEBUF];              //��ȡ��������С
    char inet_len[INET_ADDRSTRLEN];      //#define INET_ADDRSTRLEN 16

    while (1) {
        n = cd_read(pt_st->cfd, buf, SIZEBUF);                     //���ͻ���
        if (n == 0) {
            printf("client %d closed...\n", pt_st->cfd);
            break;                                              //����ѭ��,�ر�cfd
        }
        printf("received from %s at PORT %d\n",
               inet_ntop(AF_INET, &(*pt_st).cli_addr.sin_addr, inet_len, sizeof(inet_len)),
               ntohs((*pt_st).cli_addr.sin_port));                 //��ӡ�ͻ�����Ϣ(IP/PORT)

        for (i = 0; i < n; i++)
            buf[i] = toupper(buf[i]);                           //Сд-->��д

        cd_write(STDOUT_FILENO, buf, n);                           //д������Ļ
        cd_write(pt_st->cfd, buf, n);                              //��д���ͻ���
    }
    cd_close(pt_st->cfd);

    return (void *)0;
}

int main(int argc, char *argv[]){

    /*����ʹ���еı���*/
    int sfd, cfd;                               //�������˺Ϳͻ��˵�socket������
    int i=0;                                    //i�����̵߳ĸ���
    struct sockaddr_in serv_addr, cli_addr;     //�ͻ��˺ͷ�������bind�ṹ��
    char clie_ip[SIZEBUF], serv_ip[SIZEBUF];    //�����ӡ��Ϣip���ַ�����
    pthread_t tid;                              //�߳�id
    struct pth_st_info pth_st[256];                      //��������߳��������ṹ������.

    /*����socket������*/
    sfd = cd_socket(AF_INET, SOCK_STREAM, 0);      //���һ������0���ں˻��Զ����ݳ�ʹ�õ�Э��

    /*�˿ڸ��ã�TIME_WAIT�ȴ�����*/
    cd_port_reuse(sfd);

    /*�󶨷�������ַ�ṹ*/
    socklen_t serv_len, cli_len;                //�ṹ�峤��
    serv_len = sizeof(serv_addr);               //��ȡ�ṹ�峤��
    memset(&serv_addr, 0, serv_len);            //��սṹ��
    serv_addr.sin_family = AF_INET;             //ʹ�õ�Э����
    serv_addr.sin_addr.s_addr = INADDR_ANY;     //�������κ�����
    serv_addr.sin_port = htons(SERV_PORT);      //����˿ںţ�����ת�����ֽ���Ϊ0����ϵͳ�Զ����䣬ʹ��getsockname������ϡ�

    /*��ʼ��һ����ַ�ṹ man 7 ip �鿴��Ӧ��Ϣ*/
    cd_bind(sfd, (struct sockaddr *)&serv_addr, serv_len);

    /*�趨��������,ע��˴�������*/
    cd_listen(sfd, 10);                         //ͬһʱ������������������������������


    /*��ӡһЩ������Ϣ*/
    printf("client IP:%s\tport:%d\t%d\n",
           inet_ntop(AF_INET, &cli_addr.sin_addr.s_addr, clie_ip, sizeof(clie_ip)),
           ntohs(cli_addr.sin_port), cfd);
    printf("server IP:%s\tport:%d\t%d\n",
           inet_ntop(AF_INET, &serv_addr.sin_addr.s_addr, serv_ip, sizeof(serv_ip)),
           ntohs(serv_addr.sin_port), sfd);

    while (1) {
        cfd = cd_accept(sfd, (struct sockaddr *)&cli_addr, &cli_len); //ctrl+c ע��ECONNABORTED�������
        pth_st[i].cli_addr = cli_addr;              //�ͻ�����Ϣ���������󶨵�һ���Զ���Ľṹ�壬�������������߳�
        pth_st[i].cfd = cfd;

        /* �ﵽ�߳������ʱ��pthread_create������, ���ӷ������ȶ���*/
        pthread_create(&tid, NULL, pth_act, (void*)&pth_st[i]);
        pthread_detach(tid);                      //���̷߳���,��ֹ���̲߳���,Ҳ������pthread_join����
        i++;
        if(255 == i){ //256�����Լ����õģ�Ҳ����ʹ��ϵͳĬ��ֵ�� Ҳ������ulimit -s ָ��
            cd_perr_exit("pthread max error");
        }
    }

    return 0;
}
