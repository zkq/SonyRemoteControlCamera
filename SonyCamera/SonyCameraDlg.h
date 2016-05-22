
// SonyCameraDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "utils.h"

// CSonyCameraDlg 对话框
class CSonyCameraDlg : public CDialogEx
{
// 构造
public:
	CSonyCameraDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SONYCAMERA_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//ssdp协议对象
	SsdpClient ssdpClient;
	//连接的设备对象
	Device device;

	CStatic m_DESCURL;
	CStatic m_OPURL;
	CButton m_btnenter;
	afx_msg void OnBnClickedBtnenter();
	afx_msg void OnBnClickedBtnconn();
protected:
	afx_msg LRESULT OnCmFind(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCmDefind(WPARAM wParam, LPARAM lParam);
};
