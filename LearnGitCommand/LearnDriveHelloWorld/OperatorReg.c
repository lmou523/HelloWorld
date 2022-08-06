
#include "CommonDef.h"
#include "OperatorReg.h"

#include "OperatorFile.h"


NTSTATUS TestOperatorReg(PDRIVER_OBJECT pDriver, PUNICODE_STRING reg_path)
{
	if (reg_path == NULL || pDriver == NULL)
		return STATUS_UNSUCCESSFUL;

	NTSTATUS ntstatusRet = STATUS_SUCCESS;

	HANDLE hReg = NULL;
	ULONG ulKeyOp = 0;
	OBJECT_ATTRIBUTES objRegAttribute = { 0 };
	InitializeObjectAttributes(&objRegAttribute, reg_path, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

	PVOID pRegKeyInfo = NULL;
	/* Create*/
	/*
	ntstatusRet = ZwCreateKey(&hReg, KEY_ALL_ACCESS, &objRegAttribute, 0, NULL, REG_OPTION_DONT_VIRTUALIZE, &ulKeyOp);

	if (NT_SUCCESS(ntstatusRet))
	{
		if (ulKeyOp == REG_CREATED_NEW_KEY)
		{
			DbgPrint("Key has be Created\n");
		}
		else if (ulKeyOp == REG_OPENED_EXISTING_KEY)
		{
			DbgPrint("Key has be opened\n");
		}
		else
		{
			DbgPrint("OperReg:%wZ Error !--ErrCode:%d\n",reg_path,ulKeyOp);
		}
	}
	*/

	/* Open*/
	
	ntstatusRet = ZwOpenKey(&hReg, KEY_ALL_ACCESS, &objRegAttribute );

	if (NT_SUCCESS(ntstatusRet))
	{
		pRegKeyInfo = ExAllocatePool(NonPagedPool, EXTSIZE);
		if (pRegKeyInfo != NULL)
		{
			UNICODE_STRING strRegName = { 0 };
			RtlInitUnicodeString(&strRegName, L"ImagePath");
			RtlZeroMemory(pRegKeyInfo, EXTSIZE);

			// ���ֵ̫�������
			ntstatusRet = ZwQueryValueKey(hReg, &strRegName, KeyValuePartialInformation, pRegKeyInfo, EXTSIZE, &ulKeyOp);
			
			if (NT_SUCCESS(ntstatusRet))
			{
				PKEY_VALUE_PARTIAL_INFORMATION ptemRegKeyVal = (PKEY_VALUE_PARTIAL_INFORMATION)pRegKeyInfo;

				PWCHAR pwstrVal = ptemRegKeyVal->Data;
				// �õ�·�� CopyFile��sytemĿ¼
				DbgPrint("--ImagePath %ws--\n", pwstrVal);
				// �����ļ� ��ǰ·��������  L"\\??\\C:\\Windows\\System32\\drivers\\1lmdd.sys"
				// �������豸\SystemRoot\System32\drivers\kbdhid.sys

				UNICODE_STRING strNormalHead = { 0 };
				UNICODE_STRING strDriverPath = { 0 };
				RtlInitUnicodeString(&strNormalHead, PATHHEAD);
				RtlInitUnicodeString(&strDriverPath, pwstrVal);

				if (RtlPrefixUnicodeString(&strNormalHead,&strDriverPath,FALSE))
				{
					KernelCopyFile(pwstrVal, DSTFILEPATH);
				}


				// ����·��
				PWCHAR pTemVal = L"\\SystemRoot\\System32\\drivers\\1lmdd.sys";
				ntstatusRet = ZwSetValueKey(hReg,&strRegName,0,REG_EXPAND_SZ, pTemVal,(wcslen(pTemVal) + 1) * sizeof(WCHAR));


				if (!NT_SUCCESS(ntstatusRet))
				{
					DbgPrint("ZwSetValueKey Error ErrorCode %x\n",ntstatusRet);
				}

				//
				// ʹ������һ�ַ�ʽд��ע���
				//
				ULONG ulTemStart = 1;
				RtlWriteRegistryValue(RTL_REGISTRY_ABSOLUTE, reg_path->Buffer,L"Start",REG_DWORD,&ulTemStart,sizeof(ULONG));


				// ����������������
				ntstatusRet = RtlCheckRegistryKey(RTL_REGISTRY_SERVICES,L"lmdd");
				if (NT_SUCCESS(ntstatusRet))
				{
					DbgPrint("--be found-- \n");
				}
				else
				{
					DbgPrint("--Not found-- \n");
					RtlCreateRegistryKey(RTL_REGISTRY_SERVICES, L"lmdd"); 
				}
			}
			else
			{
				DbgPrint("ZwQueryValue Error\n");
			}
			ExFreePool(pRegKeyInfo);
		}

		ZwClose(hReg);
	}
	

	return ntstatusRet;
}


NTSTATUS OpenReg()
{
	NTSTATUS ntstatusRet = STATUS_SUCCESS;


	return ntstatusRet;
}




