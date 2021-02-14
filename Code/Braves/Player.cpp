//=============================================================================
//
// [Player.cpp]　
// Author : Yo Jitaku
//
//=============================================================================
#include "player.h"
#include "Input.h"
#include "PolyBlock.h"
#include "Fade.h"
#include "enemy.h"
#include "collision.h"
#include "sound.h"

LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffPlayer = NULL;
LPDIRECT3DTEXTURE9 g_pTexturePlayer[MAX_PLAYER_TEXTURE] = {};
PLAYER g_aPlayer;
int g_nCntTimeplayer;

HRESULT InitPlayer(int nStage)
{
	LPDIRECT3DDEVICE9 pDevicePy = GetDevice();
	LPDIRECT3DDEVICE9 pDeviceUpKey = GetDevice();
	VERTEX_2D *pVtx;
	PLAYER *pPlayer = &g_aPlayer;
	g_nCntTimeplayer = 0;
	SetPlayer(nStage);

	D3DXCreateTextureFromFile(pDevicePy, "data/TEXTURE/Playeridle.png", &g_pTexturePlayer[0]);
	D3DXCreateTextureFromFile(pDevicePy, "data/TEXTURE/PlayerWalk.png", &g_pTexturePlayer[1]);
	D3DXCreateTextureFromFile(pDevicePy, "data/TEXTURE/Playerattack1.png", &g_pTexturePlayer[2]);
	D3DXCreateTextureFromFile(pDevicePy, "data/TEXTURE/Playerattack2.png", &g_pTexturePlayer[3]);
	D3DXCreateTextureFromFile(pDevicePy, "data/TEXTURE/Playerattack3.png", &g_pTexturePlayer[4]);
	D3DXCreateTextureFromFile(pDevicePy, "data/TEXTURE/Playerjump.png", &g_pTexturePlayer[5]);
	D3DXCreateTextureFromFile(pDevicePy, "data/TEXTURE/PlayerDefend.png", &g_pTexturePlayer[6]);
	D3DXCreateTextureFromFile(pDevicePy, "data/TEXTURE/PlayerReflection.png", &g_pTexturePlayer[7]);
	D3DXCreateTextureFromFile(pDevicePy, "data/TEXTURE/PlayerDied.png", &g_pTexturePlayer[8]);

	if (FAILED(pDevicePy->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAX_PLAYER_TEXTURE, D3DUSAGE_WRITEONLY, FVF_VERTEX_2D, D3DPOOL_MANAGED, &g_pVtxBuffPlayer, NULL))) return E_FAIL;
	g_pVtxBuffPlayer->Lock(0, 0, (void**)&pVtx, 0);
	pVtx[0].pos = D3DXVECTOR3(pPlayer->pos.x - pPlayer->size.x, pPlayer->pos.y - pPlayer->size.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(pPlayer->pos.x + pPlayer->size.x, pPlayer->pos.y - pPlayer->size.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(pPlayer->pos.x - pPlayer->size.x, pPlayer->pos.y + pPlayer->size.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(pPlayer->pos.x + pPlayer->size.x, pPlayer->pos.y + pPlayer->size.y, 0.0f);
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;
	pVtx[0].col = pPlayer->color;
	pVtx[1].col = pPlayer->color;
	pVtx[2].col = pPlayer->color;
	pVtx[3].col = pPlayer->color;
	pVtx[0].tex = D3DXVECTOR2((float)0.0 + pPlayer->nAnime * (float)0.125, 0.0);
	pVtx[1].tex = D3DXVECTOR2((float)0.125 + pPlayer->nAnime * (float)0.125, 0.0);
	pVtx[2].tex = D3DXVECTOR2((float)0.0 + pPlayer->nAnime * (float)0.125, 1);
	pVtx[3].tex = D3DXVECTOR2((float)0.125 + pPlayer->nAnime * (float)0.125, 1);
	g_pVtxBuffPlayer->Unlock();
	return S_OK;
}

void SetPlayer(int nStage)
{
	PLAYER *pPlayer = &g_aPlayer;
	pPlayer->color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pPlayer->bJump = false;
	pPlayer->nJump = 2;
	pPlayer->bUse = true;
	pPlayer->bAnimeChange = true;
	pPlayer->fGravity = 1.5f;
	pPlayer->pos = D3DXVECTOR3(150, 600, 0);
	pPlayer->prepos = pPlayer->pos;
	pPlayer->move = D3DXVECTOR3(0, 0, 0);
	pPlayer->size = D3DXVECTOR2(60, 60);
	pPlayer->nDirection = 1;//右向き
	pPlayer->nAnime = 0;
	pPlayer->nState = P_IDLE;
	pPlayer->nLife = 3;
	pPlayer->nScore = 0;
	pPlayer->nDoor = 10;//初期化、意味ない
}

void UninitPlayer(void)
{
	if (g_pVtxBuffPlayer != NULL)
	{
		g_pVtxBuffPlayer->Release();
		g_pVtxBuffPlayer = NULL;
	}
	for (int nCnt = 0; nCnt < P_MAX; nCnt++)
	{
		if (g_pTexturePlayer[nCnt] != NULL)
		{
			g_pTexturePlayer[nCnt]->Release();
			g_pTexturePlayer[nCnt] = NULL;
		}
	}
}

void UpdatePlayer(int nStage)
{
	VERTEX_2D *pVtx;
	LPDIRECT3DDEVICE9 pDevicePy = GetDevice();
	PLAYER *pPlayer = &g_aPlayer;
	POLYGON *pPolygon = GetPolyBlock();
	ENEMY *pEnemy = GetEnemy();

	g_nCntTimeplayer++;
	int nPolyNum = PolyNum(nStage);//block number
	int nMode = GetMode();//今のstageを取る
	bool ColDet = false;//playerで一つ点の当たり判定用
	
	pPlayer->move.y += pPlayer->fGravity;//重力
	pPlayer->pos += pPlayer->move;//player移動の更新
	pPlayer->move.x = 0;//移動加速度のreset
	int nNumEnemy = EnemyNum(nStage);
	//playerとlockの当たり判定
	COLLISION_STATE pPlayer_CS = CS_MAX;
	for (int nCnt = 0; (nCnt < nPolyNum) && (pPlayer->bUse == true); nCnt++, pPolygon++)
	{
		switch (pPolygon->nTextureID)
		{
		case 4://door
			//playerの中心座標とdoorの当たり判定
			ColDet = Vtx_Vector_ColDet(pPlayer->pos, pPolygon->pos, pPolygon->size);
			if (ColDet == true)
			{
				//player
				pPolygon->bAnime = true;
				if (GetkeyboardPress(DIK_UP) == true)//enter the door
				{
					PlaySound(SOUND_LABEL_SE_DOOR);
					pPlayer->nDoor = pPolygon->nDoorID;//進入したdoorを記録
					switch (nMode)
					{
					case MODE_MENU:
						if (pPolygon->nDoorID == 1)//tutorial
						{
							SetFade(FADE_OUT, MODE_TUTORIAL);
						}
						if (pPolygon->nDoorID == 2)//start
						{
							SetFade(FADE_OUT, MODE_STAGE1);
							pPlayer->pos = D3DXVECTOR3(1 * DEFAULT_WIDTH + DEFAULT_WIDTH, 6 * DEFAULT_HEIGHT + 0.3f * DEFAULT_HEIGHT, 0);
							pPlayer->color.a = 0;
							pPlayer->move = D3DXVECTOR3(0, 0, 0);
							pPlayer->nAnime = 0;
							pPlayer->nState = P_IDLE;
						}
						if (pPolygon->nDoorID == 3)//ranking
						{
							SetFade(FADE_OUT, MODE_RANK);
						}
						break;
					case MODE_STAGE1:
						if (pPolygon->nDoorID == 2)
						{
							SetFade(FADE_OUT, MODE_MENU);
							pPlayer->pos = D3DXVECTOR3(10 * DEFAULT_WIDTH + DEFAULT_WIDTH, 6 * DEFAULT_HEIGHT + 0.3f * DEFAULT_HEIGHT, 0);
							pPlayer->color.a = 0;
							pPlayer->move = D3DXVECTOR3(0, 0, 0);
							pPlayer->nAnime = 0;
							pPlayer->nState = P_IDLE;
						}
						if (pPolygon->nDoorID == 1)
						{
							SetFade(FADE_OUT, MODE_STAGE2);
							pPlayer->pos = D3DXVECTOR3(1 * DEFAULT_WIDTH + DEFAULT_WIDTH, 6 * DEFAULT_HEIGHT + 0.3f * DEFAULT_HEIGHT, 0);
							pPlayer->color.a = 0;
							pPlayer->move = D3DXVECTOR3(0, 0, 0);
							pPlayer->nAnime = 0;
							pPlayer->nState = P_IDLE;
						}
						break;
					case MODE_STAGE2:
						if (pPolygon->nDoorID == 2)
						{
							pPlayer->nScore += pPlayer->nLife * 300;
							SetFade(FADE_OUT, MODE_RANK);
						}
					default:
						break;
					}
				}
			}
			else
			{
				pPolygon->bAnime = false;
			}
			break;
		case 8://acid
			ColDet = Vtx_Vector_ColDet(pPlayer->pos, pPolygon->pos, pPolygon->size);
			if (ColDet == true)
			{
				if (pPlayer->nLife < 1)
				{
					pPlayer->nAnime = 0;
					pPlayer->nState = P_DIED;
					PlaySound(SOUND_LABEL_SE_DEAD);
				}
				else
				{
					if (pPlayer->nState != P_HURT)
					{
						pPlayer->nState = P_HURT;
						pPlayer->nLife--;
						PlaySound(SOUND_LABEL_SE_RELIVE);
						PlaySound(SOUND_LABEL_SE_GETHIT);
					}
				}
			}
			break;
		case 13: case 16://item
			ColDet = Vtx_Vector_ColDet(pPlayer->pos, pPolygon->pos, pPolygon->size * 2);
			if (ColDet == true)
			{
				if (pPolygon->bUse == true)
				{
					if (pPolygon->nTextureID == 13)
					{
						PlaySound(SOUND_LABEL_SE_HP);
						pPolygon->bUse = false;
						pPlayer->nScore += 300;
						pPlayer->nLife++;
					}
					else
					{
						PlaySound(SOUND_LABEL_SE_COIN);
						pPolygon->bUse = false;
						pPlayer->nScore += 100;
					}
				}
			}
			break;
		case 17://UPkey
			if (abs(pPlayer->pos.x - pPolygon->pos.x) <= 100)
				pPolygon->bUse = true;
			else pPolygon->bUse = false;
			break;
		default:
			pPlayer_CS = Player_ColDet(pPlayer->pos, pPlayer->size, pPolygon->pos, pPolygon->size);
			switch (pPlayer_CS)
			{
			case CS_UP:
				pPlayer->pos.y = pPolygon->pos.y - pPolygon->size.y - pPlayer->size.y;
				if (pPlayer->move.y > 0) pPlayer->move.y = 0;
				pPlayer->move.x += pPolygon->move.x;//移動block
				//着地処理
				pPlayer->bJump = false;
				if (pPlayer->nState == P_JUMP)
				{
					pPlayer->nState = P_IDLE;
					pPlayer->nJump = 2;
				}
				break;
			case CS_DOWN:
				pPlayer->pos.y = pPolygon->pos.y + pPolygon->size.y + pPlayer->size.y;
				pPlayer->move.y = pPlayer->fGravity;
				break;
			case CS_LEFT:
				pPlayer->pos.x = pPolygon->pos.x + pPolygon->size.x + pPlayer->size.x;
				break;
			case CS_RIGHT:
				pPlayer->pos.x = pPolygon->pos.x - pPolygon->size.x - pPlayer->size.x;
				break;
			default:
				break;
			}
		}
	}
	//Player control
	if (pPlayer->color.a == 1)//描画確認前に移動できません
	{
		switch (pPlayer->nState)
		{
		case P_IDLE:
			pPlayer->size = D3DXVECTOR2(60, 60);//texture sizeの調整
			if (GetkeyboardPress(DIK_C) == true) pPlayer->nState = P_DEFEND;
			else if (GetkeyboardTrigger(DIK_Z) == true)
			{
				pPlayer->nAnime = 0;
				pPlayer->move.y = -30;
				pPlayer->bJump = true;
				pPlayer->nState = P_JUMP;
				pPlayer->nJump--;
			}
			else if (GetkeyboardPress(DIK_X) == true)
			{
				pPlayer->nAnime = 0;
				pPlayer->nState = P_ATTACK1;
				pPlayer->bAnimeChange = false;
				break;
			}
			if (GetkeyboardPress(DIK_LEFT) == true)
			{
				pPlayer->move.x += -10;
				pPlayer->nDirection = 0;//left texture set
				if (pPlayer->bAnimeChange == true) pPlayer->nState = P_WALK;
			}
			else if (GetkeyboardPress(DIK_RIGHT) == true)
			{
				pPlayer->move.x += 10;
				pPlayer->nDirection = 1;//right texture set
				if (pPlayer->bAnimeChange == true) pPlayer->nState = P_WALK;
			}
			break;
		case P_WALK:
			pPlayer->size = D3DXVECTOR2(65, 65);//texture sizeの調整
			if (GetkeyboardPress(DIK_C) == true) pPlayer->nState = P_DEFEND;
			else if (GetkeyboardTrigger(DIK_Z) == true && pPlayer->color.a == 1)
			{
				pPlayer->nAnime = 0;
				pPlayer->move.y = -30;
				pPlayer->bJump = true;
				pPlayer->nState = P_JUMP;
				pPlayer->nJump--;
			}
			else if (GetkeyboardPress(DIK_X) == true)
			{
				pPlayer->nAnime = 0;
				pPlayer->nState = P_ATTACK1;
				pPlayer->bAnimeChange = false;
				PlaySound(SOUND_LABEL_SE_ATTACK);
			}
			else if (GetkeyboardPress(DIK_LEFT) == true)
			{
				pPlayer->move.x += -10;
				pPlayer->nDirection = 0;//left texture set
			}
			else if (GetkeyboardPress(DIK_RIGHT) == true)
			{
				pPlayer->move.x += 10;
				pPlayer->nDirection = 1;//right texture set
			}
			else pPlayer->nState = P_IDLE;
			break;
		case P_ATTACK1:
			pPlayer->size = D3DXVECTOR2(80, 60);//texture sizeの調整
			if (GetkeyboardPress(DIK_X) == true && pPlayer->bAnimeChange == true)
			{
				pPlayer->nAnime = 0;
				pPlayer->nState = P_ATTACK2;
				pPlayer->bAnimeChange = false;
				PlaySound(SOUND_LABEL_SE_ATTACK);
			}
			else if (pPlayer->bAnimeChange == true) pPlayer->nState = P_IDLE;
			//jump中攻撃
			if (pPlayer->move.y > 0)
			{
				if (GetkeyboardPress(DIK_LEFT) == true)
				{
					pPlayer->move.x += -10;
					pPlayer->nDirection = 0;//left texture set
				}
				else if (GetkeyboardPress(DIK_RIGHT) == true)
				{
					pPlayer->move.x += 10;
					pPlayer->nDirection = 1;//right texture set
				}
			}
			break;
		case P_ATTACK2:
			pPlayer->size = D3DXVECTOR2(95, 70);//texture sizeの調整
			if (GetkeyboardPress(DIK_X) == true && pPlayer->bAnimeChange == true)
			{
				pPlayer->nAnime = 0;
				pPlayer->nState = P_ATTACK3;
				pPlayer->bAnimeChange = false;
				if (pPlayer->nDirection == 1) pPlayer->pos.x += 50;
				else pPlayer->pos.x -= 50;
				PlaySound(SOUND_LABEL_SE_ATTACK);
			}
			else if (pPlayer->bAnimeChange == true) pPlayer->nState = P_IDLE;
			//jump中攻撃
			if (pPlayer->move.y > 0)
			{
				if (GetkeyboardPress(DIK_LEFT) == true)
				{
					pPlayer->move.x += -10;
					pPlayer->nDirection = 0;//left texture set
				}
				else if (GetkeyboardPress(DIK_RIGHT) == true)
				{
					pPlayer->move.x += 10;
					pPlayer->nDirection = 1;//right texture set
				}
			}
			break;
		case P_ATTACK3:
			pPlayer->size = D3DXVECTOR2(110, 75);//texture sizeの調整
			if (GetkeyboardPress(DIK_X) == true && pPlayer->bAnimeChange == true)
			{
				pPlayer->nAnime = 0;
				pPlayer->nState = P_ATTACK1;
				pPlayer->bAnimeChange = false;
				PlaySound(SOUND_LABEL_SE_ATTACK);
			}
			else if (pPlayer->bAnimeChange == true) pPlayer->nState = P_IDLE;
			//jump中攻撃
			if (pPlayer->move.y > 0)
			{
				if (GetkeyboardPress(DIK_LEFT) == true)
				{
					pPlayer->move.x += -10;
					pPlayer->nDirection = 0;//left texture set
				}
				else if (GetkeyboardPress(DIK_RIGHT) == true)
				{
					pPlayer->move.x += 10;
					pPlayer->nDirection = 1;//right texture set
				}
			}
			break;
		case P_JUMP:
			pPlayer->size = D3DXVECTOR2(65, 65);//texture sizeの調整
			if (GetkeyboardPress(DIK_X) == true)
			{
				pPlayer->nAnime = 0;
				pPlayer->nState = P_ATTACK1;
			}
			else if (GetkeyboardTrigger(DIK_Z) == true && pPlayer->nJump == 1)
			{
				pPlayer->nAnime = 0;
				pPlayer->move.y = -25;
				pPlayer->nJump--;
			}
			if (GetkeyboardPress(DIK_LEFT) == true)
			{
				pPlayer->move.x += -10;
				pPlayer->nDirection = 0;//left texture set
			}
			else if (GetkeyboardPress(DIK_RIGHT) == true)
			{
				pPlayer->move.x += 10;
				pPlayer->nDirection = 1;//right texture set
			}
			break;
		case P_DEFEND:
			if (GetkeyboardPress(DIK_C) == true)
			{
				pPlayer->nState = P_DEFEND;
			}
			else pPlayer->nState = P_IDLE;
			break;
		case P_REFLECTION:
			pPlayer->bAnimeChange = false;
			if (GetkeyboardPress(DIK_X) == true)
			{
				pPlayer->nAnime = 0;
				pPlayer->nState = P_ATTACK1;
				pPlayer->bAnimeChange = false;
				break;
			}
			break;
		case P_HURT:
			pPlayer->bUse = false;
			pPolygon = GetPolyBlock();//reset
			for (int nCnt = 0; nCnt < nPolyNum; nCnt++, pPolygon++)
			{
				switch (nStage)
				{
				case 3:
					if (pPolygon->nTextureID == 4 && pPolygon->nDoorID == 2)//stage1で左側のdoorIDは2
					{
						if (pPlayer->pos.x <= pPolygon->pos.x)
						{
							pPlayer->pos.x = pPolygon->pos.x;
							pPlayer->pos.y = pPolygon->pos.y - 10;
							pPlayer->move.y = 0.f;
							pPlayer->bUse = true;
							pPlayer->nState = P_IDLE;
							pPlayer->nAnime = 0;
							StopSound(SOUND_LABEL_SE_RELIVE);
						}
						else pPlayer->move.x = -20.f;
					}
					break;
				case 4:
					if (pPolygon->nTextureID == 4 && pPolygon->nDoorID == 1)//stage2で左側のdoorIDは1
					{
						if (pPlayer->pos.x <= pPolygon->pos.x)
						{
							pPlayer->pos.x = pPolygon->pos.x;
							pPlayer->pos.y = pPolygon->pos.y - 10;
							pPlayer->move.y = 0.f;
							pPlayer->bUse = true;
							pPlayer->nState = P_IDLE;
							pPlayer->nAnime = 0;
							StopSound(SOUND_LABEL_SE_RELIVE);
						}
						else pPlayer->move.x = -30.f;
					}
					break;
				default:
					break;
				}
			}
			break;
		default:
			break;
		}
	}
	
	//PlayerとEnemyの当たり判定
	for (int nCntEm = 0; nCntEm < nNumEnemy; nCntEm++)
	{
		if (pEnemy[nCntEm].bUse == true)
		{
			pPlayer_CS = Player_ColDet(pPlayer->pos, pPlayer->size, pEnemy[nCntEm].pos, pEnemy[nCntEm].size);
			switch (pPlayer_CS)
			{
			case CS_LEFT:
				pPlayer->pos.x = pEnemy[nCntEm].pos.x + pEnemy[nCntEm].size.x + pPlayer->size.x;
				break;
			case CS_RIGHT:
				pPlayer->pos.x = pEnemy[nCntEm].pos.x - pEnemy[nCntEm].size.x - pPlayer->size.x;
				break;
			default:
				break;
			}
		}
	}

	//playerの攻撃判定
	for (int nCntEm = 0; nCntEm < nNumEnemy; nCntEm++)
	{
		if ((pPlayer->nState == P_ATTACK1 || pPlayer->nState == P_ATTACK2 || pPlayer->nState == P_ATTACK3))
		{
			if (pEnemy[nCntEm].bUse == true)
			{
				if (pPlayer->nDirection == 0)//左向き
				{
					bool ColDet = false;
					ColDet = Vtx_Vector_ColDet(D3DXVECTOR3(pPlayer->pos.x - pPlayer->size.x, pPlayer->pos.y, 0.0f), pEnemy[nCntEm].pos, pEnemy[nCntEm].size);
					if (ColDet == true &&
						pEnemy[nCntEm].nState != E_HURT && pEnemy[nCntEm].nState != E_DIED)
					{
						pEnemy[nCntEm].nLife--;
						PlaySound(SOUND_LABEL_SE_HIT);
						if (pEnemy[nCntEm].nLife < 1)
						{
							pEnemy[nCntEm].nAnime = 0;
							pEnemy[nCntEm].nState = E_DIED;
							pPlayer->nScore += 300;
						}
						else
						{
							pEnemy[nCntEm].nState = E_HURT;
							pEnemy[nCntEm].nAnime = 0;
						}
					}
				}
				else//右向き
				{
					bool ColDet = false;
					ColDet = Vtx_Vector_ColDet(D3DXVECTOR3(pPlayer->pos.x + pPlayer->size.x, pPlayer->pos.y, 0.0f), pEnemy[nCntEm].pos, pEnemy[nCntEm].size);
					if (ColDet == true &&
						pEnemy[nCntEm].nState != E_HURT && pEnemy[nCntEm].nState != E_DIED)
					{
						pEnemy[nCntEm].nLife--;
						PlaySound(SOUND_LABEL_SE_HIT);
						if (pEnemy[nCntEm].nLife < 1)
						{
							pEnemy[nCntEm].nAnime = 0;
							pEnemy[nCntEm].nState = E_DIED;
							pPlayer->nScore += 300;
						}
						else
						{
							pEnemy[nCntEm].nState = E_HURT;
							pEnemy[nCntEm].nAnime = 0;
						}
					}
				}
			}
		}
	}

	//playerの移動は画面内に絞る
	if (pPlayer->pos.x <= 0 + pPlayer->size.x) pPlayer->pos.x = 0 + pPlayer->size.x;
	if (pPlayer->pos.x >= SCREEN_WIDTH - pPlayer->size.x) pPlayer->pos.x = SCREEN_WIDTH - pPlayer->size.x;
	//playerは画面外の時
	if (pPlayer->pos.y >= SCREEN_HEIGHT && pPlayer->nState != P_HURT)
	{
		if (pPlayer->nLife < 1)
		{
			pPlayer->nAnime = 0;
			pPlayer->nState = P_DIED;
			PlaySound(SOUND_LABEL_SE_DEAD);
		}
		else
		{
			if (pPlayer->nState != P_HURT)
			{
				pPlayer->nState = P_HURT;
				pPlayer->nLife--;
				PlaySound(SOUND_LABEL_SE_RELIVE);
				PlaySound(SOUND_LABEL_SE_GETHIT);
			}
		}
	}

	g_pVtxBuffPlayer->Lock(0, 0, (void**)&pVtx, 0);
	pVtx[0].pos = D3DXVECTOR3(pPlayer->pos.x - pPlayer->size.x, pPlayer->pos.y - pPlayer->size.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(pPlayer->pos.x + pPlayer->size.x, pPlayer->pos.y - pPlayer->size.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(pPlayer->pos.x - pPlayer->size.x, pPlayer->pos.y + pPlayer->size.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(pPlayer->pos.x + pPlayer->size.x, pPlayer->pos.y + pPlayer->size.y, 0.0f);

	//fade in door
	if (pPlayer->color.a >= 1) pPlayer->color.a = 1;
	else
	{
		pPlayer->color.a += 0.01f;
		pVtx[0].col = pPlayer->color;
		pVtx[1].col = pPlayer->color;
		pVtx[2].col = pPlayer->color;
		pVtx[3].col = pPlayer->color;
	}

	//animation
	switch (pPlayer->nState)
	{
	case P_IDLE:
		if (g_nCntTimeplayer % 5 == 0) pPlayer->nAnime++;
		if (pPlayer->nDirection == 1)
		{
			pVtx[0].tex = D3DXVECTOR2(0.0f + pPlayer->nAnime * 0.125f, 0);
			pVtx[1].tex = D3DXVECTOR2(0.125f + pPlayer->nAnime * 0.125f, 0);
			pVtx[2].tex = D3DXVECTOR2(0.0f + pPlayer->nAnime * 0.125f, 1);
			pVtx[3].tex = D3DXVECTOR2(0.125f + pPlayer->nAnime *0.125f, 1);
		}
		else
		{
			pVtx[0].tex = D3DXVECTOR2(0.125f + pPlayer->nAnime * 0.125f, 0);
			pVtx[1].tex = D3DXVECTOR2(0.0f + pPlayer->nAnime * 0.125f, 0);
			pVtx[2].tex = D3DXVECTOR2(0.125f + pPlayer->nAnime * 0.125f, 1);
			pVtx[3].tex = D3DXVECTOR2(0.0f + pPlayer->nAnime * 0.125f, 1);
		}
		break;
	case P_WALK:
		if (pPlayer->nAnime >= 10 && pPlayer->nState == 8) pPlayer->bAnimeChange = true;
		if (g_nCntTimeplayer % 5 == 0) pPlayer->nAnime++;
		if (pPlayer->nDirection == 1)
		{
			pVtx[0].tex = D3DXVECTOR2(0.0f + pPlayer->nAnime * 0.1f, 0);
			pVtx[1].tex = D3DXVECTOR2(0.1f + pPlayer->nAnime * 0.1f, 0);
			pVtx[2].tex = D3DXVECTOR2(0.0f + pPlayer->nAnime * 0.1f, 1);
			pVtx[3].tex = D3DXVECTOR2(0.1f + pPlayer->nAnime *0.1f, 1);
		}
		else
		{
			pVtx[0].tex = D3DXVECTOR2(0.1f + pPlayer->nAnime * 0.1f, 0);
			pVtx[1].tex = D3DXVECTOR2(0.0f + pPlayer->nAnime * 0.1f, 0);
			pVtx[2].tex = D3DXVECTOR2(0.1f + pPlayer->nAnime * 0.1f, 1);
			pVtx[3].tex = D3DXVECTOR2(0.0f + pPlayer->nAnime * 0.1f, 1);
		}
		break;
		break;
	case P_ATTACK1: case P_ATTACK2:
		if (g_nCntTimeplayer % 3 == 0) pPlayer->nAnime++;
		if (pPlayer->nAnime > 4)
			pPlayer->bAnimeChange = true;
		else
		{
			if (pPlayer->nDirection == 1)
			{
				pVtx[0].tex = D3DXVECTOR2(0.0f + pPlayer->nAnime * 0.2f, 0);
				pVtx[1].tex = D3DXVECTOR2(0.2f + pPlayer->nAnime * 0.2f, 0);
				pVtx[2].tex = D3DXVECTOR2(0.0f + pPlayer->nAnime * 0.2f, 1);
				pVtx[3].tex = D3DXVECTOR2(0.2f + pPlayer->nAnime * 0.2f, 1);
			}
			else
			{
				pVtx[0].tex = D3DXVECTOR2(0.2f + pPlayer->nAnime * 0.2f, 0);
				pVtx[1].tex = D3DXVECTOR2(0.0f + pPlayer->nAnime * 0.2f, 0);
				pVtx[2].tex = D3DXVECTOR2(0.2f + pPlayer->nAnime * 0.2f, 1);
				pVtx[3].tex = D3DXVECTOR2(0.0f + pPlayer->nAnime * 0.2f, 1);
			}
		}
		break;
	case P_ATTACK3:
		if (g_nCntTimeplayer % 4 == 0) pPlayer->nAnime++;
		if (pPlayer->nAnime > 7)
			pPlayer->bAnimeChange = true;
		else
		{
			if (pPlayer->nDirection == 1)
			{
				pVtx[0].tex = D3DXVECTOR2(0.0f + pPlayer->nAnime * 0.125f, 0);
				pVtx[1].tex = D3DXVECTOR2(0.125f + pPlayer->nAnime * 0.125f, 0);
				pVtx[2].tex = D3DXVECTOR2(0.0f + pPlayer->nAnime * 0.125f, 1);
				pVtx[3].tex = D3DXVECTOR2(0.125f + pPlayer->nAnime * 0.125f, 1);
			}
			else
			{
				pVtx[0].tex = D3DXVECTOR2(0.125f + pPlayer->nAnime * 0.125f, 0);
				pVtx[1].tex = D3DXVECTOR2(0.0f + pPlayer->nAnime * 0.125f, 0);
				pVtx[2].tex = D3DXVECTOR2(0.125f + pPlayer->nAnime * 0.125f, 1);
				pVtx[3].tex = D3DXVECTOR2(0.0f + pPlayer->nAnime * 0.125f, 1);
			}
		}
		break;
	case P_JUMP:
		if (g_nCntTimeplayer % 5 == 0) pPlayer->nAnime++;
		if (pPlayer->nAnime > 4)
			pPlayer->bAnimeChange = true;
		if (pPlayer->nDirection == 1)
		{
			pVtx[0].tex = D3DXVECTOR2(0.0f + pPlayer->nAnime * 0.2f, 0);
			pVtx[1].tex = D3DXVECTOR2(0.2f + pPlayer->nAnime * 0.2f, 0);
			pVtx[2].tex = D3DXVECTOR2(0.0f + pPlayer->nAnime * 0.2f, 1);
			pVtx[3].tex = D3DXVECTOR2(0.2f + pPlayer->nAnime * 0.2f, 1);
		}
		else
		{
			pVtx[0].tex = D3DXVECTOR2(0.2f + pPlayer->nAnime * 0.2f, 0);
			pVtx[1].tex = D3DXVECTOR2(0.0f + pPlayer->nAnime * 0.2f, 0);
			pVtx[2].tex = D3DXVECTOR2(0.2f + pPlayer->nAnime * 0.2f, 1);
			pVtx[3].tex = D3DXVECTOR2(0.0f + pPlayer->nAnime * 0.2f, 1);
		}
		break;
	case P_DEFEND:
		if (g_nCntTimeplayer % 5 == 0) pPlayer->nAnime++;
		if (pPlayer->nDirection == 1)
		{
			pVtx[0].tex = D3DXVECTOR2(0.0f + pPlayer->nAnime * 0.2f, 0);
			pVtx[1].tex = D3DXVECTOR2(0.2f + pPlayer->nAnime * 0.2f, 0);
			pVtx[2].tex = D3DXVECTOR2(0.0f + pPlayer->nAnime * 0.2f, 1);
			pVtx[3].tex = D3DXVECTOR2(0.2f + pPlayer->nAnime * 0.2f, 1);
		}
		else
		{
			pVtx[0].tex = D3DXVECTOR2(0.2f + pPlayer->nAnime * 0.2f, 0);
			pVtx[1].tex = D3DXVECTOR2(0.0f + pPlayer->nAnime * 0.2f, 0);
			pVtx[2].tex = D3DXVECTOR2(0.2f + pPlayer->nAnime * 0.2f, 1);
			pVtx[3].tex = D3DXVECTOR2(0.0f + pPlayer->nAnime * 0.2f, 1);
		}
		break;
	case P_REFLECTION:
		if (g_nCntTimeplayer % 10 == 0) pPlayer->nAnime++;
		if (pPlayer->nAnime > 4)
		{
			pPlayer->bAnimeChange = true;
			pPlayer->nState = P_DEFEND;
		}

		if (pPlayer->nDirection == 1)
		{
			pVtx[0].tex = D3DXVECTOR2(0.0f + pPlayer->nAnime * 0.2f, 0);
			pVtx[1].tex = D3DXVECTOR2(0.2f + pPlayer->nAnime * 0.2f, 0);
			pVtx[2].tex = D3DXVECTOR2(0.0f + pPlayer->nAnime * 0.2f, 1);
			pVtx[3].tex = D3DXVECTOR2(0.2f + pPlayer->nAnime * 0.2f, 1);
}
		else
		{
			pVtx[0].tex = D3DXVECTOR2(0.2f + pPlayer->nAnime * 0.2f, 0);
			pVtx[1].tex = D3DXVECTOR2(0.0f + pPlayer->nAnime * 0.2f, 0);
			pVtx[2].tex = D3DXVECTOR2(0.2f + pPlayer->nAnime * 0.2f, 1);
			pVtx[3].tex = D3DXVECTOR2(0.0f + pPlayer->nAnime * 0.2f, 1);
		}
		break;
	case P_DIED://dead
		if (pPlayer->size == D3DXVECTOR2(800, 400))
		{
			if (g_nCntTimeplayer % 10 == 0) pPlayer->nAnime++;
			if (pPlayer->nAnime > 10)
				pPlayer->bAnimeChange = true;
			else
			{
				pVtx[0].tex = D3DXVECTOR2(0.0f + pPlayer->nAnime * 0.1f, 0);
				pVtx[1].tex = D3DXVECTOR2(0.1f + pPlayer->nAnime * 0.1f, 0);
				pVtx[2].tex = D3DXVECTOR2(0.0f + pPlayer->nAnime * 0.1f, 1);
				pVtx[3].tex = D3DXVECTOR2(0.1f + pPlayer->nAnime * 0.1f, 1);
			}
		}
		break;
	default:
		break;
	}
	g_pVtxBuffPlayer->Unlock();

}

void DrawPlayer(void)
{
	LPDIRECT3DDEVICE9 pDevicePy = GetDevice();
	LPDIRECT3DDEVICE9 pDeviceUpKey = GetDevice();
	PLAYER *pPlayer = &g_aPlayer;

	//頂点Data flowの設定
	pDevicePy->SetStreamSource(0, g_pVtxBuffPlayer, 0, sizeof(VERTEX_2D));
	//頂点modの設定
	pDevicePy->SetFVF(FVF_VERTEX_2D);
	//画像の設定
	if (pPlayer->nState != P_HURT && pPlayer->bUse == true)
	{
		pDevicePy->SetTexture(0, g_pTexturePlayer[pPlayer->nState]);
		pDevicePy->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	}
}

PLAYER *GetPlayer(void)
{
	return &g_aPlayer;
}

void UpdatePlayerFade(void)
{
	VERTEX_2D *pVtx;
	PLAYER *pPlayer;
	pPlayer = &g_aPlayer;
	pPlayer->color.a = 0;
	g_pVtxBuffPlayer->Lock(0, 0, (void**)&pVtx, 0);
	pVtx[0].col = pPlayer->color;
	pVtx[1].col = pPlayer->color;
	pVtx[2].col = pPlayer->color;
	pVtx[3].col = pPlayer->color;
	g_pVtxBuffPlayer->Unlock();
}

void ResetPlayer(int nStage)
{
	PLAYER *pPlayer;
	pPlayer = &g_aPlayer;
	POLYGON *pPolygon;
	int nPolyNum = PolyNum(nStage);
	int nCntTimeTrue = 0;
	for (int nCnt = 0; nCnt < SCREEN_WIDTH / 2; nCnt++)
	{
		if (nCntTimeTrue > 1)
		{
			pPlayer->prepos.x -= 100;
			pPlayer->pos = pPlayer->prepos;
			pPlayer->bUse = true;
			pPlayer->color.a = 0;
			pPlayer->nState = P_JUMP;
			break;
		}
		else
		{
			pPlayer->prepos.y = 0;
			pPlayer->prepos.x -= nCnt * 100;
			while (pPlayer->prepos.y < SCREEN_HEIGHT && nCntTimeTrue < 2)
			{
				pPolygon = GetPolyBlock();
				for (int nCnt = 0; nCnt < nPolyNum; nCnt++, pPolygon++)
				{
					if (pPolygon->bBlock == true)
					{
						if (((pPlayer->prepos.x <= pPolygon->pos.x + pPolygon->size.x + pPlayer->size.x / 2) && (pPlayer->prepos.x >= pPolygon->pos.x - pPolygon->size.x - pPlayer->size.x / 2)) && (pPlayer->prepos.y < pPolygon->pos.y) && (pPlayer->prepos.y > pPolygon->pos.y - pPolygon->size.y - pPlayer->size.y))
						{
							nCntTimeTrue++;
							break;
						}
					}
				}
				pPlayer->prepos.y += pPlayer->fGravity;
			}
		}
	}
}

void DrawPlayerDead(void)
{
	VERTEX_2D *pVtx;
	PLAYER *pPlayer;
	g_nCntTimeplayer++;
	pPlayer = &g_aPlayer;
	pPlayer->pos = D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0);
	pPlayer->size = D3DXVECTOR2(800, 300);
	if (g_nCntTimeplayer % 10 == 0)
		pPlayer->nAnime++;
	if (pPlayer->nAnime < 13)
	{
		g_pVtxBuffPlayer->Lock(0, 0, (void**)&pVtx, 0);
		pVtx[0].pos = D3DXVECTOR3(pPlayer->pos.x - pPlayer->size.x, pPlayer->pos.y - pPlayer->size.y, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(pPlayer->pos.x + pPlayer->size.x, pPlayer->pos.y - pPlayer->size.y, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(pPlayer->pos.x - pPlayer->size.x, pPlayer->pos.y + pPlayer->size.y, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(pPlayer->pos.x + pPlayer->size.x, pPlayer->pos.y + pPlayer->size.y, 0.0f);
		pVtx[0].tex = D3DXVECTOR2(0.0f + pPlayer->nAnime * 0.076923f, 0);
		pVtx[1].tex = D3DXVECTOR2(0.076923f + pPlayer->nAnime * 0.076923f, 0);
		pVtx[2].tex = D3DXVECTOR2(0.0f + pPlayer->nAnime * 0.076923f, 1);
		pVtx[3].tex = D3DXVECTOR2(0.076923f + pPlayer->nAnime * 0.076923f, 1);
		g_pVtxBuffPlayer->Unlock();
	}
	else
	{
		pPlayer->bUse = false;
		SetFade(FADE_OUT, MODE_RANK);
		pPlayer->nState = P_IDLE;
		pPlayer->nAnime = 0;
		pPlayer->color.a = 0;
		pPlayer->pos = D3DXVECTOR3(150, 700, 0);
		pPlayer->size = D3DXVECTOR2(60, 60);
	}
}
