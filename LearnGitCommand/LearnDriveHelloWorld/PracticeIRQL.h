#pragma once



// IRQL:中断请求级;

// 练习IRQL的一些操作


NTSTATUS TestIRQL(PDRIVER_OBJECT pDriver, PUNICODE_STRING reg_path);


// 自旋锁上锁 
NTSTATUS AcquireSpinLock(PKIRQL pkirql);
// 自旋锁解锁
NTSTATUS ReleaseSpinLock(KIRQL kirql);