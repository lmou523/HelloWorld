#include <ntifs.h>
#include <windef.h>

#include "HookOpenProcess.h"
#include "SSTDFound.h"





// ��������ʼ��
void DrvUnload(PDRIVER_OBJECT pDriver)
{
	// UninstallHook();
}
// 
// ������ں���
// 

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING reg_path)
{
	// ���÷���ʼ���Ļص�����
	pDriver->DriverUnload = DrvUnload;

	NTSTATUS ntstatus = STATUS_SUCCESS;

	// ntstatus =  InstallHook();
	ULONG64 uladdr = GetSSDTApiAddr(pDriver,114);

	return ntstatus;
}
