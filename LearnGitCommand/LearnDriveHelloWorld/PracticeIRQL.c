

#include "CommonDef.h"
#include "OperatorFile.h"
#include "PracticeIRQL.h"


// 自旋锁 与互斥量比较相似
KSPIN_LOCK spinLock = { 0 };

KDPC dpcObj = { 0 };

void RunDpcFunc(PVOID Prama)
{
	DbgPrint(" RunDpcFunc---Current Irql = %d -- \n", KeGetCurrentIrql());

	KernelCopyFile(L"\\??\\C:\\456.exe", L"\\??\\C:\\789.exe");
}


NTSTATUS TestIRQL(PDRIVER_OBJECT pDriver, PUNICODE_STRING reg_path)
{	
	NTSTATUS ntstatus = STATUS_SUCCESS;
	// 中断级 0:pass 1:apc 2:dpc  0最低
	// 获取当前代码中断级 
	DbgPrint("---Current Irql = %d -- \n", KeGetCurrentIrql());

	KIRQL kiralPre = 0;
	kiralPre = KeRaiseIrqlToDpcLevel();
	DbgPrint("---KeRaiseIrqlToDpcLevel Irql = %d -- \n", KeGetCurrentIrql());
	KeLowerIrql(kiralPre);

	// 初始化自旋锁
	KeInitializeSpinLock(&spinLock);


	// 手动插入任务到DPC队列
	KeInitializeDpc(&dpcObj, RunDpcFunc, NULL);
	KeInsertQueueDpc(&dpcObj, NULL, NULL);


	return ntstatus;
}



// 自旋锁上锁 
NTSTATUS AcquireSpinLock(PKIRQL pkirql)
{
	NTSTATUS ntstatus = STATUS_SUCCESS;
	KeAcquireSpinLock(&spinLock, pkirql);
	return ntstatus;
}
// 自旋锁解锁
NTSTATUS ReleaseSpinLock(KIRQL kirql)
{
	NTSTATUS ntstatus = STATUS_SUCCESS;
	KeReleaseSpinLock(&spinLock, kirql);
	return ntstatus;
}