#pragma once


// 此函数为测试函数 会在入口函数进行调用测试 主要是记录一些字符相关的操作

NTSTATUS TestUseStrOper();


// IRP_MJ_CREATE
NTSTATUS IrpCreateCallBack(PDEVICE_OBJECT pDevice,PIRP pIrp );

// IRP_MJ_CLOSE
NTSTATUS IrpCloseCallBack(PDEVICE_OBJECT pDevice, PIRP pIrp);

// IRP_MJ_CLEANUP
NTSTATUS IrpCleanUpCallBack(PDEVICE_OBJECT pDevice, PIRP pIrp);

// IRP_MJ_READ
NTSTATUS IrpReadCallBack(PDEVICE_OBJECT pDevice, PIRP pIrp);

// IRP_MJ_WRITE
NTSTATUS IrpWriteCallBack(PDEVICE_OBJECT pDevice, PIRP pIrp);

// IRP_MJ_DEVICE_CONTROL
NTSTATUS IrpDeviceControlCallBack(PDEVICE_OBJECT pDevice, PIRP pIrp);