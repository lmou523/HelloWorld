#include <ntifs.h>
#include <windef.h>

#include "HookOpenProcess.h"



// ****

// 由于代码段是写保护的 并且避免写入的过程中函数被调用 需要提高中断级
// 关闭写保护 提高中断级别
KIRQL WPOffX64()
{
	KIRQL irql = KeRaiseIrqlToDpcLevel();

	DWORD64 cr0 = __readcr0();

	cr0 &= 0xFFFFFFFFFFFEFFFF;
	
	__writecr0(cr0);
	_disable(); // 屏蔽中断

	return irql;
}

// 打开中断级 降低中断级别
void  WPOnX64(KIRQL irql)
{

	DWORD64 cr0 = __readcr0();

	cr0 |= 0x10000;

	__writecr0(cr0);
	_enable(); // 打开中断

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

// NtOpenProcess首地址
FuncNtOpenProcess pNtOpenProcess = NULL;
BOOL bIsInstallNtOpenProcessHook = FALSE;
// 全局数组没有执行权限
// BYTE pSaveNtOpenProcessSrcCode[100] = { 0 }; // 保存头部指令 以及jmp回去
#define SAVEBUFSZ 0x100
BYTE *pSaveNtOpenProcessSrcCode = NULL;
//nt!NtOpenProcess:
//fffff800`0416a7b0 4883ec38                sub     rsp, 38h
//fffff800`0416a7b4 65488b042588010000      mov     rax, qword ptr gs : [188h]
// 上面这两条指令会被拷贝到数组pSaveNtOpenProcessSrcCode中 然后后面会跟一个jmp
// 
// 
//fffff800`0416a7bd 448a90f6010000          mov     r10b, byte ptr[rax + 1F6h]
//fffff800`0416a7c4 4488542428              mov     byte ptr[rsp + 28h], r10b
//fffff800`0416a7c9 4488542420              mov     byte ptr[rsp + 20h], r10b
//fffff800`0416a7ce e8314effff              call    nt!PsOpenProcess(fffff800`0415f604)
//fffff800`0416a7d3 4883c438                add     rsp, 38h
//fffff800`0416a7d7 c3                      ret


// 回调函数
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

	// 原函数 里面存了头 和一个jmp
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
	// 找到了函数的首地址
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
	// 保存头部指令 20个字节
	RtlCopyMemory(pSaveNtOpenProcessSrcCode, pNtOpenProcess, 20);
	// 增加返回时的jmp 跳转回来 NtOpenProcessCallBack 的空间内
	*(PULONG64)(arrHookCode + 6) = (ULONG_PTR)pNtOpenProcess + 20;

	// 前面13个字节留给ntopenprocess 头部的指令 增加jmp指令
	RtlCopyMemory(((PUCHAR)pSaveNtOpenProcessSrcCode + 20), arrHookCode, sizeof(arrHookCode));


	{ 
		// 修改汇编代码 使得jmp到我们自己的函数地址
		*(PULONG64)(arrHookCode + 6) = (ULONG64)NtOpenProcessCallBack;

		KIRQL irqlTem = WPOffX64();
		// 写ntopenprocess头部指令
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

	// 将头写回去就行
		KIRQL irqlTem = WPOffX64();
	// 写ntopenprocess头部指令 原指令 写回去
	RtlCopyMemory(pNtOpenProcess, pSaveNtOpenProcessSrcCode,13);

	WPOnX64(irqlTem);

	ExFreePool(pSaveNtOpenProcessSrcCode);
	bIsInstallNtOpenProcessHook = FALSE;
	return ntstatus;
}






// 安装钩子
NTSTATUS InstallHook()
{
	NTSTATUS ntstatus = STATUS_SUCCESS;
	ntstatus = InstallOpenProcessHook();
	return ntstatus;
}

//卸载钩子
NTSTATUS UninstallHook()
{
	NTSTATUS ntstatus = STATUS_SUCCESS;
	ntstatus = UninstallOpenProcessHook();
	return ntstatus;
}