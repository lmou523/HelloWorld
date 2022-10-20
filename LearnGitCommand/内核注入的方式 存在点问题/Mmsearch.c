#include <ntifs.h>
#include <ntimage.h>
#include <windef.h>
#include "MmSearch.h"


typedef struct _PEB_LDR_DATA
{
	ULONG Length;
	ULONG Initialized;
	PVOID SsHandle;
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	PVOID	EntryInProgress;
	ULONG	ShutdownInProgress;
	PVOID	ShutdownThreadId;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY    InLoadOrderLinks;
	LIST_ENTRY    InMemoryOrderLinks;
	LIST_ENTRY    InInitializationOrderLinks;
	PVOID            DllBase;
	PVOID            EntryPoint;
	ULONG            SizeOfImage;
	UNICODE_STRING    FullDllName;
	UNICODE_STRING     BaseDllName;
	ULONG            Flags;
	USHORT            LoadCount;
	USHORT            TlsIndex;
	PVOID            SectionPointer;
	ULONG            CheckSum;
	PVOID            LoadedImports;
	PVOID            EntryPointActivationContext;
	PVOID            PatchInformation;
	LIST_ENTRY    ForwarderLinks;
	LIST_ENTRY    ServiceTagLinks;
	LIST_ENTRY    StaticLinks;
	PVOID            ContextInformation;
	ULONG            OriginalBase;
	LARGE_INTEGER    LoadTime;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

typedef struct _LDR_DATA_TABLE_ENTRY32
{
	LIST_ENTRY32 InLoadOrderLinks;
	LIST_ENTRY32 InMemoryOrderModuleList;
	LIST_ENTRY32 InInitializationOrderModuleList;
	ULONG DllBase;
	ULONG EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING32 FullDllName;
	UNICODE_STRING32 BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	union
	{
		LIST_ENTRY32 HashLinks;
		struct
		{
			ULONG SectionPointer;
			ULONG CheckSum;
		};
	};
	union
	{
		ULONG TimeDateStamp;
		ULONG LoadedImports;
	};
	ULONG EntryPointActivationContext;
	ULONG PatchInformation;
} LDR_DATA_TABLE_ENTRY32, *PLDR_DATA_TABLE_ENTRY32;


typedef struct _PEB_LDR_DATA32 {
	ULONG Length;
	UCHAR Initialized;
	ULONG SsHandle;
	LIST_ENTRY32 InLoadOrderModuleList;
	LIST_ENTRY32 InMemoryOrderModuleList;
	LIST_ENTRY32 InInitializationOrderModuleList;
	ULONG EntryInProgress;
} PEB_LDR_DATA32, *PPEB_LDR_DATA32;


typedef struct _MYPEB
{
	union
	{
		struct dummy00
		{
			UCHAR InheritedAddressSpace;
			UCHAR ReadImageFileExecOptions;
			UCHAR BeingDebugged;
			UCHAR BitField;
		};
		PVOID dummy01;
	};

	PVOID Mutant;
	PVOID ImageBaseAddress;
	PVOID Ldr;
	PVOID ProcessParameters;
	PVOID SubSystemData;
	PVOID ProcessHeap;
	PVOID FastPebLock;
	PVOID AtlThunkSListPtr;
	PVOID IFEOKey;
	PVOID CrossProcessFlags;
	PVOID KernelCallbackTable;
	ULONG SystemReserved;
	ULONG AtlThunkSListPtr32;
	PVOID ApiSetMap;
} MYPEB, *PMYPEB;

typedef struct _PEB32 {
	UCHAR InheritedAddressSpace;
	UCHAR ReadImageFileExecOptions;
	UCHAR BeingDebugged;
	UCHAR Spare;
	ULONG Mutant;
	ULONG ImageBaseAddress;
	ULONG/*PPEB_LDR_DATA32*/ Ldr;
} PEB32, *PPEB32;



typedef enum _SYSTEM_INFORMATION_CLASS
{
	SystemBasicInformation = 0x0,
	SystemProcessorInformation = 0x1,
	SystemPerformanceInformation = 0x2,
	SystemTimeOfDayInformation = 0x3,
	SystemPathInformation = 0x4,
	SystemProcessInformation = 0x5,
	SystemCallCountInformation = 0x6,
	SystemDeviceInformation = 0x7,
	SystemProcessorPerformanceInformation = 0x8,
	SystemFlagsInformation = 0x9,
	SystemCallTimeInformation = 0xa,
	SystemModuleInformation = 0xb,
	SystemLocksInformation = 0xc,
	SystemStackTraceInformation = 0xd,
	SystemPagedPoolInformation = 0xe,
	SystemNonPagedPoolInformation = 0xf,
	SystemHandleInformation = 0x10,
	SystemObjectInformation = 0x11,
	SystemPageFileInformation = 0x12,
	SystemVdmInstemulInformation = 0x13,
	SystemVdmBopInformation = 0x14,
	SystemFileCacheInformation = 0x15,
	SystemPoolTagInformation = 0x16,
	SystemInterruptInformation = 0x17,
	SystemDpcBehaviorInformation = 0x18,
	SystemFullMemoryInformation = 0x19,
	SystemLoadGdiDriverInformation = 0x1a,
	SystemUnloadGdiDriverInformation = 0x1b,
	SystemTimeAdjustmentInformation = 0x1c,
	SystemSummaryMemoryInformation = 0x1d,
	SystemMirrorMemoryInformation = 0x1e,
	SystemPerformanceTraceInformation = 0x1f,
	SystemObsolete0 = 0x20,
	SystemExceptionInformation = 0x21,
	SystemCrashDumpStateInformation = 0x22,
	SystemKernelDebuggerInformation = 0x23,
	SystemContextSwitchInformation = 0x24,
	SystemRegistryQuotaInformation = 0x25,
	SystemExtendServiceTableInformation = 0x26,
	SystemPrioritySeperation = 0x27,
	SystemVerifierAddDriverInformation = 0x28,
	SystemVerifierRemoveDriverInformation = 0x29,
	SystemProcessorIdleInformation = 0x2a,
	SystemLegacyDriverInformation = 0x2b,
	SystemCurrentTimeZoneInformation = 0x2c,
	SystemLookasideInformation = 0x2d,
	SystemTimeSlipNotification = 0x2e,
	SystemSessionCreate = 0x2f,
	SystemSessionDetach = 0x30,
	SystemSessionInformation = 0x31,
	SystemRangeStartInformation = 0x32,
	SystemVerifierInformation = 0x33,
	SystemVerifierThunkExtend = 0x34,
	SystemSessionProcessInformation = 0x35,
	SystemLoadGdiDriverInSystemSpace = 0x36,
	SystemNumaProcessorMap = 0x37,
	SystemPrefetcherInformation = 0x38,
	SystemExtendedProcessInformation = 0x39,
	SystemRecommendedSharedDataAlignment = 0x3a,
	SystemComPlusPackage = 0x3b,
	SystemNumaAvailableMemory = 0x3c,
	SystemProcessorPowerInformation = 0x3d,
	SystemEmulationBasicInformation = 0x3e,
	SystemEmulationProcessorInformation = 0x3f,
	SystemExtendedHandleInformation = 0x40,
	SystemLostDelayedWriteInformation = 0x41,
	SystemBigPoolInformation = 0x42,
	SystemSessionPoolTagInformation = 0x43,
	SystemSessionMappedViewInformation = 0x44,
	SystemHotpatchInformation = 0x45,
	SystemObjectSecurityMode = 0x46,
	SystemWatchdogTimerHandler = 0x47,
	SystemWatchdogTimerInformation = 0x48,
	SystemLogicalProcessorInformation = 0x49,
	SystemWow64SharedInformationObsolete = 0x4a,
	SystemRegisterFirmwareTableInformationHandler = 0x4b,
	SystemFirmwareTableInformation = 0x4c,
	SystemModuleInformationEx = 0x4d,
	SystemVerifierTriageInformation = 0x4e,
	SystemSuperfetchInformation = 0x4f,
	SystemMemoryListInformation = 0x50,
	SystemFileCacheInformationEx = 0x51,
	SystemThreadPriorityClientIdInformation = 0x52,
	SystemProcessorIdleCycleTimeInformation = 0x53,
	SystemVerifierCancellationInformation = 0x54,
	SystemProcessorPowerInformationEx = 0x55,
	SystemRefTraceInformation = 0x56,
	SystemSpecialPoolInformation = 0x57,
	SystemProcessIdInformation = 0x58,
	SystemErrorPortInformation = 0x59,
	SystemBootEnvironmentInformation = 0x5a,
	SystemHypervisorInformation = 0x5b,
	SystemVerifierInformationEx = 0x5c,
	SystemTimeZoneInformation = 0x5d,
	SystemImageFileExecutionOptionsInformation = 0x5e,
	SystemCoverageInformation = 0x5f,
	SystemPrefetchPatchInformation = 0x60,
	SystemVerifierFaultsInformation = 0x61,
	SystemSystemPartitionInformation = 0x62,
	SystemSystemDiskInformation = 0x63,
	SystemProcessorPerformanceDistribution = 0x64,
	SystemNumaProximityNodeInformation = 0x65,
	SystemDynamicTimeZoneInformation = 0x66,
	SystemCodeIntegrityInformation = 0x67,
	SystemProcessorMicrocodeUpdateInformation = 0x68,
	SystemProcessorBrandString = 0x69,
	SystemVirtualAddressInformation = 0x6a,
	SystemLogicalProcessorAndGroupInformation = 0x6b,
	SystemProcessorCycleTimeInformation = 0x6c,
	SystemStoreInformation = 0x6d,
	SystemRegistryAppendString = 0x6e,
	SystemAitSamplingValue = 0x6f,
	SystemVhdBootInformation = 0x70,
	SystemCpuQuotaInformation = 0x71,
	SystemNativeBasicInformation = 0x72,
	SystemErrorPortTimeouts = 0x73,
	SystemLowPriorityIoInformation = 0x74,
	SystemBootEntropyInformation = 0x75,
	SystemVerifierCountersInformation = 0x76,
	SystemPagedPoolInformationEx = 0x77,
	SystemSystemPtesInformationEx = 0x78,
	SystemNodeDistanceInformation = 0x79,
	SystemAcpiAuditInformation = 0x7a,
	SystemBasicPerformanceInformation = 0x7b,
	SystemQueryPerformanceCounterInformation = 0x7c,
	SystemSessionBigPoolInformation = 0x7d,
	SystemBootGraphicsInformation = 0x7e,
	SystemScrubPhysicalMemoryInformation = 0x7f,
	SystemBadPageInformation = 0x80,
	SystemProcessorProfileControlArea = 0x81,
	SystemCombinePhysicalMemoryInformation = 0x82,
	SystemEntropyInterruptTimingInformation = 0x83,
	SystemConsoleInformation = 0x84,
	SystemPlatformBinaryInformation = 0x85,
	SystemThrottleNotificationInformation = 0x86,
	SystemHypervisorProcessorCountInformation = 0x87,
	SystemDeviceDataInformation = 0x88,
	SystemDeviceDataEnumerationInformation = 0x89,
	SystemMemoryTopologyInformation = 0x8a,
	SystemMemoryChannelInformation = 0x8b,
	SystemBootLogoInformation = 0x8c,
	SystemProcessorPerformanceInformationEx = 0x8d,
	SystemSpare0 = 0x8e,
	SystemSecureBootPolicyInformation = 0x8f,
	SystemPageFileInformationEx = 0x90,
	SystemSecureBootInformation = 0x91,
	SystemEntropyInterruptTimingRawInformation = 0x92,
	SystemPortableWorkspaceEfiLauncherInformation = 0x93,
	SystemFullProcessInformation = 0x94,
	SystemKernelDebuggerInformationEx = 0x95,
	SystemBootMetadataInformation = 0x96,
	SystemSoftRebootInformation = 0x97,
	SystemElamCertificateInformation = 0x98,
	SystemOfflineDumpConfigInformation = 0x99,
	SystemProcessorFeaturesInformation = 0x9a,
	SystemRegistryReconciliationInformation = 0x9b,
	MaxSystemInfoClass = 0x9c,
} SYSTEM_INFORMATION_CLASS;

NTSTATUS NTAPI ZwQuerySystemInformation(
	__in SYSTEM_INFORMATION_CLASS SystemInformationClass,
	__out_bcount_opt(SystemInformationLength) PVOID SystemInformation,
	__in ULONG SystemInformationLength,
	__out_opt PULONG ReturnLength
	);

NTKERNELAPI
PPEB
PsGetProcessPeb(
__in PEPROCESS Process
);

NTKERNELAPI
PVOID
PsGetProcessWow64Process(
_In_ PEPROCESS  Process
);




PVOID       AsdGetModuleExport(IN PVOID pBase, IN PCCHAR name_ord)
{
	PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER)pBase;
	PIMAGE_NT_HEADERS32 pNtHdr32 = NULL;
	PIMAGE_NT_HEADERS64 pNtHdr64 = NULL;
	PIMAGE_EXPORT_DIRECTORY pExport = NULL;
	ULONG expSize = 0;
	ULONG_PTR pAddress = 0;
	PUSHORT pAddressOfOrds;
	PULONG  pAddressOfNames;
	PULONG  pAddressOfFuncs;
	ULONG i;


	ASSERT(pBase != NULL);
	if (pBase == NULL)
	{
		return NULL;
	}


	/// Not a PE file
	if (pDosHdr->e_magic != IMAGE_DOS_SIGNATURE)
	{
		return NULL;
	}


	pNtHdr32 = (PIMAGE_NT_HEADERS32)((PUCHAR)pBase + pDosHdr->e_lfanew);
	pNtHdr64 = (PIMAGE_NT_HEADERS64)((PUCHAR)pBase + pDosHdr->e_lfanew);

	// Not a PE file
	if (pNtHdr32->Signature != IMAGE_NT_SIGNATURE)
	{
		return NULL;
	}


	// 64 bit image
	if (pNtHdr32->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
	{
		pExport = (PIMAGE_EXPORT_DIRECTORY)(pNtHdr64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress + (ULONG_PTR)pBase);
		expSize = pNtHdr64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
	}
	// 32 bit image
	else
	{
		pExport = (PIMAGE_EXPORT_DIRECTORY)(pNtHdr32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress + (ULONG_PTR)pBase);
		expSize = pNtHdr32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
	}

	pAddressOfOrds = (PUSHORT)(pExport->AddressOfNameOrdinals + (ULONG_PTR)pBase);
	pAddressOfNames = (PULONG)(pExport->AddressOfNames + (ULONG_PTR)pBase);
	pAddressOfFuncs = (PULONG)(pExport->AddressOfFunctions + (ULONG_PTR)pBase);

	for (i = 0; i < pExport->NumberOfFunctions; ++i)
	{
		USHORT OrdIndex = 0xFFFF;
		PCHAR  pName = NULL;

		// Find by index
		if ((ULONG_PTR)name_ord <= 0xFFFF)
		{
			OrdIndex = (USHORT)i;
		}
		// Find by name
		else if ((ULONG_PTR)name_ord > 0xFFFF && i < pExport->NumberOfNames)
		{
			pName = (PCHAR)(pAddressOfNames[i] + (ULONG_PTR)pBase);
			OrdIndex = pAddressOfOrds[i];
		}
		// Weird params
		else
		{
			return NULL;
		}


		if (((ULONG_PTR)name_ord <= 0xFFFF && (USHORT)((ULONG_PTR)name_ord) == OrdIndex + pExport->Base) ||
			((ULONG_PTR)name_ord > 0xFFFF && strcmp(pName, name_ord) == 0))
		{
			pAddress = pAddressOfFuncs[OrdIndex] + (ULONG_PTR)pBase;

			// Check forwarded export
			if (pAddress >= (ULONG_PTR)pExport && pAddress <= (ULONG_PTR)pExport + expSize)
			{
				return NULL;
			}
			break;
		}
	}
	return (PVOID)pAddress;
}

ULONG64     AsdGetProcessMoudleBase(PEPROCESS   pep, PWCHAR  buffer)
{
	KAPC_STATE  kapc = { 0 };

	BOOLEAN     battached = FALSE;

	PMYPEB      peb = NULL;

	PPEB32      peb32 = NULL;

	UNICODE_STRING	queryModuleName = { 0 };

	KIRQL        tempirql = 0;

	ULONG64      uret = 0;

	RtlInitUnicodeString(&queryModuleName, (PCWSTR)buffer);

	peb32 = PsGetProcessWow64Process(pep);

	if (peb32 == NULL)
	{
		//
		//64 bit
		//
		KeStackAttachProcess(pep, &kapc);

		battached = TRUE;

		peb = (PMYPEB)PsGetProcessPeb(pep);

		PPEB_LDR_DATA	pPebLdrData = (PPEB_LDR_DATA)peb->Ldr;

		PLIST_ENTRY		pListEntryStart = NULL;

		PLIST_ENTRY		pListEntryEnd = NULL;

		PLDR_DATA_TABLE_ENTRY pLdrDataEntry = NULL;

		pListEntryStart = pListEntryEnd = pPebLdrData->InMemoryOrderModuleList.Blink;

		do
		{

			pLdrDataEntry = (PLDR_DATA_TABLE_ENTRY)CONTAINING_RECORD(pListEntryStart, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

			if (RtlCompareUnicodeString(&pLdrDataEntry->BaseDllName, &queryModuleName, TRUE) == 0)
			{
				uret = (ULONG64)pLdrDataEntry->DllBase;
				break;
			}

			pListEntryStart = pListEntryStart->Blink;

		} while (pListEntryStart != pListEntryEnd);

		DbgPrint("64bit\n");
	}
	else
	{
		//
		//wow 64
		//

		KeStackAttachProcess(pep, &kapc);

		battached = TRUE;

		PLIST_ENTRY32 pListEntryStart32 = NULL, pListEntryEnd32 = NULL;

		PLDR_DATA_TABLE_ENTRY32 pLdrDataEntry32 = NULL;

		ULONG64              ModulesBase = 0;

		pListEntryStart32 = pListEntryEnd32 = (PLIST_ENTRY32)(((PEB_LDR_DATA32*)peb32->Ldr)->InMemoryOrderModuleList.Flink);

		do {
			pLdrDataEntry32 = (PLDR_DATA_TABLE_ENTRY32)CONTAINING_RECORD(pListEntryStart32, LDR_DATA_TABLE_ENTRY32, InMemoryOrderModuleList);

			UNICODE_STRING QueryModuleName = { 0 };

			RtlInitUnicodeString(&QueryModuleName, (PWCHAR)pLdrDataEntry32->BaseDllName.Buffer);

			if (RtlEqualUnicodeString(&queryModuleName, &QueryModuleName, TRUE))
			{
				ModulesBase = (ULONG64)pLdrDataEntry32->DllBase;

				uret = ModulesBase;

			}
			pListEntryStart32 = (PLIST_ENTRY32)pListEntryStart32->Flink;

		} while (pListEntryStart32 != pListEntryEnd32);

		DbgPrint("wow64\n");
	}

	if (battached)
	{
		KeUnstackDetachProcess(&kapc);
		battached = FALSE;
	}

	return uret;
}

