#include<Windows.h>
#include<stdio.h>
#include<psapi.h>
//首先获得当前进程的句柄 OpenProcess
//
#include<processthreadsapi.h>
int main()
{
	HMODULE* phMods = NULL;
	HANDLE hProcess = NULL;
	HANDLE hp = NULL;
	DWORD dwNeeded = 0;
	DWORD i = 0;
	TCHAR szModName[MAX_PATH] = {};

	DWORD dwProcessId = GetCurrentProcessId();
	printf("输出processid  %d\n", dwProcessId);
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId);  //dwProcessId
	if (NULL == hProcess)
	{
		printf("不能打开进程[ID:0x%x]句柄,错误码:0x%08x\n", dwProcessId);
		return -1;
	}

	EnumProcessModules(hProcess, NULL, 0, &dwNeeded);
	printf("输出processid  %d\n", dwProcessId);
	phMods = (HMODULE*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwProcessId);

	if (EnumProcessModules(hProcess, phMods, dwNeeded, &dwNeeded))
	{
		for (i = 0; i < (dwNeeded / sizeof(HMODULE)); i++)
		{
			ZeroMemory(szModName, MAX_PATH * sizeof(TCHAR));

			//在这如果使用GetModuleFileName，有的模块名称获取不到，函数返回无法找到该模块的错误
			if (GetModuleFileNameEx(hProcess, phMods[i], szModName, MAX_PATH))
			{
				printf("%s\n", szModName);
			}
		}
	}

	HeapFree(GetProcessHeap(), 0, phMods);
	CloseHandle(hProcess);
}