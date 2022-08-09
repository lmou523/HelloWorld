#pragma once




extern BOOLEAN bUserModeEventInit;
extern PKEVENT PUserModeEventThread;


NTSTATUS StartThread();

NTSTATUS EndThread();
