
#include "stdafx.h"
#include "utils.h"
#include "rapidjson\stringbuffer.h"
#include "rapidjson\writer.h"

//将可用api保存到mAvailableCameraApiSet
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

//将支持api保存到mSupportedApiSet
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



//检查给定的api是否可用
bool Device::isCameraApiAvailable(const CString &apiName)
{
	auto a = mAvailableCameraApiSet.find(apiName);

	return a != mAvailableCameraApiSet.end();
}
//检查给定的api是否支持
boolean Device::isApiSupported(const CString &apiName)
{
	auto a = mSupportedApiSet.find(apiName);

	return a != mSupportedApiSet.end();
}



//获取指定服务名称的操作地址
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

//发送json命令的基础函数
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


//***********************************设备命令实现*****************************
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
	//direction
	Value s;
	s.SetString(direction, d.GetAllocator());
	params.PushBack(s, d.GetAllocator());
	//movement
	s.SetString(movement, d.GetAllocator());
	params.PushBack(s, d.GetAllocator());

	Value version;
	version = "1.0";
	return sendApi(type, method, params, version);
}


Document Device::getAvailableStillSize()
{
	Document d;
	CString type = "camera";
	Value method;
	method = "getAvailableStillSize";
	Value params;
	params.SetArray();

	Value version;
	version = "1.0";
	return sendApi(type, method, params, version);
}




Document Device::setStillSize()
{
	Document d;
	CString type = "camera";
	Value method;
	method = "setStillSize";
	Value params;
	params.SetArray();
	params.PushBack("4:3", d.GetAllocator());
	params.PushBack("5M", d.GetAllocator());

	Value version;
	version = "1.0";
	return sendApi(type, method, params, version);
}


Document Device::setPostviewImageSize(){
	Document d;
	CString type = "camera";
	Value method;
	method = "setPostviewImageSize";
	Value params;
	params.SetArray();
	params.PushBack("Original", d.GetAllocator());

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


//ISO
Document Device::getAvailableIsoSpeedRate()
{
	CString type = "camera";
	Value method;
	method = "getAvailableIsoSpeedRate";
	Value params;
	params.SetArray();
	Value version;
	version = "1.0";
	return sendApi(type, method, params, version);
}


Document Device::setIsoSpeedRate(CString rate)
{
	Document d;
	CString type = "camera";
	Value method;
	method = "setIsoSpeedRate";
	Value params;
	params.SetArray();
	Value s;
	s.SetString(rate, d.GetAllocator());
	params.PushBack(s, d.GetAllocator());
	Value version;
	version = "1.0";
	return sendApi(type, method, params, version);

}


//曝光
Document Device::getAvailableExposureMode()
{
	CString type = "camera";
	Value method;
	method = "getAvailableExposureMode";
	Value params;
	params.SetArray();
	Value version;
	version = "1.0";
	return sendApi(type, method, params, version);
}


Document Device::setExposureMode(CString mode)
{
	Document d;
	CString type = "camera";
	Value method;
	method = "setExposureMode";
	Value params;
	params.SetArray();
	Value s;
	s.SetString(mode, d.GetAllocator());
	params.PushBack(s, d.GetAllocator());
	Value version;
	version = "1.0";
	return sendApi(type, method, params, version);
}