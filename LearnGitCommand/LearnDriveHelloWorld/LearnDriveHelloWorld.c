
#include "CommonDef.h"

#include "MajorFunc.h"

// �������̵Ļص�����
void  CreateProcessNotify_CallBack(HANDLE hPid, HANDLE hMyPid, BOOLEAN bCreate);
// ����IRP�Ļص�����
NTSTATUS MajorFuncLink(PDRIVER_OBJECT pDriver);


// ��������ʼ��
void DrvUnload(PDRIVER_OBJECT pDriver)
{
	DbgPrint("[%s] [%s] Unload\n", LMDDPUBNAME, MODELENAME);

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
	DbgPrint("[%s] [%s]--%wZ--\n", LMDDPUBNAME, MODELENAME,reg_path);

	PsSetCreateProcessNotifyRoutine(CreateProcessNotify_CallBack , FALSE);
	
	NTSTATUS ntstatus = STATUS_SUCCESS;

	ntstatus  = DrvInit(pDriver, reg_path);

	return ntstatus;
}


////////////////////////////////////

// �������̵Ļص�����
void  CreateProcessNotify_CallBack(HANDLE hPid, HANDLE hMyPid, BOOLEAN bCreate)
{

//	DbgPrint("[%s] [%s] Create Process Notify hPid : 0x%p hMyPid : 0x%p bCreate:%d\n", LMDDPUBNAME, MODELENAME, hPid, hMyPid, bCreate);
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
