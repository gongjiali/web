#include "_freecplus.h"

void *pthmain(void *arg);  // �߳���������

vector<long> vpthid;  // ����߳�id��������

void mainexit(int sig);   // �ź�2��15�Ĵ���������

void pthmainexit(void *arg); // �߳�����������
 
CLogFile logfile;       // ��������������־��
CTcpServer TcpServer;   // ��������˶���

// ����ҵ�����������
bool _main(const char *strrecvbuffer,char *strsendbuffer);

// �������ġ�
bool biz000(const char *strrecvbuffer,char *strsendbuffer);

// ������֤ҵ����������
bool biz001(const char *strrecvbuffer,char *strsendbuffer);

// ��ѯ���ҵ����������
bool biz002(const char *strrecvbuffer,char *strsendbuffer);

int main(int argc,char *argv[])
{
  if (argc!=3)
  {
    printf("Using:./mtserver_biz port logfile\nExample:./mtserver_biz 5005 /tmp/mtserver_biz.log\n\n"); return -1;
  }

  // �ر�ȫ�����ź�
  for (int ii=0;ii<100;ii++) signal(ii,SIG_IGN);

  // ����־�ļ���
  if (logfile.Open(argv[2],"a+")==false) { printf("logfile.Open(%s) failed.\n",argv[2]); return -1;}

  // �����ź�,��shell״̬�¿��� "kill + ���̺�" ������ֹЩ����
  // ���벻Ҫ�� "kill -9 +���̺�" ǿ����ֹ
  signal(SIGINT,mainexit); signal(SIGTERM,mainexit);
 
  if (TcpServer.InitServer(atoi(argv[1]))==false) // ��ʼ��TcpServer��ͨ�Ŷ˿ڡ�
  {
    logfile.Write("TcpServer.InitServer(%s) failed.\n",argv[1]); return -1;
  }
 
  while (true)
  {
    if (TcpServer.Accept()==false)   // �ȴ��ͻ������ӡ�
    {
      logfile.Write("TcpServer.Accept() failed.\n"); continue;
    }

    logfile.Write("�ͻ���(%s)�����ӡ�\n",TcpServer.GetIP());

    pthread_t pthid;
    if (pthread_create(&pthid,NULL,pthmain,(void *)(long)TcpServer.m_connfd)!=0)
    { logfile.Write("pthread_create failed.\n"); return -1; }

    vpthid.push_back(pthid); // ���߳�id���浽vpthid�����С�
  }

  return 0;
}

void *pthmain(void *arg)
{
  pthread_cleanup_push(pthmainexit,arg);  // �����߳�����������

  pthread_detach(pthread_self());  // �����̡߳�

  pthread_setcanceltype(PTHREAD_CANCEL_DISABLE,NULL);  // ����ȡ����ʽΪ����ȡ����

  int sockfd=(int)(long)arg;  // ��ͻ��˵�socket���ӡ�

  int  ibuflen=0;
  char strrecvbuffer[1024],strsendbuffer[1024];  // ������ݵĻ�������
 
  while (true)
  {
    memset(strrecvbuffer,0,sizeof(strrecvbuffer));
    memset(strsendbuffer,0,sizeof(strsendbuffer));

    if (TcpRead(sockfd,strrecvbuffer,&ibuflen,50)==false) break; // ���տͻ��˷������������ġ�
    logfile.Write("���գ�%s\n",strrecvbuffer);
 
    // ����ҵ�����������
    if (_main(strrecvbuffer,strsendbuffer)==false) break;

    logfile.Write("���ͣ�%s\n",strsendbuffer);

    if (TcpWrite(sockfd,strsendbuffer)==false) break;     // ��ͻ��˻�Ӧ���ġ�
  }

  pthread_cleanup_pop(1);

  pthread_exit(0);
}

// �ź�2��15�Ĵ���������
void mainexit(int sig)
{
  logfile.Write("mainexit begin.\n");

  // �رռ�����socket��
  TcpServer.CloseListen();

  // ȡ��ȫ�����̡߳�
  for (int ii=0;ii<vpthid.size();ii++)
  {
    logfile.Write("cancel %ld\n",vpthid[ii]);
    pthread_cancel(vpthid[ii]);
  }

  logfile.Write("mainexit end.\n");

  exit(0);
}

// �߳�����������
void pthmainexit(void *arg)
{
  logfile.Write("pthmainexit begin.\n");

  // �ر���ͻ��˵�socket��
  close((int)(long)arg);

  // ��vpthid��ɾ�����̵߳�id��
  for (int ii=0;ii<vpthid.size();ii++)
  {
    if (vpthid[ii]==pthread_self())
    {
      vpthid.erase(vpthid.begin()+ii);
    }
  }

 logfile.Write("pthmainexit end.\n");
}

bool _main(const char *strrecvbuffer,char *strsendbuffer)  // ����ҵ�����������
{
  int ibizcode=-1;
  GetXMLBuffer(strrecvbuffer,"bizcode",&ibizcode);

  switch (ibizcode)
  {
    case 0:  // ����
      biz000(strrecvbuffer,strsendbuffer); break;
    case 1:  // ������֤��
      biz001(strrecvbuffer,strsendbuffer); break;
    case 2:  // ��ѯ��
      biz002(strrecvbuffer,strsendbuffer); break;

    default:
      logfile.Write("�Ƿ����ģ�%s\n",strrecvbuffer); return false;
  }

  return true;
}

// ������֤ҵ����������
bool biz001(const char *strrecvbuffer,char *strsendbuffer)
{
  char username[51],password[51];
  memset(username,0,sizeof(username));
  memset(password,0,sizeof(password));

  GetXMLBuffer(strrecvbuffer,"username",username,50);
  GetXMLBuffer(strrecvbuffer,"password",password,50);

  if ( (strcmp(username,"wucz")==0) && (strcmp(password,"p@ssw0rd")==0) )
    sprintf(strsendbuffer,"<retcode>0</retcode><message>�ɹ���</message>");
  else
    sprintf(strsendbuffer,"<retcode>-1</retcode><message>�û��������벻��ȷ��</message>");

  return true;
}

// ��ѯ���ҵ����������
bool biz002(const char *strrecvbuffer,char *strsendbuffer)
{
  char cardid[51];
  memset(cardid,0,sizeof(cardid));

  GetXMLBuffer(strrecvbuffer,"cardid",cardid,50);

  if (strcmp(cardid,"62620000000001")==0)
    sprintf(strsendbuffer,"<retcode>0</retcode><message>�ɹ���</message><ye>100.50</ye>");
  else
    sprintf(strsendbuffer,"<retcode>-1</retcode><message>���Ų����ڡ�</message>");

  return true;
}

// ��������
bool biz000(const char *strrecvbuffer,char *strsendbuffer)
{
  sprintf(strsendbuffer,"<retcode>0</retcode><message>�ɹ���</message>");

  return true;
}


