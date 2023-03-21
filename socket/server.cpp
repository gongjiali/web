#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
 
int main(int argc,char *argv[])
{
  if (argc!=2)
  {
    printf("Using:./server port\nExample:./server 5005\n\n"); return -1;
  }

  // ��1������������˵�socket��
  int listenfd;
  if ( (listenfd = socket(AF_INET,SOCK_STREAM,0))==-1) { perror("socket"); return -1; }
  
  // ��2�����ѷ��������ͨ�ŵĵ�ַ�Ͷ˿ڰ󶨵�socket�ϡ�
  struct sockaddr_in servaddr;    // ����˵�ַ��Ϣ�����ݽṹ��
  memset(&servaddr,0,sizeof(servaddr));
  servaddr.sin_family = AF_INET;  // Э���壬��socket�����ֻ����AF_INET��
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);          // ����ip��ַ��
  //servaddr.sin_addr.s_addr = inet_addr("192.168.190.134"); // ָ��ip��ַ��
  servaddr.sin_port = htons(atoi(argv[1]));  // ָ��ͨ�Ŷ˿ڡ�
  if (bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) != 0 )
  { perror("bind"); close(listenfd); return -1; }
 
  // ��3������socket����Ϊ����ģʽ��
  if (listen(listenfd,5) != 0 ) { perror("listen"); close(listenfd); return -1; }
 
  // ��4�������ܿͻ��˵����ӡ�
  int  clientfd;                  // �ͻ��˵�socket��
  int  socklen=sizeof(struct sockaddr_in); // struct sockaddr_in�Ĵ�С
  struct sockaddr_in clientaddr;  // �ͻ��˵ĵ�ַ��Ϣ��
  clientfd=accept(listenfd,(struct sockaddr *)&clientaddr,(socklen_t*)&socklen);
  printf("�ͻ��ˣ�%s�������ӡ�\n",inet_ntoa(clientaddr.sin_addr));
 
  // ��5������ͻ���ͨ�ţ����տͻ��˷������ı��ĺ󣬻ظ�ok��
  char buffer[1024];
  while (1)
  {
    int iret;
    memset(buffer,0,sizeof(buffer));
    if ( (iret=recv(clientfd,buffer,sizeof(buffer),0))<=0) // ���տͻ��˵������ġ�
    {
       printf("iret=%d\n",iret); break;   
    }
    printf("���գ�%s\n",buffer);
 
    strcpy(buffer,"ok");
    if ( (iret=send(clientfd,buffer,strlen(buffer),0))<=0) // ��ͻ��˷�����Ӧ�����
    { perror("send"); break; }
    printf("���ͣ�%s\n",buffer);
  }
 
  // ��6�����ر�socket���ͷ���Դ��
  close(listenfd); close(clientfd); 
}
