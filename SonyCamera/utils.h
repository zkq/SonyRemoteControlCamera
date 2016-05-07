


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
class Device
{
public:

	typedef pair<CString, CString> service;
	vector<service> services;
	int reqId = 0;
	CString curShootMode;
	bool recording;
	set<CString> mAvailableCameraApiSet;
	set<CString> mSupportedApiSet;
	vector<CString> mAvailableShootMod;

	bool isJsonOk(const Document &json)
	{
		return !json.IsNull() && !json.HasMember("error");
	}

	void loadAvailableCameraApiList(const Document &replyJson);
	void loadSupportedApiList(const Document &replyJson);
	bool isCameraApiAvailable(const CString &apiName);
	boolean isApiSupported(const CString &apiName);

	CString findActionListUrl(CString service);
	Document sendApi(CString &type, Value &method, Value &params, Value &version);

	Document getApplicationInfo();
	Document getAvailableApiList();
	Document getAvcontentMethodTypes();
	Document startRecMode();
	
	//设置拍摄模式
	Document getAvailableShootMode();
	Document setShootMode(const CString &shootMode);

	//开始和暂停视频流
	Document startLiveview();
	Document stopLiveview();

	//拍照
	Document actTakePicture();

	//录制视频
	Document startMovieRec();
	Document stopMovieRec();

	//变焦
	Document actZoom(CString direction, CString movement);
};

class SsdpClient
{
public:
	bool SearchDevice();
	CString descStr;
	CString xml;
private:
	PCHAR destIP = "239.255.255.250";
	USHORT destPort = 1900;
	char *sendData = "M-SEARCH * HTTP/1.1\r\n"
		"HOST: 239.255.255.250:1900\r\n"
		"MAN: \"ssdp:discover\"\r\n"
		"MX: 1\r\n"
		"ST: urn:schemas-sony-com:service:ScalarWebAPI:1\r\n\r\n";


	CString findParameterValue(PCHAR ssdp, PCHAR param);
};




class HttpClient
{
public:
	static CString get(CString url)
	{
		return get(url, defaultTimeout);
	}
	static PUCHAR getData(CString url, ULONG &size)
	{
		return getData(url, size, defaultTimeout);
	}


	static void getFile(CString url, CInternetSession **session, CHttpFile **file)
	{
		getFile(url, session, file, defaultTimeout);
	}
	static CString post(CString url, CString data)
	{
		return post(url, data, defaultTimeout);
	}

private:
	const static int defaultTimeout = 2000;
	static CString get(CString url, int timeout);
	static PUCHAR getData(CString url, ULONG &size, int timeout);
	static void getFile(CString url, CInternetSession **session, CHttpFile **file, int timeout);
	static CString post(CString url, CString data, int timeout);
};


class Payload
{
public:
	PUCHAR jpegData = NULL;
	ULONG jpegDataSize = 0;
	PUCHAR paddingData = NULL;
	ULONG paddingDataSize = 0;
};


class LiveviewSlicer
{
public:
	~LiveviewSlicer()
	{
		close();
	}

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

	Payload nextPayload()
	{

		Payload payload;

		while (file != NULL && payload.jpegDataSize == 0) {
			//读取Common Header
			int readLength = 1 + 1 + 2 + 4;
			PUCHAR commonHeader = new UCHAR[readLength];
			file->Read(commonHeader, readLength);
			if (commonHeader[0] != (byte)0xFF) {
				//data format error
				delete[] commonHeader;
				return payload;
			}

			PUCHAR header;
			switch (commonHeader[1]) {
			case 0x12:
				readLength = 4 + 3 + 1 + 2 + 118 + 4 + 4 + 24;
				header = new UCHAR[readLength];
				file->Read(header, readLength);
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
			// Payload Header
			int readLength = 4 + 3 + 1 + 4 + 1 + 115;
			PUCHAR payloadHeader = new UCHAR[readLength];
			file->Read(payloadHeader, readLength);
			if (payloadHeader[0] != (byte)0x24 || payloadHeader[1] != (byte)0x35
				|| payloadHeader[2] != (byte)0x68 || payloadHeader[3] != (byte)0x79)
			{
				delete[] payloadHeader;
				return payload;
			}

			int jpegSize = 0;
			jpegSize += payloadHeader[4];
			jpegSize = jpegSize << 8;
			jpegSize += payloadHeader[5];
			jpegSize = jpegSize << 8;
			jpegSize += payloadHeader[6];
			int paddingSize = payloadHeader[7];
			delete[] payloadHeader;


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