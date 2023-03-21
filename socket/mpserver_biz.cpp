#include "_freecplus.h"
 
CLogFile logfile;
CTcpServer TcpServer;   // ��������˶���

// �����˳�ʱ���õĺ���
void FathEXIT(int sig);   // �������˳�������
void ChldEXIT(int sig);   // �ӽ����˳�������

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
    printf("Using:./mpserver_biz port logfile\nExample:./mpserver_biz 5005 /tmp/mpserver_biz.log\n\n"); return -1;
  }

  // �ر�ȫ�����ź�
  for (int ii=0;ii<100;ii++) signal(ii,SIG_IGN);

  // ����־�ļ���
  if (logfile.Open(argv[2],"a+")==false) { printf("logfile.Open(%s) failed.\n",argv[2]); return -1;}

  // �����ź�,��shell״̬�¿��� "kill + ���̺�" ������ֹЩ����
  // ���벻Ҫ�� "kill -9 +���̺�" ǿ����ֹ
  signal(SIGINT,FathEXIT); signal(SIGTERM,FathEXIT);
 
  if (TcpServer.InitServer(atoi(argv[1]))==false) // ��ʼ��TcpServer��ͨ�Ŷ˿ڡ�
  {
    logfile.Write("TcpServer.InitServer(%s) failed.\n",argv[1]); FathEXIT(-1);
  }
 
  while (true)
  {
    if (TcpServer.Accept()==false)   // �ȴ��ͻ������ӡ�
    {
      logfile.Write("TcpServer.Accept() failed.\n"); continue;
    }

    if (fork()>0) { TcpServer.CloseClient(); continue; } // �����̷��ص�ѭ���ײ���

    // �ӽ������������˳��źš�
    signal(SIGINT,ChldEXIT); signal(SIGTERM,ChldEXIT);

    TcpServer.CloseListen();
 
    // �������ӽ��̣�������ͻ���ͨ�š�
    logfile.Write("�ͻ���(%s)�����ӡ�\n",TcpServer.GetIP());
 
    char strrecvbuffer[1024],strsendbuffer[1024];  // ������ݵĻ�������
 
    while (true)
    {
      memset(strrecvbuffer,0,sizeof(strrecvbuffer));
      memset(strsendbuffer,0,sizeof(strsendbuffer));

      if (TcpServer.Read(strrecvbuffer,30)==false) break; // ���տͻ��˷������������ġ�
      logfile.Write("���գ�%s\n",strrecvbuffer);

      // ����ҵ�����������
      if (_main(strrecvbuffer,strsendbuffer)==false) ChldEXIT(-1); 
 
      logfile.Write("���ͣ�%s\n",strsendbuffer);
      if (TcpServer.Write(strsendbuffer)==false) break;     // ��ͻ��˻�Ӧ���ġ�
    }
 
    logfile.Write("�ͻ����ѶϿ���\n");    // ����ֱ���˳��������������ͷ���Դ��

    ChldEXIT(-1);  // ͨ����ɺ��ӽ����˳���
  }
}

// �������˳�ʱ���õĺ���
void FathEXIT(int sig)
{
  if (sig > 0)
  {
    signal(sig,SIG_IGN); signal(SIGINT,SIG_IGN); signal(SIGTERM,SIG_IGN);
    logfile.Write("catching the signal(%d).\n",sig);
  }

  kill(0,15);  // ֪ͨ�������ӽ����˳���

  logfile.Write("�������˳���\n");

  // ��д�ƺ���루�ͷ���Դ���ύ��ع�����
  TcpServer.CloseClient();

  exit(0);
}

// �ӽ����˳�ʱ���õĺ���
void ChldEXIT(int sig)
{
  if (sig > 0)
  {
    signal(sig,SIG_IGN); signal(SIGINT,SIG_IGN); signal(SIGTERM,SIG_IGN);
  }

  logfile.Write("�ӽ����˳���\n");

  // ��д�ƺ���루�ͷ���Դ���ύ��ع�����
  TcpServer.CloseClient();

  exit(0);
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
