#pragma once

#include <ntifs.h>
#include <Ntstrsafe.h>

#define LMDDPUBNAME "LMDDDRIVE"

#define MODELENAME	"HelloWorld"


#define PATHHEAD L"\\??\\"

#define DRIVERNAME L"\\Device\\LMDD_Device"

#define SYMBALNAME L"\\??\\LMDD_Device" 

#define EXTSIZE 1024

#define COPYFILEBUFSZ 4096

#define TESTMODNAME L"TEST DEMO"
#define TESTUNOCODESTR L"TEST STR FFFFFFFFFFFFFFFFFFF"
#define IOCTL_MUL CTL_CODE(FILE_DEVICE_UNKNOWN,0x9027,METHOD_BUFFERED,FILE_ANY_ACCESS)