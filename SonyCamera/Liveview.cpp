// Liveview.cpp : 实现文件
//

#include "stdafx.h"
#include "SonyCamera.h"
#include "SonyCameraDlg.h"
#include "Liveview.h"
#include "afxdialogex.h"


// CLiveview 对话框

IMPLEMENT_DYNAMIC(CLiveview, CDialogEx)

CLiveview::CLiveview(Device *device, CWnd* pParent /*=NULL*/)
: CDialogEx(CLiveview::IDD, pParent)
{
	indevice = device;
}

CLiveview::~CLiveview()
{
}

void CLiveview::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIVEVIEW, m_Liveview);
	DDX_Control(pDX, IDC_BTNSTOP, mBtnStop);
	DDX_Control(pDX, IDC_BTNPIC, mBtnPic);
	DDX_Control(pDX, IDC_BTNREC, mBtnRec);
	DDX_Control(pDX, IDC_COMBO1, mComMod);
	DDX_Control(pDX, IDC_PIC, mPic);
	DDX_Control(pDX, IDC_STATUS, mStatus);
	DDX_Control(pDX, IDC_BTNZOOMIN, mBtnZoomIn);
	DDX_Control(pDX, IDC_BTNZOOMOUT, mBtnZoomOut);
}


BEGIN_MESSAGE_MAP(CLiveview, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BTNSTOP, &CLiveview::OnBnClickedBtnstop)
	ON_BN_CLICKED(IDC_BTNPIC, &CLiveview::OnBnClickedBtnpic)
	ON_BN_CLICKED(IDC_BTNREC, &CLiveview::OnBnClickedBtnrec)
	ON_BN_CLICKED(IDC_BTNZOOMIN, &CLiveview::OnBnClickedBtnzoomin)
	ON_BN_CLICKED(IDC_BTNZOOMOUT, &CLiveview::OnBnClickedBtnzoomout)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CLiveview::OnCbnSelchangeCombo1)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CLiveview 消息处理程序


void CLiveview::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO:  在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialogEx::OnPaint()
	start();
}


int CLiveview::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	Document replyJson;
	replyJson = indevice->getApplicationInfo();

	replyJson = indevice->getAvailableApiList();
	indevice->loadAvailableCameraApiList(replyJson);

	replyJson = indevice->getAvcontentMethodTypes();
	indevice->loadSupportedApiList(replyJson);

	return 0;
}


void CLiveview::FreshUI()
{
	if (!fetching)
	{
		mBtnStop.SetWindowTextA("开始");
		mBtnPic.EnableWindow(false);
		mBtnRec.EnableWindow(false);
		mStatus.SetWindowTextA("空闲");
	}
	else
	{
		mBtnStop.SetWindowTextA("暂停");
		if (indevice->curShootMode == "still")
		{
			mBtnRec.EnableWindow(false);
			mBtnPic.EnableWindow();
			mStatus.SetWindowTextA("静态照片模式");
		}
		else if (indevice->curShootMode == "movie")
		{
			mBtnRec.EnableWindow();
			mBtnPic.EnableWindow(false);

			if (indevice->recording)
			{
				mBtnRec.SetWindowTextA("停止录制");
				mStatus.SetWindowTextA("正在录制视频");
			}
			else
			{
				mBtnRec.SetWindowTextA("录制");
				mStatus.SetWindowTextA("视频拍摄模式");
			}
				
		}


	}
}


void CLiveview::start()
{
	Document replyJson;
	//如果支持startRecMode
	if (indevice->isCameraApiAvailable("startRecMode")) {
		replyJson = indevice->startRecMode();

		//重新获取可用API
		replyJson = indevice->getAvailableApiList();
		indevice->loadAvailableCameraApiList(replyJson);
	}

	//此处应该开始相机事件监听


	//开始liveview
	if (indevice->isCameraApiAvailable("startLiveview")) {
		CreateThread(0, 0, retrieveJPG, this, 0, 0);
		CreateThread(0, 0, drawJPEG, this, 0, 0);
	}

	//获取支持的拍摄模式
	mComMod.ResetContent();
	const Document replyJson2 = indevice->getAvailableShootMode();
	if (indevice->isJsonOk(replyJson2))
	{
		indevice->curShootMode = replyJson2["result"].GetArray()[0].GetString();
		indevice->recording = false;
		Value::ConstArray a = replyJson2["result"].GetArray()[1].GetArray();
		for (Value::ConstValueIterator itr = a.Begin(); itr != a.End(); ++itr)
		{
			indevice->mAvailableShootMod.push_back(itr->GetString());
			mComMod.AddString(itr->GetString());
		}
		mComMod.SelectString(0, indevice->curShootMode);
	}

	//获取是否支持调节焦距
	if (!indevice->isCameraApiAvailable("actZoom"))
	{
		mBtnZoomIn.EnableWindow(0);
		mBtnZoomOut.EnableWindow(0);
	}

	FreshUI();
}


IPicture *getPicFromHGlobal(HGLOBAL global)
{
	IStream *pStm = NULL;
	IPicture *pPic = NULL;

	HRESULT hr = CreateStreamOnHGlobal(global, TRUE, &pStm);
	
	if (hr != S_OK)
	{
		return NULL;
	}
	hr = OleLoadPicture(pStm, 0, false, IID_IPicture, (LPVOID*)&pPic);
	if (hr != S_OK)
	{
		return NULL;
	}
	return pPic;

}


