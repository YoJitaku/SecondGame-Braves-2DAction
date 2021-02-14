//=============================================================================
//
// [DirectX.cpp]
// Author : Yo Jitaku
//
//=============================================================================
#include "DirectX.h"
#include "Input.h"
#include "Fade.h"
#include "title.h"
#include "menu.h"
#include "stage1.h"
#include "stage2.h"
#include "Ranking.h"
#include "Tutorial.h"
#include "PolyBlock.h"
#include "sound.h"
#include "collision.h"
#include "UI.h"

LPDIRECT3D9 g_pD3D = NULL;//user pc's graphics caxrd information get
LPDIRECT3DDEVICE9 g_pD3DDevice = NULL;
MODE g_Mode = MODE_TITLE;//始めるMODEはTitle1
//MODE g_Mode = MODE_MENU;
LPD3DXFONT g_pFont = NULL;//画面にの文字描画pointer
int g_nCountFPS;//fps保存する変数

HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	//check the user's hardware character
	D3DDISPLAYMODE d3ddm;//graphics card mode

	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);//create a d3d object
	if (g_pD3D == NULL) return E_FAIL;

	//check the default graphics card's displaymode
	if (FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))return E_FAIL;

	//prepare for get userdevice interface
	D3DPRESENT_PARAMETERS d3dpp;//struct group
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth = SCREEN_WIDTH;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;
	d3dpp.BackBufferFormat = d3ddm.Format;
	d3dpp.BackBufferCount = 1;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.Windowed = bWindow;//FULLSCREEN mode is false
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

	//check pc device about vertex draw（graphics card version 2 HAL and REF 3 object window handle 4 定点运算标志类型 互斥 5 point to a struct, this struct is describe the device Set which we want to create. 6 return device pointer
	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &g_pD3DDevice)))
	{
		if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pD3DDevice)))
		{
			if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pD3DDevice)))
			{
				return E_FAIL;
			}
		}
	}

	//Render set base
	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//Render set about fade (alpha)
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	//FPS表示 create
	D3DXCreateFont
	(
		g_pD3DDevice,
		30,//size
		0,
		0,
		0,
		FALSE,
		SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH,
		"Terminal",
		&g_pFont
	);

	//Game最初の初期化
	Initkeyboard(hInstance, hWnd);
	InitSound(hWnd);
	PlaySound(SOUND_LABEL_BGM);
	InitFade(MODE_TITLE);
	SetMode(MODE_TITLE);
	return S_OK;
}

void Uninit(void)
{
	if (g_pD3DDevice != NULL)
	{
		g_pD3DDevice->Release();
		g_pD3DDevice = NULL;
	}
	if (g_pD3D != NULL)
	{
		g_pD3D->Release();
		g_pD3D = NULL;
	}
	if (g_pFont != NULL)
	{
		g_pFont->Release();
		g_pFont = NULL;
	}
	Uninitkeyboard();
	UninitSound();
	StopSound();
}

void Update(void)
{
	Updatekeyboard();
	if (GetFade() != FADE_NONE)
		UpdateFade();
	else UpdateMode(g_Mode);
}

void Draw(void)
{
	g_pD3DDevice->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

	//BeginScene and EndScene to tell the system when it should be start or end the rendering, u can put anyother rendering set between
	if (SUCCEEDED(g_pD3DDevice->BeginScene()))
	{
		DrawMode(g_Mode);
		DrawFade();
#ifdef _DEBUG
		DrawFPS();
#endif
		g_pD3DDevice->EndScene();
	}

	//show the rendering
	g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}

//player設備のinterfaceを伝送
LPDIRECT3DDEVICE9 GetDevice(void)
{
	return g_pD3DDevice;
}

void SetMode(MODE mode)
{
	switch (g_Mode)//Uninit
	{
	case MODE_TITLE:
		UninitTitle();
		break;
	case MODE_MENU:
		UninitMenu();
		break;
	case MODE_TUTORIAL:
		UninitTutorial();
		break;
	case MODE_STAGE1:
		UninitStage1();
		break;
	case MODE_STAGE2:
		UninitStage2();
		break;
	case MODE_RANK:
		UninitUI();
		UninitRank();
		UninitPolyBlock(5);
		break;
	}

	switch (mode)//Init
	{
	case MODE_TITLE:
		InitTitle();
		break;
	case MODE_MENU:
		InitMenu();
		InitUI();
		break;
	case MODE_TUTORIAL:
		InitTutorial();
		break;
	case MODE_STAGE1:
		InitStage1();
		break;
	case MODE_STAGE2:
		InitStage2();
		break;
	case MODE_RANK:
		InitRank();
		InitPolyBlock(5);
		break;
	}
	g_Mode = mode;
}

void UpdateMode(MODE mode)//更新処理
{
	switch (mode)
	{
	case MODE_TITLE:
		UpdateTitle();
		break;
	case MODE_MENU:
		UpdateMenu();
		break;
	case MODE_TUTORIAL:
		UpdateTutorial();
		break;
	case MODE_STAGE1:
		UpdateStage1();
		break;
	case MODE_STAGE2:
		UpdateStage2();
		break;
	case MODE_RANK:
		UpdateRank();
		UpdatePolyBlock(5);
		break;
	}
}

void DrawMode(MODE mode)//描画処理
{
	switch (mode)
	{
	case MODE_TITLE:
		DrawTitle();
		break;
	case MODE_MENU:
		DrawMenu();
		break;
	case MODE_TUTORIAL:
		DrawTutorial();
		break;
	case MODE_STAGE1:
		DrawStage1();
		break;
	case MODE_STAGE2:
		DrawStage2();
		break;
	case MODE_RANK:
		DrawRank();
		DrawPolyBlock(5);
		break;
	}
}

MODE GetMode(void)//今のmod typeを伝送
{
	return g_Mode;
}

void DrawFPS(void)
{
	RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };//処理範囲の確認
	char aStr[256];//number of Fps
	wsprintf(&aStr[0], "FPS:%d\n", g_nCountFPS);//wsprintf can print on screen directly
	g_pFont->DrawText(NULL, &aStr[0], -1, &rect, DT_LEFT, D3DCOLOR_RGBA(255, 255, 255, 255));//draw
}