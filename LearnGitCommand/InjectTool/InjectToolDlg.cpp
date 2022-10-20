
// InjectToolDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"

#include "InjectTool.h"
#include "InjectToolDlg.h"
#include "afxdialogex.h"
#include "InjectLmdd.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStnClickedStaticGit();
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_STN_CLICKED(IDC_STATIC_GIT, &CAboutDlg::OnStnClickedStaticGit)
END_MESSAGE_MAP()

// 关于对话框

void CAboutDlg::OnStnClickedStaticGit()
{
	// TODO: 在此添加控件通知处理程序代码
	MessageBox(_T("坤鸣：你~干~嘛\n存在任何问题 本人概不负责(微笑)！\n"));
}


// End 关于对话框

// CInjectToolDlg 对话框



CInjectToolDlg::CInjectToolDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_INJECTTOOL_DIALOG, pParent)
	, m_iFlag(0)
	, m_strDllPath(_T(""))
	, m_dwPID(0)
	, m_strPName(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CInjectToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_USEPID, m_iFlag);
	DDX_Text(pDX, IDC_EDIT_DLLPATH, m_strDllPath);
	DDX_Text(pDX, IDC_EDIT_PID, m_dwPID);
	DDV_MinMaxUInt(pDX, m_dwPID, 0, 65536);
	DDX_Text(pDX, IDC_EDIT_PNAME, m_strPName);
}

BEGIN_MESSAGE_MAP(CInjectToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_RADIO_USEPNAME, &CInjectToolDlg::OnBnClickedRadioUsepname)
	ON_BN_CLICKED(IDC_RADIO_USEPID, &CInjectToolDlg::OnBnClickedRadioUsepid)
	ON_BN_CLICKED(IDC_BUTTON_REMOTE_THREAD, &CInjectToolDlg::OnBnClickedButtonRemoteThread)
END_MESSAGE_MAP()


// CInjectToolDlg 消息处理程序

BOOL CInjectToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// TODO: 在此添加额外的初始化代码
	// 自己的初始化

	InitData();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CInjectToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CInjectToolDlg::OnPaint()
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
HCURSOR CInjectToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CInjectToolDlg::InitData()
{
	((CButton*)GetDlgItem(IDC_RADIO_USEPNAME))->SetCheck(1);
	((CButton*)GetDlgItem(IDC_RADIO_USEPID))->SetCheck(0);
	((CEdit*)GetDlgItem(IDC_EDIT_PID))->SetReadOnly(TRUE);
}



void CInjectToolDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	WCHAR wstrDllPath[MAX_PATH] = { 0 };

	UpdateData();
	if (DragQueryFile(hDropInfo, 0, wstrDllPath, MAX_PATH) > 0)
	{
		m_strDllPath = wstrDllPath;
		UpdateData(FALSE);
	}

	CDialogEx::OnDropFiles(hDropInfo);
}


void CInjectToolDlg::OnBnClickedRadioUsepname()
{
	// TODO: 在此添加控件通知处理程序代码
	((CEdit*)GetDlgItem(IDC_EDIT_PID))->SetReadOnly(TRUE);
	((CEdit*)GetDlgItem(IDC_EDIT_PNAME))->SetReadOnly(FALSE);
}


void CInjectToolDlg::OnBnClickedRadioUsepid()
{
	// TODO: 在此添加控件通知处理程序代码
	((CEdit*)GetDlgItem(IDC_EDIT_PID))->SetReadOnly(FALSE);
	((CEdit*)GetDlgItem(IDC_EDIT_PNAME))->SetReadOnly(TRUE);
}




void CInjectToolDlg::OnBnClickedButtonRemoteThread()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	vector<DWORD> arrPid;

	if (m_iFlag == USE_PROCESS_ID)
	{
		arrPid.push_back(m_dwPID);
	}
	else if (m_iFlag == USE_PROCESS_NAME)
	{
		CInjectLmdd::GetProcessID(m_strPName, &arrPid);
	}



	BOOL bRet = FALSE;
	HRESULT hRet = S_OK;
	for (DWORD dwPid : arrPid)
	{
		// 拿着PID注入
		hRet = CInjectLmdd::RemoteThreadInject(m_strDllPath, dwPid);
		if (hRet == S_OK)
			bRet = TRUE;
	}

	if (bRet)
	{
		MessageBox(_T("注入成功！\n"));
	}
	else
	{
		CString strMsg;
		strMsg.Format(_T("注入失败 错误代码:%s\n"), CInjectLmdd::TransErrorCode(hRet));
		MessageBox(strMsg);
	}

}


