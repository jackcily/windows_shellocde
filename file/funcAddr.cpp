
#include<Windows.h>
#include<stdio.h>
void test()
{
	_asm
	{
		; 获取kernel32.dll入口地址

		xor ecx, ecx
		mov eax, fs:[ecx + 0x30]; EAX = PEB
		mov eax, [eax + 0xc]; EAX = PEB->Ldr
		mov esi, [eax + 0x14]; ESI = PEB->Ldr.InMemOrder  dll双端链表head
		lodsd; EAX = Second module
		xchg eax, esi; EAX = ESI, ESI = EAX;此时esi中是第一个模块的起点list
		lodsd; EAX = Third(kernel32)
		mov ebx, [eax + 0x10]; EBX = Base address

		; 查找kernel32.dll的导出表
		mov edx, [ebx + 0x3c]; EDX = DOS->e_lfanew
		add edx, ebx; EDX = PE Header 加上基地址获取绝对地址
		mov edx, [edx + 0x78]; EDX = Offset export table
		add edx, ebx; EDX = Export table
		mov esi, [edx + 0x20]; ESI = Offset namestable
		add esi, ebx; ESI = Names table
		xor ecx, ecx; EXC = 0;ecx用于Get_Function的循环技计数

		; 查找GetProcAddress函数名
		Get_Function:
		inc ecx; Increment the ordinal
			lodsd; Get name offset
			add eax, ebx; Get function name

			cmp dword ptr[eax], 0x50746547; GetP; 判断是不是GetProcAddress
			jnz Get_Function
			cmp dword ptr[eax + 0x4], 0x41636f72; rocA
			jnz Get_Function
			cmp dword ptr[eax + 0x8], 0x65726464; ddre
			jnz Get_Function

			mov esi, [edx + 0x24]; ESI = Offset ordinals
			add esi, ebx; ESI = Ordinals table
			mov cx, [esi + ecx * 2]; Number of function 序号表中存储的word是相对地址 只需两个字节 cx就是函数序号
			dec ecx
			mov esi, [edx + 0x1c]; Offset address table
			add esi, ebx; ESI = Address table
			mov edx, [esi + ecx * 4]; EDX = Pointer(offset)  GetProcAddress起始地址的相对地址
			add edx, ebx; EDX = GetProcAddress


			; 已经找到GetProcAddress的起始地址 寻找LoadLibraryA起始地址
			xor ecx, ecx; ECX = 0
			push ebx; Kernel32 base address
			push edx; GetProcAddress
			push ecx; 0
			push 0x41797261; aryA
			push 0x7262694c; Libr
			push 0x64616f4c; Load
			push esp; "LoadLibrary"
			push ebx; Kernel32 base address
			call edx; GetProcAddress(LL); 调用GetProcAddress函数寻找LoadLibrary  call函数自己清理调用栈的入栈参数  返回值存储于eax

			add esp, 0xc; pop "LoadLibrary"
			pop ecx; ECX = 0
			push eax; EAX = LoadLibrary
			push ecx
			mov cx, 0x6c6c; 00LL
			push ecx
			push 0x642e7472; rt.d
			push 0x6376736d; msvc
			push esp; "msvcrt.dll"
			call eax; LoadLibrary("msvcrt.dll");调用LoadLibraryA函数载入msvcrt.dll
			

			; system内存地址
			add esp, 0x10; Clean stack
			mov edx, [esp + 0x4]; EDX = GetProcAddress
			xor ecx, ecx; ECX = 0
			push ecx; 73797374 656d
			mov  ecx, 0x61626d65; emba
			push ecx
			sub dword ptr[esp + 0x3], 0x61; Remove “a”
			sub dword ptr[esp + 0x2], 0x62; Remove “b”
			push 0x74737973; syst
			push esp; system
			push eax; msvcrt.dll address
			call edx; GetProc(system);查找system函数

			add esp, 0x10; Cleanup stack
			; 执行核心程序
			push ebp
			mov  ebp, esp
			sub  esp, 0x4
			xor esi, esi
			mov  esi, 0x00726964; dir
			mov  dword ptr[ebp - 04h], esi
			lea  esi, [ebp - 04h]
			push esi
			call eax

			; 堆栈平衡
			add esp, 0x8; 恢复esp
			pop esi

			; 退出程序
			pop edx; GetProcAddress
			pop ebx; kernel32.dll base address
			mov ecx, 0x61737365; essa
			push ecx
			sub dword ptr[esp + 0x3], 0x61; Remove "a"
			push 0x636f7250; Proc
			push 0x74697845; Exit
			push esp
			push ebx; kernel32.dll base address
			call edx; GetProc(Exec)
			xor ecx, ecx; ECX = 0
			push ecx; Return code = 0
			call eax; ExitProcess
	}
}
int main()
{

	test();
		return 0;
}