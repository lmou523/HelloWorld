

#pragma once

#include <vector>

using namespace std;



#define DSTALLOCSZ 0x1000


class CInjectLmdd
{
public:
	// 远程线程注入
	static HRESULT RemoteThreadInject(LPCWSTR pwstrMod, DWORD dwPid);

	// 提权
	static BOOL EnableDebugPrivilege(); 
	// 枚举进程得到进程id
	static HRESULT GetProcessID(LPCWSTR pProcessName, vector<DWORD>* parrPid);

	static LPCWSTR TransErrorCode(HRESULT hRet);
};

