


#include "CommonDef.h"

#include "MajorFunc.h"


// IRP_CREATE_OPERATION
NTSTATUS IrpCreateCallBack(PDEVICE_OBJECT pDevice, PIRP pIrp)
{
	NTSTATUS ntstatus = STATUS_SUCCESS;

	DbgPrint("Lmdd Drvice has been Created\n");

	pIrp->IoStatus.Status = ntstatus;
	pIrp->IoStatus.Information = 0;

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return ntstatus;
}


// IRP_MJ_CLOSE
NTSTATUS IrpCloseCallBack(PDEVICE_OBJECT pDevice, PIRP pIrp)
{
	NTSTATUS ntstatus = STATUS_SUCCESS;

	DbgPrint("Lmdd Drvice has been Close\n");

	pIrp->IoStatus.Status = ntstatus;
	pIrp->IoStatus.Information = 0;

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return ntstatus;
}

// IRP_MJ_CLEANUP
NTSTATUS IrpCleanUpCallBack(PDEVICE_OBJECT pDevice, PIRP pIrp)
{
	NTSTATUS ntstatus = STATUS_SUCCESS;

	DbgPrint("Lmdd Drvice has been CleanUp\n");

	pIrp->IoStatus.Status = ntstatus;
	pIrp->IoStatus.Information = 0;

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return ntstatus;
}

// IRP_MJ_READ
NTSTATUS IrpReadCallBack(PDEVICE_OBJECT pDevice, PIRP pIrp)
{
	NTSTATUS ntstatus = STATUS_SUCCESS;

	DbgPrint("Lmdd Drvice has been Read\n");
	PIO_STACK_LOCATION pStackLocation = IoGetCurrentIrpStackLocation(pIrp);

	ULONG ulWantReadSize = pStackLocation->Parameters.Read.Length;
	PCHAR pDstBuff = pIrp->AssociatedIrp.SystemBuffer;

	const char szMessage[] = "This is a Kernel Buffer";
	ULONG ulMessageLen = strlen(szMessage) + 1;

	RtlCopyMemory(pDstBuff, szMessage, ulMessageLen);

	pIrp->IoStatus.Status = ntstatus;
	pIrp->IoStatus.Information = ulMessageLen;

	DbgPrint("read Buf Len is %d\n",ulMessageLen);
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return ntstatus;
}


// IRP_MJ_WRITE
NTSTATUS IrpWriteCallBack(PDEVICE_OBJECT pDevice, PIRP pIrp)
{
	NTSTATUS ntstatus = STATUS_SUCCESS;

	DbgPrint("Lmdd Drvice has been Read\n");
	PIO_STACK_LOCATION pStackLocation = IoGetCurrentIrpStackLocation(pIrp);

	ULONG ulWantWriteSize = pStackLocation->Parameters.Read.Length;
	PCHAR pSrcBuff = pIrp->AssociatedIrp.SystemBuffer;



	RtlZeroMemory(pDevice->DeviceExtension, EXTSIZE);
	RtlCopyMemory(pDevice->DeviceExtension, pSrcBuff, ulWantWriteSize);

	DbgPrint("--%p--%s--\n", pSrcBuff, (PCHAR)pDevice->DeviceExtension);

	const int iTestWriteSize = 13;
	pIrp->IoStatus.Status = ntstatus;
	pIrp->IoStatus.Information = iTestWriteSize;

	DbgPrint("Write Buf Len is %d\n", iTestWriteSize);
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return ntstatus;
}


// IRP_MJ_DEVICE_CONTROL
NTSTATUS IrpDeviceControlCallBack(PDEVICE_OBJECT pDevice, PIRP pIrp)
{
	NTSTATUS ntstatus = STATUS_SUCCESS;

	PIO_STACK_LOCATION pStack = IoGetCurrentIrpStackLocation(pIrp);
	
	ULONG ulIOCode = pStack->Parameters.DeviceIoControl.IoControlCode;
	ULONG ulInLength = pStack->Parameters.DeviceIoControl.InputBufferLength;
	ULONG ulOutLength = pStack->Parameters.DeviceIoControl.OutputBufferLength;
	ULONG ulIOInfo = 0;


	switch (ulIOCode)
	{
	case IOCTL_MUL:
	{
		const ULONG ulTemMul = 5;
		DWORD32 dwIndata = *(PDWORD32)pIrp->AssociatedIrp.SystemBuffer;
		DbgPrint("Kernel--In: %d--\n",dwIndata);

		dwIndata *= ulTemMul;
		*(PDWORD32)pIrp->AssociatedIrp.SystemBuffer = dwIndata;

		ulIOInfo = 4;

		break;
	}
	default:
	{
		ntstatus = STATUS_UNSUCCESSFUL;
		ulIOInfo = 0;
		break;
	}
	}

	pIrp->IoStatus.Status = ntstatus;
	pIrp->IoStatus.Information = ulIOInfo;

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return ntstatus;
}

