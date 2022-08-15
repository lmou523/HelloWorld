

#include "DNCommDef.h"
#include "Notify_CallBack.h"
#include "Process_Callback.h"



// ��������PsSetCreateProcessNotifyRoutineע��Ļص�����
void FindProcessNotify()
{
	UNICODE_STRING strApiName = { 0 };
	PUCHAR pApiAddr = NULL;
	LONG lOffset = 0;

	PUCHAR pPspCreateProcessNotifyRoutine = NULL;

	RtlInitUnicodeString(&strApiName, L"PsSetCreateProcessNotifyRoutine");

	pApiAddr = MmGetSystemRoutineAddress(&strApiName);

	if (pApiAddr == NULL)
	{
		DbgPrint(" Not Found PsSetCreateProcessNotifyRoutine Addr\n");

		return;
	}

	pApiAddr = pApiAddr + 3;
	lOffset = *(PLONG)(pApiAddr + 1);
	pApiAddr = pApiAddr + 5 + lOffset;


#define FOUNDSIZE 1024
	INT i = 0;
	for (i = 0; i < FOUNDSIZE; ++i)
	{
		// lea r14 ,xxx
		if ((*(pApiAddr + i) == 0x4c) && 
			(*(pApiAddr + i + 1) == 0x8d) &&
			(*(pApiAddr + i + 2) == 0x35))
		{
			pApiAddr = pApiAddr + i;  // i ��ʼ ǰ���� lea r14
			lOffset = *(PLONG)(pApiAddr + 3); // 3-7 �� xxx
			pPspCreateProcessNotifyRoutine = pApiAddr + 7 + lOffset;// �������ƫ�Ƶ��ĵط�
			break;
		}
	}

	if (pPspCreateProcessNotifyRoutine != NULL)
	{
		PDWORD64 pTemNotifyList = (PDWORD64)pPspCreateProcessNotifyRoutine;
		DWORD64	 dwNotify = 0;

		for (i = 0; i < MAXNOTIIFYSZ; ++i)
		{
			dwNotify = pTemNotifyList[i];

			if (dwNotify == 0)
				break;

			dwNotify = dwNotify & 0xfffffffffffffff8;

			PDWORD64 pRealNotify = (PDWORD64)dwNotify;
			DWORD64 dwRealNotify = *pRealNotify;

			DbgPrint("-- Create -- ProcessNotify = <%p>-- \n", dwRealNotify);
		}
	}
	else
	{
		DbgPrint(" Not Found PsSetCreateProcessNotifyRoutine Addr i == 1024 \n");
	}

	return;
}



// ��������ʼ��
void DrvUnload(PDRIVER_OBJECT pDriver)
{
	// ж��֪ͨ�Ļص�����
	UninstallNotifyCallBack();
	//ж�ؽ���֪ͨ�Ļص�
	UninstallProcessRegCallback();
}

// 
// ������ں���
// 

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING reg_path)
{
	// ���÷���ʼ���Ļص�����
	pDriver->DriverUnload = DrvUnload;

	NTSTATUS ntstatus = STATUS_SUCCESS;

	ntstatus = InstallNotifyCallBack();
	ERRNTSTATUSRET(ntstatus);

	// FindProcessNotify();

	ntstatus = InstallProcessRegCallback(pDriver);
	ERRNTSTATUSRET(ntstatus);

	return ntstatus;
}