#pragma once



// IRQL:�ж�����;

// ��ϰIRQL��һЩ����


NTSTATUS TestIRQL(PDRIVER_OBJECT pDriver, PUNICODE_STRING reg_path);


// ���������� 
NTSTATUS AcquireSpinLock(PKIRQL pkirql);
// ����������
NTSTATUS ReleaseSpinLock(KIRQL kirql);