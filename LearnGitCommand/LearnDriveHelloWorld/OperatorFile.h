
#pragma once

// ��ͷ�ļ�������һЩ�ں˵��ж��ļ������ĺ���
// �������ᱻ���ã�Ϊ��ϰ�ļ������Ĵ��� ����ʵ����Operator.cpp��


/////////////////////
/*
	���ò���nt������zw����û������
	������������Ĳ�һ��
	Zw������Nt�����Ĺ�ϵ��Zw�������ȼ���������鷢�������ģʽ,�ٵ���Nt����
	����ʹ��Zw���� �����ȫ
*/

////////////////////

// ɾ���ļ�
NTSTATUS KernelDeleteFile(PCWCHAR pwFilePath);

// ����ʾ�� KernelDeleteFile(L"\\??\\C:\\123.exe");


// �����ļ� ����������õ���һ����ȫ����ȡ ���ļ��������
NTSTATUS KernelCopyFile(PCWCHAR pwSrcFilePath, PCWCHAR pwDstFilePath);

// �����ļ� ����������õ��Ƿֶζ�ȡ ��������СΪCOPYFILEBUFSZ
NTSTATUS KernelCopyFileEx(PCWCHAR pwSrcFilePath, PCWCHAR pwDstFilePath);