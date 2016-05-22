


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
//����һ���������ӵ��豸  ��װ���豸״̬���豸֧�ֵĲ���
class Device
{
public:
	//�豸֧�ֵķ�������
	typedef pair<CString, CString> service;
	vector<service> services;

	//һ���豸���������id
	int reqId = 0;
	//�豸��ǰ����ģʽ   still����movie
	CString curShootMode;
	//�豸�Ƿ�����¼��
	bool recording;
	//�豸���õ�api��������أ�
	set<CString> mAvailableCameraApiSet;
	//�豸֧�ֵ�api��ϵͳ������أ�
	set<CString> mSupportedApiSet;
	//�豸֧�ֵ�����ģʽ  still movie��
	vector<CString> mAvailableShootMod;

	//��鷵�ص�json�Ƿ��������ɹ�
	bool isJsonOk(const Document &json)
	{
		return !json.IsNull() && !json.HasMember("error");
	}

	//������api���浽mAvailableCameraApiSet
	void loadAvailableCameraApiList(const Document &replyJson);
	//��֧��api���浽mSupportedApiSet
	void loadSupportedApiList(const Document &replyJson);
	//��������api�Ƿ����
	bool isCameraApiAvailable(const CString &apiName);
	//��������api�Ƿ�֧��
	boolean isApiSupported(const CString &apiName);

	//��ȡָ���������ƵĲ�����ַ
	CString findActionListUrl(CString service);
	//����json����Ļ�������
	Document sendApi(CString &type, Value &method, Value &params, Value &version);

	//��ȡ�豸version��Ϣ
	Document getApplicationInfo();
	//��ȡ����api
	Document getAvailableApiList();
	//��ȡ֧�ֵ�api
	Document getAvcontentMethodTypes();
	//����startRecMode����豸�����豸����Ҫ���ʹ����
	Document startRecMode();
	
	/*******��������ģʽ��غ���***********/
	//��ȡ��������ģʽ
	Document getAvailableShootMode();
	//��������ģʽ
	Document setShootMode(const CString &shootMode);

	/*******��Ƶ����غ���**************/
	//��ʼ��ȡliveview������
	Document startLiveview();
	//ֹͣ��ȡliveview��Ƶ��
	Document stopLiveview();

	//����
	Document getAvailableStillSize();
	Document setStillSize();
	Document setPostviewImageSize();
	Document actTakePicture();

	//¼����Ƶ
	Document startMovieRec();
	Document stopMovieRec();

	//�佹
	Document actZoom(CString direction, CString movement);

	//ISO
	Document getAvailableIsoSpeedRate();
	Document setIsoSpeedRate(CString rate);

	//�ع�
	Document getAvailableExposureMode();
	Document setExposureMode(CString mode);
};


//�򵥷�����Э����
class SsdpClient
{
public:
	//��Ѱ�����豸
	bool SearchDevice();
	//�豸��������ַ
	CString descStr;
	//�豸������
	CString xml;
private:
	//ssdpЭ���ַ
	PCHAR destIP = "239.255.255.250";
	//ssdpЭ��˿�
	USHORT destPort = 1900;
	//ssdpЭ�����ݸ�ʽ
	char *sendData = "M-SEARCH * HTTP/1.1\r\n"
		"HOST: 239.255.255.250:1900\r\n"
		"MAN: \"ssdp:discover\"\r\n"
		"MX: 1\r\n"
		"ST: urn:schemas-sony-com:service:ScalarWebAPI:1\r\n\r\n";

	//�ӷ��ص�json�л�ȡָ����Ϣ
	CString findParameterValue(PCHAR ssdp, PCHAR param);
};



//�������http�������
class HttpClient
{
public:
	//ʹ��get������ȡ�ı���Ϣ
	static CString get(CString url)
	{
		return get(url, defaultTimeout);
	}
	//ʹ��get������ȡ��������Ϣ
	static PUCHAR getData(CString url, ULONG &size)
	{
		return getData(url, size, defaultTimeout);
	}

	//��httpfile���󴫵ݳ���  ��liveview��ʹ��
	static void getFile(CString url, CInternetSession **session, CHttpFile **file)
	{
		getFile(url, session, file, defaultTimeout);
	}

	//ʹ��post���������ı���Ϣ ��ʹ��json����������
	static CString post(CString url, CString data)
	{
		return post(url, data, defaultTimeout);
	}

private:
	//Ĭ�ϳ�ʱʱ��
	const static int defaultTimeout = 2000;
	static CString get(CString url, int timeout);
	static PUCHAR getData(CString url, ULONG &size, int timeout);
	static void getFile(CString url, CInternetSession **session, CHttpFile **file, int timeout);
	static CString post(CString url, CString data, int timeout);
};



//����һ��liveview���ݵ���
class Payload
{
public:
	PUCHAR jpegData = NULL;
	ULONG jpegDataSize = 0;
	PUCHAR paddingData = NULL;
	ULONG paddingDataSize = 0;
};



//��ȡLiveview���ݲ��������е�jpg���ݵ���
class LiveviewSlicer
{
public:
	~LiveviewSlicer()
	{
		close();
	}

	//�򿪻�ȡliveview���ݵ�����
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


	//��ȡ��һ��jpegͼƬ����
	Payload nextPayload()
	{

		Payload payload;

		while (file != NULL && payload.jpegDataSize == 0) {
			//��ȡCommon Header
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

			//����common header������ݰ�����
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
			//��ȡpayload header
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

			//����jpgͼƬ��С������payload������
			int jpegSize = 0;
			jpegSize += payloadHeader[4];
			jpegSize = jpegSize << 8;
			jpegSize += payloadHeader[5];
			jpegSize = jpegSize << 8;
			jpegSize += payloadHeader[6];
			int paddingSize = payloadHeader[7];
			delete[] payloadHeader;

			//ʵ��һ��payload�洢��С
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