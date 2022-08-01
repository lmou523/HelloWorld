


#include <ntifs.h>

#include "MajorFunc.h"


// IRP_CREATE_OPERATION
NTSTATUS IrpCreateCallBack(PDEVICE_OBJECT pDriver, PIRP pIrp)
{
	NTSTATUS ntstatus = STATUS_SUCCESS;

	DbgPrint("Lmdd Drvice has been Created\n");

	pIrp->IoStatus.Status = ntstatus;
	pIrp->IoStatus.Information = 0;

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return ntstatus;
}


// IRP_MJ_CLOSE
NTSTATUS IrpCloseCallBack(PDEVICE_OBJECT pDriver, PIRP pIrp)
{
	NTSTATUS ntstatus = STATUS_SUCCESS;

	DbgPrint("Lmdd Drvice has been Close\n");

	pIrp->IoStatus.Status = ntstatus;
	pIrp->IoStatus.Information = 0;

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return ntstatus;
}

// IRP_MJ_CLEANUP
NTSTATUS IrpCleanUpCallBack(PDEVICE_OBJECT pDriver, PIRP pIrp)
{
	NTSTATUS ntstatus = STATUS_SUCCESS;

	DbgPrint("Lmdd Drvice has been CleanUp\n");

	pIrp->IoStatus.Status = ntstatus;
	pIrp->IoStatus.Information = 0;

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return ntstatus;
}