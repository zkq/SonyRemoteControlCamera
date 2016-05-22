
// SonyCameraDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "utils.h"

// CSonyCameraDlg �Ի���
class CSonyCameraDlg : public CDialogEx
{
// ����
public:
	CSonyCameraDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SONYCAMERA_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//ssdpЭ�����
	SsdpClient ssdpClient;
	//���ӵ��豸����
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
