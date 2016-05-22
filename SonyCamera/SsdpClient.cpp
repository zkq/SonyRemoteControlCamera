
#include "stdafx.h"
#include "SonyCamera.h"
#include "SonyCameraDlg.h"

bool SsdpClient::SearchDevice()
{
	//加载socket库
	WSADATA wsadata;
	WSAStartup(0x0201, &wsadata);
	int iret;

	//设置socket超时时间为3秒
	SOCKET connectSocket = socket(AF_INET, SOCK_DGRAM, 0);
	int time = 3000;
	iret = setsockopt(connectSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&time, sizeof(time));
	if (iret < 0)
	{
		return false;
	}

	//设置ip地址
	SOCKADDR_IN addrin;
	ZeroMemory(&addrin, sizeof(addrin));
	addrin.sin_family = AF_INET;
	addrin.sin_addr.s_addr = inet_addr(destIP);
	addrin.sin_port = htons(destPort);

	//发送三次ssdp信息
	iret = sendto(connectSocket, sendData, strlen(sendData), 0, (sockaddr*)&addrin, sizeof(addrin));
	Sleep(100);
	iret = sendto(connectSocket, sendData, strlen(sendData), 0, (sockaddr*)&addrin, sizeof(addrin));
	Sleep(100);
	iret = sendto(connectSocket, sendData, strlen(sendData), 0, (sockaddr*)&addrin, sizeof(addrin));
	Sleep(100);
	if (iret < 0)
	{
		return false;
	}
	//读取设备返回的信息
	int recvbuflen = 512;
	int fromLen;
	char revbuf[512];
	iret = recv(connectSocket, revbuf, 512, 0);
	if (iret < 0)
	{
		return false;
	}

	//获取设备描述符地址
	if (!(descStr = findParameterValue(revbuf, "LOCATION:")))
	{
		return false;
	}
	
	//使用返回的设备描述符地址 获取设备描述
	HttpClient httpClient;
	xml = httpClient.get(descStr);
	if ( !xml || xml.IsEmpty())
	{
		return false;
	}

	return true;
}

//从ssdp返回信息提取参数
CString SsdpClient::findParameterValue(PCHAR ssdp, PCHAR param) {
	CString ssdpMessage(ssdp);
	CString paramName(param);

	int start = ssdpMessage.Find(param, 0);
	int end = ssdpMessage.Find("\r\n", start);

	if (start != -1 && end != -1) {
		start += strlen(param);
		CString val = ssdpMessage.Mid(start, end - start);
		if (!val.IsEmpty()) {
			return val.Trim();
		}
	}
	return NULL;
}