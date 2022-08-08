#pragma once


NTSTATUS TestMemory(PDRIVER_OBJECT pDriver, PUNICODE_STRING reg_path);


#define PROCESSNAMESZ 64

// 链表节点
typedef struct _LinkList
{
	HANDLE hPid;
	PEPROCESS peprocess;
	CHAR ProcessName[PROCESSNAMESZ];
	
	LIST_ENTRY listPtr;
}LinkList,*PLinkList;

// 增加一个节点 尾插
NTSTATUS AddProcessToList(HANDLE hPid, PEPROCESS peprocess,PCHAR pProcessName);

// 遍历 删除
NTSTATUS RemoveAllProcessList();

// 遍历链表 不删除
NTSTATUS ForEachList();