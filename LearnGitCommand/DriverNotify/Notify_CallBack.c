

#include "DNCommDef.h"
#include "Notify_CallBack.h"


static LARGE_INTEGER cookie = { 0 };

// 创建进程的通知回调函数
void  CreateProcessNotify_CallBack(HANDLE hParentPid, HANDLE hPid, BOOLEAN bCreate)
{
	if (bCreate)
	{
		PEPROCESS pepTem = NULL;
		NTSTATUS ntstatus = STATUS_SUCCESS;
		
		ntstatus = PsLookupProcessByProcessId(hPid, &pepTem);

		if (NT_SUCCESS(ntstatus))
		{
			ObDereferenceObject(pepTem);

			PUCHAR pstrImagName = PsGetProcessImageFileName(pepTem);

			DbgPrint("--pid<%p>,--path:<%s>", hPid, pstrImagName);
		}

	} 
	return;
}


// 模块加载的通知回调函数
void LoadImageNotify_CallBack(
	PUNICODE_STRING FullImageName,
	HANDLE ProcessId,                // pid into which image is being mapped
	PIMAGE_INFO ImageInfo)
{
	PEPROCESS pepTem = NULL;
	NTSTATUS ntstatus = STATUS_SUCCESS;

	ntstatus = PsLookupProcessByProcessId(ProcessId, &pepTem);

	if (NT_SUCCESS(ntstatus))
	{
		ObDereferenceObject(pepTem);

		PUCHAR pstrImagName = PsGetProcessImageFileName(pepTem);

		DbgPrint("<%s> Load Imag <%wZ> -BaseAddr<%p> -Size is<%llx>",
			pstrImagName,FullImageName,ImageInfo->ImageBase,ImageInfo->ImageSize);
	}

}

// 线程创建的通知回调函数
void  CreateThreadNotify_CallBack(
	HANDLE ProcessId,
	HANDLE ThreadId,
	BOOLEAN Create
)
{
	if (Create)
	{
		PEPROCESS pepTem = NULL;
		NTSTATUS ntstatus = STATUS_SUCCESS;

		ntstatus = PsLookupProcessByProcessId(ProcessId, &pepTem);

		if (NT_SUCCESS(ntstatus))
		{
			ObDereferenceObject(pepTem);

			PUCHAR pstrImagName = PsGetProcessImageFileName(pepTem);

			DbgPrint("--Process<%s> Create Thread:<%d>", pstrImagName, ThreadId);
		}
	}
	return;
}


// 操作注册表的回调
// CallbackContext是我们注册时的第二个参数
NTSTATUS
Register_CallBack(
	PVOID CallbackContext,
	PVOID Argument1,
	PVOID Argument2
)
{
	// DbgPrint("Input Ptr <%p>\n", CallbackContext);

	NTSTATUS ntstatus = STATUS_SUCCESS;

	// 注册类型
	REG_NOTIFY_CLASS regTemClass = (REG_NOTIFY_CLASS)Argument1;
	PREG_CREATE_KEY_INFORMATION pKeyInfo = (PREG_CREATE_KEY_INFORMATION)Argument2;

	UNICODE_STRING wstrRegName = { 0 };
	RtlInitUnicodeString(&wstrRegName, L"*LMDD");

	__try 
	{
		switch (regTemClass)
		{
		case RegNtPreOpenKey:
		case RegNtPreOpenKeyEx:
		case RegNtPreCreateKey:
		case RegNtPreCreateKeyEx:
		{
			DbgPrint("Create/OPen Key Info <%wZ>", pKeyInfo->CompleteName);

			if (FsRtlIsNameInExpression(&wstrRegName, pKeyInfo->CompleteName, TRUE, NULL))
			{
				DbgPrint("Bad Create/Open \n");
				ntstatus = STATUS_UNSUCCESSFUL;
			}
			break;
		}
		case RegNtPreDeleteKey:
		{
			break;
		}
		default:
			break;
		}
	}
	__except (1)
	{
		DbgPrint("Bad Memory\n");
		// ntstatus = STATUS_UNSUCCESSFUL;
	}

	return ntstatus;
}


// 安装通知的回调函数
NTSTATUS InstallNotifyCallBack()
{ 
	NTSTATUS ntstatus = STATUS_SUCCESS;

	//ntstatus = PsSetCreateProcessNotifyRoutine(CreateProcessNotify_CallBack, FALSE);
	//ERRNTSTATUSRET(ntstatus);
	//ntstatus = PsSetLoadImageNotifyRoutine(LoadImageNotify_CallBack);
	//ERRNTSTATUSRET(ntstatus);
	//ntstatus = PsSetCreateThreadNotifyRoutine(CreateThreadNotify_CallBack);
	//ERRNTSTATUSRET(ntstatus);
	ntstatus = CmRegisterCallback(Register_CallBack,(PVOID)0x123456,&cookie);
	ERRNTSTATUSRET(ntstatus);

	return ntstatus;
}


// 卸载通知的回调函数
NTSTATUS UninstallNotifyCallBack()
{
	NTSTATUS ntstatus = STATUS_SUCCESS;

	//PsSetCreateProcessNotifyRoutine(CreateProcessNotify_CallBack, TRUE);
	//PsRemoveLoadImageNotifyRoutine(LoadImageNotify_CallBack);
	//PsRemoveCreateThreadNotifyRoutine(CreateThreadNotify_CallBack);

	CmUnRegisterCallback(cookie);
	return ntstatus;
}

