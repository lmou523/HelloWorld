
#ifndef __SSTDFOUND_H__
#define __SSTDFOUND_H__

#pragma once


#define IMAGE_SIZEOF_SHORT_NAME              8
#define FOUNDSZ 0x1000


/*

ptablebase  --------偏移量--------------
指向一个LONG的数组 存放偏移量 然后首地址加 （偏移量右移四位）  就是具体函数地址

*/
typedef struct _SYSTEM_SERVICETABLE
{
    PLONG       pTableBase;
    PVOID       pServiceCountBase;
    ULONG_PTR   ulNumberOfServices;
    ULONG       NumberOfServices;
    PCHAR       pArgumentTable;

}SYSTEM_SERVICETABLE, *PSYSTEM_SERVICETABLE;



// 许多函数未公开 在查找地址时 使用MmGetSystemRoutineAddress 找不到
// 这就需要遍历ssdt表去查询

// 输入当前驱动对象 索引 返回地址
ULONG64 GetSSDTApiAddr(PDRIVER_OBJECT pDriver, ULONG dwIndex);


#endif