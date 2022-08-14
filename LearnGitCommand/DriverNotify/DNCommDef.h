#pragma once

#include <ntifs.h>
#include <windef.h>


#define MAXNOTIIFYSZ 64


#define  ERRNTSTATUSRET(ntstatus) 	if (!NT_SUCCESS(ntstatus))\
									{\
										return ntstatus;\
									}



////////////////////////////////////
NTKERNELAPI UCHAR* PsGetProcessImageFileName(__in PEPROCESS Process);