//=============================================================================
//
// [Enemy.cpp]　
// Author : Yo Jitaku
//
//=============================================================================
#define _CRT_SECURE_NO_WARNINGS
#include "enemy.h"
#include "PolyBlock.h"
#include "player.h"
#include "sound.h"
#include "collision.h"

LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffEnemy = NULL;
LPDIRECT3DTEXTURE9 g_pTextureEnemy[15] = {};
ENEMY *pEnemy;
int g_aEnemy[6] = { 0, 0, 0, 2, 2, 0 }; //0title 1menu 2handbook 3stage1 4stage2 5rank
int g_nCntTimeEm;//時間count

void SetEnemy(void)
{
	int *pMapData = MapData();//読み込だmapdataを取る
	int nCntEnemy = 0;
	for (int nCntRow = 0; nCntRow < 20; nCntRow++)
	{
		for (int nCntColumn = 0; nCntColumn < 500; nCntColumn++)
		{
			if (*pMapData == 50)
			{
				pEnemy[nCntEnemy].bUse = true;
				pEnemy[nCntEnemy].bBlock = true;
				pEnemy[nCntEnemy].bHurt = false;
				pEnemy[nCntEnemy].bAlert = false;
				pEnemy[nCntEnemy].nState = E_IDLE;
				pEnemy[nCntEnemy].nDirection = rand() % 2;//開始の向こう random　0left 1right
				pEnemy[nCntEnemy].nLife = 5;
				pEnemy[nCntEnemy].pos = D3DXVECTOR3((float)nCntColumn * DEFAULT_WIDTH + DEFAULT_WIDTH, (float)nCntRow * DEFAULT_HEIGHT, 0);
				pEnemy[nCntEnemy].size = D3DXVECTOR2(80, 80);
				pEnemy[nCntEnemy].move = D3DXVECTOR3(0, 0, 0);
				pEnemy[nCntEnemy].nAnime = 0;
				pEnemy[nCntEnemy].nMoveRange = 0;
				pEnemy[nCntEnemy].bAnimeChange = true;
				pEnemy[nCntEnemy].nType = 0;
				pEnemy[nCntEnemy].nDeadTime = 0;
				nCntEnemy++;
			}
			pMapData++;
		}
	}
	//敵の種類
	//if (g_aMapData[nCntR][nCntC] == 50 || g_aMapData[nCntR][nCntC] == 51 || g_aMapData[nCntR][nCntC] == 52)
	//{
	//	ppEnemy[nCntEnemy].nDeadTime = 0;
	//	if (g_aMapData[nCntR][nCntC] == 50)
	//	{
	//		
	//	}
	//	else if (g_aMapData[nCntR][nCntC] == 51)
	//	{
	//		ppEnemy[nCntEnemy].bUse = true;
	//		ppEnemy[nCntEnemy].bBlock = true;
	//		ppEnemy[nCntEnemy].bHurt = false;
	//		ppEnemy[nCntEnemy].bAlert = false;
	//		ppEnemy[nCntEnemy].nState = E_IDLE;
	//		ppEnemy[nCntEnemy].nDirection = rand() % 2;//開始の向こう random
	//		ppEnemy[nCntEnemy].nLife = 3;
	//		ppEnemy[nCntEnemy].pos = D3DXVECTOR3((float)nCntC * DEFAULT_WIDTH + DEFAULT_WIDTH, (float)nCntR * DEFAULT_HEIGHT, 0);
	//		ppEnemy[nCntEnemy].size = D3DXVECTOR2(80, 80);
	//		ppEnemy[nCntEnemy].move = D3DXVECTOR3(0, 0, 0);
	//		ppEnemy[nCntEnemy].nAnime = 0;
	//		ppEnemy[nCntEnemy].nMoveRange = 0;
	//		ppEnemy[nCntEnemy].bAnimeChange = true;
	//		//ppEnemy->nMoveType = 1;
	//		ppEnemy[nCntEnemy].nType = 1;
	//		nCntEnemy++;
	//	}
	//	else if (g_aMapData[nCntR][nCntC] == 52)
	//	{
	//		ppEnemy[nCntEnemy].bUse = true;
	//		ppEnemy[nCntEnemy].bBlock = true;
	//		ppEnemy[nCntEnemy].bHurt = false;
	//		ppEnemy[nCntEnemy].bAlert = false;
	//		ppEnemy[nCntEnemy].nState = E_IDLE;
	//		ppEnemy[nCntEnemy].nDirection = rand() % 2;//開始の向こう random
	//		ppEnemy[nCntEnemy].nLife = 10;
	//		ppEnemy[nCntEnemy].pos = D3DXVECTOR3((float)nCntC * DEFAULT_WIDTH + DEFAULT_WIDTH, (float)nCntR * DEFAULT_HEIGHT, 0);
	//		ppEnemy[nCntEnemy].size = D3DXVECTOR2(80, 80);
	//		ppEnemy[nCntEnemy].move = D3DXVECTOR3(0, 0, 0);
	//		ppEnemy[nCntEnemy].nAnime = 0;
	//		ppEnemy[nCntEnemy].nMoveRange = 0;
	//		ppEnemy[nCntEnemy].bAnimeChange = true;
	//		//ppEnemy->nMoveType = 2;
	//		ppEnemy[nCntEnemy].nType = 2;
	//		nCntEnemy++;
	//	}
	//}
}

