
// ProjWYFDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ProjWYF.h"
#include "ProjWYFDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
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

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CProjWYFDlg �Ի���




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


// CProjWYFDlg ��Ϣ�������

CRasterWYF R;
CVectorWYF V;
CScanLineWYF Scan;


BOOL CProjWYFDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	DWORD dwStyle = m_List_Raster.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	m_List_Raster.SetExtendedStyle(dwStyle);
	m_List_Raster.InsertColumn(0, _T("����"), LVCFMT_LEFT, 80);
	m_List_Raster.InsertColumn(1, _T("ֵ"), LVCFMT_LEFT, 80);

	dwStyle = m_List_Vector.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	m_List_Vector.SetExtendedStyle(dwStyle);
	m_List_Vector.InsertColumn(0, _T("����"), LVCFMT_LEFT, 80);
	m_List_Vector.InsertColumn(1, _T("ֵ"), LVCFMT_LEFT, 80);

	m_CB_ChooseMode.InsertString(0, _T("�߽�����"));
	m_CB_ChooseMode.InsertString(1, _T("�߽粻����"));
	m_CB_ChooseMode.SetCurSel(0);
	
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CProjWYFDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CProjWYFDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CProjWYFDlg::OnBnClickedAddraster()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CFileDialog hFileDlg(true,NULL,NULL, OFN_FILEMUSTEXIST | OFN_READONLY | OFN_PATHMUSTEXIST, TEXT("Image|*img|Tiff|*tif|�����ļ�|*.*|"),NULL);
	hFileDlg.m_ofn.lpstrTitle = "���դ������";
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
	m_List_Raster.InsertItem(0, "����");
	CString TmpStr1;
	TmpStr1.Format("%d", R.nYSize);
	m_List_Raster.SetItemText(0, 1, TmpStr1);
	m_List_Raster.InsertItem(1, "����");
	CString TmpStr2;
	TmpStr2.Format("%d", R.nXSize);
	m_List_Raster.SetItemText(1, 1, TmpStr2);
	m_List_Raster.InsertItem(2, "������");
	CString TmpStr3;
	TmpStr3.Format("%d", R.iBandNum);
	m_List_Raster.SetItemText(2, 1, TmpStr3);

}

void CProjWYFDlg::OnBnClickedAddvector()
{
	// TODO: �ڴ���ӿؼ�֪ͨ���������� 
	CFileDialog InputFile(true,NULL,NULL, OFN_FILEMUSTEXIST | OFN_READONLY | OFN_PATHMUSTEXIST, TEXT("Shape|*.shp|�����ļ�|*.*|"),NULL);
	InputFile.m_ofn.lpstrTitle = "���ʸ������";
	if(InputFile.DoModal() == IDOK)
	{
		m_strFilePathVector = InputFile.GetPathName();
		UpdateData(FALSE);
	}
	V.OpenVectorFile(m_strFilePathVector);
	m_List_Vector.DeleteAllItems();
	m_List_Vector.InsertItem(0, "�ֶ���");
	CString TmpStr1;
	TmpStr1.Format("%d", V.AttTable.iFieldNum);
	m_List_Vector.SetItemText(0, 1, TmpStr1);
	m_List_Vector.InsertItem(1, "�������");
	CString TmpStr2;
	TmpStr2.Format("%d", V.i64ObjectNum);
	m_List_Vector.SetItemText(1, 1, TmpStr2);


}

void CProjWYFDlg::OnEnChangeAddvector()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�
	
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

}





void CProjWYFDlg::OnBnClickedOutput()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	CFileDialog OutputFile( FALSE, _T( "shp" ), _T( "" ), OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY, _T( "Shape|*.shp|�����ļ�|*.*|" ) );
	OutputFile.m_ofn.lpstrTitle=("ѡ�����·��");
	if ( OutputFile.DoModal() == IDOK ) {
		m_OutputFilePath = OutputFile.GetPathName();
	}
	UpdateData(FALSE);
}

void CProjWYFDlg::OnBnClickedExecute()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
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
		m_strFilePathVector,//ʸ���ļ�·��
		m_OutputFilePath,//���·��
		1, //ѡȡ�Ĳ��α��
		Border_Count_In//�ж�����һ�ַ�ʽ����ɨ��
	);

	
	int x = 0;
}


void CProjWYFDlg::OnBnClickedExit()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if( MessageBox( "��ȷ��Ҫ�˳���?", "", MB_YESNO | MB_ICONQUESTION ) == IDYES)
		this -> SendMessage( WM_CLOSE );
}
