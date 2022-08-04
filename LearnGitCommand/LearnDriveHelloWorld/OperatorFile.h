
#pragma once

// 此头文件声明了一些内核当中对文件操作的函数
// 函数不会被调用，为练习文件操作的代码 函数实现在Operator.cpp中


/////////////////////
/*
	引用层中nt函数和zw函数没有区别
	但是在驱动层的不一样
	Zw函数和Nt函数的关系：Zw函数是先检查参数，检查发起操作是模式,再调用Nt函数
	尽量使用Zw函数 会更安全
*/

////////////////////

// 删除文件
NTSTATUS KernelDeleteFile(PCWCHAR pwFilePath);

// 调用示例 KernelDeleteFile(L"\\??\\C:\\123.exe");


// 拷贝文件 这个函数采用的是一次性全部读取 大文件会出问题
NTSTATUS KernelCopyFile(PCWCHAR pwSrcFilePath, PCWCHAR pwDstFilePath);

// 拷贝文件 这个函数采用的是分段读取 缓冲区大小为COPYFILEBUFSZ
NTSTATUS KernelCopyFileEx(PCWCHAR pwSrcFilePath, PCWCHAR pwDstFilePath);