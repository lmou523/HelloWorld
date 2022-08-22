


#include <ntifs.h>
#include <windef.h>

#include "SSTDFound.h" 

typedef struct _LDR_DATA_TABLE_ENTRY {
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	union {
		LIST_ENTRY HashLinks;
		struct {
			PVOID SectionPointer;
			ULONG CheckSum;
		};
	};
	union {
		struct {
			ULONG TimeDateStamp;
		};
		struct {
			PVOID LoadedImports;
		};
	};
	struct _ACTIVATION_CONTEXT* EntryPointActivationContext;

	PVOID PatchInformation;

} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;


static ULONG_PTR GetKernelMoudelBase(PDRIVER_OBJECT pDriver,PULONG pImagSize , PWCHAR pMoudleName)
{
	UNICODE_STRING strKernelName = { 0 };
	ULONG_PTR ulRet = NULL;

	PLDR_DATA_TABLE_ENTRY pEntry = (PLDR_DATA_TABLE_ENTRY)pDriver->DriverSection;
	PLDR_DATA_TABLE_ENTRY pFirst = NULL;

	RtlInitUnicodeString(&strKernelName, pMoudleName);

	__try
	{
		do
		{
			if (pEntry->BaseDllName.Buffer != NULL)
			{
				if (RtlCompareUnicodeString(&pEntry->BaseDllName, &strKernelName, TRUE) == 0)
				{
					ulRet = (ULONG_PTR)pEntry->DllBase;
					if (pImagSize != NULL)
					{
						*pImagSize = pEntry->SizeOfImage;
					}
					break;
				}

				pEntry = (PLDR_DATA_TABLE_ENTRY)pEntry->InLoadOrderLinks.Blink;
			}

		} while (pEntry->InLoadOrderLinks.Blink != (ULONGLONG)pFirst);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return ulRet;
	}
	return ulRet;
}



//Based on: https://github.com/hfiref0x/WinObjEx64
static PSYSTEM_SERVICETABLE SSDTfind(PDRIVER_OBJECT pDriver)
{
	static PSYSTEM_SERVICETABLE pSSDT = NULL;


	if (pSSDT != NULL)
		return pSSDT;

	PUCHAR pKernelBase = NULL;
	ULONG ulKernelSize = 0;
	pKernelBase = (PUCHAR)GetKernelMoudelBase(pDriver,&ulKernelSize,L"ntoskrnl.exe");

	if (pKernelBase == NULL)
		return NULL;

#ifdef  _DEBUG
	DbgPrint("kernel <%p>--size<%x>\n", pKernelBase, ulKernelSize);
#endif //  _DEBUG
	const unsigned char KiSystemServiceStartPattern[] = { 0x8B, 0xF8, 0xC1, 0xEF, 0x07, 0x83, 0xE7, 0x20, 0x25, 0xFF, 0x0F, 0x00, 0x00 };
	const ULONG signatureSize = sizeof(KiSystemServiceStartPattern);
	BOOL found = FALSE;
	ULONG KiSSSOffset = 0;
	for (KiSSSOffset = 0; KiSSSOffset < ulKernelSize - signatureSize; KiSSSOffset++)
	{
		if (RtlCompareMemory(((unsigned char*)pKernelBase + KiSSSOffset), KiSystemServiceStartPattern, signatureSize) == signatureSize)
		{
			found = TRUE;
			break;
		}
	}
	if (!found)
		return NULL;

	ULONG_PTR address = pKernelBase +KiSSSOffset + signatureSize;
	LONG relativeOffset = 0;
	if ((*(unsigned char*)address == 0x4c) &&
		(*(unsigned char*)(address + 1) == 0x8d) &&
		(*(unsigned char*)(address + 2) == 0x15))
	{
		relativeOffset = *(LONG*)(address + 3);
	}
	if (relativeOffset == 0)
	{
		return NULL;
	}
	pSSDT = (PSYSTEM_SERVICETABLE)(address + relativeOffset + 7);

	return pSSDT;
}

ULONG64 GetSSDTApiAddr(PDRIVER_OBJECT pDriver,ULONG dwIndex)
{
	PSYSTEM_SERVICETABLE pServiceTable = NULL;

	/*
	// 这种搜索方法 在win10 1809 以后就不通用了

	DWORD64 dwmsr = __readmsr(0xC0000082);
	// 指向KiSystemCall64
	PUCHAR temPtr = (PUCHAR)dwmsr;
	LONG offset = 0;

	for (int i = 0; i < FOUNDSZ; ++i)
	{
		if (*(temPtr + i) == 0x4c && *(temPtr + i + 1) == 0x8d && *(temPtr + i + 2) == 0x15)
		{
			offset = *(PLONG)(temPtr + i + 3);
			pServiceTable = (PSYSTEM_SERVICETABLE)(temPtr + i + 7 + offset);
			break;
		}
	}
	*/
	// 采取更通用的方式来获取ServiceTable 会更慢

	pServiceTable = SSDTfind(pDriver);


	if (pServiceTable == NULL)
	{
		return NULL;
	}

	PLONG arrOffsetStart = pServiceTable->pTableBase;

	LONG apioffset = arrOffsetStart[dwIndex] >> 4;

	ULONG64 ulFoundApiAddr = (ULONG64)pServiceTable->pTableBase + (ULONG64)apioffset;

	DbgPrint("found ssdt index : <%u>  addr<%p> \n ", dwIndex, ulFoundApiAddr);
	
	return ulFoundApiAddr;
}