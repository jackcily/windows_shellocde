#include<Windows.h>
#include<stdio.h>
void test()
{
	_asm
	{
		//system("dir"); //64 69 72
		push ebp
		mov  ebp, esp

		xor ebx, ebx
		push ebx
		mov  byte ptr[ebp - 04h], 64h
		mov  byte ptr[ebp - 03h], 69h
		mov  byte ptr[ebp - 02h], 72h

		lea  ebx, [ebp - 04h]     ;获取字符串的首地址   传参数时 字符串只需要传送地址 
		push ebx
		mov ebx, 0x74323b10       ;system函数地址
		call ebx

		; 恢复堆栈
		add esp, 0x4             ;恢复esp  此处的行为默认call函数不会自己清理栈中的参数
		pop ebx
		pop ebp
	}

}

int main()
{
	HINSTANCE LibHandle = LoadLibrary("msvcrt.dll"); //要获取DLL的内存地址
	printf("msvcrt Address = 0x%x \n", LibHandle);
	LPTSTR getaddr = (LPTSTR)GetProcAddress(LibHandle, "system"); //获取DLL中导出函数地址
	printf("system Address = 0x % x \n", getaddr);
	
	test(); 
	return 0;
}