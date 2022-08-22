
#ifndef  __HOOKOPENPROCESS_H__
#define  __HOOKOPENPROCESS_H__


#pragma once



typedef NTSTATUS (* FuncNtOpenProcess)(
	PHANDLE            ProcessHandle,
	ACCESS_MASK        DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PCLIENT_ID         ClientId
);


// 安装钩子
NTSTATUS InstallHook();

//卸载钩子
NTSTATUS UninstallHook();



#endif // ! __HOOKOPENPROCESS_H__