
#ifndef __SSTDFOUND_H__
#define __SSTDFOUND_H__

#pragma once

#define FOUNDSZ 0x1000


// 许多函数未公开 在查找地址时 使用MmGetSystemRoutineAddress 找不到
// 这就需要遍历ssdt表去查询

// 输入索引 返回地址
ULONG64 GetSSDTApiAddr(ULONG dwIndex);


#endif