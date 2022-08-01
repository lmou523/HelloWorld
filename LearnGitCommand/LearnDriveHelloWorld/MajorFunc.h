#pragma once





// IRP_MJ_CREATE
NTSTATUS IrpCreateCallBack(PDEVICE_OBJECT pDriver,PIRP pIrp );

// IRP_MJ_CLOSE
NTSTATUS IrpCloseCallBack(PDEVICE_OBJECT pDriver, PIRP pIrp);

// IRP_MJ_CLEANUP
NTSTATUS IrpCleanUpCallBack(PDEVICE_OBJECT pDriver, PIRP pIrp);