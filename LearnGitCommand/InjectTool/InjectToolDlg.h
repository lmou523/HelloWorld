
// InjectToolDlg.h: 头文件
//

#pragma once


#define USE_PROCESS_ID 0
#define USE_PROCESS_NAME 1

// CInjectToolDlg 对话框
class CInjectToolDlg : public CDialogEx
{
// 构造
public:
	CInjectToolDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INJECTTOOL_DIALOG };
#endif

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


	afx_msg void OnBnClickedRadioUsepname();
	afx_msg void OnBnClickedRadioUsepid();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	
	
	DECLARE_MESSAGE_MAP()

	// 初始化数据
	void InitData();


private:

	int		m_iFlag;
	CString m_strDllPath;

	DWORD	m_dwPID;
	CString m_strPName;
public:
	afx_msg void OnBnClickedButtonRemoteThread();
};
