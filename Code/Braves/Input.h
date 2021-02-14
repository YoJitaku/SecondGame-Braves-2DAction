//=============================================================================
//
// [DirectX.h]Å@
// Author : Yo Jitaku
//
//=============================================================================
#pragma once
#include "DirectX.h"

HRESULT Initkeyboard(HINSTANCE hInstance, HWND hWnd);

bool GetkeyboardPress(int nkey);
bool GetkeyboardTrigger(int nkey);
bool GetkeyboardRelease(int nkey);

void Uninitkeyboard(void);
void Updatekeyboard(void);