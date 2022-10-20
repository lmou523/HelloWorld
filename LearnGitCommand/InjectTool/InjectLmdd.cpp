#include "pch.h"
#include "tlhelp32.h"
#include "InjectLmdd.h"




// ��Ȩ
BOOL CInjectLmdd::EnableDebugPrivilege() //Debug
{

	HANDLE hToken = NULL;
	TOKEN_PRIVILEGES TokenPrivilege;

	BOOL bRet = FALSE;
	//��Ȩ������
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		return bRet;
	}
	TokenPrivilege.PrivilegeCount = 1;
	TokenPrivilege.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	do
	{
		if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &TokenPrivilege.Privileges[0].Luid))
			break;

		if (!AdjustTokenPrivileges(hToken, FALSE, &TokenPrivilege, 0, NULL, NULL))
			break;

		bRet = TRUE;
	} while (FALSE);

	CloseHandle(hToken);


	return bRet;
}



 HRESULT CInjectLmdd::GetProcessID(LPCWSTR pProcessName, vector<DWORD>* parrPid)
{

	if (pProcessName == NULL || parrPid == NULL)
		return E_POINTER;

	CInjectLmdd::EnableDebugPrivilege();
	// ö�ٽ���
	HANDLE hProcessSnap = NULL;
	PROCESSENTRY32 pe32 = { 0 };
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == (HANDLE)-1)
	{
		return S_FALSE;
	}
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hProcessSnap, &pe32))
	{
		do
		{
			//PCWSTR pPrProcessName = StrRChrW(pe32.szExeFile, NULL, L'\\');
			//if (pProcessName != NULL)
			//{
			//	if (_wcsicmp(pProcessName + 1, pProcessName) == 0)
			//		parrPid->push_back(pe32.th32ProcessID);
			//}
			if (_wcsicmp(pe32.szExeFile, pProcessName) == 0)
				parrPid->push_back(pe32.th32ProcessID);

		} while (Process32Next(hProcessSnap, &pe32));
	}
	else
	{
		CloseHandle(hProcessSnap);
		return S_FALSE;
	}
	CloseHandle(hProcessSnap);
	return S_OK;
}


 LPCWSTR CInjectLmdd::TransErrorCode(HRESULT hRet)
 {
	 static struct InjectError
	 {
		 HRESULT hRet;
		 LPCWSTR pErrMsg;
	 } arrInject[] = {  {S_OK, L"NULL"} };

	 DWORD dw = 0;
	 while (arrInject[dw].hRet != S_OK)
	 {
		 if (hRet == arrInject[dw].hRet)
			 break;
		 ++dw;
	 }

	 return arrInject[dw].pErrMsg;
}


// Զ���߳�ע��
HRESULT CInjectLmdd::RemoteThreadInject(LPCWSTR pwstrMod, DWORD dwPid)
{
	HRESULT hRet = S_OK;


	do
	{
		// ��Ȩ
		if (!EnableDebugPrivilege())
		{
			hRet = GetLastError();
			break;
		}

		// ��ȡģ����
		HMODULE hMod = LoadLibrary(_T("Kernel32.dll"));

		if (hMod == NULL)
		{
			hRet = GetLastError();
			break;
		}
		// ��ȡ������ַ ���н��̵����������ַ����һ����
		PVOID pLoadLibrary = GetProcAddress(hMod, "LoadLibraryW");
		
		if (pLoadLibrary != NULL)
		{
			// ��ȡĿ����̾��
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
			if (hProcess != NULL) 
			{
				// ��Ŀ����̷���ռ� �洢ע��dll·��
				PVOID pTemDllPath = VirtualAllocEx(hProcess, NULL, DSTALLOCSZ, MEM_COMMIT, PAGE_READWRITE);
				if (pTemDllPath != NULL)
				{
					// д��DLL·����Ŀ�����
					SIZE_T dwOutSize = 0;
					if (WriteProcessMemory(hProcess,pTemDllPath, pwstrMod, (wcslen(pwstrMod) + 1)*sizeof(WCHAR),&dwOutSize))
					{
						HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibrary, pTemDllPath, 0, NULL);

						if (hThread != NULL)
						{
							WaitForSingleObject(hThread, -1);
							GetExitCodeThread(hThread, (LPDWORD)&hRet);
							CloseHandle(hThread);
						}
						hRet = S_OK;
					}
				
					VirtualFreeEx(hProcess, pTemDllPath, 0, MEM_FREE);
				}
				CloseHandle(hProcess);
			}
			else
			{
				hRet = GetLastError();
			}
		}

		FreeLibrary(hMod);

	} while (FALSE);
	
	return hRet;
}

