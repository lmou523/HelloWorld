#include <ntifs.h>
#include <windef.h>

#include "HookOpenProcess.h"



// ****

// ���ڴ������д������ ���ұ���д��Ĺ����к��������� ��Ҫ����жϼ�
// �ر�д���� ����жϼ���
KIRQL WPOffX64()
{
	KIRQL irql = KeRaiseIrqlToDpcLevel();

	DWORD64 cr0 = __readcr0();

	cr0 &= 0xFFFFFFFFFFFEFFFF;
	
	__writecr0(cr0);
	_disable(); // �����ж�

	return irql;
}

// ���жϼ� �����жϼ���
void  WPOnX64(KIRQL irql)
{

	DWORD64 cr0 = __readcr0();

	cr0 |= 0x10000;

	__writecr0(cr0);
	_enable(); // ���ж�

	KeLowerIrql(irql);
}

// ****

///*
//mov rax,Myaddr  
//jmp rax
//*/
//BYTE arrHookCode[] = {	0x48,0xB8,//mov rax,
//						0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//Myaddr
//						0xFF,0xE0};//jmp rax

BYTE arrHookCode[] = {	0xFF,0x25,//jmp
						0x00,0x00,0x00,0x00,
						0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//Myaddr
						};

// NtOpenProcess�׵�ַ
FuncNtOpenProcess pNtOpenProcess = NULL;
BOOL bIsInstallNtOpenProcessHook = FALSE;
// ȫ������û��ִ��Ȩ��
// BYTE pSaveNtOpenProcessSrcCode[100] = { 0 }; // ����ͷ��ָ�� �Լ�jmp��ȥ
#define SAVEBUFSZ 0x100
BYTE *pSaveNtOpenProcessSrcCode = NULL;
//nt!NtOpenProcess:
//fffff800`0416a7b0 4883ec38                sub     rsp, 38h
//fffff800`0416a7b4 65488b042588010000      mov     rax, qword ptr gs : [188h]
// ����������ָ��ᱻ����������pSaveNtOpenProcessSrcCode�� Ȼ�������һ��jmp
// 
// 
//fffff800`0416a7bd 448a90f6010000          mov     r10b, byte ptr[rax + 1F6h]
//fffff800`0416a7c4 4488542428              mov     byte ptr[rsp + 28h], r10b
//fffff800`0416a7c9 4488542420              mov     byte ptr[rsp + 20h], r10b
//fffff800`0416a7ce e8314effff              call    nt!PsOpenProcess(fffff800`0415f604)
//fffff800`0416a7d3 4883c438                add     rsp, 38h
//fffff800`0416a7d7 c3                      ret


// �ص�����
NTSTATUS NtOpenProcessCallBack(
	PHANDLE            ProcessHandle,
	ACCESS_MASK        DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PCLIENT_ID         ClientId
)
{
	if (ClientId->UniqueProcess == 1108)
	{
		return STATUS_UNSUCCESSFUL;
	}

	// ԭ���� �������ͷ ��һ��jmp
	FuncNtOpenProcess realNtOpenProcess = (FuncNtOpenProcess)pSaveNtOpenProcessSrcCode;
	return realNtOpenProcess(ProcessHandle, DesiredAccess,ObjectAttributes, ClientId);
}


NTSTATUS InstallOpenProcessHook()
{
	if (bIsInstallNtOpenProcessHook)
		return STATUS_SUCCESS;

	NTSTATUS ntstatus = STATUS_SUCCESS;

	UNICODE_STRING strOpenProcess = { 0 };
	RtlInitUnicodeString(&strOpenProcess, L"NtOpenProcess");

	pNtOpenProcess = MmGetSystemRoutineAddress(&strOpenProcess);
	// �ҵ��˺������׵�ַ
	if (pNtOpenProcess == NULL)
	{
		return STATUS_NOT_FOUND;
	}

	pSaveNtOpenProcessSrcCode = ExAllocatePool(NonPagedPool, SAVEBUFSZ);

	if (pSaveNtOpenProcessSrcCode == NULL)
	{
		return STATUS_NOT_FOUND;
	}

	RtlZeroMemory(pSaveNtOpenProcessSrcCode, SAVEBUFSZ);
	// ����ͷ��ָ�� 20���ֽ�
	RtlCopyMemory(pSaveNtOpenProcessSrcCode, pNtOpenProcess, 20);
	// ���ӷ���ʱ��jmp ��ת���� NtOpenProcessCallBack �Ŀռ���
	*(PULONG64)(arrHookCode + 6) = (ULONG_PTR)pNtOpenProcess + 20;

	// ǰ��13���ֽ�����ntopenprocess ͷ����ָ�� ����jmpָ��
	RtlCopyMemory(((PUCHAR)pSaveNtOpenProcessSrcCode + 20), arrHookCode, sizeof(arrHookCode));


	{ 
		// �޸Ļ����� ʹ��jmp�������Լ��ĺ�����ַ
		*(PULONG64)(arrHookCode + 6) = (ULONG64)NtOpenProcessCallBack;

		KIRQL irqlTem = WPOffX64();
		// дntopenprocessͷ��ָ��
		RtlCopyMemory(pNtOpenProcess, arrHookCode, sizeof(arrHookCode));

		WPOnX64(irqlTem);
		bIsInstallNtOpenProcessHook = TRUE;
	}


	return ntstatus;
}


NTSTATUS UninstallOpenProcessHook()
{
	if (!bIsInstallNtOpenProcessHook)
		return STATUS_SUCCESS;

	NTSTATUS ntstatus = STATUS_SUCCESS;

	// ��ͷд��ȥ����
		KIRQL irqlTem = WPOffX64();
	// дntopenprocessͷ��ָ�� ԭָ�� д��ȥ
	RtlCopyMemory(pNtOpenProcess, pSaveNtOpenProcessSrcCode,13);

	WPOnX64(irqlTem);

	ExFreePool(pSaveNtOpenProcessSrcCode);
	bIsInstallNtOpenProcessHook = FALSE;
	return ntstatus;
}






// ��װ����
NTSTATUS InstallHook()
{
	NTSTATUS ntstatus = STATUS_SUCCESS;
	ntstatus = InstallOpenProcessHook();
	return ntstatus;
}

//ж�ع���
NTSTATUS UninstallHook()
{
	NTSTATUS ntstatus = STATUS_SUCCESS;
	ntstatus = UninstallOpenProcessHook();
	return ntstatus;
}