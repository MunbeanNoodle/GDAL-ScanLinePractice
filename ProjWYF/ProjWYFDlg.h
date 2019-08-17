
// ProjWYFDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "ScanLine.h"

// CProjWYFDlg 对话框
class CProjWYFDlg : public CDialog
{
// 构造
public:
	CProjWYFDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = DIALOG_ProjWYF };

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
