#include "stdafx.h"
#include "utils.h"


CString HttpClient::get(CString url, int timeout)
{
	CInternetSession session;
	session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, timeout);
	CHttpFile *httpFile = (CHttpFile *)session.OpenURL(url);
	DWORD dwStatusCode;
	httpFile->QueryInfoStatusCode(dwStatusCode);
	CString result("");
	if (dwStatusCode == HTTP_STATUS_OK)
	{
		CString line;
		while (httpFile->ReadString(line))
		{
			result += line;
		}
		result.TrimRight();
	}

	httpFile->Close();
	session.Close();

	return result;
}


PUCHAR HttpClient::getData(CString url, ULONG &size, int timeout)
{
	CInternetSession session;
	session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, timeout);
	CHttpFile *httpFile = (CHttpFile *)session.OpenURL(url);
	DWORD dwStatusCode;
	httpFile->QueryInfoStatusCode(dwStatusCode);
	PUCHAR result = new UCHAR[1];
	if (dwStatusCode == HTTP_STATUS_OK)
	{
		#define LINE_LEN  1024
		ULONG totalLen = 0;
		ULONG readLen = 0;
		PUCHAR line = new UCHAR[LINE_LEN];
		
		while (readLen = httpFile->Read(line, LINE_LEN))
		{
			ULONG oldLen = totalLen;
			totalLen += readLen;
			PUCHAR cur = new UCHAR[totalLen];
			memcpy(cur, result, oldLen);
			memcpy(cur + oldLen, line, readLen);
			delete[] result;
			result = cur;
		}
		size = totalLen;
		delete[] line;
	}

	httpFile->Close();
	session.Close();

	return result;

}



void HttpClient::getFile(CString url, CInternetSession **session, CHttpFile **file, int timeout)
{
	 *session = new CInternetSession();
	(*session)->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, timeout);
	*file = (CHttpFile *)(*session)->OpenURL(url);
}


CString HttpClient::post(CString url, CString data, int timeout)
{
	if (url.Find("http://") >= 0)
		url.Delete(0, 7);

	int serverEndPos = url.Find(":");
	CString server = url.Left(serverEndPos);
	int portEndPos = url.Find("/", serverEndPos);
	CString port = url.Mid(serverEndPos + 1, portEndPos - serverEndPos -1);
	int iport = atoi(port.GetBuffer());
	CString action = url.Right(url.GetLength() - portEndPos);

	CInternetSession session;
	CHttpConnection *httpCon = session.GetHttpConnection(server, (INTERNET_PORT)iport);
	httpCon->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, timeout);

	CHttpFile *httpFile = (CHttpFile *)httpCon->OpenRequest("POST", action);
	httpFile->SendRequest(NULL, 0, data.GetBuffer(), data.GetLength());
	DWORD dwStatusCode;
	httpFile->QueryInfoStatusCode(dwStatusCode);
	CString result("");
	if (dwStatusCode == HTTP_STATUS_OK)
	{
		CString line;
		while (httpFile->ReadString(line))
		{
			result += line;
		}
		result.TrimRight();
	}

	httpFile->Close();
	session.Close();

	return result;
}




