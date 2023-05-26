#include<Windows.h>
#include<winternl.h>
#include<stdio.h>

// function definitions
typedef NTSTATUS(NTAPI* pdef_RtlAdjustPrivilege) (
	ULONG privilege,
	BOOLEAN enable,
	BOOLEAN current_thread,
	PBOOLEAN enabled);

typedef NTSTATUS(NTAPI* pf_NtRaiseError) (
	NTSTATUS error_status,
	ULONG number_of_param,
	PUNICODE_STRING UnicodeStringParameterMask,
	PVOID* param,
	ULONG ResponseOption,
	PULONG Response
	);
 
int main(int argc, char *argv[]) {
	int bsod, time;
	bool show = true;
	if (argc > 1) {
		sscanf(argv[1], "%x", &bsod);
	}
	if (argc > 3) {
		sscanf(argv[3], "%d", &show);
		if (show == 0) ShowWindow(FindWindow("ConsoleWindowClass",NULL), SW_HIDE);
	}
	if (argc > 2) {
		sscanf(argv[2], "%d", &time);
		Sleep(time);
	}	
	pdef_RtlAdjustPrivilege RtlAdjustPrivilege = (pdef_RtlAdjustPrivilege)GetProcAddress(LoadLibraryA("ntdll.dll"), "RtlAdjustPrivilege");
	BOOLEAN g;
	if (RtlAdjustPrivilege(19, TRUE, FALSE, &g) == 0)
	{
		//NtRaiseHardError is a way to raise errors inside ntdll.dll . Now sice ntdll.dll deals with the userland to kernel transition, we are knowingly invoking a hard exception inside kernel that triggers BSOD.
		//BDOS is exception error and hence NtRaiseHardError does the trick.
		pf_NtRaiseError NtRaiseHardError = (pf_NtRaiseError)GetProcAddress(LoadLibraryA("ntdll.dll"), "NtRaiseHardError");
		ULONG op;
		if (argc == 1) {
			printf("0x");
			scanf("%8X", &bsod);
		}
		printf("bsod = 0x%x", bsod);
		return NtRaiseHardError(bsod, 0, 0, 0, 6, &op);
	}
}
