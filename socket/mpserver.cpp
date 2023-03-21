#include "_freecplus.h"
 
CLogFile logfile;       // ��������������־��
CTcpServer TcpServer;   // ��������˶���

// �����˳�ʱ���õĺ���
void FathEXIT(int sig);   // �������˳�������
void ChldEXIT(int sig);   // �ӽ����˳�������

int main(int argc,char *argv[])
{
  if (argc!=3)
  {
    printf("Using:./mpserver port logfile\nExample:./mpserver 5005 /tmp/mpserver.log\n\n"); return -1;
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
    // ctrl-c���ź�           kill���ź�
    signal(SIGINT,ChldEXIT); signal(SIGTERM,ChldEXIT);

    TcpServer.CloseListen();
 
    // �������ӽ��̣�������ͻ���ͨ�š�
    logfile.Write("�ͻ���(%s)�����ӡ�\n",TcpServer.GetIP());
 
    char strbuffer[1024];  // ������ݵĻ�������
 
    while (true)
    {
      memset(strbuffer,0,sizeof(strbuffer));
      if (TcpServer.Read(strbuffer,50)==false) break; // ���տͻ��˷������������ġ�
      logfile.Write("���գ�%s\n",strbuffer);
 
      strcat(strbuffer,"ok");      // �ڿͻ��˵ı��ĺ����"ok"��
      logfile.Write("���ͣ�%s\n",strbuffer);
      if (TcpServer.Write(strbuffer)==false) break;     // ��ͻ��˻�Ӧ���ġ�
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
