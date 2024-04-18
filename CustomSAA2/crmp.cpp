#include <Windows.h>
#include "SAA2.h"

HINSTANCE g_hInst = NULL;
HMODULE g_hModule = NULL;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason ==  DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(hinstDLL);
		g_hInst = hinstDLL;
		SAA2Load();
	}
	if (fdwReason == DLL_PROCESS_DETACH) {
		if (g_hModule != NULL) {
			FreeLibrary(g_hModule);
			g_hModule = NULL;
		}
	}
	return TRUE;
}