
#include "DNCommDef.h"
#include "Process_Callback.h"




HANDLE hRegProcess = NULL;

// ֪ͨǰ�ص�
OB_PREOP_CALLBACK_STATUS
PreOpProcess(
	PVOID RegistrationContext,
	POB_PRE_OPERATION_INFORMATION OperationInformation
)
{
	PUCHAR strImageFileName = PsGetProcessImageFileName(OperationInformation->Object);

	DbgPrint("--PreOpProcess Process Name <%s>\n", strImageFileName);

	if (strstr(strImageFileName, "notepad") != NULL)
	{
		OperationInformation->Parameters->CreateHandleInformation.DesiredAccess = 0;
		OperationInformation->Parameters->DuplicateHandleInformation.DesiredAccess = 0;
	}

	return OB_PREOP_SUCCESS;
}

// ֪ͨ��ص�

VOID
PostOpProcess(
	PVOID RegistrationContext,
	POB_POST_OPERATION_INFORMATION OperationInformation
)
{
	PUCHAR strImageFileName = PsGetProcessImageFileName(OperationInformation->Object);

	DbgPrint("--PostOpProcess Process Name <%s>\n", strImageFileName);

}



NTSTATUS InstallProcessRegCallback(PDRIVER_OBJECT pDriver)
{
	NTSTATUS ntstatus = STATUS_SUCCESS;

	// ȥ��ǩ������
	PKLDR_DATA_TABLE_ENTRY pldr = pDriver->DriverSection;
	pldr->Flags |= 0x20;
	// End

	OB_CALLBACK_REGISTRATION obcReg = { 0 };
	OB_OPERATION_REGISTRATION oboReg = { 0 };

	obcReg.Version = ObGetFilterVersion();
	obcReg.OperationRegistrationCount = 1;
	obcReg.OperationRegistration = &oboReg;


	UNICODE_STRING wstrAttHigh = { 0 };
	RtlInitUnicodeString(&wstrAttHigh, L"321999");

	// ������һ���ں˶���PsProcessType ���̶���
	oboReg.ObjectType = PsProcessType;
	// ��������͸���
	oboReg.Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE; 
	oboReg.PreOperation = PreOpProcess;
	oboReg.PostOperation = PostOpProcess;

	ntstatus = ObRegisterCallbacks(&obcReg, &hRegProcess);



	return ntstatus;
}
void UninstallProcessRegCallback()
{
	ObUnRegisterCallbacks(hRegProcess);
}