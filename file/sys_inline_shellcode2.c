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
	

	//test(); 
	unsigned char shellcode[] = "\x55\x8B\xEC\x81\xEC\xC0\x00\x00\x00\x53\x56\x57\x8D\xBD\x40\xFF\xFF\xFF\xB9\x30\x00\x00\x00\xB8\xCC\xCC\xCC\xCC\xF3\xAB\x55\x8B\xEC\x81\xEC\xC0\x00\x00\x00\x55\x8B\xEC\x83\xEC\x04\x33\xDB\xBB\x64\x69\x72\x00\x89\x5D\xFC\x8D\x5D\xFC\x53\xBB\x10\x3B\x32\x74\xFF\xD3\x83\xC4\x04\x5B\x5D\x5F\x5E\x5B\x81\xC4\xC0\x00\x00\x00\x3B\xEC\xE8\xB3\xFA\xFF\xFF\x8B\xE5\x5D\xC3";
	((void(*)()) & shellcode)();
	return 0;
}