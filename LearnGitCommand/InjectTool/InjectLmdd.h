

#pragma once

#include <vector>

using namespace std;



#define DSTALLOCSZ 0x1000


class CInjectLmdd
{
public:
	// Զ���߳�ע��
	static HRESULT RemoteThreadInject(LPCWSTR pwstrMod, DWORD dwPid);

	// ��Ȩ
	static BOOL EnableDebugPrivilege(); 
	// ö�ٽ��̵õ�����id
	static HRESULT GetProcessID(LPCWSTR pProcessName, vector<DWORD>* parrPid);

	static LPCWSTR TransErrorCode(HRESULT hRet);
};

