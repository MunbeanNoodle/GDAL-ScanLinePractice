
// ProjWYFDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ProjWYF.h"
#include "ProjWYFDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
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

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CProjWYFDlg 对话框




CProjWYFDlg::CProjWYFDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProjWYFDlg::IDD, pParent)
	, m_strFilePathVector(_T(""))
	, m_strFilePathRaster(_T(""))
	, m_OutputFilePath(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CProjWYFDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, EDIT_AddVector, m_strFilePathVector);
	DDX_Text(pDX, EDIT_AddRaster, m_strFilePathRaster);
	DDX_Control(pDX, LIST_RasterBI, m_List_Raster);
	DDX_Control(pDX, LIST_VectorBI, m_List_Vector);
	DDX_Control(pDX, COMBO_ChooseMode, m_CB_ChooseMode);
	DDX_Control(pDX, COMBO_ChooseBand, m_CB_ChooseBand);
	DDX_Text(pDX, EDIT_Output, m_OutputFilePath);
}

BEGIN_MESSAGE_MAP(CProjWYFDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(BUTTON_AddVector, &CProjWYFDlg::OnBnClickedAddvector)
	ON_EN_CHANGE(EDIT_AddVector, &CProjWYFDlg::OnEnChangeAddvector)
	ON_BN_CLICKED(BUTTON_AddRaster, &CProjWYFDlg::OnBnClickedAddraster)
	ON_BN_CLICKED(BUTTON_Execute, &CProjWYFDlg::OnBnClickedExecute)
	ON_BN_CLICKED(BUTTON_Output, &CProjWYFDlg::OnBnClickedOutput)
	ON_BN_CLICKED(BUTTON_Exit, &CProjWYFDlg::OnBnClickedExit)
END_MESSAGE_MAP()


// CProjWYFDlg 消息处理程序

CRasterWYF R;
CVectorWYF V;
CScanLineWYF Scan;


BOOL CProjWYFDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	DWORD dwStyle = m_List_Raster.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	m_List_Raster.SetExtendedStyle(dwStyle);
	m_List_Raster.InsertColumn(0, _T("属性"), LVCFMT_LEFT, 80);
	m_List_Raster.InsertColumn(1, _T("值"), LVCFMT_LEFT, 80);

	dwStyle = m_List_Vector.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	m_List_Vector.SetExtendedStyle(dwStyle);
	m_List_Vector.InsertColumn(0, _T("属性"), LVCFMT_LEFT, 80);
	m_List_Vector.InsertColumn(1, _T("值"), LVCFMT_LEFT, 80);

	m_CB_ChooseMode.InsertString(0, _T("边界算入"));
	m_CB_ChooseMode.InsertString(1, _T("边界不算入"));
	m_CB_ChooseMode.SetCurSel(0);
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CProjWYFDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CProjWYFDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CProjWYFDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CProjWYFDlg::OnBnClickedAddraster()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog hFileDlg(true,NULL,NULL, OFN_FILEMUSTEXIST | OFN_READONLY | OFN_PATHMUSTEXIST, TEXT("Image|*img|Tiff|*tif|所有文件|*.*|"),NULL);
	hFileDlg.m_ofn.lpstrTitle = "添加栅格数据";
	if(hFileDlg.DoModal() == IDOK)
	{
		m_strFilePathRaster = hFileDlg.GetPathName();
		UpdateData(FALSE);
	}
	R.OpenRasterFile(m_strFilePathRaster);
	((CComboBox*)GetDlgItem(COMBO_ChooseBand))->ResetContent();
	for(int i = 0; i < R.iBandNum; i++)
	{
		CString TmpStr;
		TmpStr.Format("%d", i + 1);
		m_CB_ChooseBand.InsertString(i, TmpStr);
	}
	m_CB_ChooseBand.SetCurSel(0);
	
	m_List_Raster.DeleteAllItems();
	m_List_Raster.InsertItem(0, "行数");
	CString TmpStr1;
	TmpStr1.Format("%d", R.nYSize);
	m_List_Raster.SetItemText(0, 1, TmpStr1);
	m_List_Raster.InsertItem(1, "列数");
	CString TmpStr2;
	TmpStr2.Format("%d", R.nXSize);
	m_List_Raster.SetItemText(1, 1, TmpStr2);
	m_List_Raster.InsertItem(2, "波段数");
	CString TmpStr3;
	TmpStr3.Format("%d", R.iBandNum);
	m_List_Raster.SetItemText(2, 1, TmpStr3);

}

void CProjWYFDlg::OnBnClickedAddvector()
{
	// TODO: 在此添加控件通知处理程序代码 
	CFileDialog InputFile(true,NULL,NULL, OFN_FILEMUSTEXIST | OFN_READONLY | OFN_PATHMUSTEXIST, TEXT("Shape|*.shp|所有文件|*.*|"),NULL);
	InputFile.m_ofn.lpstrTitle = "添加矢量数据";
	if(InputFile.DoModal() == IDOK)
	{
		m_strFilePathVector = InputFile.GetPathName();
		UpdateData(FALSE);
	}
	V.OpenVectorFile(m_strFilePathVector);
	m_List_Vector.DeleteAllItems();
	m_List_Vector.InsertItem(0, "字段数");
	CString TmpStr1;
	TmpStr1.Format("%d", V.AttTable.iFieldNum);
	m_List_Vector.SetItemText(0, 1, TmpStr1);
	m_List_Vector.InsertItem(1, "对象个数");
	CString TmpStr2;
	TmpStr2.Format("%d", V.i64ObjectNum);
	m_List_Vector.SetItemText(1, 1, TmpStr2);


}

void CProjWYFDlg::OnEnChangeAddvector()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。
	
	// TODO:  在此添加控件通知处理程序代码

}





void CProjWYFDlg::OnBnClickedOutput()
{
	// TODO: 在此添加控件通知处理程序代码
	
	CFileDialog OutputFile( FALSE, _T( "shp" ), _T( "" ), OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY, _T( "Shape|*.shp|所有文件|*.*|" ) );
	OutputFile.m_ofn.lpstrTitle=("选择输出路径");
	if ( OutputFile.DoModal() == IDOK ) {
		m_OutputFilePath = OutputFile.GetPathName();
	}
	UpdateData(FALSE);
}

void CProjWYFDlg::OnBnClickedExecute()
{
	// TODO: 在此添加控件通知处理程序代码
	
	/*CRasterWYF R;
	R.OpenRasterFile("E:\Fusion.tif");
	int ii = 0;
	R.CloseRasterFile();*/
	
	/*CVectorWYF V;
	V.OpenVectorFile("E:\\zj_Polygon.shp");
	V.ReadObject(1444);
	V.CreateVectorFile("E:\\ZJ.shp");
	for (int jj = 0; jj < V.i64ObjectNum; jj++)
		V.WriteObject(1,1,1,1,1,1,1);
	
	V.CloseVectorFile();
    int ii = 0;*/

	Scan.InputFilePath(
		m_strFilePathRaster,
		m_strFilePathVector,//矢量文件路径
		m_OutputFilePath,//输出路径
		1, //选取的波段编号
		Border_Count_In//判断以哪一种方式进行扫描
	);

	
	int x = 0;
}


void CProjWYFDlg::OnBnClickedExit()
{
	// TODO: 在此添加控件通知处理程序代码
	if( MessageBox( "您确定要退出吗?", "", MB_YESNO | MB_ICONQUESTION ) == IDYES)
		this -> SendMessage( WM_CLOSE );
}
