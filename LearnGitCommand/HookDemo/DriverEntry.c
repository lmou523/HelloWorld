#include <ntifs.h>
#include <windef.h>

#include "HookOpenProcess.h"
#include "SSTDFound.h"





// 驱动反初始化
void DrvUnload(PDRIVER_OBJECT pDriver)
{
	// UninstallHook();
}
// 
// 驱动入口函数
// 

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING reg_path)
{
	// 设置反初始化的回调函数
	pDriver->DriverUnload = DrvUnload;

	NTSTATUS ntstatus = STATUS_SUCCESS;

	// ntstatus =  InstallHook();
	ULONG64 uladdr = GetSSDTApiAddr(pDriver,114);

	return ntstatus;
}
