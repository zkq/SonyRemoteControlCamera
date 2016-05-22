#pragma once
#include "afxwin.h"
#include "utils.h"
#include <queue>
#include "afxcmn.h"

using std::queue;
// CLiveview 对话框

class CLiveview : public CDialogEx
{
	DECLARE_DYNAMIC(CLiveview)

public:
	CLiveview(Device *device, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLiveview();

// 对话框数据
	enum { IDD = IDD_LIVEVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_Liveview;
	Device *indevice;
	
	void start();
	bool fetching;
	queue<HGLOBAL> mJpegQueue;

	CButton mBtnStop;
	CButton mBtnPic;
	CButton mBtnRec;
	CComboBox mComMod;
	CStatic mPic;

	void FreshUI();

	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedBtnstop();
	afx_msg void OnBnClickedBtnpic();
	afx_msg void OnBnClickedBtnrec();
	afx_msg void OnBnClickedBtnzoomin();
	afx_msg void OnBnClickedBtnzoomout();
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnClose();
	
	CStatic mStatus;
	CButton mBtnZoomIn;
	CButton mBtnZoomOut;
	CComboBox mIsoCombo;
	afx_msg void OnCbnSelchangeCombo2();
	CComboBox mExpoCombo;
	afx_msg void OnCbnSelchangeCombo3();
};

DWORD WINAPI retrieveJPG(LPVOID param);
DWORD WINAPI drawJPEG(LPVOID param);
IPicture *getPicFromHGlobal(HGLOBAL global);