DWORD WINAPI retrieveJPG(LPVOID param)
{
	CLiveview *dlg = (CLiveview *)param;
	Document replyJson;
	replyJson = dlg->indevice->startLiveview();
	if (replyJson.IsNull())
	{
		//通讯失败
		dlg->fetching = false;
		dlg->FreshUI();
		return -1;
	}
	if (replyJson.HasMember("error"))
	{
		//相机故障
		dlg->fetching = false;
		dlg->FreshUI();
		return -1;
	}


	CString liveviewUrl = replyJson["result"].GetArray()[0].GetString();
	LiveviewSlicer slicer;
	if (!slicer.open(liveviewUrl))
	{
		//读取数据失败
		dlg->fetching = false;
		dlg->FreshUI();
		return -1;
	}

	dlg->fetching = true;
	while (dlg->fetching)
	{
		Payload payload = slicer.nextPayload();
		LONG size = payload.jpegDataSize;
		if (size <= 0)
		{
			slicer.close();
			break;
		}
		HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, size);
		LPVOID pData = GlobalLock(hGlobal);
		slicer.file->Read(pData, size);
		GlobalUnlock(hGlobal);
		payload.paddingData = new UCHAR[payload.paddingDataSize];
		slicer.file->Read(payload.paddingData, payload.paddingDataSize);
		delete[] payload.paddingData;

		while (dlg->mJpegQueue.size() >= 5)
		{
			GlobalFree(dlg->mJpegQueue.front());
			dlg->mJpegQueue.pop();
		}
		dlg->mJpegQueue.push(hGlobal);
	}

	//退出了
	while (!dlg->mJpegQueue.empty())
	{
		dlg->mJpegQueue.pop();
	}
	dlg->fetching = false;
	dlg->FreshUI();
	return -1;
}


DWORD WINAPI drawJPEG(LPVOID param)
{
	CLiveview *dlg = (CLiveview *)param;
	dlg->fetching = true;
	while (dlg->fetching)
	{
	if (dlg->mJpegQueue.size() == 0)
	{
		continue;
	}
	IPicture *pPic = getPicFromHGlobal(dlg->mJpegQueue.front());
	dlg->mJpegQueue.pop();

	if (pPic == NULL)
		break;

		OLE_XSIZE_HIMETRIC hmWidth;
		OLE_YSIZE_HIMETRIC hmHeight;
		pPic->get_Width(&hmWidth);
		pPic->get_Height(&hmHeight);
		double fX, fY;
		CRect rect;
		GetClientRect(dlg->m_Liveview.m_hWnd, rect);
		fX = rect.Width();
		fY = rect.Height();
		HDC *pDC = &dlg->m_Liveview.GetDC()->m_hDC;
		pPic->Render(*pDC, 0, 0, (DWORD)fX, (DWORD)fY, 0, hmHeight, hmWidth, -hmHeight, NULL);
		pPic->Release();
	}
	dlg->fetching = false;
	dlg->FreshUI();
	return -1;
}




void CLiveview::OnBnClickedBtnstop()
{
	if (fetching)
	{
		fetching = false;
		indevice->stopLiveview();
	}
	else
	{
		start();
	}
	FreshUI();
}


void CLiveview::OnBnClickedBtnpic()
{
	Document d = indevice->actTakePicture();
	if (indevice->isJsonOk(d))
	{
		CString pictureurl = d["result"].GetArray()[0].GetArray()[0].GetString();
		ULONG size = 0;
		PUCHAR picture = HttpClient::getData(pictureurl, size);
		if (size <= 0)
		{
			return;
		}
		HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, size);
		LPVOID pData = GlobalLock(hGlobal);
		memcpy(pData, picture, size);
		GlobalUnlock(hGlobal);
		delete[] picture;
		IPicture *pPic = getPicFromHGlobal(hGlobal);

		if (pPic == NULL)
			return;

		OLE_XSIZE_HIMETRIC hmWidth;
		OLE_YSIZE_HIMETRIC hmHeight;
		pPic->get_Width(&hmWidth);
		pPic->get_Height(&hmHeight);
		double fX, fY;
		CRect rect;
		mPic.GetWindowRect(&rect);
		fX = rect.Width();
		fY = rect.Height();
		HDC *pDC = &mPic.GetDC()->m_hDC;
		pPic->Render(*pDC, 0, 0, (DWORD)fX, (DWORD)fY, 0, hmHeight, hmWidth, -hmHeight, NULL);
		pPic->Release();
	}
}


void CLiveview::OnBnClickedBtnrec()
{
	if (!indevice->recording)
	{
		Document d = indevice->startMovieRec();
		indevice->recording = true;
	}
	else
	{
		Document d = indevice->stopMovieRec();
		indevice->recording = false;
	}
	FreshUI();
}


void CLiveview::OnBnClickedBtnzoomin()
{
	Document d = indevice->actZoom("in", "1shot");
	if (!indevice->isJsonOk(d))
	{
		MessageBox("已经飙到极限了");
	}
}


void CLiveview::OnBnClickedBtnzoomout()
{
	Document d = indevice->actZoom("out", "1shot");
	if (!indevice->isJsonOk(d))
	{
		MessageBox("已经飙到极限了");
	}
}


void CLiveview::OnCbnSelchangeCombo1()
{
	// TODO:  在此添加控件通知处理程序代码
	int nIndex = mComMod.GetCurSel();
	CString selmode;
	mComMod.GetLBText(nIndex, selmode);
	if (selmode == indevice->curShootMode)
	{
		return;
	}

	Document d = indevice->setShootMode(selmode);
	if (indevice->isJsonOk(d))
	{
		indevice->curShootMode = selmode;
		FreshUI();
	}
	else
	{
		MessageBox("切换失败,请检查是否在录制视频或拍摄照片");
		mComMod.SelectString(0, indevice->curShootMode);
	}
}


void CLiveview::OnClose()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (fetching)
	{
		fetching = false;
		indevice->stopLiveview();
	}
	CDialogEx::OnClose();
}
