

#include "DNCommDef.h"
#include "Notify_CallBack.h"


static LARGE_INTEGER cookie = { 0 };

// �������̵�֪ͨ�ص�����
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


// ģ����ص�֪ͨ�ص�����
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

// �̴߳�����֪ͨ�ص�����
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


// ����ע���Ļص�
// CallbackContext������ע��ʱ�ĵڶ�������
NTSTATUS
Register_CallBack(
	PVOID CallbackContext,
	PVOID Argument1,
	PVOID Argument2
)
{
	// DbgPrint("Input Ptr <%p>\n", CallbackContext);

	NTSTATUS ntstatus = STATUS_SUCCESS;

	// ע������
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


// ��װ֪ͨ�Ļص�����
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


// ж��֪ͨ�Ļص�����
NTSTATUS UninstallNotifyCallBack()
{
	NTSTATUS ntstatus = STATUS_SUCCESS;

	//PsSetCreateProcessNotifyRoutine(CreateProcessNotify_CallBack, TRUE);
	//PsRemoveLoadImageNotifyRoutine(LoadImageNotify_CallBack);
	//PsRemoveCreateThreadNotifyRoutine(CreateThreadNotify_CallBack);

	CmUnRegisterCallback(cookie);
	return ntstatus;
}

