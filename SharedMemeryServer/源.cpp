//共享内存的进程一行要以 “管理员”身份运行visual studio程序才行！！！！
#include "GloveData.h"
#include <iostream>
#include <Windows.h>
#include <string>
#include <tchar.h>
using namespace std;

#define BUF_SIZE 1024
TCHAR szName[] = TEXT("Global\\MyFileMappingObject");    //指向同一块共享内存的名字

HANDLE hMapFile_out;
LPCTSTR pBuf_out;
TCHAR szName_out[] = TEXT("Global\\MyFileMappingObject_out");    //指向同一块共享内存的名字

int main()
{

	//注意这两个声明
	HANDLE hMapFile;
	LPCTSTR pBuf;


#pragma region ShareMemory
	//创建内存共享区域
	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security
		PAGE_READWRITE,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		BUF_SIZE,                // maximum object size (low-order DWORD)
		szName);                 // name of mapping object

	if (hMapFile == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d).\n"),
			GetLastError());
		return 1;
	}

	//将内存映射到该进程
	pBuf = (LPTSTR)MapViewOfFile(hMapFile,   // handle to map object
		FILE_MAP_ALL_ACCESS, // read/write permission
		0,
		0,
		BUF_SIZE);

	if (pBuf == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());

		CloseHandle(hMapFile);

		return 1;
	}
#pragma endregion ShareMemory

#pragma region SharedMemery_out
	hMapFile_out = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security
		PAGE_READWRITE,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		BUF_SIZE,                // maximum object size (low-order DWORD)
		szName_out);                 // name of mapping object

	if (hMapFile_out == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d).\n"),
			GetLastError());
		return 1;
	}
	pBuf_out = (LPTSTR)MapViewOfFile(hMapFile_out,   // handle to map object
		FILE_MAP_ALL_ACCESS, // read/write permission
		0,
		0,
		BUF_SIZE);

	if (pBuf_out == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());

		CloseHandle(hMapFile_out);

		return 1;
	}
#pragma endregion SharedMemery_out
	//从main开始至此是获取名为"Global\\MyFileMappingObject"的共享内存的指针

	//以下代码，B不停写共享内存pBuf

	GloveData glovedata;

	float *Transdata = new float[26];
	float *OptimizedData = new float[26];
	while (1)
	{
		glovedata.GetGloveData();
		for (int i = 0;i < 27;i++)
		{
			Transdata[i] = glovedata.HandinfParams[i];
		}
		memcpy((PVOID)pBuf, Transdata, sizeof(float) * 26);

		memcpy(OptimizedData, (float*)pBuf_out, sizeof(float) * 26);
		glovedata.SetOptimizedData(OptimizedData);
		Sleep(10);
	}

	return 0;
}
