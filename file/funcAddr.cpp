
#include<Windows.h>
#include<stdio.h>
void test()
{
	_asm
	{
		; ��ȡkernel32.dll��ڵ�ַ

		xor ecx, ecx
		mov eax, fs:[ecx + 0x30]; EAX = PEB
		mov eax, [eax + 0xc]; EAX = PEB->Ldr
		mov esi, [eax + 0x14]; ESI = PEB->Ldr.InMemOrder  dll˫������head
		lodsd; EAX = Second module
		xchg eax, esi; EAX = ESI, ESI = EAX;��ʱesi���ǵ�һ��ģ������list
		lodsd; EAX = Third(kernel32)
		mov ebx, [eax + 0x10]; EBX = Base address

		; ����kernel32.dll�ĵ�����
		mov edx, [ebx + 0x3c]; EDX = DOS->e_lfanew
		add edx, ebx; EDX = PE Header ���ϻ���ַ��ȡ���Ե�ַ
		mov edx, [edx + 0x78]; EDX = Offset export table
		add edx, ebx; EDX = Export table
		mov esi, [edx + 0x20]; ESI = Offset namestable
		add esi, ebx; ESI = Names table
		xor ecx, ecx; EXC = 0;ecx����Get_Function��ѭ��������

		; ����GetProcAddress������
		Get_Function:
		inc ecx; Increment the ordinal
			lodsd; Get name offset
			add eax, ebx; Get function name

			cmp dword ptr[eax], 0x50746547; GetP; �ж��ǲ���GetProcAddress
			jnz Get_Function
			cmp dword ptr[eax + 0x4], 0x41636f72; rocA
			jnz Get_Function
			cmp dword ptr[eax + 0x8], 0x65726464; ddre
			jnz Get_Function

			mov esi, [edx + 0x24]; ESI = Offset ordinals
			add esi, ebx; ESI = Ordinals table
			mov cx, [esi + ecx * 2]; Number of function ��ű��д洢��word����Ե�ַ ֻ�������ֽ� cx���Ǻ������
			dec ecx
			mov esi, [edx + 0x1c]; Offset address table
			add esi, ebx; ESI = Address table
			mov edx, [esi + ecx * 4]; EDX = Pointer(offset)  GetProcAddress��ʼ��ַ����Ե�ַ
			add edx, ebx; EDX = GetProcAddress


			; �Ѿ��ҵ�GetProcAddress����ʼ��ַ Ѱ��LoadLibraryA��ʼ��ַ
			xor ecx, ecx; ECX = 0
			push ebx; Kernel32 base address
			push edx; GetProcAddress
			push ecx; 0
			push 0x41797261; aryA
			push 0x7262694c; Libr
			push 0x64616f4c; Load
			push esp; "LoadLibrary"
			push ebx; Kernel32 base address
			call edx; GetProcAddress(LL); ����GetProcAddress����Ѱ��LoadLibrary  call�����Լ��������ջ����ջ����  ����ֵ�洢��eax

			add esp, 0xc; pop "LoadLibrary"
			pop ecx; ECX = 0
			push eax; EAX = LoadLibrary
			push ecx
			mov cx, 0x6c6c; 00LL
			push ecx
			push 0x642e7472; rt.d
			push 0x6376736d; msvc
			push esp; "msvcrt.dll"
			call eax; LoadLibrary("msvcrt.dll");����LoadLibraryA��������msvcrt.dll
			

			; system�ڴ��ַ
			add esp, 0x10; Clean stack
			mov edx, [esp + 0x4]; EDX = GetProcAddress
			xor ecx, ecx; ECX = 0
			push ecx; 73797374 656d
			mov  ecx, 0x61626d65; emba
			push ecx
			sub dword ptr[esp + 0x3], 0x61; Remove ��a��
			sub dword ptr[esp + 0x2], 0x62; Remove ��b��
			push 0x74737973; syst
			push esp; system
			push eax; msvcrt.dll address
			call edx; GetProc(system);����system����

			add esp, 0x10; Cleanup stack
			; ִ�к��ĳ���
			push ebp
			mov  ebp, esp
			sub  esp, 0x4
			xor esi, esi
			mov  esi, 0x00726964; dir
			mov  dword ptr[ebp - 04h], esi
			lea  esi, [ebp - 04h]
			push esi
			call eax

			; ��ջƽ��
			add esp, 0x8; �ָ�esp
			pop esi

			; �˳�����
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