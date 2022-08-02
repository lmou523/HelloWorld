#pragma once

#include <ntifs.h>

#define LMDDPUBNAME "LMDDDRIVE"

#define MODELENAME	"HelloWorld"


#define DRIVERNAME L"\\Device\\LMDD_Device"

#define SYMBALNAME L"\\??\\LMDD_Device" 

#define EXTSIZE 1024


#define IOCTL_MUL CTL_CODE(FILE_DEVICE_UNKNOWN,0x9027,METHOD_BUFFERED,FILE_ANY_ACCESS)