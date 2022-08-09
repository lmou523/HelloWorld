// OpenDeviceLmdd.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>

#include <stdlib.h>
#include <process.h>

#define WDeviceName L"\\\\.\\LMDD_Device"
#define DeviceName  "\\\\.\\LMDD_Device"


#define IOCTL_MUL CTL_CODE(FILE_DEVICE_UNKNOWN,0x9027,METHOD_BUFFERED,FILE_ANY_ACCESS)

BOOL bEventThreadIsExit = FALSE;
BOOL bHadExit = FALSE;
void TestThread(PVOID pData)
{
    HANDLE hEvent = *(PHANDLE)pData;

    while (!bEventThreadIsExit)
    {
        printf("Set kernel Event Handle \n");

        SetEvent(hEvent);
        Sleep(2000);
    }
    bHadExit = TRUE;
}


int main()
{
    HANDLE hDevice = INVALID_HANDLE_VALUE;
    const int iTemBufSize = 50;

    BYTE szBuff[iTemBufSize] = { 0 };
    DWORD dwReaded = 0;
    DWORD dwWriteed = 0;
 
    hDevice = CreateFile(WDeviceName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hDevice == INVALID_HANDLE_VALUE)
    {
        printf("[Device :%s]   Open  Fail\n", DeviceName);
        system("pause");
        return 0;
    }

    printf("[Device :%s]   Open  Successful\n", DeviceName);
    /*system("pause");
    printf("Will Read\n");

    BOOL bRet = ReadFile(hDevice, szBuff, iTemBufSize, &dwReaded, NULL);

    if (bRet)
    {
        printf("--%s----ReadSize:%d\n", (char*)szBuff, dwReaded);
    }

    system("pause");
    printf("Will Write\n");

    CHAR szWriteBuf[] = "lmdd  This message is from r3";
    WriteFile(hDevice,szWriteBuf,strlen(szWriteBuf),&dwWriteed,NULL);

    system("pause");
    printf("User Define Control\n");

    DWORD dwSrc = 8888;
    DWORD dwDst = 0;
    DWORD dwOperatorSize = 0;

    DeviceIoControl(hDevice, IOCTL_MUL, &dwSrc, sizeof(DWORD), &dwDst, sizeof(DWORD), &dwOperatorSize, NULL);

    printf("--In %d--Out %d--Really Info %d--\n", dwSrc, dwDst, dwOperatorSize);*/


    HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    HANDLE hEventThread = NULL;
    if (hEvent == NULL)
    {
        printf("Create Event failed");
        CloseHandle(hDevice);
        return -1;
    }

    if (DeviceIoControl(hDevice, IOCTL_MUL, &hEvent, sizeof(HANDLE), &dwReaded, sizeof(DWORD), &dwWriteed, NULL))
    {
        hEventThread = (HANDLE)_beginthread(TestThread, 0, &hEvent);
    }
    system("pause");

    if (hEventThread != NULL)
    {
        bEventThreadIsExit = TRUE;
        while (!bHadExit) { Sleep(5000); };
        CloseHandle(hEvent);
    }

    CloseHandle(hDevice);
    CloseHandle(hEvent);
    return 0;
}

