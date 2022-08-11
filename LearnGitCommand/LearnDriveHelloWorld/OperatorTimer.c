
#include "CommonDef.h"
#include "OperatorTimer.h"


KTIMER timer = { 0 };

static KDPC dpcObj = { 0 };
static WORK_QUEUE_ITEM workObj = { 0 };
static BOOLEAN bIsExit = FALSE;
static BOOLEAN bExited = FALSE;

void TimeWorker()
{
	DbgPrint("--IRQL : %d\n", KeGetCurrentIrql());
	DbgPrint("--ProcessName : %s\n", PsGetProcessImageFileName(PsGetCurrentProcess()));
}

void WorkItemRoutine(PVOID pdata)
{
	DbgPrint("--IRQL : %d\n", KeGetCurrentIrql());
	DbgPrint("--ProcessName : %s\n", PsGetProcessImageFileName(PsGetCurrentProcess()));


	LARGE_INTEGER sleepTime = { 0 };
	sleepTime.QuadPart = -10 * 1000 * 1000 * 2;

	while (!bIsExit)
	{
		DbgPrint("--WorkItemRoutine--\n");
		KeDelayExecutionThread(KernelMode, FALSE, &sleepTime);
	}
	bExited = TRUE;
}

void TimerInit(PDEVICE_OBJECT pDevice)
{
	if (pDevice == NULL)
		return;
	// ���һ��һ��
	// ����ע�������pchunter����
	IoInitializeTimer(pDevice, TimeWorker, NULL);
	// IoStartTimer(pDevice);


	//**** DPC��ʱ�� ֻ�ᴥ��һ��
	KeInitializeTimer(&timer);
	KeInitializeDpc(&dpcObj, TimeWorker, NULL);

	LARGE_INTEGER dpcTime = { 0 };
	dpcTime.QuadPart = -10 * 1000 * 1000 * 4;
	LARGE_INTEGER outTime = { 0 };
	outTime.QuadPart = -10 * 1000 * 1000 * 2;
	// ��������� dpcobj ��ô����Ͳ����½�dpc���� ���ǻᴥ����ʱ��
	KeSetTimer(&timer, dpcTime, &dpcObj);

	NTSTATUS ntstatus = KeWaitForSingleObject(&timer, Executive, KernelMode, FALSE, &outTime);
	if (ntstatus == STATUS_TIMEOUT)
	{
		KeCancelTimer(&timer);
		DbgPrint(" Time Out Cancle Timer  \n");
	}
	DbgPrint("aaa Set Timer");
	//****
	

	//    PWORK_QUEUE_ITEM Item,
	//    PWORKER_THREAD_ROUTINE Routine,
	//    PVOID Context

	ExInitializeWorkItem(&workObj, WorkItemRoutine, NULL); 
	ExQueueWorkItem(&workObj, CriticalWorkQueue);
}
void TimeUninit()
{
	bIsExit = TRUE;
	LARGE_INTEGER sleepTime = { 0 };
	sleepTime.QuadPart = - 5 * 1000 * 1000 ;
	while(!bExited)
	{
		KeDelayExecutionThread(KernelMode, FALSE, &sleepTime);
	}
	return;
}