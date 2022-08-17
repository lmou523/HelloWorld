#include <ntifs.h>



typedef struct  _CM_CALLBACK
{
	LIST_ENTRY listPtr;
	ULONG64 ulUnknown1;
	LARGE_INTEGER cookie;
	PVOID content;
	PEX_CALLBACK_FUNCTION CALLback;

}CM_CALLBACK,*PCM_CALLBACK;



// 枚举所有的Cm回调函数
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
		// 已经遍历到了 取消注册的方法 第一种 主动修改回调函数内存 第二种用cookie删除 第三种 有个全局变量 修改(这个最好）

	}
	else
	{
		DbgPrint(" Not Found PCmCallbackListHead Addr i == 1024 \n");
	}

	return;
}




// 驱动反初始化
void DrvUnload(PDRIVER_OBJECT pDriver)
{

}

// 
// 驱动入口函数
// 

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING reg_path)
{
	// 设置反初始化的回调函数
	pDriver->DriverUnload = DrvUnload;

	NTSTATUS ntstatus = STATUS_SUCCESS;

	EnumCmCallBack();


	return ntstatus;
}