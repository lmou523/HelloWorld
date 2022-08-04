
#include "CommonDef.h"
#include "OperatorFile.h"




NTSTATUS KernelDeleteFile(PCWCHAR pwFilePath)
{
	if (pwFilePath == NULL)
		return STATUS_UNSUCCESSFUL;

	NTSTATUS ntstatusRet = STATUS_SUCCESS;
	UNICODE_STRING wstrPath = { 0 };
	OBJECT_ATTRIBUTES objFileAttribute = { 0 };

	RtlInitUnicodeString(&wstrPath, pwFilePath);
	InitializeObjectAttributes(&objFileAttribute, &wstrPath, OBJ_CASE_INSENSITIVE, NULL, NULL);
	ntstatusRet = ZwDeleteFile(&objFileAttribute);

	if (!NT_SUCCESS(ntstatusRet))
	{
		DbgPrint("--Delete File %wZ Falied -- Error Code:%x\n", &wstrPath, ntstatusRet);
	}

	return ntstatusRet;
}

// �����ļ� ʾ������ Ч�ʵ� 
NTSTATUS KernelCopyFile(PCWCHAR pwSrcFilePath, PCWCHAR pwDstFilePath)
{
	if (pwSrcFilePath == NULL || pwDstFilePath == NULL)
		return STATUS_UNSUCCESSFUL;

	NTSTATUS ntstatusRet = STATUS_SUCCESS;

	HANDLE				hFileSrc = NULL;
	UNICODE_STRING		wstrSrcPath = { 0 };
	OBJECT_ATTRIBUTES	objSrcFileAttribute = { 0 };
	IO_STATUS_BLOCK		iostackSrc = { 0 };

	RtlInitUnicodeString(&wstrSrcPath, pwSrcFilePath);
	InitializeObjectAttributes(&objSrcFileAttribute, &wstrSrcPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

	ntstatusRet = ZwOpenFile(&hFileSrc, GENERIC_ALL, &objSrcFileAttribute, &iostackSrc, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_SYNCHRONOUS_IO_NONALERT);

	if (!NT_SUCCESS(ntstatusRet))
	{
		DbgPrint("--Open File %wZ Falied -- Error Code:%x\n", &wstrSrcPath, ntstatusRet);
		return ntstatusRet;
	}


	FILE_STANDARD_INFORMATION fileInfoSrc = { 0 };

	ntstatusRet = ZwQueryInformationFile(hFileSrc, &iostackSrc, &fileInfoSrc, sizeof(fileInfoSrc),FileStandardInformation);

	if (!NT_SUCCESS(ntstatusRet))
	{
		DbgPrint("--Query File  %wZ  Info Falied -- Error Code:%x\n", &wstrSrcPath, ntstatusRet);
		ZwClose(hFileSrc);

		return ntstatusRet;
	}


	PVOID pFileBuff = ExAllocatePool(NonPagedPool, fileInfoSrc.EndOfFile.QuadPart);

	if (pFileBuff == NULL)
	{
		DbgPrint("--ExAllocatePool Falied -- Error Code:%x\n", ntstatusRet);
		ZwClose(hFileSrc);

		return ntstatusRet;
	}

	RtlZeroMemory(pFileBuff, fileInfoSrc.EndOfFile.QuadPart);

	LARGE_INTEGER readOffset = { 0 };
	readOffset.QuadPart = 0;

	ntstatusRet = ZwReadFile(hFileSrc, NULL, NULL, NULL, &iostackSrc, pFileBuff, fileInfoSrc.EndOfFile.QuadPart,&readOffset,NULL);

	if (!NT_SUCCESS(ntstatusRet))
	{
		DbgPrint("--ZwReadFile Falied -- Error Code:%x\n", ntstatusRet);
		
		ExFreePool(pFileBuff);
		ZwClose(hFileSrc);

		return ntstatusRet;
	}
	ZwClose(hFileSrc);

	DbgPrint("--IOInfo -- %lld--\n",iostackSrc.Information);

	// ����Ĵ����Ѿ����ļ� ��ȡ���ڴ���

	HANDLE				hFileDst = NULL;
	UNICODE_STRING		wstrDstPath = { 0 };
	OBJECT_ATTRIBUTES	objDstFileAttribute = { 0 };
	IO_STATUS_BLOCK		iostackDst = { 0 };

	RtlInitUnicodeString(&wstrDstPath, pwDstFilePath);
	InitializeObjectAttributes(&objDstFileAttribute, &wstrDstPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

	ntstatusRet = ZwCreateFile(	&hFileDst, 
								GENERIC_ALL, 
								&objDstFileAttribute, 
								&iostackDst, 
								NULL, 
								FILE_ATTRIBUTE_NORMAL,
								FILE_SHARE_WRITE,
								FILE_SUPERSEDE, 
								FILE_SYNCHRONOUS_IO_NONALERT,
								NULL,
								0);

	if (!NT_SUCCESS(ntstatusRet))
	{
		DbgPrint("--ZwCreateFile : %wZ Falied  -- Error Code:%x\n",&wstrDstPath,ntstatusRet);

		ExFreePool(pFileBuff);

		ZwClose(hFileDst);

		return ntstatusRet;
	}

	LARGE_INTEGER writeoffset = { 0 };

	ntstatusRet = ZwWriteFile(hFileDst, NULL, NULL, NULL, &iostackDst, pFileBuff, fileInfoSrc.EndOfFile.QuadPart, &writeoffset, NULL);

	if (!NT_SUCCESS(ntstatusRet))
	{
		DbgPrint("--ZwWriteFile : %wZ Falied  -- Error Code:%x\n", &wstrDstPath, ntstatusRet);

		ExFreePool(pFileBuff);
		ZwClose(hFileDst);

		return ntstatusRet;
	}

	DbgPrint("--ZwWriteFile : %wZ Successful  -- Write Size :%lld\n", &wstrDstPath, iostackDst.Information);

	ExFreePool(pFileBuff);
	ZwClose(hFileDst);

	return ntstatusRet;
}


// �˺���δʵ��
NTSTATUS KernelCopyFileEx(PCWCHAR pwSrcFilePath, PCWCHAR pwDstFilePath)
{
	NTSTATUS ntstatusRet = STATUS_SUCCESS;


	return  ntstatusRet;
}
