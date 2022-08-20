
#include <ntifs.h>
#include <windef.h>
#include <TdiKrnl.h>



typedef struct _NETWORK_ADDRESS
{
	UCHAR address[4];
	USHORT usPort;
}NETWORK_ADDRESS,*PNETWORK_ADDRESS;


// PDRIVER_OBJECT pDeviceObject = NULL;
PDEVICE_OBJECT pDeviceObject = NULL;
PDEVICE_OBJECT pNextDeviceObject = NULL;
// 不关心的操作
NTSTATUS NotSupport(PDEVICE_OBJECT pDevice, PIRP pirp)
{
	IoSkipCurrentIrpStackLocation(pirp);

	return IoCallDriver(pNextDeviceObject, pirp);
}

// 关心的操作
NTSTATUS MyDispath(PDEVICE_OBJECT pDevice, PIRP pirp)
{
	PIO_STACK_LOCATION pioStack = NULL;
	if (pDevice == pDeviceObject)
	{
		// DbgPrint("This is Filter");

		pioStack = IoGetCurrentIrpStackLocation(pirp);
		if (pioStack == NULL)
			return STATUS_UNSUCCESSFUL;

		if (pioStack->MinorFunction == TDI_CONNECT)
		{
			PTDI_REQUEST_KERNEL_CONNECT pTDIConnect = (PTDI_REQUEST_KERNEL_CONNECT)(&pioStack->Parameters);

			PTA_ADDRESS ta_addr = ((PTRANSPORT_ADDRESS)(pTDIConnect->RequestConnectionInformation->RemoteAddress))->Address;

			PTDI_ADDRESS_IP tdi_addr = (PTDI_ADDRESS_IP)ta_addr->Address;

			DWORD dwAddress = tdi_addr->in_addr;
			USHORT usPort = tdi_addr->sin_port;
		
			NETWORK_ADDRESS netaddr = { 0 };

			netaddr.address[0] = ((PUCHAR)&dwAddress)[0];
			netaddr.address[1] = ((PUCHAR)&dwAddress)[1];
			netaddr.address[2] = ((PUCHAR)&dwAddress)[2];
			netaddr.address[3] = ((PUCHAR)&dwAddress)[3];
			netaddr.usPort = ((usPort & 0xff) << 8) + (usPort>>8) ;

			DbgPrint("Connect Ip Address <%d.%d.%d.%d Port:%d>\n",
				netaddr.address[0], netaddr.address[1], netaddr.address[2], netaddr.address[3], 
				netaddr.usPort);

		}
	}


	IoSkipCurrentIrpStackLocation(pirp);

	return IoCallDriver(pNextDeviceObject, pirp);
}

// 驱动反初始化
void DrvUnload(PDRIVER_OBJECT pDriver)
{
	IoDetachDevice(pNextDeviceObject);

	IoDeleteDevice(pDeviceObject);
	return;
}

// 
// 驱动入口函数
// 

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING reg_path)
{
	// 设置反初始化的回调函数
	pDriver->DriverUnload = DrvUnload;

	NTSTATUS ntstatus = STATUS_SUCCESS;

	ntstatus = IoCreateDevice(pDriver, 0, NULL, FILE_DEVICE_NETWORK, FILE_DEVICE_SECURE_OPEN, FALSE, &pDeviceObject);

	if (!NT_SUCCESS(ntstatus))
	{
		DbgPrint("[FilterNet] Bad Create \n");
		return ntstatus;
	}
	
	for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		pDriver->MajorFunction[i] = NotSupport;
	}
	pDriver->MajorFunction[IRP_MJ_INTERNAL_DEVICE_CONTROL] = MyDispath;


	UNICODE_STRING strDeviceName = { 0 };
	RtlInitUnicodeString(&strDeviceName, L"\\Device\\Tcp");

	ntstatus = IoAttachDevice(pDeviceObject, &strDeviceName, &pNextDeviceObject);

	if (!NT_SUCCESS(ntstatus))
	{
		DbgPrint("[FilterNet] IoAttachDevice Error \n");
		IoDeleteDevice(pDeviceObject);
		return ntstatus;
	}


	return ntstatus;
}