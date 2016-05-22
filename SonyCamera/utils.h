


#include "stdafx.h"
#include <afxinet.h>
#include <vector>
#include <set>
#include <utility>
#include "rapidjson\document.h"

using namespace rapidjson;
using std::vector;
using std::pair;
using std::set;

#pragma once
//代表一个建立连接的设备  封装了设备状态和设备支持的操作
class Device
{
public:
	//设备支持的服务种类
	typedef pair<CString, CString> service;
	vector<service> services;

	//一次设备操作请求的id
	int reqId = 0;
	//设备当前拍摄模式   still或者movie
	CString curShootMode;
	//设备是否正在录制
	bool recording;
	//设备可用的api（拍摄相关）
	set<CString> mAvailableCameraApiSet;
	//设备支持的api（系统设置相关）
	set<CString> mSupportedApiSet;
	//设备支持的拍摄模式  still movie等
	vector<CString> mAvailableShootMod;

	//检查返回的json是否代表操作成功
	bool isJsonOk(const Document &json)
	{
		return !json.IsNull() && !json.HasMember("error");
	}

	//将可用api保存到mAvailableCameraApiSet
	void loadAvailableCameraApiList(const Document &replyJson);
	//将支持api保存到mSupportedApiSet
	void loadSupportedApiList(const Document &replyJson);
	//检查给定的api是否可用
	bool isCameraApiAvailable(const CString &apiName);
	//检查给定的api是否支持
	boolean isApiSupported(const CString &apiName);

	//获取指定服务名称的操作地址
	CString findActionListUrl(CString service);
	//发送json命令的基础函数
	Document sendApi(CString &type, Value &method, Value &params, Value &version);

	//获取设备version信息
	Document getApplicationInfo();
	//获取可用api
	Document getAvailableApiList();
	//获取支持的api
	Document getAvcontentMethodTypes();
	//发送startRecMode命令到设备（此设备不需要发送此命令）
	Document startRecMode();
	
	/*******设置拍摄模式相关函数***********/
	//获取可用拍摄模式
	Document getAvailableShootMode();
	//设置拍摄模式
	Document setShootMode(const CString &shootMode);

	/*******视频流相关函数**************/
	//开始获取liveview数据流
	Document startLiveview();
	//停止获取liveview视频流
	Document stopLiveview();

	//拍照
	Document getAvailableStillSize();
	Document setStillSize();
	Document setPostviewImageSize();
	Document actTakePicture();

	//录制视频
	Document startMovieRec();
	Document stopMovieRec();

	//变焦
	Document actZoom(CString direction, CString movement);

	//ISO
	Document getAvailableIsoSpeedRate();
	Document setIsoSpeedRate(CString rate);

	//曝光
	Document getAvailableExposureMode();
	Document setExposureMode(CString mode);
};


//简单服务发现协议类
class SsdpClient
{
public:
	//搜寻服务设备
	bool SearchDevice();
	//设备描述符地址
	CString descStr;
	//设备描述符
	CString xml;
private:
	//ssdp协议地址
	PCHAR destIP = "239.255.255.250";
	//ssdp协议端口
	USHORT destPort = 1900;
	//ssdp协议数据格式
	char *sendData = "M-SEARCH * HTTP/1.1\r\n"
		"HOST: 239.255.255.250:1900\r\n"
		"MAN: \"ssdp:discover\"\r\n"
		"MX: 1\r\n"
		"ST: urn:schemas-sony-com:service:ScalarWebAPI:1\r\n\r\n";

	//从返回的json中获取指定信息
	CString findParameterValue(PCHAR ssdp, PCHAR param);
};



//负责进行http传输的类
class HttpClient
{
public:
	//使用get方法获取文本信息
	static CString get(CString url)
	{
		return get(url, defaultTimeout);
	}
	//使用get方法获取二进制信息
	static PUCHAR getData(CString url, ULONG &size)
	{
		return getData(url, size, defaultTimeout);
	}

