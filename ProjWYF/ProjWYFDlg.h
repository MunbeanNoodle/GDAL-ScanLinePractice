
// ProjWYFDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "ScanLine.h"

// CProjWYFDlg �Ի���
class CProjWYFDlg : public CDialog
{
// ����
public:
	CProjWYFDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = DIALOG_ProjWYF };

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
	afx_msg void OnBnClickedAddvector();
	afx_msg void OnEnChangeAddvector();
	afx_msg void OnBnClickedAddraster();
	CString m_strFilePathRaster;
	CString m_strFilePathVector;
	afx_msg void OnBnClickedExecute();
	afx_msg void OnBnClickedOutput();
	CListCtrl m_List_Raster;
	CListCtrl m_List_Vector;
	CComboBox m_CB_ChooseMode;
	afx_msg void OnBnClickedExit();
	CComboBox m_CB_ChooseBand;
	CString m_OutputFilePath;
};
