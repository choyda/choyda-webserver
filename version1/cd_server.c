#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

/*
 * DESCRIPTION:
 * �����̵Ŀͻ��˽��������ܿ�������ͻ��ˡ��¸��汾����Ӷ����
 */

#define SERV_PORT 9999 //����������˿�
#define SIZEBUF 1024 //buf��С ���1500

int main(int argc, char *argv[]){

    /*����ʹ���еı���*/
    int sfd, cfd;                               //�������˺Ϳͻ��˵�socket������
    char buf[SIZEBUF];                          //��ȡд��buf�Ĵ�С
    int len, i;                                 //��ȡ���Ⱥ�ѭ������
    struct sockaddr_in serv_addr, cli_addr;     //�ͻ��˺ͷ�������bind�ṹ��
    char clie_ip[SIZEBUF], serv_ip[SIZEBUF];    //�����ӡ��Ϣip���ַ�����

    /*����socket������*/
    sfd = socket(AF_INET, SOCK_STREAM, 0);      //���һ������0���ں˻��Զ����ݳ�ʹ�õ�Э��
    if(sfd == -1){
        perror("fail to create socket");
        exit(1);
    }

    /*�˿ڸ��ã�TIME_WAIT�ȴ�����*/
    int opt = 1;
    if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt)) == -1){
        perror("fail to setsockopt socket");
        exit(1);
    }

    /*�󶨷�������ַ�ṹ*/
    socklen_t serv_len, cli_len;                //�ṹ�峤��
    serv_len = sizeof(serv_addr);               //��ȡ�ṹ�峤��
    memset(&serv_addr, 0, serv_len);            //��սṹ��
    serv_addr.sin_family = AF_INET;             //ʹ�õ�Э����
    serv_addr.sin_addr.s_addr = INADDR_ANY;     //�������κ�����
    serv_addr.sin_port = htons(SERV_PORT);      //����˿ںţ�����ת�����ֽ���Ϊ0����ϵͳ�Զ����䣬ʹ��getsockname������ϡ�

    /*��ʼ��һ����ַ�ṹ man 7 ip �鿴��Ӧ��Ϣ*/
    if(bind(sfd, (struct sockaddr *)&serv_addr, serv_len) == -1){
        perror("fail to bind socket");
        exit(1);
    }

    /*�趨��������,ע��˴�������*/
    if(listen(sfd, 10) == -1){                   //ͬһʱ������������������������������
        perror("fail to listen socket");
        exit(1);
    }

    /*��ȡ�Ѿ��������ֳɹ������󣬲���1��sfd; ��2��������, ��3���봫�����, ȫ����client�˵Ĳ���*/
    cli_len = sizeof(cli_addr);                  //��ȡ�ͻ��˽ṹ���С
    if((cfd = accept(sfd, (struct sockaddr *)&cli_addr, &cli_len)) == -1){  //�����ͻ�������, ������
        perror("fail to bind socket");
        exit(1);
    }

    /*��ӡһЩ������Ϣ*/
    printf("client IP:%s\tport:%d\t%d\n",
           inet_ntop(AF_INET, &cli_addr.sin_addr.s_addr, clie_ip, sizeof(clie_ip)),
           ntohs(cli_addr.sin_port), cfd);
    printf("server IP:%s\tport:%d\t%d\n",
           inet_ntop(AF_INET, &serv_addr.sin_addr.s_addr, serv_ip, sizeof(serv_ip)),
           ntohs(serv_addr.sin_port), sfd);

    /*��д�ļ�������*/
    while(1){
        /*��ȡ�ͻ��˷�������*/
        len = read(cfd, buf, sizeof(SIZEBUF));
        write(STDOUT_FILENO, buf, len);

        /*����ͻ�������*/
        for(i=0; i<len; i++){
            buf[i] = toupper(buf[i]);
        }

        /*���������ݻ�д���ͻ���*/
        write(cfd, buf, len);
    }

    /*�ر�����*/
    close(sfd);
    close(cfd);
    return 0;
}
