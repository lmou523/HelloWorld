
#include "CommonDef.h"

#include "MajorFunc.h"
#include "OperatorFile.h"
#include "OperatorReg.h"
#include "PracticeIRQL.h"
#include "OperatorMem.h"


// �������̵Ļص�����
void  CreateProcessNotify_CallBack(HANDLE hPid, HANDLE hMyPid, BOOLEAN bCreate);
// ����IRP�Ļص�����
NTSTATUS MajorFuncLink(PDRIVER_OBJECT pDriver);


// ��������ʼ��
void DrvUnload(PDRIVER_OBJECT pDriver)
{
	DbgPrint("[%s] [%s] Unload\n", LMDDPUBNAME, MODELENAME);
	
	// ��ӡ�����м�¼��Ԫ�ز��ͷ��ڴ�
	ForEachList();
	// ��ӡ�����м�¼��Ԫ�ز��ͷ��ڴ�
	RemoveAllProcessList();

	PsSetCreateProcessNotifyRoutine(CreateProcessNotify_CallBack, TRUE);

	if (pDriver->DeviceObject != NULL)
	{
		IoDeleteDevice(pDriver->DeviceObject);

		UNICODE_STRING strSymbalName = { 0 };
		RtlInitUnicodeString(&strSymbalName, SYMBALNAME);

		IoDeleteSymbolicLink(&strSymbalName);
	}
}



// ��ʼ��
NTSTATUS DrvInit(PDRIVER_OBJECT pDriver, PUNICODE_STRING reg_path)
{
	if (pDriver == NULL || reg_path == NULL)
		return  STATUS_FAILED_DRIVER_ENTRY;

	NTSTATUS ntstatus = STATUS_SUCCESS;
	UNICODE_STRING strDeviceName = { 0 };
	UNICODE_STRING strSymbalName = { 0 };
	PDEVICE_OBJECT pMyDevice = NULL;

	RtlInitUnicodeString(&strDeviceName, DRIVERNAME);
	RtlInitUnicodeString(&strSymbalName, SYMBALNAME);
	// Ϊ���������豸
	ntstatus = IoCreateDevice(pDriver, EXTSIZE, &strDeviceName, FILE_DEVICE_UNKNOWN, 0, TRUE, &pMyDevice);

	if (!NT_SUCCESS(ntstatus))
	{
		DbgPrint("Create Device Failed :%x\n",ntstatus);
		return ntstatus;
	}
	// ���ö�д��ʽ
	pMyDevice->Flags |= DO_BUFFERED_IO;

	// ���÷�������
	ntstatus = IoCreateSymbolicLink(&strSymbalName, &strDeviceName);

	if (!NT_SUCCESS(ntstatus))
	{
		DbgPrint("Create Symbal Link Failed :%x\n", ntstatus);
		IoDeleteDevice(pMyDevice);
		return ntstatus;
	}
	// ����IRP�Ļص�����
	ntstatus = MajorFuncLink(pDriver);

	if (!NT_SUCCESS(ntstatus))
	{
		DbgPrint("Major Func Link Failed :%x\n", ntstatus);
		IoDeleteDevice(pMyDevice);
		return ntstatus;
	}

	return ntstatus;
}


// 
// ������ں���
// 

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING reg_path)
{
	// ���÷���ʼ���Ļص�����
	pDriver->DriverUnload = DrvUnload;
	// �󶨴������̵Ļص�����
	DbgPrint("[%s] [%s]--%wZ--\n", LMDDPUBNAME, MODELENAME, reg_path);

	PsSetCreateProcessNotifyRoutine(CreateProcessNotify_CallBack , FALSE);
	
	NTSTATUS ntstatus = STATUS_SUCCESS;

	ntstatus  = DrvInit(pDriver, reg_path);


	// **** ���ò��Ժ���
	// 
	//�����ڴ��������
	TestMemory(pDriver, reg_path);
	// 
	// ����IRQL��غ���
	// TestIRQL(pDriver, reg_path);
	// 
	// �����ַ�����غ���
	// TestUseStrOper();
	// ����ע��� �ﵽ������������Ŀ�� 
	// ���Ҹ��������ļ���system32/driversĿ¼ ͬʱ�޸�ע���luj
	// TestOperatorReg(pDriver, reg_path);

	// �����ļ� ɾ���ļ�
	// KernelDeleteFile(L"\\??\\C:\\123.exe");
	// KernelCopyFile(L"\\??\\C:\\456.exe", L"\\??\\C:\\789.exe");

	// ****
	return ntstatus;
}


////////////////////////////////////
NTKERNELAPI UCHAR* PsGetProcessImageFileName(__in PEPROCESS Process);

// �������̵Ļص�����
void  CreateProcessNotify_CallBack(HANDLE hPid, HANDLE hMyPid, BOOLEAN bCreate)
{
	DbgPrint("[%s] [%s] Create Process Notify hPid : 0x%p hMyPid : 0x%p bCreate:%d\n",
	LMDDPUBNAME, MODELENAME, hPid, hMyPid, bCreate);

	// ÿ�δ������̶������ݷŵ���������ȥ
	if (bCreate)
	{
		PEPROCESS peTem = NULL;
		NTSTATUS ntstatus = PsLookupProcessByProcessId(hMyPid, &peTem);
		if (peTem == NULL)
		{
			DbgPrint("PsLookupProcessByProcessId Is Error,ErrorCode:%x", ntstatus); 
			return;
		}

		PUCHAR processname = PsGetProcessImageFileName(peTem);
		DbgPrint("Process Name Is %s", processname);

		AddProcessToList(hMyPid, peTem, processname);
	}

}

// ��������IRP�Ļص�����
NTSTATUS MajorFuncLink(PDRIVER_OBJECT pDriver)
{
	NTSTATUS ntstatus = STATUS_SUCCESS;

	pDriver->MajorFunction[IRP_MJ_CREATE]	= IrpCreateCallBack;
	pDriver->MajorFunction[IRP_MJ_CLOSE]	= IrpCloseCallBack;
	pDriver->MajorFunction[IRP_MJ_CLEANUP]	= IrpCleanUpCallBack;
	pDriver->MajorFunction[IRP_MJ_READ]		= IrpReadCallBack;
	pDriver->MajorFunction[IRP_MJ_WRITE]	= IrpWriteCallBack;
	pDriver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IrpDeviceControlCallBack;


	return ntstatus;
}
