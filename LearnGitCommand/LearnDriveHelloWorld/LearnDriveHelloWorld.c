
#include "CommonDef.h"

#include "MajorFunc.h"
#include "OperatorFile.h"
#include "OperatorReg.h"
#include "PracticeIRQL.h"
#include "OperatorMem.h"


// 创建进程的回调函数
void  CreateProcessNotify_CallBack(HANDLE hPid, HANDLE hMyPid, BOOLEAN bCreate);
// 链接IRP的回调函数
NTSTATUS MajorFuncLink(PDRIVER_OBJECT pDriver);


// 驱动反初始化
void DrvUnload(PDRIVER_OBJECT pDriver)
{
	DbgPrint("[%s] [%s] Unload\n", LMDDPUBNAME, MODELENAME);
	
	// 打印链表中记录的元素不释放内存
	ForEachList();
	// 打印链表中记录的元素并释放内存
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



// 初始化
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
	// 为驱动创建设备
	ntstatus = IoCreateDevice(pDriver, EXTSIZE, &strDeviceName, FILE_DEVICE_UNKNOWN, 0, TRUE, &pMyDevice);

	if (!NT_SUCCESS(ntstatus))
	{
		DbgPrint("Create Device Failed :%x\n",ntstatus);
		return ntstatus;
	}
	// 设置读写方式
	pMyDevice->Flags |= DO_BUFFERED_IO;

	// 设置符号链接
	ntstatus = IoCreateSymbolicLink(&strSymbalName, &strDeviceName);

	if (!NT_SUCCESS(ntstatus))
	{
		DbgPrint("Create Symbal Link Failed :%x\n", ntstatus);
		IoDeleteDevice(pMyDevice);
		return ntstatus;
	}
	// 设置IRP的回调函数
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
// 驱动入口函数
// 

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING reg_path)
{
	// 设置反初始化的回调函数
	pDriver->DriverUnload = DrvUnload;
	// 绑定创建进程的回调函数
	DbgPrint("[%s] [%s]--%wZ--\n", LMDDPUBNAME, MODELENAME, reg_path);

	PsSetCreateProcessNotifyRoutine(CreateProcessNotify_CallBack , FALSE);
	
	NTSTATUS ntstatus = STATUS_SUCCESS;

	ntstatus  = DrvInit(pDriver, reg_path);


	// **** 调用测试函数
	// 
	//调用内存操作函数
	TestMemory(pDriver, reg_path);
	// 
	// 调用IRQL相关函数
	// TestIRQL(pDriver, reg_path);
	// 
	// 测试字符串相关函数
	// TestUseStrOper();
	// 操作注册表 达到设置自启动的目的 
	// 并且复制驱动文件到system32/drivers目录 同时修改注册表luj
	// TestOperatorReg(pDriver, reg_path);

	// 复制文件 删除文件
	// KernelDeleteFile(L"\\??\\C:\\123.exe");
	// KernelCopyFile(L"\\??\\C:\\456.exe", L"\\??\\C:\\789.exe");

	// ****
	return ntstatus;
}


////////////////////////////////////
NTKERNELAPI UCHAR* PsGetProcessImageFileName(__in PEPROCESS Process);

// 创建进程的回调函数
void  CreateProcessNotify_CallBack(HANDLE hPid, HANDLE hMyPid, BOOLEAN bCreate)
{
	DbgPrint("[%s] [%s] Create Process Notify hPid : 0x%p hMyPid : 0x%p bCreate:%d\n",
	LMDDPUBNAME, MODELENAME, hPid, hMyPid, bCreate);

	// 每次创建进程都将数据放到链表里面去
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

// 链接驱动IRP的回调函数
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
