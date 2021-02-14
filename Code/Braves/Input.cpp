//=============================================================================
//
// [Input.cpp]@direct input ver.8
// Author : Yo Jitaku
//
//=============================================================================
#include "input.h"

//get device
LPDIRECTINPUT8 g_pInput = NULL;
LPDIRECTINPUTDEVICE8 g_pDevkeyborad = NULL;

BYTE g_aKeyState[256];//save the last frame
BYTE g_aKeyStateTrigger[256];
BYTE g_aKeyStateRelease[256];

HRESULT Initkeyboard(HINSTANCE hInstance, HWND hWnd)
{
	//create the directinput object 1 application handle 2 version 3 defalut value DIRECTINPUT_VERSION (define in dinput.h) 3 interface label default value IID_IDirectInput8 4 save the Directinput pointer after success create
	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_pInput, NULL))) return E_FAIL;
	//create a keyboard object use got interface 1 GUID mouse has a different one 2 pointer of this device
	if (FAILED(g_pInput->CreateDevice(GUID_SysKeyboard, &g_pDevkeyborad, NULL))) return E_FAIL;
	//set the data type of keyboard c_dfDIKeyboard 
	if (FAILED(g_pDevkeyborad->SetDataFormat(&c_dfDIKeyboard)))	return E_FAIL;
	//set the cooperativelevel
	/*
	DISCL_FOREGROUND
	have been chosen can get the message
	DISCL_BACKGROUND
	any time can get the message
	DISCL_EXCLUSIVE monopolize
	DISCL_NONEXCLUSIVE unmonopolize
	*/
	if (FAILED(g_pDevkeyborad->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))) return E_FAIL;
	//use acquire function to get the device
	g_pDevkeyborad->Acquire();
	return S_OK;
}

//release the memory
void Uninitkeyboard(void)
{
	if (g_pDevkeyborad != NULL)
	{
		g_pDevkeyborad->Unacquire();
		g_pDevkeyborad->Release();
		g_pDevkeyborad = NULL;
	}
	if (g_pInput != NULL)
	{
		g_pInput->Release();
		g_pInput = NULL;
	}
}

void Updatekeyboard(void)
{
	BYTE aKeyState[256];//currently frame
	
	//get the information which input by keyboard 1 the data save struct's size 2 save position
	if (SUCCEEDED(g_pDevkeyborad->GetDeviceState(sizeof(aKeyState), &aKeyState[0])))
	{
		for (int nKey = 0; nKey < 256; nKey++)
		{
			g_aKeyStateTrigger[nKey] = (g_aKeyState[nKey] ^ aKeyState[nKey]) & (aKeyState[nKey] & 0x80);
			g_aKeyStateRelease[nKey] = (g_aKeyState[nKey] ^ aKeyState[nKey]) & (g_aKeyState[nKey] & 0x80);
			g_aKeyState[nKey] = aKeyState[nKey];
		}
	}
	else g_pDevkeyborad->Acquire();
}

bool GetkeyboardPress(int nkey)
{
	return (g_aKeyState[nkey] & 0x80) ? true : false;
}
bool GetkeyboardTrigger(int nkey)
{
	return (g_aKeyStateTrigger[nkey] & 0x80) ? true : false;
}
bool GetkeyboardRelease(int nkey)
{
	return (g_aKeyStateRelease[nkey] & 0x80) ? true : false;
}