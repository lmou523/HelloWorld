#pragma once

#include <ntifs.h>
#include <Ntstrsafe.h>

#define LMDDPUBNAME "LMDDDRIVE"

#define MODELENAME	"HelloWorld"

#define SYSPATHHEAD L"\\"

#define PATHHEAD L"\\??\\"

#define DRIVERNAME L"\\Device\\LMDD_Device"

#define SYMBALNAME L"\\??\\LMDD_Device" 

#define DSTFILEPATH L"\\??\\C:\\Windows\\System32\\drivers\\1lmdd.sys"

// #define MEMNAMETAG 0x0927
#define MEMNAMETAG 'lmdd'
#define EXTSIZE 1024

#define COPYFILEBUFSZ 4096

#define TESTMODNAME L"TEST DEMO"
#define TESTUNOCODESTR L"TEST STR FFFFFFFFFFFFFFFFFFF"
#define IOCTL_MUL CTL_CODE(FILE_DEVICE_UNKNOWN,0x9027,METHOD_BUFFERED,FILE_ANY_ACCESS)