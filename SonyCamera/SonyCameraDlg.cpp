
// SonyCameraDlg.cpp : ʵ���ļ�
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


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	// ʵ��
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


// CSonyCameraDlg �Ի���



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


// CSonyCameraDlg ��Ϣ�������

BOOL CSonyCameraDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CSonyCameraDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CSonyCameraDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}






//��Ѱ�豸�����̺߳���
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


//ʹ��ssdpЭ����Ѱ�豸
void CSonyCameraDlg::OnBnClickedBtnconn()
{
	m_btnenter.EnableWindow(0);
	m_btnenter.SetWindowTextA("����Ѱ���豸...");
	CreateThread(0, 0, search, this, 0, 0);
}


//������밴ť
void CSonyCameraDlg::OnBnClickedBtnenter()
{
	//��device���󴫵ݵ�dlg����
	CLiveview dlg(&device);
	dlg.DoModal();
}


//�Զ�����ҵ��豸������
afx_msg LRESULT CSonyCameraDlg::OnCmFind(WPARAM wParam, LPARAM lParam)
{
	//���������ļ�xml
	TiXmlDocument doc;
	doc.Parse(ssdpClient.xml.GetBuffer(), 0, TIXML_ENCODING_UTF8);
	doc.SaveFile("desc.xml");

	//��������ĵ���˵�����豸����xml�ļ���ʽ ��ȡ�豸�����ַ
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

	//һ�ж�ok
	m_DESCURL.SetWindowTextA(ssdpClient.descStr);
	m_OPURL.SetWindowTextA(url->GetText());
	m_btnenter.EnableWindow(1);
	m_btnenter.SetWindowTextA("����");
	return 0;

FAIL:
	//error
	m_DESCURL.SetWindowTextA("");
	m_OPURL.SetWindowTextA("");
	m_btnenter.EnableWindow(0);
	m_btnenter.SetWindowTextA("����ʧ��");
	return -1;
}

//�Զ����δ�ҵ��豸������
afx_msg LRESULT CSonyCameraDlg::OnCmDefind(WPARAM wParam, LPARAM lParam)
{
	m_DESCURL.SetWindowTextA("");
	m_OPURL.SetWindowTextA("");
	m_btnenter.EnableWindow(0);
	m_btnenter.SetWindowTextA("����ʧ��");
	return -1;
}
