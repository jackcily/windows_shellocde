#include<Windows.h>
#include<stdio.h>
#include<psapi.h>
//���Ȼ�õ�ǰ���̵ľ�� OpenProcess
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
	printf("���processid  %d\n", dwProcessId);
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId);  //dwProcessId
	if (NULL == hProcess)
	{
		printf("���ܴ򿪽���[ID:0x%x]���,������:0x%08x\n", dwProcessId);
		return -1;
	}

	EnumProcessModules(hProcess, NULL, 0, &dwNeeded);
	printf("���processid  %d\n", dwProcessId);
	phMods = (HMODULE*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwProcessId);

	if (EnumProcessModules(hProcess, phMods, dwNeeded, &dwNeeded))
	{
		for (i = 0; i < (dwNeeded / sizeof(HMODULE)); i++)
		{
			ZeroMemory(szModName, MAX_PATH * sizeof(TCHAR));

			//�������ʹ��GetModuleFileName���е�ģ�����ƻ�ȡ���������������޷��ҵ���ģ��Ĵ���
			if (GetModuleFileNameEx(hProcess, phMods[i], szModName, MAX_PATH))
			{
				printf("%s\n", szModName);
			}
		}
	}

	HeapFree(GetProcessHeap(), 0, phMods);
	CloseHandle(hProcess);
}