HRESULT InitEnemy(int nStage)
{
	LPDIRECT3DDEVICE9 pDeviceEm = GetDevice();
	VERTEX_2D *pVtx;
	g_nCntTimeEm = 0;//時間count

	if (FAILED(pEnemy = (ENEMY *)malloc(g_aEnemy[nStage] * sizeof(ENEMY)))) return E_FAIL;
	SetEnemy();//初期化
	D3DXCreateTextureFromFile(pDeviceEm, "data//TEXTURE//Enemy1Idle.png", &g_pTextureEnemy[0]);
	D3DXCreateTextureFromFile(pDeviceEm, "data//TEXTURE//Enemy1Walk.png", &g_pTextureEnemy[1]);
	D3DXCreateTextureFromFile(pDeviceEm, "data//TEXTURE//Enemy1Attack.png", &g_pTextureEnemy[2]);
	D3DXCreateTextureFromFile(pDeviceEm, "data//TEXTURE//Enemy1Dead.png", &g_pTextureEnemy[3]);
	D3DXCreateTextureFromFile(pDeviceEm, "data//TEXTURE//Enemy1Hurt.png", &g_pTextureEnemy[4]);
	//D3DXCreateTextureFromFile(pDeviceEm, "data//TEXTURE//Enemy2Idle.png", &g_pTextureEnemy[5]);
	//D3DXCreateTextureFromFile(pDeviceEm, "data//TEXTURE//Enemy2Walk.png", &g_pTextureEnemy[6]);
	//D3DXCreateTextureFromFile(pDeviceEm, "data//TEXTURE//Enemy2Attack.png", &g_pTextureEnemy[7]);
	//D3DXCreateTextureFromFile(pDeviceEm, "data//TEXTURE//Enemy2Dead.png", &g_pTextureEnemy[8]);
	//D3DXCreateTextureFromFile(pDeviceEm, "data//TEXTURE//Enemy2Hurt.png", &g_pTextureEnemy[9]);
	//D3DXCreateTextureFromFile(pDeviceEm, "data//TEXTURE//Enemy3Idle.png", &g_pTextureEnemy[10]);
	//D3DXCreateTextureFromFile(pDeviceEm, "data//TEXTURE//Enemy3Walk.png", &g_pTextureEnemy[11]);
	//D3DXCreateTextureFromFile(pDeviceEm, "data//TEXTURE//Enemy3Attack.png", &g_pTextureEnemy[12]);
	//D3DXCreateTextureFromFile(pDeviceEm, "data//TEXTURE//Enemy3Dead.png", &g_pTextureEnemy[13]);
	//D3DXCreateTextureFromFile(pDeviceEm, "data//TEXTURE//Enemy3Hurt.png", &g_pTextureEnemy[14]);

	//vtx buffer
	if (FAILED(pDeviceEm->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * g_aEnemy[nStage], D3DUSAGE_WRITEONLY, FVF_VERTEX_2D, D3DPOOL_MANAGED, &g_pVtxBuffEnemy, NULL))) return E_FAIL;

	g_pVtxBuffEnemy->Lock(0, 0, (void**)&pVtx, 0);
	for (int nCnt = 0; nCnt < g_aEnemy[nStage]; nCnt++, pVtx += 4)
	{
		pVtx[0].pos = D3DXVECTOR3(pEnemy[nCnt].pos.x - pEnemy[nCnt].size.x, pEnemy[nCnt].pos.y - pEnemy[nCnt].size.y, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(pEnemy[nCnt].pos.x + pEnemy[nCnt].size.x, pEnemy[nCnt].pos.y - pEnemy[nCnt].size.y, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(pEnemy[nCnt].pos.x - pEnemy[nCnt].size.x, pEnemy[nCnt].pos.y + pEnemy[nCnt].size.y, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(pEnemy[nCnt].pos.x + pEnemy[nCnt].size.x, pEnemy[nCnt].pos.y + pEnemy[nCnt].size.y, 0.0f);
		pVtx[0].rhw = 1.0f;
		pVtx[1].rhw = 1.0f;
		pVtx[2].rhw = 1.0f;
		pVtx[3].rhw = 1.0f;
		pVtx[0].col = D3DCOLOR_RGBA(255, 255, 255, 255);
		pVtx[1].col = D3DCOLOR_RGBA(255, 255, 255, 255);
		pVtx[2].col = D3DCOLOR_RGBA(255, 255, 255, 255);
		pVtx[3].col = D3DCOLOR_RGBA(255, 255, 255, 255);
		//敵の向き
		if (pEnemy[nCnt].nDirection == 1)
		{
			pVtx[0].tex = D3DXVECTOR2(0.1f * pEnemy[nCnt].nAnime, 0.0);
			pVtx[1].tex = D3DXVECTOR2(0.1f + 0.1f * pEnemy[nCnt].nAnime, 0.0);
			pVtx[2].tex = D3DXVECTOR2(0.1f * pEnemy[nCnt].nAnime, 1.0);
			pVtx[3].tex = D3DXVECTOR2(0.1f + 0.1f * pEnemy[nCnt].nAnime, 1.0);
		}
		else
		{
			pVtx[0].tex = D3DXVECTOR2(0.1f + 0.1f * pEnemy[nCnt].nAnime, 0.0);
			pVtx[1].tex = D3DXVECTOR2(0.1f * pEnemy[nCnt].nAnime, 0.0);
			pVtx[2].tex = D3DXVECTOR2(0.1f + 0.1f * pEnemy[nCnt].nAnime, 1.0);
			pVtx[3].tex = D3DXVECTOR2(0.1f * pEnemy[nCnt].nAnime, 1.0);
		}
	}
	g_pVtxBuffEnemy->Unlock();
	return S_OK;
}

void UninitEnemy(int nStage)
{
	if (g_pVtxBuffEnemy != NULL)
	{
		g_pVtxBuffEnemy->Release();
		g_pVtxBuffEnemy = NULL;
	}
	for (int nCnt = 0; nCnt < 15; nCnt++)
	{
		if (g_pTextureEnemy[nCnt] != NULL)
		{
			g_pTextureEnemy[nCnt]->Release();
			g_pTextureEnemy[nCnt] = NULL;
		}
	}
	free(pEnemy);
}

void UpdateEnemy(int nStage)
{
	VERTEX_2D *pVtx;
	PLAYER *pPlayer = GetPlayer();
	POLYGON *pPolygon = GetPolyBlock();
	D3DXVECTOR2 *pAllmove = GetAllMove();//スクロール
	int nPolyNum = PolyNum(nStage);//敵とblockの当たり判定用
	g_nCntTimeEm++;//時間count
	bool bColAttack = false;
	//世界物理とアニメーション
	for (int nCnt = 0; nCnt < g_aEnemy[nStage]; nCnt++)
	{
		pEnemy[nCnt].pos.x += pAllmove->x;//スクロール移動
		pEnemy[nCnt].move.y += pPlayer->fGravity;//重力
		pEnemy[nCnt].pos.y += pEnemy[nCnt].move.y;
		if (g_nCntTimeEm % 10 == 0) pEnemy[nCnt].nAnime++;//animation
	}

	//BLOCK当たり判定
	for (int nCnt = 0; nCnt < g_aEnemy[nStage]; nCnt++)
	{
		pPolygon = GetPolyBlock();
		COLLISION_STATE pEnemy_CS = CS_MAX;
		for (int nCntPoly = 0; nCntPoly < nPolyNum; nCntPoly++, pPolygon++)
		{
			if (pPolygon->bBlock == true)
			{
				//top
				if (((pEnemy[nCnt].pos.x <= pPolygon->pos.x + pPolygon->size.x + pEnemy[nCnt].size.x / 2)
					&& (pEnemy[nCnt].pos.x >= pPolygon->pos.x - pPolygon->size.x - pEnemy[nCnt].size.x / 2))
					&& (pEnemy[nCnt].pos.y < pPolygon->pos.y)
					&& (pEnemy[nCnt].pos.y > pPolygon->pos.y - pPolygon->size.y - pEnemy[nCnt].size.y))
				{
					pEnemy[nCnt].pos.y = pPolygon->pos.y - pPolygon->size.y - pEnemy[nCnt].size.y;
					if (pEnemy[nCnt].move.y >= 0) pEnemy[nCnt].move.y = 0;
				}
			}
		}
	}

	//Enemy状態遷移
	for (int nCnt = 0; nCnt < g_aEnemy[nStage]; nCnt++)
	{
		switch (pEnemy[nCnt].nState)
		{
		case E_IDLE:
			if (pEnemy[nCnt].nDirection == 0)//左向き
			{
				//playerは左側400以内にいる時
				if (pPlayer->pos.x < pEnemy[nCnt].pos.x && abs(pPlayer->pos.x - pEnemy[nCnt].pos.x) < 400)
				{
					pEnemy[nCnt].move.x = -WALK_SPEED;
					pEnemy[nCnt].nAnime = 0;
					pEnemy[nCnt].nState = E_ALERT;
				}
				else if (g_nCntTimeEm % 150 == 0) pEnemy[nCnt].nDirection = 1;//右向き
			}
			else//右向き
			{
				//playerは左側400以内にいる時
				if (pPlayer->pos.x > pEnemy[nCnt].pos.x && abs(pPlayer->pos.x - pEnemy[nCnt].pos.x) < 400)
				{
					pEnemy[nCnt].move.x = WALK_SPEED;
					pEnemy[nCnt].nAnime = 0;
					pEnemy[nCnt].nState = E_ALERT;
				}
				else if (g_nCntTimeEm % 150 == 0) pEnemy[nCnt].nDirection = 0;//左向き
			}
			break;
		case E_WALK:
			pEnemy[nCnt].pos.x += pEnemy[nCnt].move.x;//移動の更新
			pEnemy[nCnt].nMoveRange += (int)pEnemy[nCnt].move.x;//移動距離を記録
			if (pEnemy[nCnt].nMoveRange >= 100)//移動距離の最大値
			{
				pEnemy[nCnt].move.x = 0;
				pEnemy[nCnt].nAnime = 0;
				pEnemy[nCnt].nState = E_IDLE;
				pEnemy[nCnt].nMoveRange = 0;
			}
			break;
		case E_ALERT:
			if (abs(pPlayer->pos.x - pEnemy[nCnt].pos.x) > 400)//範囲外
			{
				pEnemy[nCnt].nState = E_WALK;
				pEnemy[nCnt].nDirection = pEnemy[nCnt].nDirection ^ 1;
				pEnemy[nCnt].move.x = -pEnemy[nCnt].move.x;
			}
			else if(abs(pPlayer->pos.x - pEnemy[nCnt].pos.x) <= (pPlayer->size.x + pEnemy[nCnt].size.x))//攻撃範囲内
			{
				pEnemy[nCnt].nAnime = 0;
				pEnemy[nCnt].nState = E_ATTACK;
			}
			else
			{
				pEnemy[nCnt].pos.x += pEnemy[nCnt].move.x;//移動の更新
				pEnemy[nCnt].nMoveRange += (int)pEnemy[nCnt].move.x;//移動距離を記録
				if (pEnemy[nCnt].nMoveRange >= 400)
				{
					pEnemy[nCnt].nMoveRange = 0;
					pEnemy[nCnt].nState = E_WALK;
				}
			}
			break;
		case E_ATTACK:
			pEnemy[nCnt].move.x = 0;
			if (abs(pPlayer->pos.x - pEnemy[nCnt].pos.x) > (pPlayer->size.x + pEnemy[nCnt].size.x))//攻撃範囲外
			{
				pEnemy[nCnt].nAnime = 0;
				pEnemy[nCnt].nState = E_ALERT;
				if (pEnemy[nCnt].nDirection == 0) pEnemy[nCnt].move.x = -WALK_SPEED;
				else pEnemy[nCnt].move.x = WALK_SPEED;
			}
			else
			{
				if (pEnemy[nCnt].nAnime % 10 == 8)//攻撃動作第８フレームの時当たり判定をする
				{
					if (pEnemy[nCnt].nDirection == 0)
					{
						bColAttack = Vtx_Vector_ColDet(D3DXVECTOR3(pEnemy[nCnt].pos.x - pEnemy[nCnt].size.x - 1, pEnemy[nCnt].pos.y, pEnemy[nCnt].pos.z), pPlayer->pos, pPlayer->size);
						if (bColAttack == true)
						{
							if (pPlayer->nState == P_DEFEND && pPlayer->nDirection == 1)
							{
								pPlayer->nAnime = 0;
								pPlayer->nState = P_REFLECTION;
								pEnemy[nCnt].nState = E_HURT;
								pEnemy[nCnt].nLife--;
								if (pEnemy[nCnt].nLife < 1) pEnemy[nCnt].nState = E_DIED;
							}
							else if (pPlayer->nState != P_HURT)
							{
								pPlayer->nLife--;
								if (pPlayer->nLife < 1)
								{
									PlaySound(SOUND_LABEL_SE_DEAD);
									pPlayer->nState = P_DIED;
								}
								else
								{
									PlaySound(SOUND_LABEL_SE_GETHIT);
									pPlayer->nState = P_HURT;
								}
							}
						}
					}
					else
					{
						bColAttack = Vtx_Vector_ColDet(D3DXVECTOR3(pEnemy[nCnt].pos.x + pEnemy[nCnt].size.x + 1, pEnemy[nCnt].pos.y, pEnemy[nCnt].pos.z), pPlayer->pos, pPlayer->size);
						if (bColAttack == true)
						{
							if (pPlayer->nState == P_DEFEND && pPlayer->nDirection == 0)
							{
								pPlayer->nAnime = 0;
								pPlayer->nState = P_REFLECTION;
								pEnemy[nCnt].nState = E_HURT;
								pEnemy[nCnt].nLife--;
								if (pEnemy[nCnt].nLife < 1) pEnemy[nCnt].nState = E_DIED;
							}
							else if (pPlayer->nState != P_HURT)
							{
								pPlayer->nLife--;
								if (pPlayer->nLife < 1)
								{
									PlaySound(SOUND_LABEL_SE_DEAD);
									pPlayer->nState = P_DIED;
								}
								else
								{
									PlaySound(SOUND_LABEL_SE_GETHIT);
									pPlayer->nState = P_HURT;
								}
							}
						}
					}
				}
			}
		
			break;
		case E_HURT:
			if (pEnemy[nCnt].bAnimeChange == true)
			{
				pEnemy[nCnt].move.x = 0;//移動停止
				pEnemy[nCnt].nAnime = 0;//animation reset
				pEnemy[nCnt].bAnimeChange = false;//animation lock

				//撃退効果
				if (pPlayer->nDirection == 1) pEnemy[nCnt].pos.x += 50.0f;
				else pEnemy[nCnt].pos.x -= 50.0f;
			}
			if (pEnemy[nCnt].nAnime > 9)
			{
				pEnemy[nCnt].nState = E_IDLE;
				pEnemy[nCnt].bAnimeChange = true;
			}
			break;
		case E_DIED:
			pEnemy[nCnt].move.x = 0;
			pEnemy[nCnt].bBlock = false;
			if (pEnemy[nCnt].nAnime > 9) pEnemy[nCnt].nAnime = 9;
			//死体を消す
			pEnemy[nCnt].nDeadTime++;
			if (pEnemy[nCnt].nDeadTime % 300 == 0)
			{
				pEnemy[nCnt].bUse = false;
				pEnemy[nCnt].nDeadTime = 0;
			}
			break;
		}
	}

	//vtx情報更新
	g_pVtxBuffEnemy->Lock(0, 0, (void**)&pVtx, 0);
	for (int nCnt = 0; nCnt < g_aEnemy[nStage]; nCnt++, pVtx += 4)
	{
		pVtx[0].pos = D3DXVECTOR3(pEnemy[nCnt].pos.x - pEnemy[nCnt].size.x, pEnemy[nCnt].pos.y - pEnemy[nCnt].size.y, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(pEnemy[nCnt].pos.x + pEnemy[nCnt].size.x, pEnemy[nCnt].pos.y - pEnemy[nCnt].size.y, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(pEnemy[nCnt].pos.x - pEnemy[nCnt].size.x, pEnemy[nCnt].pos.y + pEnemy[nCnt].size.y, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(pEnemy[nCnt].pos.x + pEnemy[nCnt].size.x, pEnemy[nCnt].pos.y + pEnemy[nCnt].size.y, 0.0f);
		if (pEnemy[nCnt].nDirection == 1)
		{
			pVtx[0].tex = D3DXVECTOR2(0.1f * pEnemy[nCnt].nAnime, 0.0);
			pVtx[1].tex = D3DXVECTOR2(0.1f + 0.1f * pEnemy[nCnt].nAnime, 0.0);
			pVtx[2].tex = D3DXVECTOR2(0.1f * pEnemy[nCnt].nAnime, 1.0);
			pVtx[3].tex = D3DXVECTOR2(0.1f + 0.1f * pEnemy[nCnt].nAnime, 1.0);
		}
		else
		{
			pVtx[0].tex = D3DXVECTOR2(0.1f + 0.1f * pEnemy[nCnt].nAnime, 0.0);
			pVtx[1].tex = D3DXVECTOR2(0.1f * pEnemy[nCnt].nAnime, 0.0);
			pVtx[2].tex = D3DXVECTOR2(0.1f + 0.1f * pEnemy[nCnt].nAnime, 1.0);
			pVtx[3].tex = D3DXVECTOR2(0.1f * pEnemy[nCnt].nAnime, 1.0);
		}
	}
	g_pVtxBuffEnemy->Unlock();
}

void DrawEnemy(int nStage)
{
	LPDIRECT3DDEVICE9 pDeviceBk = GetDevice();
	pDeviceBk->SetStreamSource(0, g_pVtxBuffEnemy, 0, sizeof(VERTEX_2D));
	pDeviceBk->SetFVF(FVF_VERTEX_2D);
	for (int nCnt = 0; nCnt < g_aEnemy[nStage]; nCnt++)
	{
		if (pEnemy[nCnt].bUse == true)
		{
			switch (pEnemy[nCnt].nType)
			{
			case 0:
				if (pEnemy[nCnt].nState == E_ALERT)
					pDeviceBk->SetTexture(0, g_pTextureEnemy[E_WALK]);
				else
					pDeviceBk->SetTexture(0, g_pTextureEnemy[pEnemy[nCnt].nState]);
				break;
			}
			pDeviceBk->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCnt * 4, 2);
		}
	}
}

int EnemyNum(int nStage)
{
	return g_aEnemy[nStage];
}

ENEMY *GetEnemy(void)
{
	return pEnemy;
}