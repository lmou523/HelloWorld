#include <ntifs.h>
#include <windef.h>

#include "HookOpenProcess.h"






// ��������ʼ��
void DrvUnload(PDRIVER_OBJECT pDriver)
{
	UninstallHook();
}
// 
// ������ں���
// 

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING reg_path)
{
	// ���÷���ʼ���Ļص�����
	pDriver->DriverUnload = DrvUnload;

	NTSTATUS ntstatus = STATUS_SUCCESS;

	ntstatus =  InstallHook();

	return ntstatus;
}
