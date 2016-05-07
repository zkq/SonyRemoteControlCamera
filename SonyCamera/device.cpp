
#include "stdafx.h"
#include "utils.h"
#include "rapidjson\stringbuffer.h"
#include "rapidjson\writer.h"


void Device::loadAvailableCameraApiList(const Document &replyJson)
{
	mAvailableCameraApiSet.clear();
	if (replyJson == NULL || replyJson.IsNull())
		return;
	if (replyJson.HasMember("error"))
	{
		return;
	}

	Value::ConstArray a = replyJson["result"].GetArray()[0].GetArray();

	for (Value::ConstValueIterator itr = a.Begin(); itr != a.End(); ++itr)
	{
		mAvailableCameraApiSet.insert(itr->GetString());
	}
}


void Device::loadSupportedApiList(const Document &replyJson)
{
	mSupportedApiSet.clear();
	if (replyJson == NULL || replyJson.IsNull())
		return;
	if (replyJson.HasMember("error"))
	{
		return;
	}
	Value::ConstArray a = replyJson["result"].GetArray()[0].GetArray();

	for (Value::ConstValueIterator itr = a.Begin(); itr != a.End(); ++itr)
	{
		mSupportedApiSet.insert(itr->GetString());
	}
}




bool Device::isCameraApiAvailable(const CString &apiName)
{
	auto a = mAvailableCameraApiSet.find(apiName);

	return a != mAvailableCameraApiSet.end();
}

boolean Device::isApiSupported(const CString &apiName)
{
	auto a = mSupportedApiSet.find(apiName);

	return a != mSupportedApiSet.end();
}



CString Device::findActionListUrl(CString service)
{
	for (auto ite = services.begin(); ite != services.end(); ite++)
	{
		if (ite->first == service)
		{
			return ite->second;
		}
	}
	return NULL;
}

Document Device::sendApi(CString &type, Value &method, Value &params, Value &version)
{
	CString actionurl = findActionListUrl(type);
	if (!actionurl || actionurl.IsEmpty())
	{
		return NULL;
	}
	CString url = actionurl + "/" + type;

	Document requestJson;
	requestJson.SetObject();
	Document::AllocatorType& allocator = requestJson.GetAllocator();
	
	requestJson.AddMember("method", method, allocator);
	requestJson.AddMember("params", params, allocator);
	requestJson.AddMember("id", ++reqId, allocator);
	requestJson.AddMember("version", version, allocator);
	
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	requestJson.Accept(writer);
	const char* output = buffer.GetString();

	CString response = HttpClient::post(url, CString(output));
	if (!response || response.IsEmpty())
	{
		return NULL;
	}

	Document responseJson;
	responseJson.Parse(response);
	return responseJson;
}


//***********************************API Impletation*****************************
Document Device::getApplicationInfo()
{
	CString type = "camera";
	Value method;
	method = "getApplicationInfo";
	Value params;
	params.SetArray();
	Value version;
	version = "1.0";
	return sendApi(type, method, params, version);	
}

Document Device::getAvailableApiList()
{
	CString type = "camera";
	Value method;
	method = "getAvailableApiList";
	Value params;
	params.SetArray();
	Value version;
	version = "1.0";
	return sendApi(type, method, params, version);
}

Document Device::getAvcontentMethodTypes()
{
	CString type = "avContent";
	Value method;
	method = "getMethodTypes";
	Value params;
	params.SetArray();
	Value version;
	version = "1.0";
	return sendApi(type, method, params, version);
}


Document Device::startRecMode()
{
	CString type = "camera";
	Value method;
	method = "startRecMode";
	Value params;
	params.SetArray();
	Value version;
	version = "1.0";
	return sendApi(type, method, params, version);
}

Document Device::startLiveview()
{
	CString type = "camera";
	Value method;
	method = "startLiveview";
	Value params;
	params.SetArray();
	Value version;
	version = "1.0";
	return sendApi(type, method, params, version);
}


Document Device::stopLiveview()
{
	CString type = "camera";
	Value method;
	method = "stopLiveview";
	Value params;
	params.SetArray();
	Value version;
	version = "1.0";
	return sendApi(type, method, params, version);
}

Document Device::getAvailableShootMode()
{
	CString type = "camera";
	Value method;
	method = "getAvailableShootMode";
	Value params;
	params.SetArray();
	Value version;
	version = "1.0";
	return sendApi(type, method, params, version);
}

Document Device::setShootMode(const CString &shootMode)
{
	Document d;
	CString type = "camera";
	Value method;
	method = "setShootMode";
	Value params;
	params.SetArray();
	if (shootMode == "still")
	params.PushBack("still", d.GetAllocator());
	if (shootMode == "movie")
		params.PushBack("movie", d.GetAllocator());
	Value version;
	version = "1.0";
	return sendApi(type, method, params, version);

}



Document Device::actZoom(CString direction, CString movement)
{
	Document d;
	CString type = "camera";
	Value method;
	method = "actZoom";

	Value params;
	params.SetArray();
	//Ã¶¾Ùdirection
	if (direction == "in")
	params.PushBack("in", d.GetAllocator());
	if (direction == "out")
		params.PushBack("out", d.GetAllocator());

	//Ã¶¾Ùmovement
	if (movement == "1shot")
	params.PushBack("1shot", d.GetAllocator());
	if (movement == "start")
		params.PushBack("start", d.GetAllocator());
	if (movement == "stop")
		params.PushBack("stop", d.GetAllocator());

	Value version;
	version = "1.0";
	return sendApi(type, method, params, version);
}



Document Device::actTakePicture()
{
	CString type = "camera";
	Value method;
	method = "actTakePicture";
	Value params;
	params.SetArray();
	Value version;
	version = "1.0";
	return sendApi(type, method, params, version);
}

Document Device::startMovieRec()
{
	CString type = "camera";
	Value method;
	method = "startMovieRec";
	Value params;
	params.SetArray();
	Value version;
	version = "1.0";
	return sendApi(type, method, params, version);
}



Document Device::stopMovieRec()
{
	CString type = "camera";
	Value method;
	method = "stopMovieRec";
	Value params;
	params.SetArray();
	Value version;
	version = "1.0";
	return sendApi(type, method, params, version);
}