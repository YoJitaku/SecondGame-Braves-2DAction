//=============================================================================
//
// [UI.cpp]　
// Author : Yo Jitaku
//
//=============================================================================
#include "UI.h"
#include "player.h"

LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffUI = NULL;
LPDIRECT3DTEXTURE9 g_pTextureUI[UI_TEXTURE] = {};
UI *pUI;
int g_nCntTimeUI;

void SetUI(void)
{
	//health
	for (int nCnt = 0; nCnt < UI_COMPONENT; nCnt++)
	{
		if (nCnt < 5)//hp
		{
			pUI[nCnt].bUse = false;
			pUI[nCnt].pos = D3DXVECTOR3(100 + (float)nCnt * 100, 950, 0);
			pUI[nCnt].size = D3DXVECTOR2(40, 40);
			pUI[nCnt].nTexture = 0;
			pUI[nCnt].nPatternAnime = 0;
		}
		else//score
		{
			pUI[nCnt].bUse = true;
			pUI[nCnt].pos = D3DXVECTOR3(1400 + (float)(nCnt - 6) * 60, 950, 0);
			pUI[nCnt].size = D3DXVECTOR2(20, 40);
			pUI[nCnt].nDisplayNumber = 0;//表示したいnumber
			pUI[nCnt].nZeroNumber = UI_COMPONENT - 1 - nCnt;//MAX_COMPONENT_SCORE - 1 - nCntNumber;//計算するとき　その位の後ろの0の個数
			pUI[nCnt].nMultipleNumber = 0;//除数としての整数1->00000
			pUI[nCnt].nTexture = 1;
		}
	}
}

