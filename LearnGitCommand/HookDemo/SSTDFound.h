
#ifndef __SSTDFOUND_H__
#define __SSTDFOUND_H__

#pragma once


#define IMAGE_SIZEOF_SHORT_NAME              8
#define FOUNDSZ 0x1000


/*

ptablebase  --------ƫ����--------------
ָ��һ��LONG������ ���ƫ���� Ȼ���׵�ַ�� ��ƫ����������λ��  ���Ǿ��庯����ַ

*/
typedef struct _SYSTEM_SERVICETABLE
{
    PLONG       pTableBase;
    PVOID       pServiceCountBase;
    ULONG_PTR   ulNumberOfServices;
    ULONG       NumberOfServices;
    PCHAR       pArgumentTable;

}SYSTEM_SERVICETABLE, *PSYSTEM_SERVICETABLE;



// ��ຯ��δ���� �ڲ��ҵ�ַʱ ʹ��MmGetSystemRoutineAddress �Ҳ���
// �����Ҫ����ssdt��ȥ��ѯ

// ���뵱ǰ�������� ���� ���ص�ַ
ULONG64 GetSSDTApiAddr(PDRIVER_OBJECT pDriver, ULONG dwIndex);


#endif