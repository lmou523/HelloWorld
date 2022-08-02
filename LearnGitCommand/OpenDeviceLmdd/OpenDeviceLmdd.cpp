// OpenDeviceLmdd.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>

#include <stdlib.h>

#define WDeviceName L"\\\\.\\LMDD_Device"
#define DeviceName  "\\\\.\\LMDD_Device"


#define IOCTL_MUL CTL_CODE(FILE_DEVICE_UNKNOWN,0x9027,METHOD_BUFFERED,FILE_ANY_ACCESS)

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

        return 0;
    }

    printf("[Device :%s]   Open  Successful\n", DeviceName);
    system("pause");
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

    printf("--In %d--Out %d--Really Info %d--\n", dwSrc, dwDst, dwOperatorSize);


    CloseHandle(hDevice);
    system("pause");
    return 0;
}

