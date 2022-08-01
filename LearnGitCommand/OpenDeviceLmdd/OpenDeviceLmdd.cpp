// OpenDeviceLmdd.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>

#include <stdlib.h>

#define WDeviceName L"\\\\.\\LMDD_Device"
#define DeviceName  "\\\\.\\LMDD_Device"

int main()
{
    HANDLE hDevice = INVALID_HANDLE_VALUE;

    hDevice = CreateFile(WDeviceName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hDevice == INVALID_HANDLE_VALUE)
    {
        printf("[Device :%s]   Open  Fail\n", DeviceName);

        return 0;
    }

    printf("[Device :%s]   Open  Successful\n", DeviceName);

    system("pause");

    CloseHandle(hDevice);

    system("pause");

    return 0;
}

