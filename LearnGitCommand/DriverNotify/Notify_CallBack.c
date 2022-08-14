

#include "DNCommDef.h"
#include "Notify_CallBack.h"


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



// ��װ֪ͨ�Ļص�����
NTSTATUS InstallNotifyCallBack()
{
	NTSTATUS ntstatus = STATUS_SUCCESS;

	ntstatus = PsSetCreateProcessNotifyRoutine(CreateProcessNotify_CallBack, FALSE);
	ERRNTSTATUSRET(ntstatus);
	ntstatus = PsSetLoadImageNotifyRoutine(LoadImageNotify_CallBack);
	ERRNTSTATUSRET(ntstatus);
	ntstatus = PsSetCreateThreadNotifyRoutine(CreateThreadNotify_CallBack);
	ERRNTSTATUSRET(ntstatus);

	return ntstatus;
}


// ж��֪ͨ�Ļص�����
NTSTATUS UninstallNotifyCallBack()
{
	NTSTATUS ntstatus = STATUS_SUCCESS;

	PsSetCreateProcessNotifyRoutine(CreateProcessNotify_CallBack, TRUE);
	PsRemoveLoadImageNotifyRoutine(LoadImageNotify_CallBack);
	PsRemoveCreateThreadNotifyRoutine(CreateThreadNotify_CallBack);

	return ntstatus;
}