// **** 
// 此函数为测试函数 会在入口函数进行调用测试 主要是记录一些字符相关的操作

NTSTATUS TestUseStrOper()
{
	NTSTATUS ntstatus = STATUS_SUCCESS;

	//UNICODE_STRING  wDeviceName = RTL_CONSTANT_STRING(DRIVERNAME);
	//DbgPrint("--[%ws]--%wZ--\n", TESTMODNAME, &wDeviceName);

	//PWCHAR pwTestBUFF = ExAllocatePool(NonPagedPool,EXTSIZE);
	//if (pwTestBUFF == NULL)
	//	return ntstatus;

	//RtlZeroMemory(pwTestBUFF, EXTSIZE);
	//RtlCopyMemory(pwTestBUFF, TESTUNOCODESTR,sizeof(TESTUNOCODESTR));
	//RtlInitUnicodeString(&wDeviceName, pwTestBUFF);

	//DbgPrint("--[%ws]--%wZ--\n", TESTMODNAME, &wDeviceName);
	//ExFreePool(pwTestBUFF);

	UNICODE_STRING  wDeviceName = { 0 };
	UNICODE_STRING  wCopyDeviceName = { 0 };
	STRING DeviceName = { 0 };
	PCHAR pTemBuffer = "c:\\1231\\1\\345\\546";

	RtlInitAnsiString(&DeviceName, pTemBuffer);
	RtlAnsiStringToUnicodeString(&wDeviceName, &DeviceName, TRUE);
	DbgPrint("--[%ws]--%wZ--\n", TESTMODNAME, &wDeviceName);

	// 先给目标分配内存
	wCopyDeviceName.Buffer = ExAllocatePool(NonPagedPool, EXTSIZE);
	if (wCopyDeviceName.Buffer == NULL)
		return ntstatus;
	wCopyDeviceName.MaximumLength = EXTSIZE;
	RtlZeroMemory(wCopyDeviceName.Buffer, EXTSIZE);


	// 字符串复制
	RtlCopyUnicodeString(&wCopyDeviceName, &wDeviceName);
	DbgPrint("--[%ws]--%wZ--\n", TESTMODNAME, &wCopyDeviceName);

	// 转大写 
	RtlUpcaseUnicodeString(&wDeviceName,&wDeviceName, FALSE);

	// 释放由RtlAnsiStringToUnicodeString 和 ExAllocatePool 分配的内存
	RtlFreeUnicodeString(&wDeviceName);
	ExFreePool(wCopyDeviceName.Buffer); 


	/////////////////////// 
	// new
	////////////////////
	PWCHAR pwTemBuffer = ExAllocatePool(NonPagedPool, EXTSIZE);
	if (pwTemBuffer == NULL)
		return ntstatus;
	RtlZeroMemory(pwTemBuffer, EXTSIZE);

	RtlStringCbCopyW(pwTemBuffer, EXTSIZE, PATHHEAD);
	RtlStringCbCatW(pwTemBuffer, EXTSIZE, pTemBuffer);

	UNICODE_STRING Tem1 = { 0 }, Tem2 = { 0 };
	RtlInitUnicodeString(&Tem1, pwTemBuffer);
	RtlInitUnicodeString(&Tem2, PATHHEAD);
	// 匹配前缀 tem2 是否是tem1 的前缀 FALSE 大小写敏感
	if (RtlPrefixUnicodeString(&Tem2, &Tem1, FALSE))
	{
		DbgPrint("--RtlPrefixUnicodeString Be Find--\n");
	}


	UNICODE_STRING Tem3 = { 0 }, Tem4 = { 0 };
	RtlInitUnicodeString(&Tem3, L"c:\\1ABCS\\abdw\\aavb\\dbsa.TXT");
	RtlInitUnicodeString(&Tem4, L"c:\\1ABCS\\ABDw\\AAvb\\dbsa.TXT");

	// TRUE 大小写不敏感
	if (RtlEqualUnicodeString(&Tem3, &Tem4, TRUE))
	{
		DbgPrint("--RtlEqualUnicodeString Be Find--\n");
	}

	UNICODE_STRING Tem5 = { 0 };
	RtlInitUnicodeString(&Tem5, L"*DBSA.TXT");
	// 搜索/匹配函数
	if (FsRtlIsNameInExpression(&Tem5, &Tem4, TRUE, NULL))
	{
		DbgPrint("--FsRtlIsNameInExpression Searched--\n");
	}
	
	return ntstatus;
}

// ****