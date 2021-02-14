//=============================================================================
//
// [DirectX.h]
// Author : Yo Jitaku
//
//=============================================================================
#ifndef DIRECTX_H_
#define DIRECTX_H_

#include "d3dx9.h"
#include "d3dx9math.h"
#define DIRECTINPUT_VERSION (0x0800)//どうして必ずdinput.hの前に定義する?　dinput.hの中でこのマクロを含めている
#include "dinput.h"
#include "xaudio2.h"
#include <cmath>
#include <time.h>
#include <stdlib.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")//D3DXCreateTextureFromFile
#pragma comment(lib, "dinput8.lib")//keyboard
#pragma comment(lib, "dxguid.lib")//IID_IDirectInput8A,GUID_SysKeyboard,GUID_Key
#pragma comment(lib, "winmm.lib")//FPS計測

//頂点座標とalpha+diffuse色表示+texture
#define FVF_VERTEX_2D (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

typedef struct
{
	D3DXVECTOR3 pos;
	float rhw;
	D3DCOLOR col;
	D3DXVECTOR2 tex;
}VERTEX_2D;//2D頂点情報を保存する

typedef enum
{
	MODE_TITLE = 0,
	MODE_MENU,
	MODE_TUTORIAL,
	MODE_STAGE1,
	MODE_STAGE2,
	MODE_BOSSSTAGE,
	MODE_GAMEOVER,
	MODE_GAMECLEAR,
	MODE_RANK,
	MODE_MAX,
}MODE;

HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Update(void);
void Uninit(void);
void Draw(void);
void SetMode(MODE mode);
void UpdateMode(MODE mode);
void DrawMode(MODE mode);
void DrawFPS(void);
LPDIRECT3DDEVICE9 GetDevice(void);
MODE GetMode(void);
#endif