	//将httpfile对象传递出来  供liveview类使用
	static void getFile(CString url, CInternetSession **session, CHttpFile **file)
	{
		getFile(url, session, file, defaultTimeout);
	}

	//使用post方法传输文本信息 如使用json发送命令到相机
	static CString post(CString url, CString data)
	{
		return post(url, data, defaultTimeout);
	}

private:
	//默认超时时间
	const static int defaultTimeout = 2000;
	static CString get(CString url, int timeout);
	static PUCHAR getData(CString url, ULONG &size, int timeout);
	static void getFile(CString url, CInternetSession **session, CHttpFile **file, int timeout);
	static CString post(CString url, CString data, int timeout);
};



//代表一个liveview数据的类
class Payload
{
public:
	PUCHAR jpegData = NULL;
	ULONG jpegDataSize = 0;
	PUCHAR paddingData = NULL;
	ULONG paddingDataSize = 0;
};



//获取Liveview数据并解析其中的jpg数据的类
class LiveviewSlicer
{
public:
	~LiveviewSlicer()
	{
		close();
	}

	//打开获取liveview数据的连接
	bool open(CString url)
	{
		HttpClient::getFile(url, &session, &file);

		DWORD dwStatusCode;
		file->QueryInfoStatusCode(dwStatusCode);
		if (dwStatusCode == HTTP_STATUS_OK)
		{
			return true;
		}
		else
		{
			session->Close();
			file->Close();
			session = NULL;
			file = NULL;
			return false;
		}
	}

	void close()
	{
		if (session)
		{
			session->Close();
			session = NULL;
		}
		if (file)
		{
			file->Close();
			file = NULL;
		}
	}


	//获取下一个jpeg图片数据
	Payload nextPayload()
	{

		Payload payload;

		while (file != NULL && payload.jpegDataSize == 0) {
			//读取Common Header
			int readLength = 1 + 1 + 2 + 4;
			PUCHAR commonHeader = new UCHAR[readLength];
			int readLen = 0;
			while (readLen < readLength)
			{
				readLen += file->Read(commonHeader + readLen, readLength - readLen);
			}
			if (commonHeader[0] != (byte)0xFF) {
				//data format error
				delete[] commonHeader;
				return payload;
			}

			PUCHAR header;

			//根据common header辨别数据包类型
			switch (commonHeader[1]) {
			case 0x12:
				readLength = 4 + 3 + 1 + 2 + 118 + 4 + 4 + 24;
				header = new UCHAR[readLength];
				readLen = 0;
				while (readLen < readLength)
				{
					readLen += file->Read(header + readLen, readLength - readLen);
				}
				delete[] header;
				break;
			case 0x01:
			case 0x11:
				payload = readPayload();
				break;
			default:
				break;
			}
			delete[] commonHeader;
		}
		return payload;
	}

	Payload readPayload()
	{
		Payload payload;
		if (file != NULL) {
			//读取payload header
			int readLength = 4 + 3 + 1 + 4 + 1 + 115;
			PUCHAR payloadHeader = new UCHAR[readLength];
			int readLen = 0;
			while (readLen < readLength)
			{
				readLen += file->Read(payloadHeader + readLen, readLength - readLen);
			}
			if (payloadHeader[0] != (byte)0x24 || payloadHeader[1] != (byte)0x35
				|| payloadHeader[2] != (byte)0x68 || payloadHeader[3] != (byte)0x79)
			{
				delete[] payloadHeader;
				return payload;
			}

			//计算jpg图片大小并存在payload对象中
			int jpegSize = 0;
			jpegSize += payloadHeader[4];
			jpegSize = jpegSize << 8;
			jpegSize += payloadHeader[5];
			jpegSize = jpegSize << 8;
			jpegSize += payloadHeader[6];
			int paddingSize = payloadHeader[7];
			delete[] payloadHeader;

			//实例一个payload存储大小
			Payload payload;
			payload.jpegDataSize = jpegSize;
			payload.paddingDataSize = paddingSize;
			return payload;
		}
		return payload;
	}

public:
	CInternetSession *session = NULL;
	CHttpFile *file = NULL;

};