
#ifndef __SSTDFOUND_H__
#define __SSTDFOUND_H__

#pragma once

#define FOUNDSZ 0x1000


// ��ຯ��δ���� �ڲ��ҵ�ַʱ ʹ��MmGetSystemRoutineAddress �Ҳ���
// �����Ҫ����ssdt��ȥ��ѯ

// �������� ���ص�ַ
ULONG64 GetSSDTApiAddr(ULONG dwIndex);


#endif