HRESULT InitUI(void)
{
	LPDIRECT3DDEVICE9 pDeviceUI;
	pDeviceUI = GetDevice();
	VERTEX_2D *pVtx;
	g_nCntTimeUI = 0;

	if ((pUI = (UI *)malloc(UI_COMPONENT * sizeof(UI))) == NULL) return E_FAIL;

	SetUI();

	D3DXCreateTextureFromFile(pDeviceUI, "data//TEXTURE//HP.png", &g_pTextureUI[0]);
	//D3DXCreateTextureFromFile(pDeviceUI, "data//TEXTURE//Time.png", &g_pTextureUI[1]);
	D3DXCreateTextureFromFile(pDeviceUI, "data//TEXTURE//Score.png", &g_pTextureUI[1]);

	if (FAILED(pDeviceUI->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * UI_COMPONENT, D3DUSAGE_WRITEONLY, FVF_VERTEX_2D, D3DPOOL_MANAGED, &g_pVtxBuffUI, NULL))) return E_FAIL;
	g_pVtxBuffUI->Lock(0, 0, (void**)&pVtx, 0);
	for (int nCnt = 0; nCnt < UI_COMPONENT; nCnt++, pVtx += 4)
	{
		pVtx[0].pos = D3DXVECTOR3(pUI[nCnt].pos.x - pUI[nCnt].size.x, pUI[nCnt].pos.y - pUI[nCnt].size.y, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(pUI[nCnt].pos.x + pUI[nCnt].size.x, pUI[nCnt].pos.y - pUI[nCnt].size.y, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(pUI[nCnt].pos.x - pUI[nCnt].size.x, pUI[nCnt].pos.y + pUI[nCnt].size.y, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(pUI[nCnt].pos.x + pUI[nCnt].size.x, pUI[nCnt].pos.y + pUI[nCnt].size.y, 0.0f);
		pVtx[0].rhw = 1.0f;
		pVtx[1].rhw = 1.0f;
		pVtx[2].rhw = 1.0f;
		pVtx[3].rhw = 1.0f;
		pVtx[0].col = D3DCOLOR_RGBA(255, 255, 255, 255);
		pVtx[1].col = D3DCOLOR_RGBA(255, 255, 255, 255);
		pVtx[2].col = D3DCOLOR_RGBA(255, 255, 255, 255);
		pVtx[3].col = D3DCOLOR_RGBA(255, 255, 255, 255);
		if (nCnt < 5)
		{
			pVtx[0].tex = D3DXVECTOR2(0.0, 0.0);
			pVtx[1].tex = D3DXVECTOR2(1.0, 0.0);
			pVtx[2].tex = D3DXVECTOR2(0.0, 1.0);
			pVtx[3].tex = D3DXVECTOR2(1.0, 1.0);
		}
		else
		{
			pVtx[0].tex = D3DXVECTOR2((pUI[nCnt].nDisplayNumber * 0.1f), 0.0);
			pVtx[1].tex = D3DXVECTOR2((pUI[nCnt].nDisplayNumber * 0.1f + 0.1f), 0.0);
			pVtx[2].tex = D3DXVECTOR2((pUI[nCnt].nDisplayNumber * 0.1f), 1.0);
			pVtx[3].tex = D3DXVECTOR2((pUI[nCnt].nDisplayNumber * 0.1f + 0.1f), 1.0);
		}
	}
	g_pVtxBuffUI->Unlock();
	return S_OK;
}

void UninitUI(void)
{
	if (g_pVtxBuffUI != NULL)
	{
		g_pVtxBuffUI->Release();
		g_pVtxBuffUI = NULL;
	}
	for (int nCnt = 0; nCnt < UI_TEXTURE; nCnt++)
	{
		if (g_pTextureUI[nCnt] != NULL)
		{
			g_pTextureUI[nCnt]->Release();
			g_pTextureUI[nCnt] = NULL;
		}
	}
	free(pUI);
}

void UpdateUI(void)
{
	LPDIRECT3DDEVICE9 pDeviceSc;
	pDeviceSc = GetDevice();
	PLAYER *pPlayer;
	pPlayer = GetPlayer();
	for (int nCntNumber = 0; nCntNumber < UI_COMPONENT; nCntNumber++)
	{
		if (nCntNumber > 4)
		{
			//pow function -> headfile<math.h>
			pUI[nCntNumber].nMultipleNumber = (unsigned int)pow(10.0, pUI[nCntNumber].nZeroNumber);
			//(1.%定位 2./位取り)
			pUI[nCntNumber].nDisplayNumber = pPlayer->nScore % (pUI[nCntNumber].nMultipleNumber * 10) / pUI[nCntNumber].nMultipleNumber;
			VERTEX_2D *pVtx;
			g_pVtxBuffUI->Lock(0, 0, (void**)&pVtx, 0);
			pVtx[nCntNumber * 4 + 0].tex = D3DXVECTOR2((pUI[nCntNumber].nDisplayNumber * 0.1f), 0.0);
			pVtx[nCntNumber * 4 + 1].tex = D3DXVECTOR2((pUI[nCntNumber].nDisplayNumber * 0.1f + 0.1f), 0.0);
			pVtx[nCntNumber * 4 + 2].tex = D3DXVECTOR2((pUI[nCntNumber].nDisplayNumber * 0.1f), 1.0);
			pVtx[nCntNumber * 4 + 3].tex = D3DXVECTOR2((pUI[nCntNumber].nDisplayNumber * 0.1f + 0.1f), 1.0);
			g_pVtxBuffUI->Unlock();
		}
		else
		{
			if (pPlayer->nLife > 5) pPlayer->nLife = 5;
			if (nCntNumber < pPlayer->nLife) pUI[nCntNumber].bUse = true;
			else pUI[nCntNumber].bUse = false;
		}
	}
}
void DrawUI(void)
{
	LPDIRECT3DDEVICE9 pDeviceUI;
	pDeviceUI = GetDevice();

	pDeviceUI->SetStreamSource(0, g_pVtxBuffUI, 0, sizeof(VERTEX_2D));
	pDeviceUI->SetFVF(FVF_VERTEX_2D);
	for (int nCnt = 0; nCnt < UI_COMPONENT; nCnt++)
	{
		if (pUI[nCnt].bUse == true)
		{
			pDeviceUI->SetTexture(0, g_pTextureUI[pUI[nCnt].nTexture]);
			pDeviceUI->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCnt * 4, 2);
		}
	}
}
