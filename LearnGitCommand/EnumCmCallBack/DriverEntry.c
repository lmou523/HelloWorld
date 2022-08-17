#include <ntifs.h>



typedef struct  _CM_CALLBACK
{
	LIST_ENTRY listPtr;
	ULONG64 ulUnknown1;
	LARGE_INTEGER cookie;
	PVOID content;
	PEX_CALLBACK_FUNCTION CALLback;

}CM_CALLBACK,*PCM_CALLBACK;



// ö�����е�Cm�ص�����
void EnumCmCallBack()
{

	UNICODE_STRING strApiName = { 0 };
	PUCHAR pApiAddr = NULL;
	LONG lOffset = 0;

	BOOLEAN bFound = FALSE;
	PUCHAR PCmCallbackListHead = NULL;

	RtlInitUnicodeString(&strApiName, L"CmUnRegisterCallback");

	pApiAddr = MmGetSystemRoutineAddress(&strApiName);

	if (pApiAddr == NULL)
	{
		DbgPrint(" Not Found CmUnRegisterCallback  Addr\n");

		return;
	}


#define FOUNDSIZE 1024
	INT i = 0;
	for (i = 0; i < FOUNDSIZE; ++i)
	{
		// 48XXXX	  lea     rdx,[rsp+20h]
		// 488d0dXXXX lea     rcx,[nt!CallbackListHead XXX ]
		if ((*(pApiAddr + i) == 0x48) &&
			(*(pApiAddr + i + 5) == 0x48) &&
			(*(pApiAddr + i + 6) == 0x8d) &&
			(*(pApiAddr + i + 7) == 0x0d))
		{
			pApiAddr = pApiAddr + i + 5;
			bFound = TRUE;
			break;
		}
	}
	if (bFound)
	{
		

		PCmCallbackListHead = pApiAddr + 7 + *(PLONG)(pApiAddr + 3);
		PCM_CALLBACK pCmCallbackHead = PCmCallbackListHead;
		PCM_CALLBACK pCmCallbackTem = (PCM_CALLBACK)pCmCallbackHead->listPtr.Blink;

		while (pCmCallbackTem->listPtr.Flink != pCmCallbackHead->listPtr.Flink)
		{
			DbgPrint("[CMCallBack:Func<0x%p>\tCookie:<0x%p>]\n",
				(PVOID)pCmCallbackTem->CALLback, 
				(PVOID)pCmCallbackTem->cookie.QuadPart);

			// pCmCallbackTem->CALLback = (PVOID)MyRegister_CallBack;

			pCmCallbackTem = pCmCallbackTem->listPtr.Blink;

			CmUnRegisterCallback(((PCM_CALLBACK)(pCmCallbackTem->listPtr.Flink))->cookie);
		}
		// �Ѿ��������� ȡ��ע��ķ��� ��һ�� �����޸Ļص������ڴ� �ڶ�����cookieɾ�� ������ �и�ȫ�ֱ��� �޸�(�����ã�

	}
	else
	{
		DbgPrint(" Not Found PCmCallbackListHead Addr i == 1024 \n");
	}

	return;
}




// ��������ʼ��
void DrvUnload(PDRIVER_OBJECT pDriver)
{

}

// 
// ������ں���
// 

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING reg_path)
{
	// ���÷���ʼ���Ļص�����
	pDriver->DriverUnload = DrvUnload;

	NTSTATUS ntstatus = STATUS_SUCCESS;

	EnumCmCallBack();


	return ntstatus;
}