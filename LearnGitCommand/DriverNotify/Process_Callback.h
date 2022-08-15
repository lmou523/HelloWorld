#pragma once

typedef struct _NON_PAGED_DEBUG_INFO                                            // 9 / 9 elements; 0x0020 / 0x0020 Bytes
{
    UINT16                      Signature;                                      // 0x0000 / 0x0000; 0x0002 / 0x0002 Bytes
    UINT16                      Flags;                                          // 0x0002 / 0x0002; 0x0002 / 0x0002 Bytes
    ULONG32                     Size;                                           // 0x0004 / 0x0004; 0x0004 / 0x0004 Bytes
    UINT16                      Machine;                                        // 0x0008 / 0x0008; 0x0002 / 0x0002 Bytes
    UINT16                      Characteristics;                                // 0x000A / 0x000A; 0x0002 / 0x0002 Bytes
    ULONG32                     TimeDateStamp;                                  // 0x000C / 0x000C; 0x0004 / 0x0004 Bytes
    ULONG32                     CheckSum;                                       // 0x0010 / 0x0010; 0x0004 / 0x0004 Bytes
    ULONG32                     SizeOfImage;                                    // 0x0014 / 0x0014; 0x0004 / 0x0004 Bytes
    UINT64                      ImageBase;                                      // 0x0018 / 0x0018; 0x0008 / 0x0008 Bytes
} NON_PAGED_DEBUG_INFO, * PNON_PAGED_DEBUG_INFO;

typedef struct _KLDR_DATA_TABLE_ENTRY
{
    LIST_ENTRY InLoadOrderLinks;
    PVOID ExceptionTable;
    ULONG ExceptionTableSize;
    // ULONG padding on IA64
    PVOID GpValue;
    PNON_PAGED_DEBUG_INFO NonPagedDebugInfo;
    PVOID DllBase;
    PVOID EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
    ULONG Flags;
    USHORT LoadCount;
    USHORT __Unused5;
    PVOID SectionPointer;
    ULONG CheckSum;
    // ULONG padding on IA64
    PVOID LoadedImports;
    PVOID PatchInformation;
} KLDR_DATA_TABLE_ENTRY, * PKLDR_DATA_TABLE_ENTRY;


NTSTATUS InstallProcessRegCallback(PDRIVER_OBJECT pDriver);
void UninstallProcessRegCallback();