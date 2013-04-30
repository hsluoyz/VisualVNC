#include "Hook.h"
HOOKINFO g_Hook_CreateFontIndirect;

typedef HFONT (WINAPI *TypeCreateFontIndirect)(LOGFONT* lplf);

HFONT WINAPI MyCreateFontIndirect(LOGFONT* lplf)
{
	TypeCreateFontIndirect pOrigin = (TypeCreateFontIndirect) g_Hook_CreateFontIndirect.Stub;
	if (lplf->lfHeight < 12)
		lplf->lfHeight = 12;
	_tcscpy(lplf->lfFaceName, _T("simsun"));
	//lplf->lfFaceName = L"Marlett"; //"Segoe UI" "Arial" "MS Shell Dlg"
	return pOrigin(lplf);
}

void doWithTheAPIHookStuff()
{
#ifdef UNICODE
	HEInitHook(&g_Hook_CreateFontIndirect, _T("gdi32"), _T("CreateFontIndirectW"), (void*) MyCreateFontIndirect);
#else
	HEInitHook(&g_Hook_CreateFontIndirect, _T("gdi32"), _T("CreateFontIndirectA"), (void*) MyCreateFontIndirect);
#endif
	HEStartHook(&g_Hook_CreateFontIndirect);
}
