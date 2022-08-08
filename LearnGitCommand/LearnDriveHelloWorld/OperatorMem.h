#pragma once


NTSTATUS TestMemory(PDRIVER_OBJECT pDriver, PUNICODE_STRING reg_path);


#define PROCESSNAMESZ 64

// ����ڵ�
typedef struct _LinkList
{
	HANDLE hPid;
	PEPROCESS peprocess;
	CHAR ProcessName[PROCESSNAMESZ];
	
	LIST_ENTRY listPtr;
}LinkList,*PLinkList;

// ����һ���ڵ� β��
NTSTATUS AddProcessToList(HANDLE hPid, PEPROCESS peprocess,PCHAR pProcessName);

// ���� ɾ��
NTSTATUS RemoveAllProcessList();

// �������� ��ɾ��
NTSTATUS ForEachList();