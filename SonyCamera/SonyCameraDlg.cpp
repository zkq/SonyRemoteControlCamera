
// SonyCameraDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SonyCamera.h"
#include "SonyCameraDlg.h"
#include "afxdialogex.h"

#include "Liveview.h"
#include "tinyxml.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSonyCameraDlg 对话框



CSonyCameraDlg::CSonyCameraDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSonyCameraDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSonyCameraDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Text(pDX, IDC_STAURL, m_StrURL);
	DDX_Control(pDX, IDC_STAURL, m_DESCURL);
	DDX_Control(pDX, IDC_STAOPURL, m_OPURL);
	DDX_Control(pDX, IDC_BTNENTER, m_btnenter);
}

BEGIN_MESSAGE_MAP(CSonyCameraDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTNCONN, &CSonyCameraDlg::OnBnClickedBtnconn)
	ON_BN_CLICKED(IDC_BTNENTER, &CSonyCameraDlg::OnBnClickedBtnenter)
	ON_MESSAGE(CM_FIND, CSonyCameraDlg::OnCmFind)
	ON_MESSAGE(CM_DEFIND, &CSonyCameraDlg::OnCmDefind)
END_MESSAGE_MAP()


// CSonyCameraDlg 消息处理程序

BOOL CSonyCameraDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSonyCameraDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSonyCameraDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSonyCameraDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}






//搜寻设备的子线程函数
DWORD WINAPI search(LPVOID param)
{
	CSonyCameraDlg *dlg = (CSonyCameraDlg*)param;
	if (dlg->ssdpClient.SearchDevice())
	{
		PostMessage(dlg->m_hWnd, CM_FIND, 0, 0);
		return 0;
	}
	PostMessage(dlg->m_hWnd, CM_DEFIND, 0, 0);
	return -1;
}


//使用ssdp协议搜寻设备
void CSonyCameraDlg::OnBnClickedBtnconn()
{
	m_btnenter.EnableWindow(0);
	m_btnenter.SetWindowTextA("正在寻找设备...");
	CreateThread(0, 0, search, this, 0, 0);
}


//点击进入按钮
void CSonyCameraDlg::OnBnClickedBtnenter()
{
	//将device对象传递到dlg里面
	CLiveview dlg(&device);
	dlg.DoModal();
}


//自定义的找到设备处理函数
afx_msg LRESULT CSonyCameraDlg::OnCmFind(WPARAM wParam, LPARAM lParam)
{
	//解析描述文件xml
	TiXmlDocument doc;
	doc.Parse(ssdpClient.xml.GetBuffer(), 0, TIXML_ENCODING_UTF8);
	doc.SaveFile("desc.xml");

	//根据相机文档里说明的设备描述xml文件格式 提取设备服务地址
	TiXmlElement *root = doc.RootElement();
	if (!root)
	{
		goto FAIL;
	}
	TiXmlElement *devicenode = root->FirstChildElement("device");
	if (!devicenode)
	{
		goto FAIL;
	}
	TiXmlElement *deviceInfo = devicenode->FirstChildElement("av:X_ScalarWebAPI_DeviceInfo");
	if (!deviceInfo)
	{
		goto FAIL;
	}
	TiXmlElement *serList = deviceInfo->FirstChildElement("av:X_ScalarWebAPI_ServiceList");
	if (!deviceInfo)
	{
		goto FAIL;
	}
	TiXmlElement *ser, *type, *url;
	ser = type = url = NULL;
	while (ser = (TiXmlElement *)serList->IterateChildren(ser))
	{
		type = ser->FirstChildElement("av:X_ScalarWebAPI_ServiceType");
		url = ser->FirstChildElement("av:X_ScalarWebAPI_ActionList_URL");
		if (!type || !url)
		{
			goto FAIL;
		}
		Device::service pair(CString(type->GetText()),
			CString(url->GetText()));
		this->device.services.push_back(pair);
	}

	//一切都ok
	m_DESCURL.SetWindowTextA(ssdpClient.descStr);
	m_OPURL.SetWindowTextA(url->GetText());
	m_btnenter.EnableWindow(1);
	m_btnenter.SetWindowTextA("进入");
	return 0;

FAIL:
	//error
	m_DESCURL.SetWindowTextA("");
	m_OPURL.SetWindowTextA("");
	m_btnenter.EnableWindow(0);
	m_btnenter.SetWindowTextA("查找失败");
	return -1;
}

//自定义的未找到设备处理函数
afx_msg LRESULT CSonyCameraDlg::OnCmDefind(WPARAM wParam, LPARAM lParam)
{
	m_DESCURL.SetWindowTextA("");
	m_OPURL.SetWindowTextA("");
	m_btnenter.EnableWindow(0);
	m_btnenter.SetWindowTextA("查找失败");
	return -1;
}
