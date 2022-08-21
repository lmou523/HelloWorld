

#include <ntifs.h>
#include <windef.h>
#include "SSTDFound.h" 

/*

ptablebase  --------ƫ����--------------
ָ��һ��LONG������ ���ƫ���� Ȼ���׵�ַ�� ��ƫ����������λ��  ���Ǿ��庯����ַ 

*/
typedef struct _SYSTEM_SERVICETABLE
{
	PVOID pTableBase;
	PVOID pServiceCountBase;
	ULONG64 ulNumberOfServices;
	PVOID pUnknown;
}SYSTEM_SERVICETABLE,*PSYSTEM_SERVICETABLE;


ULONG64 GetSSDTApiAddr(ULONG dwIndex)
{
	DWORD64 dwmsr = __readmsr(0xC0000082);
	// ָ��KiSystemCall64
	PUCHAR temPtr = (PUCHAR)dwmsr;
	LONG offset = 0;

	PSYSTEM_SERVICETABLE pServiceTable = NULL;

	for (int i = 0; i < FOUNDSZ; ++i)
	{
		if (*(temPtr + i) == 0x4c && *(temPtr + i + 1) == 0x8d && *(temPtr + i + 2) == 0x15)
		{
			offset = *(PLONG)(temPtr + i + 3);
			pServiceTable = (PSYSTEM_SERVICETABLE)(temPtr + i + 7 + offset);
			break;
		}
	}

	if (pServiceTable == NULL)
	{
		return NULL;
	}

	PLONG arrOffsetStart = (PLONG)pServiceTable->pTableBase;

	LONG apioffset = arrOffsetStart[dwIndex] >> 4;

	ULONG64 ulFoundApiAddr = (ULONG64)pServiceTable->pTableBase + (ULONG64)apioffset;

	DbgPrint("found ssdt index : <%u>  addr<%p> \n ", dwIndex, ulFoundApiAddr);
	
	return ulFoundApiAddr;
}