#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

/*
 * DESCRIPTION:
 * Linux�������µ����̿ͻ��˺ͷ������˽�����ʵ�ֻ��Թ��ܣ����ܿ�������ͻ��ˡ�
 */

#define SERV_PORT 9999 //����������˿�
#define SERV_IP "127.0.0.1"

int main(int argc, char *argv[]){

    /*����ʹ���еı���*/
    int sfd;                                    //�ͻ��˵�socket������
    char buf[BUFSIZ];                          //��ȡд���buf
    int len;                                    //��ȡ����
    struct sockaddr_in serv_addr, cli_addr;     //�ͻ��˺ͷ�������bind�ṹ��
    char cli_ip[BUFSIZ], serv_ip[BUFSIZ];    //�����ӡ��Ϣip���ַ�����

    /*����socket������*/
    sfd = socket(AF_INET, SOCK_STREAM, 0);      //���һ������0���ں˻��Զ����ݳ�ʹ�õ�Э��
    if(sfd == -1){
        perror("fail to create socket");
        exit(1);
    }

    /*�󶨷�������ַ�ṹ*/
    socklen_t serv_len;                         //�ṹ�峤��
    serv_len = sizeof(serv_addr);               //��ȡ�ṹ�峤��
    memset(&serv_addr, 0, serv_len);            //��սṹ��
    serv_addr.sin_family = AF_INET;             //ʹ�õ�Э����
    inet_pton(AF_INET, SERV_IP, &serv_addr.sin_addr.s_addr);  //ָ��IP �ַ�������ת��Ϊ�����ֽ��� ��3:��������
    serv_addr.sin_port = htons(SERV_PORT);      //ָ���˿� ����ת�����ֽ���

    /*���ݵ�ַ�ṹ����ָ������������*/
    if(connect(sfd, (struct sockaddr *) &serv_addr, serv_len) == -1){
        perror("fail to create socket");
        exit(1);
    }

    /*��ӡһЩ������Ϣ*/
    printf("client IP:%s\tport:%d\t%d\n",
           inet_ntop(AF_INET, &cli_addr.sin_addr.s_addr, cli_ip, sizeof(cli_ip)),
           ntohs(cli_addr.sin_port), sfd);
    printf("server IP:%s\tport:%d\t%d\n",
           inet_ntop(AF_INET, &serv_addr.sin_addr.s_addr, serv_ip, sizeof(serv_ip)),
           ntohs(serv_addr.sin_port), sfd);

    /*��д�ļ�������*/
    while (1) {
        /*�ӱ�׼�����ȡ����*/
        fgets(buf, sizeof(buf), stdin);
        /*������д��������*/
        write(sfd, buf, strlen(buf));
        /*�ӷ���������ת��������*/
        len = read(sfd, buf, sizeof(buf));
        /*д����׼���*/
        write(STDOUT_FILENO, buf, len);
    }
    /*�ر�����*/
    close(sfd);
    return 0;
}
