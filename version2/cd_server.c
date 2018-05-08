#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

//�Լ�����ĺ�����ͷ�ļ�
#include "cd_sock_wrap.h"
#include "cd_io_wrap.h"


/*����̿ͻ��˽���������ͬʱ֧�ֶ���ͻ�������*/

#define SERV_PORT 9999 //����������˿�
#define SIZEBUF 1024 //buf��С

void act_sin_fun(int sin_num)
{
    perror("child wait");
    while (waitpid(0, NULL, WNOHANG) > 0)
        ;
}

int main(int argc, char *argv[]){

    /*����ʹ���еı���*/
    int sfd, cfd;                               //�������˺Ϳͻ��˵�socket������
    char buf[SIZEBUF];                          //��ȡд��buf�Ĵ�С
    int len, i;                                 //��ȡ���Ⱥ�ѭ������
    struct sockaddr_in serv_addr, cli_addr;     //�ͻ��˺ͷ�������bind�ṹ��
    char clie_ip[SIZEBUF], serv_ip[SIZEBUF];    //�����ӡ��Ϣip���ַ�����
    struct sigaction signal_act;                //�����źŵĽṹ��
    int pid;                                    //����id

    /*����socket������*/
    sfd = Socket(AF_INET, SOCK_STREAM, 0);      //���һ������0���ں˻��Զ����ݳ�ʹ�õ�Э��

    /*�˿ڸ��ã�TIME_WAIT�ȴ�����*/
    rio_port_reuse(sfd);

    /*��ʼ���Žṹ�壬�����ӽ��̻��շ����źţ�Ҳ������signal�������棬���ǲ����顣*/
    signal_act.sa_handler = act_sin_fun;        //ע���źŴ�����
    sigemptyset(&signal_act.sa_mask);           //����źŵ�mask��
    signal_act.sa_flags = 0;                    //ͨ������Ϊ0����ʹ��Ĭ�����ԡ�
    sigaction(SIGCHLD, &signal_act, NULL);      //Ҫ������ź�,SIGCHLD �����ӽ����˳���

    /*�󶨷�������ַ�ṹ*/
    socklen_t serv_len, cli_len;                //�ṹ�峤��
    serv_len = sizeof(serv_addr);               //��ȡ�ṹ�峤��
    memset(&serv_addr, 0, serv_len);            //��սṹ��
    serv_addr.sin_family = AF_INET;             //ʹ�õ�Э����
    serv_addr.sin_addr.s_addr = INADDR_ANY;     //�������κ�����
    serv_addr.sin_port = htons(SERV_PORT);      //����˿ںţ�����ת�����ֽ���Ϊ0����ϵͳ�Զ����䣬ʹ��getsockname������ϡ�

    /*��ʼ��һ����ַ�ṹ man 7 ip �鿴��Ӧ��Ϣ*/
    Bind(sfd, (struct sockaddr *)&serv_addr, serv_len);

    /*�趨��������,ע��˴�������*/
    Listen(sfd, 10);                            //ͬһʱ������������������������������


    /*��ӡһЩ������Ϣ*/
    printf("client IP:%s\tport:%d\t%d\n",
           inet_ntop(AF_INET, &cli_addr.sin_addr.s_addr, clie_ip, sizeof(clie_ip)),
           ntohs(cli_addr.sin_port), cfd);
    printf("server IP:%s\tport:%d\t%d\n",
           inet_ntop(AF_INET, &serv_addr.sin_addr.s_addr, serv_ip, sizeof(serv_ip)),
           ntohs(serv_addr.sin_port), sfd);

    while (1) {
        cfd = Accept(sfd, (struct sockaddr *)&cli_addr, &cli_len); //ctrl+c ע��ECONNABORTED�������
        pid = fork(); //�����ӽ���
        if (pid == 0) {
            Close(sfd);//������ӽ��̣��رռ�������������Ϊ�ӽ��̲���Ҫ
            while (1) {
                /*��ȡ�ͻ��˷�������*/
                len = cd_read(cfd, buf, sizeof(SIZEBUF));
                if(len == 0){ //�����0 ��ʾ�ͻ��˹ر�socket
                    break;
                }
                cd_write(STDOUT_FILENO, buf, len);

                /*����ͻ�������*/
                for (i = 0; i < len; i++) {
                    buf[i] = toupper(buf[i]);
                }

                /*���������ݻ�д���ͻ���*/
                cd_write(cfd, buf, len);
            }

            cd_close(cfd); //��д��Ϲرտͻ���������
            return 0;
        }else if(pid > 0){ //����Ǹ����̣��رտͻ�������������������accept,��ȡ�ͻ���������
            cd_close(cfd);
        }else{
            exit(1); //�����forkʧ��ֱ���˳�
        }
    }
    return 0;
}
