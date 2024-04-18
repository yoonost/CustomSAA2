#include <Windows.h>
#include <iostream>
#include "SAA2.h"

const char* SAA2_FILES_A[] = {
	"loadscs.txd",
	"HANDLING.CFG",
	"surface.dat",
	"TIMECYC.DAT",
	"stream.ini",
	"melee.dat",
	"DEFAULT.DAT",
	"DEFAULT.IDE",
	"VEHICLES.IDE",
	"PEDS.IDE",
	"GTA.DAT",
	"SCRIPT.IMG",
	"LAn2.IDE",
	"LAxref.IDE",
	"props.IDE",
	"OBJECT.DAT",
	"CARMODS.DAT",
	"VEHICLE.TXD",
	"tracks2.dat",
	"tracks4.dat",
	"PED.DAT",
	"WEAPON.DAT",
	"main.scm",
	"AR_STATS.DAT",
	"shopping.dat",
};

void* CreateFileAAddr = NULL;
char def_dir[MAX_PATH + 1] = { 0 };
static long long redirectBackup;

void Patch(void* source, void* replace, int num)
{
	DWORD dwOld;
	VirtualProtect(source, 5, PAGE_EXECUTE_READWRITE, &dwOld);
	memcpy(source, replace, num);
}

void Redirect(void* source, void* destination)
{
	DWORD dwOld;
	VirtualProtect(source, 5, PAGE_EXECUTE_READWRITE, &dwOld);
	redirectBackup = *(long long*)source;
	*((unsigned char*)source) = 0xE9;
	*(DWORD*)(((char*)source) + 1) = (DWORD)((DWORD)destination - (DWORD)source) - 5;
}

void Restore(void* source)
{
	*(long long*)source = redirectBackup;
}

const char* GetFileNameFromPath(const char* szPath) {
	const char* szFileName = szPath;

	if (szFileName != NULL)
	{
		int len = strlen(szPath);
		if (len > 0)
		{
			szFileName = szPath + len - 1;

			while (szFileName != szPath &&
				*szFileName != '\\' && *szFileName != '/')
				szFileName--;

			if (*szFileName == '\\' || *szFileName == '/')
				szFileName++;
		}
	}

	return szFileName;
}

HANDLE WINAPI CreateFileAHooked(_In_ LPCTSTR lpFileName, _In_ DWORD dwDesiredAccess, _In_ DWORD dwShareMode, _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes, _In_ DWORD dwCreationDisposition, _In_ DWORD dwFlagsAndAttributes, _In_opt_ HANDLE hTemplateFile) {
	if (lpFileName == NULL) return INVALID_HANDLE_VALUE;

	const char* fileName = GetFileNameFromPath(lpFileName);
	bool fnd = false;

	for (int i = 0; !fnd && i < sizeof(SAA2_FILES_A) / sizeof(SAA2_FILES_A[0]); i++) {
		if (_stricmp(fileName, SAA2_FILES_A[i]) == 0) fnd = true;
	}

	HANDLE hFile = INVALID_HANDLE_VALUE;

	Restore(CreateFileAAddr);

	hFile = CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

	Redirect(CreateFileAAddr, CreateFileAHooked);

	if (fnd) {
		char szFile[MAX_PATH + 1] = { 0 };
		wchar_t wszFileName[MAX_PATH + 1] = { 0 };
		int len = wsprintf(szFile, "%s\\tumple\\saa\\%s", def_dir, fileName);
		size_t convertedChars;
		mbstowcs_s(&convertedChars, wszFileName, szFile, len);

		if (hFile != INVALID_HANDLE_VALUE) {
			CloseHandle(hFile);
		}

		hFile = CreateFileW(wszFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	}

	return hFile;
}

void SAA2Load()
{
	GetCurrentDirectory(sizeof(def_dir), def_dir);
	CreateFileAAddr = GetProcAddress(GetModuleHandle("kernel32.dll"), "CreateFileA");
	Redirect(CreateFileAAddr, CreateFileAHooked);
}