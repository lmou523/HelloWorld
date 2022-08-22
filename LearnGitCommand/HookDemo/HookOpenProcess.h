
#ifndef  __HOOKOPENPROCESS_H__
#define  __HOOKOPENPROCESS_H__


#pragma once



typedef NTSTATUS (* FuncNtOpenProcess)(
	PHANDLE            ProcessHandle,
	ACCESS_MASK        DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PCLIENT_ID         ClientId
);


// ��װ����
NTSTATUS InstallHook();

//ж�ع���
NTSTATUS UninstallHook();



#endif // ! __HOOKOPENPROCESS_H__