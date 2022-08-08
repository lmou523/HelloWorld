

#include "CommonDef.h"
#include "OperatorFile.h"
#include "PracticeIRQL.h"


// ������ �뻥�����Ƚ�����
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
	// �жϼ� 0:pass 1:apc 2:dpc  0���
	// ��ȡ��ǰ�����жϼ� 
	DbgPrint("---Current Irql = %d -- \n", KeGetCurrentIrql());

	KIRQL kiralPre = 0;
	kiralPre = KeRaiseIrqlToDpcLevel();
	DbgPrint("---KeRaiseIrqlToDpcLevel Irql = %d -- \n", KeGetCurrentIrql());
	KeLowerIrql(kiralPre);

	// ��ʼ��������
	KeInitializeSpinLock(&spinLock);


	// �ֶ���������DPC����
	KeInitializeDpc(&dpcObj, RunDpcFunc, NULL);
	KeInsertQueueDpc(&dpcObj, NULL, NULL);


	return ntstatus;
}



// ���������� 
NTSTATUS AcquireSpinLock(PKIRQL pkirql)
{
	NTSTATUS ntstatus = STATUS_SUCCESS;
	KeAcquireSpinLock(&spinLock, pkirql);
	return ntstatus;
}
// ����������
NTSTATUS ReleaseSpinLock(KIRQL kirql)
{
	NTSTATUS ntstatus = STATUS_SUCCESS;
	KeReleaseSpinLock(&spinLock, kirql);
	return ntstatus;
}