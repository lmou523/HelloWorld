
#include "CommonDef.h"
#include "OperatorKernelObject.h"


KEVENT g_Event = { 0 };


CHAR memShare[] = "aaaaaaaa";

HANDLE hThread1 = NULL;
HANDLE hThread2 = NULL; 

BOOLEAN bIsExit = FALSE;
BOOLEAN bThread1Exit = FALSE;
BOOLEAN bThread2Exit = FALSE;

BOOLEAN bUserModeEventInit = FALSE;
PKEVENT PUserModeEventThread = NULL;
HANDLE  hUserModeThread = NULL;
BOOLEAN bUserModeEventThreadExit = FALSE;



void UserModeThread1(PVOID pData)
{
	LARGE_INTEGER timeOut = { 0 };
	timeOut.QuadPart = -1000 * 10 * 1000 * 5;

	while (!bIsExit)
	{
		if (bUserModeEventInit && PUserModeEventThread != NULL)
		{
			if (KeWaitForSingleObject(PUserModeEventThread, Executive, KernelMode, FALSE, &timeOut) == STATUS_TIMEOUT)
			{
				DbgPrint("Time Out");
				bUserModeEventInit = FALSE;
				PUserModeEventThread = NULL;
				continue;
			}
			DbgPrint("This is User Mode SetEnevt\n");
		}
	 }
	bUserModeEventThreadExit = TRUE;
	PsTerminateSystemThread(0);
}

void KernelThreadFun2(PVOID pdata)
{
	LARGE_INTEGER sleepTime = { 0 };
	PKEVENT pEvent = (PKEVENT)pdata;

	sleepTime.QuadPart = -1000 * 10 * 1000 * 3;

	while (!bIsExit)
	{
		KeDelayExecutionThread(KernelMode, FALSE, &sleepTime);  //Sleep
		DbgPrint("KernelThreadFun2 Set g_Event \n ");
		for (int i = 0; i < sizeof(memShare); ++i)
		{
			++(memShare[i]);
		}

		KeSetEvent(pEvent, IO_NO_INCREMENT, TRUE);
	}
	bThread2Exit = TRUE;
	PsTerminateSystemThread(0);
}


void KernelThreadFun1()
{

	while (!bIsExit)
	{
		KeWaitForSingleObject(&g_Event, Executive, KernelMode, FALSE, NULL);
		// KeResetEvent(&g_Event);
		DbgPrint("KeWaitForSingleObject Success  g_Event Has Seted\n");
		
		DbgPrint("-memShare:-%s--\n", memShare);
	}
	bThread1Exit = TRUE;
	PsTerminateSystemThread(0);
}


NTSTATUS StartThread()
{
	NTSTATUS ntstatusRet = STATUS_SUCCESS;

	// 通知事件  在设置完之后需要主动取消设置 true 需要手动变成false
	// KeInitializeEvent(&g_Event, NotificationEvent, FALSE);
	// 同步事件 在设置完之后   时间被读取会自动取消设置 true 自动变成false
	KeInitializeEvent(&g_Event, SynchronizationEvent, FALSE);

	ntstatusRet = PsCreateSystemThread(&hThread2, 0, NULL, NULL, NULL, KernelThreadFun2, &g_Event);

	if (!NT_SUCCESS(ntstatusRet))
	{
		DbgPrint("PsCreateSystemThread  hThread1 Failed\n");
		return STATUS_UNSUCCESSFUL;
	}

	ntstatusRet = PsCreateSystemThread(&hThread1, 0, NULL, NULL, NULL, KernelThreadFun1, NULL);

	if (!NT_SUCCESS(ntstatusRet))
	{
		DbgPrint("PsCreateSystemThread  hThread2 Failed\n");
		return STATUS_UNSUCCESSFUL;
	}
	ntstatusRet = PsCreateSystemThread(&hUserModeThread, 0, NULL, NULL, NULL, UserModeThread1, NULL);

	if (!NT_SUCCESS(ntstatusRet))
	{
		DbgPrint("PsCreateSystemThread  hUserModeThread Failed\n");
		return STATUS_UNSUCCESSFUL;
	}
	
	return ntstatusRet;
}

NTSTATUS EndThread()
{
	bIsExit = TRUE;

	while (1)
	{
		if (bThread1Exit && bThread2Exit && bUserModeEventThreadExit)
			break;
	}

	if (hThread1 != NULL)
		ZwClose(hThread1);
	if (hThread2 != NULL)
		ZwClose(hThread2);
	if (hUserModeThread != NULL)
		ZwClose(hUserModeThread);

	hThread1 = hUserModeThread = hThread2 = NULL;

	return STATUS_SUCCESS;
}