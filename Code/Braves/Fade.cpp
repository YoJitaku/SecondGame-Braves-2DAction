//=============================================================================
//
// [Fade.cpp]　
// Author : Yo Jitaku
//
//=============================================================================
#include "fade.h"
#include "input.h"

LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffFade = NULL;
LPDIRECT3DDEVICE9 pDeviceFd;

float g_fFadeCnt = (float)0.03;
FADE g_fade;
MODE g_modeNext;
D3DXCOLOR g_colorFade;

HRESULT InitFade(MODE modeNext)
{
	LPDIRECT3DDEVICE9 pDeviceFd;
	pDeviceFd = GetDevice();
	VERTEX_2D *pVtx;

	g_fade = FADE_IN;
	g_modeNext = modeNext;
	g_colorFade = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

	if (FAILED(pDeviceFd->CreateVertexBuffer(sizeof(VERTEX_2D) * 4, D3DUSAGE_WRITEONLY, FVF_VERTEX_2D, D3DPOOL_MANAGED, &g_pVtxBuffFade, NULL))) return E_FAIL;

	g_pVtxBuffFade->Lock(0, 0, (void**)&pVtx, 0);
	pVtx[0].pos = D3DXVECTOR3(0, 0, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(SCREEN_WIDTH, 0, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(0, SCREEN_HEIGHT, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);
	pVtx[0].rhw = 1.0f;//？
	pVtx[1].rhw = 1.0f;//？
	pVtx[2].rhw = 1.0f;//？
	pVtx[3].rhw = 1.0f;//？
	pVtx[0].col = g_colorFade;
	pVtx[1].col = g_colorFade;
	pVtx[2].col = g_colorFade;
	pVtx[3].col = g_colorFade;
	pVtx[0].tex = D3DXVECTOR2(0.0, 0.0);
	pVtx[1].tex = D3DXVECTOR2(1.0, 0.0);
	pVtx[2].tex = D3DXVECTOR2(0.0, 1.0);
	pVtx[3].tex = D3DXVECTOR2(1.0, 1.0);
	g_pVtxBuffFade->Unlock();
	return S_OK;
}

void UninitFade(void)
{
	if (g_pVtxBuffFade != NULL)
	{
		g_pVtxBuffFade->Release();
		g_pVtxBuffFade = NULL;
	}
}

void UpdateFade(void)
{
	LPDIRECT3DDEVICE9 pDeviceFd;
	pDeviceFd = GetDevice();
	VERTEX_2D *pVtx;

	if (g_fade == FADE_IN)
	{
		if (g_fFadeCnt != 0.03f) g_fFadeCnt = 0.03f;
		if (g_colorFade.a <= 0)
		{
			g_fade = FADE_NONE;
			g_colorFade.a = 0;
		}
		else
		{
			g_colorFade.a -= g_fFadeCnt;
			g_pVtxBuffFade->Lock(0, 0, (void**)&pVtx, 0);
			pVtx[0].col = g_colorFade;
			pVtx[1].col = g_colorFade;
			pVtx[2].col = g_colorFade;
			pVtx[3].col = g_colorFade;
			g_pVtxBuffFade->Unlock();
		}
	}
	else if (g_fade == FADE_OUT)
	{
		if (g_colorFade.a >= 1)
		{
			g_colorFade.a = 1;
			SetMode(g_modeNext);
			g_fade = FADE_IN;
		}
		else
		{
			if (GetkeyboardTrigger(DIK_RETURN) == true) g_colorFade.a = (float)0.95;
			g_colorFade.a += g_fFadeCnt;
			g_pVtxBuffFade->Lock(0, 0, (void**)&pVtx, 0);
			pVtx[0].col = g_colorFade;
			pVtx[1].col = g_colorFade;
			pVtx[2].col = g_colorFade;
			pVtx[3].col = g_colorFade;
			g_pVtxBuffFade->Unlock();
		}
	}
}

void DrawFade(void)
{
	LPDIRECT3DDEVICE9 pDeviceFd;
	pDeviceFd = GetDevice();
	pDeviceFd->SetStreamSource(0, g_pVtxBuffFade, 0, sizeof(VERTEX_2D));
	pDeviceFd->SetFVF(FVF_VERTEX_2D);
	pDeviceFd->SetTexture(0, NULL);
	pDeviceFd->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

void SetFade(FADE fade, MODE modeNext)
{
	/*int nScore = GetScore();*/
	g_fade = fade;
	g_modeNext = modeNext;
	/*if (modeNext == MODE_RANKING && nScore > 0) g_fFadeCnt = 0.005f;*/
}

FADE GetFade(void)
{
	return g_fade;
}