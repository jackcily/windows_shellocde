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

		lea  ebx, [ebp - 04h]     ;��ȡ�ַ������׵�ַ   ������ʱ �ַ���ֻ��Ҫ���͵�ַ 
		push ebx
		mov ebx, 0x74323b10       ;system������ַ
		call ebx

		; �ָ���ջ
		add esp, 0x4             ;�ָ�esp  �˴�����ΪĬ��call���������Լ�����ջ�еĲ���
		pop ebx
		pop ebp
	}

}

int main()
{
	HINSTANCE LibHandle = LoadLibrary("msvcrt.dll"); //Ҫ��ȡDLL���ڴ��ַ
	printf("msvcrt Address = 0x%x \n", LibHandle);
	LPTSTR getaddr = (LPTSTR)GetProcAddress(LibHandle, "system"); //��ȡDLL�е���������ַ
	printf("system Address = 0x % x \n", getaddr);
	
	test(); 
	return 0;
}