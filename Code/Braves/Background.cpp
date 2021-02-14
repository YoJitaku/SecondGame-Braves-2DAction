//=============================================================================
//
// [BackGround.cpp]　全ステージの背景を集めて管理する
// Author : Yo Jitaku
//
//=============================================================================
#include "Background.h"
#include "PolyBlock.h"

LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffBackground = NULL;
LPDIRECT3DTEXTURE9 g_pTextureBackground[BACKGROUND_NUM] = {};

//各ステージ使用したtextureの数。1title 6menu 1handbook 6stage1 6stage2 1rank
int nCntTimeBg, aBackGroundNum[6] = {1, 6, 1, 6, 6, 1}; 
BACKGROUND *pBackground;

//今のステージ番号を導入する
HRESULT InitBackground(int nStage)
{
	LPDIRECT3DDEVICE9 pDeviceBg = GetDevice();
	VERTEX_2D *pVtx;
	nCntTimeBg = 0;

	//メモリ定義
	if (FAILED(pBackground = (BACKGROUND *)malloc(aBackGroundNum[nStage] * sizeof(BACKGROUND)))) return E_FAIL;
	
	//pointer初期化
	SetBackground(nStage);

	//texture導入
	switch (nStage)
	{
		case 0://title
			D3DXCreateTextureFromFile(pDeviceBg, "data/TEXTURE/title.png", &g_pTextureBackground[0]);
			break;
		case 1: case 3:
			//menu //stage1
			D3DXCreateTextureFromFile(pDeviceBg, "data/TEXTURE/0bg1ruins.png", &g_pTextureBackground[0]);
			D3DXCreateTextureFromFile(pDeviceBg, "data/TEXTURE/1bg1ruins_closer.png", &g_pTextureBackground[1]);
			D3DXCreateTextureFromFile(pDeviceBg, "data/TEXTURE/2bg1ruins_main.png", &g_pTextureBackground[2]);
			D3DXCreateTextureFromFile(pDeviceBg, "data/TEXTURE/3bg1floor_ruins.png", &g_pTextureBackground[3]);
			D3DXCreateTextureFromFile(pDeviceBg, "data/TEXTURE/4bg1ruins_low.png", &g_pTextureBackground[4]);
			D3DXCreateTextureFromFile(pDeviceBg, "data/TEXTURE/5bg1chains.png", &g_pTextureBackground[5]);
			break;
		case 2: //tutorial
			D3DXCreateTextureFromFile(pDeviceBg, "data/TEXTURE/Tutorial.png", &g_pTextureBackground[0]);
			break;
		case 4:	//stage2
			D3DXCreateTextureFromFile(pDeviceBg, "data/TEXTURE/0bg2rocks.png", &g_pTextureBackground[0]);
			D3DXCreateTextureFromFile(pDeviceBg, "data/TEXTURE/1bg2rocks_closer.png", &g_pTextureBackground[1]);
			D3DXCreateTextureFromFile(pDeviceBg, "data/TEXTURE/2bg2rocks_main.png", &g_pTextureBackground[2]);
			D3DXCreateTextureFromFile(pDeviceBg, "data/TEXTURE/3bg2floor_rocks.png", &g_pTextureBackground[3]);
			D3DXCreateTextureFromFile(pDeviceBg, "data/TEXTURE/4bg2ceiling_rocks.png", &g_pTextureBackground[4]);
			D3DXCreateTextureFromFile(pDeviceBg, "data/TEXTURE/5bg2myst.png", &g_pTextureBackground[5]);
			break;
		case 5://rank
			D3DXCreateTextureFromFile(pDeviceBg, "data/TEXTURE/Ranking.png", &g_pTextureBackground[0]);
			break;
	}
	
	//頂点Buffer初期化
	if (FAILED(pDeviceBg->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * aBackGroundNum[nStage], D3DUSAGE_WRITEONLY, FVF_VERTEX_2D, D3DPOOL_MANAGED, &g_pVtxBuffBackground, NULL))) return E_FAIL;
	g_pVtxBuffBackground->Lock(0, 0, (void**)&pVtx, 0);
	for (int nCnt = 0; nCnt < aBackGroundNum[nStage]; nCnt++, pVtx += 4)
	{
		pVtx[0].pos = D3DXVECTOR3(pBackground[nCnt].pos.x - pBackground[nCnt].size.x, pBackground[nCnt].pos.y - pBackground[nCnt].size.y, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(pBackground[nCnt].pos.x + pBackground[nCnt].size.x, pBackground[nCnt].pos.y - pBackground[nCnt].size.y, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(pBackground[nCnt].pos.x - pBackground[nCnt].size.x, pBackground[nCnt].pos.y + pBackground[nCnt].size.y, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(pBackground[nCnt].pos.x + pBackground[nCnt].size.x, pBackground[nCnt].pos.y + pBackground[nCnt].size.y, 0.0f);
		pVtx[0].rhw = 1.0f;
		pVtx[1].rhw = 1.0f;
		pVtx[2].rhw = 1.0f;
		pVtx[3].rhw = 1.0f;
		pVtx[0].col = D3DCOLOR_RGBA(255, 255, 255, 255);
		pVtx[1].col = D3DCOLOR_RGBA(255, 255, 255, 255);
		pVtx[2].col = D3DCOLOR_RGBA(255, 255, 255, 255);
		pVtx[3].col = D3DCOLOR_RGBA(255, 255, 255, 255);
		pVtx[0].tex = D3DXVECTOR2(0.0, 0.0);
		pVtx[1].tex = D3DXVECTOR2(1.0, 0.0);
		pVtx[2].tex = D3DXVECTOR2(0.0, 1.0);
		pVtx[3].tex = D3DXVECTOR2(1.0, 1.0);
		g_pVtxBuffBackground->Unlock();
	}
	return S_OK;
}

void UninitBackground(int nStage)
{
	if (g_pVtxBuffBackground != NULL)
	{
		g_pVtxBuffBackground->Release();
		g_pVtxBuffBackground = NULL;
	}
	for (int nCnt = 0; nCnt < aBackGroundNum[nStage]; nCnt++)
	{
		if (g_pTextureBackground[nCnt] != NULL)
		{
			g_pTextureBackground[nCnt]->Release();
			g_pTextureBackground[nCnt] = NULL;
		}
	}
	free(pBackground);//メモリをリリース
}

void UpdateBackground(int nStage)
{
	nCntTimeBg++;
	VERTEX_2D *pVtx;
	D3DXVECTOR2 *pAllmove = GetAllMove();//スクロールの移動速度はプレイアーに対して更新します
	pBackground->move.x = 0;//増量の初期化

	//スクロールの移動更新
	if (nStage == 3 || nStage == 4)//stage1 と stage2
	{
		g_pVtxBuffBackground->Lock(0, 0, (void**)&pVtx, 0);
		for (int nCnt = 0; nCnt < aBackGroundNum[nStage]; nCnt++, pVtx += 4)
		{
			if (pAllmove->x != 0) pBackground[nCnt].fTexture -= pAllmove->x;
			pVtx[0].tex = D3DXVECTOR2(pBackground[nCnt].fTexture * pBackground[nCnt].fSpeed, 0.0);
			pVtx[1].tex = D3DXVECTOR2(1.0f + pBackground[nCnt].fTexture * pBackground[nCnt].fSpeed, 0.0);
			pVtx[2].tex = D3DXVECTOR2(pBackground[nCnt].fTexture * pBackground[nCnt].fSpeed, 1.0);
			pVtx[3].tex = D3DXVECTOR2(1.0f + pBackground[nCnt].fTexture * pBackground[nCnt].fSpeed, 1.0);
		
			if (nStage == 4 && nCnt == 5)
			{
				pBackground[nCnt].fTexture += 0.1f;
				pVtx[0].tex = D3DXVECTOR2(pBackground[nCnt].fTexture * pBackground[nCnt].fSpeed + nCntTimeBg * -0.001f, 0.0);
				pVtx[1].tex = D3DXVECTOR2(1.0f + pBackground[nCnt].fTexture * pBackground[nCnt].fSpeed + nCntTimeBg * -0.001f, 0.0);
				pVtx[2].tex = D3DXVECTOR2(pBackground[nCnt].fTexture * pBackground[nCnt].fSpeed + nCntTimeBg * -0.001f, 1.0);
				pVtx[3].tex = D3DXVECTOR2(1.0f + pBackground[nCnt].fTexture * pBackground[nCnt].fSpeed + nCntTimeBg * -0.001f, 1.0);
			}
		}
		g_pVtxBuffBackground->Unlock();
	}
}
void DrawBackground(int nStage)
{
	LPDIRECT3DDEVICE9 pDeviceBg = GetDevice();

	pDeviceBg->SetStreamSource(0, g_pVtxBuffBackground, 0, sizeof(VERTEX_2D));//data stream
	pDeviceBg->SetFVF(FVF_VERTEX_2D);//2d描画設定
	
	for (int nCnt = 0; nCnt < aBackGroundNum[nStage]; nCnt++)
	{
		pDeviceBg->SetTexture(0, g_pTextureBackground[pBackground[nCnt].nType]);
		pDeviceBg->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCnt * 4, 2);
	}
}

void SetBackground(int nStage)
{
	//polygon初期化
	for (int nCnt = 0; nCnt < aBackGroundNum[nStage]; nCnt++)
	{
		pBackground[nCnt].pos = D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f);
		pBackground[nCnt].size = D3DXVECTOR2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
		pBackground[nCnt].move = D3DXVECTOR3(0, 0, 0);
		pBackground[nCnt].nType = nCnt;
		pBackground[nCnt].fSpeed = 0;
		pBackground[nCnt].fTexture = 0;
	}

	//スクロール移動速度
	if (nStage == 3 || nStage == 4)
	{
		pBackground[0].fSpeed = 0.00005f;
		pBackground[1].fSpeed = 0.0001f;
		pBackground[2].fSpeed = 0.0002f;
		pBackground[3].fSpeed = 0.0004f;
		pBackground[4].fSpeed = 0.0004f;
		pBackground[5].fSpeed = 0.0004f;
	}
}

BACKGROUND GetBackground(void)
{
	return pBackground[5];
}