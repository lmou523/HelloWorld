


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