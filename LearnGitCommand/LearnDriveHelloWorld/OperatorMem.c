
#include "CommonDef.h"
#include "OperatorMem.h"
#include "PracticeIRQL.h"

LIST_ENTRY listHead = { 0 };

NTSTATUS TestMemory(PDRIVER_OBJECT pDriver, PUNICODE_STRING reg_path)
{
	NTSTATUS ntstatusRet = STATUS_SUCCESS;
	PVOID pTemBuf = ExAllocatePoolWithTag(NonPagedPool, EXTSIZE, MEMNAMETAG);

	if (pTemBuf != NULL)
	{
		RtlZeroMemory(pTemBuf, EXTSIZE);
		RtlFillMemory(pTemBuf, EXTSIZE, 0xcc);

		ExFreePoolWithTag(pTemBuf, MEMNAMETAG);

		// 内存比较函数
		// RtlCompareMemory(); 有多少个字节相等
		// RtlEqualMemory(); 判断两片内存是否相等
	}
	if(listHead.Blink == NULL || listHead.Flink == NULL)
		InitializeListHead(&listHead);

	DbgPrint("-listHead:-%p-Flink:-%p-Blink:-%p\n",&listHead, listHead.Flink, listHead.Blink);

	return ntstatusRet;
}


NTSTATUS AddProcessToList(HANDLE hPid, PEPROCESS peprocess, PCHAR pProcessName)
{
	if (listHead.Blink == NULL || listHead.Flink == NULL)
		InitializeListHead(&listHead);

	NTSTATUS ntstatusRet = STATUS_SUCCESS;
	if (hPid == NULL || peprocess == NULL || pProcessName == NULL)
		return STATUS_UNSUCCESSFUL;

	PLinkList pTem = ExAllocatePool(NonPagedPool, sizeof(LinkList));
	// PLIST_ENTRY pTemList = NULL;
	// pTemList = CONTAINING_RECORD(pTem, LinkList, listPtr);

	if (pTem == NULL)
		return STATUS_UNSUCCESSFUL;
	RtlZeroMemory(pTem, sizeof(LinkList));

	// 赋值
	pTem->hPid = hPid;
	pTem->peprocess = peprocess;
	RtlCopyMemory(pTem->ProcessName, pProcessName, strlen(pProcessName));

	// 使用自旋锁
	KIRQL kirql = 0;
	AcquireSpinLock(&kirql);
	InsertTailList(&listHead, &(pTem->listPtr));
	ReleaseSpinLock(kirql);

	return ntstatusRet;
}




NTSTATUS RemoveAllProcessList()
{
	if (listHead.Blink == NULL || listHead.Flink == NULL)
		InitializeListHead(&listHead);

	NTSTATUS ntstatusRet = STATUS_SUCCESS;

	PLIST_ENTRY pTemNode = NULL;
	PLinkList pTemData = NULL;
	while (listHead.Blink != &listHead)
	{
		pTemNode = RemoveTailList(&listHead);
		pTemData = CONTAINING_RECORD(pTemNode,LinkList, listPtr);

		DbgPrint("-pid:-%d-peprocess:-%p-ProcessName-%s\n",pTemData->hPid,pTemData->peprocess,pTemData->ProcessName);

		ExFreePool(pTemData);
		pTemData = NULL;
	}

	return ntstatusRet;
}


// 遍历链表 不删除
NTSTATUS ForEachList()
{
	if (listHead.Blink == NULL || listHead.Flink == NULL)
		InitializeListHead(&listHead);

	NTSTATUS ntstatusRet = STATUS_SUCCESS;

	PLIST_ENTRY pTemNode = NULL;
	PLinkList pTemData = NULL;

	for (pTemNode = listHead.Flink; pTemNode != &listHead; pTemNode = pTemNode->Flink)
	{
		pTemData = CONTAINING_RECORD(pTemNode, LinkList, listPtr);
		DbgPrint("-pid:-%d-peprocess:-%p-ProcessName-%s\n", pTemData->hPid, pTemData->peprocess, pTemData->ProcessName);
	}

	return ntstatusRet;
}