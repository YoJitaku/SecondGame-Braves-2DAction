//=============================================================================
//
// [Ranking.cpp]　
// Author : Yo Jitaku
//
//=============================================================================
#define _CRT_SECURE_NO_WARNINGS
#include "Ranking.h"
#include "background.h"
#include "PolyBlock.h"
#include "player.h"
#include "Input.h"
#include "Fade.h"
//create buffer pointer
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffRank = NULL;
//texture pointer
LPDIRECT3DTEXTURE9 g_pTextureRank[30] = {};
//title polygon group
RANK g_aRank[MAX_COMPONENT_RANK];
RESULT g_aResult[RESULT_ROW][MAX_COMPONENT_RESULT];//5 row , 8 number
RANKPLAYER g_aRankPlayer[6];
int g_nCntAnimeRk = 0;
char g_Input;
//name
NAME g_aName[RESULT_ROW][6];
bool bInput;

void ReadFile(RANKPLAYER *pRankPlayer)
{
	FILE *pFile;
	pFile = fopen("Ranking.txt", "r");
	if (pFile != NULL)
	{
		for (int nCount = 0; nCount < 5; nCount++)
		{
			//fgets(&pRankPlayer[nCount].aName[0], 100, pFile);
			fscanf(pFile, "%s", &pRankPlayer[nCount].aName[0]);
			fscanf(pFile, "%d", &pRankPlayer[nCount].nScore);
		}
	}
	fclose(pFile);
}

void InputNewScore(RANKPLAYER *pRankPlayer)
{
	int nTop, nHistory;
	char aTop[256];
	for (int nCountS = 0; nCountS < 5; nCountS++)pRankPlayer[nCountS].nHistory = 0;
	PLAYER *pPlayer;
	pPlayer = GetPlayer();
	pRankPlayer[5].nScore = pPlayer->nScore;//player's score
	pRankPlayer[5].nHistory = 1;//new input mark

	//compositor
	for (int nCount2 = 0; nCount2 < 5; nCount2++)
	{
		nHistory = pRankPlayer[nCount2].nHistory;
		nTop = pRankPlayer[nCount2].nScore;
		strcpy(aTop, pRankPlayer[nCount2].aName);
		for (int nCount3 = (nCount2 + 1); nCount3 < 6; nCount3++)
		{
			if (nTop < pRankPlayer[nCount3].nScore)
			{
				nTop = pRankPlayer[nCount3].nScore;
				strcpy(aTop, pRankPlayer[nCount3].aName);
				nHistory = pRankPlayer[nCount3].nHistory;

				pRankPlayer[nCount3].nScore = pRankPlayer[nCount2].nScore;
				strcpy(pRankPlayer[nCount3].aName, pRankPlayer[nCount2].aName);
				pRankPlayer[nCount3].nHistory = pRankPlayer[nCount2].nHistory;

				pRankPlayer[nCount2].nScore = nTop;
				strcpy(pRankPlayer[nCount2].aName, aTop);
				pRankPlayer[nCount2].nHistory = nHistory;
			}
		}
	}
}

void OutPutRanking(RANKPLAYER *pRankPlayer)
{
	FILE *pFile;
	pFile = fopen("Ranking.txt", "w");
	if (pFile != NULL)
	{
		for (int nCount4 = 0; nCount4 < 5; nCount4++)
		{
			if (pRankPlayer[nCount4].aName != NULL) fprintf(pFile, "%s\n", pRankPlayer[nCount4].aName);
			else fprintf(pFile, "UNNAME\n");
			fprintf(pFile, "%d\n", pRankPlayer[nCount4].nScore);
			fprintf(pFile, "\n");
		}
	}
	fclose(pFile);
	printf("\n");
}

void SetRank(void)
{
	//background
	g_aRank[0].bUse = true;
	g_aRank[0].pos = D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f);
	g_aRank[0].size = D3DXVECTOR2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

	//new
	g_aRank[1].bUse = false;
	g_aRank[1].pos = D3DXVECTOR3(1680, 250, 0.0f);
	g_aRank[1].size = D3DXVECTOR2(120, 70);

	//score
	for (int nCntResultRow = 0; nCntResultRow < RESULT_ROW; nCntResultRow++)
	{
		for (int nCntResultZero = 0; nCntResultZero < MAX_COMPONENT_RESULT; nCntResultZero++)
		{
			g_aResult[nCntResultRow][nCntResultZero].bUse = true;
			g_aResult[nCntResultRow][nCntResultZero].pos.x = 250 + (float)nCntResultZero * 90;
			g_aResult[nCntResultRow][nCntResultZero].pos.y = 300 + (float)nCntResultRow * 120;
			g_aResult[nCntResultRow][nCntResultZero].pos.z = 0;
			g_aResult[nCntResultRow][nCntResultZero].size.x = 40;
			g_aResult[nCntResultRow][nCntResultZero].size.y = 45;
			g_aResult[nCntResultRow][nCntResultZero].nDisplayNum = 0;//表示したいnumber
			g_aResult[nCntResultRow][nCntResultZero].nZeroNum = MAX_COMPONENT_RESULT - 1 - nCntResultZero;//計算するとき　その位の後ろの0の個数
			g_aResult[nCntResultRow][nCntResultZero].nMultipleNum = 0;//除数としての整数1->00000
		}
	}

	for (int nCntrow = 0; nCntrow < RESULT_ROW; nCntrow++)
	{
		for (int nCnt = 0; nCnt < 6; nCnt++)
		{
			g_aName[nCntrow][nCnt].bUse = false;
			g_aName[nCntrow][nCnt].pos.x = 1100 + (float)nCnt * 80;
			g_aName[nCntrow][nCnt].pos.y = 300 + (float)nCntrow * 120;
			g_aName[nCntrow][nCnt].size.x = 40;
			g_aName[nCntrow][nCnt].size.y = 45;
			g_aName[nCntrow][nCnt].nType = 100;
			g_aName[nCntrow][nCnt].NewsIn = false;
		}
	}
}

HRESULT InitRank(void)
{
	LPDIRECT3DDEVICE9 pDeviceRk;
	pDeviceRk = GetDevice();
	VERTEX_2D *pVtx;
	bInput = false;
	ReadFile(&g_aRankPlayer[0]);
	InputNewScore(&g_aRankPlayer[0]);
	SetRank();//init

	D3DXCreateTextureFromFile(pDeviceRk, "data/TEXTURE/Ranking.png", &g_pTextureRank[0]);
	D3DXCreateTextureFromFile(pDeviceRk, "data/TEXTURE/RankNew.png", &g_pTextureRank[1]);
	D3DXCreateTextureFromFile(pDeviceRk, "data/TEXTURE/Score.png", &g_pTextureRank[2]);
	D3DXCreateTextureFromFile(pDeviceRk, "data/TEXTURE/cursor.png", &g_pTextureRank[3]);
	D3DXCreateTextureFromFile(pDeviceRk, "data/TEXTURE/az/A.png", &g_pTextureRank[4]);
	D3DXCreateTextureFromFile(pDeviceRk, "data/TEXTURE/az/B.png", &g_pTextureRank[5]);
	D3DXCreateTextureFromFile(pDeviceRk, "data/TEXTURE/az/C.png", &g_pTextureRank[6]);
	D3DXCreateTextureFromFile(pDeviceRk, "data/TEXTURE/az/D.png", &g_pTextureRank[7]);
	D3DXCreateTextureFromFile(pDeviceRk, "data/TEXTURE/az/E.png", &g_pTextureRank[8]);
	D3DXCreateTextureFromFile(pDeviceRk, "data/TEXTURE/az/F.png", &g_pTextureRank[9]);
	D3DXCreateTextureFromFile(pDeviceRk, "data/TEXTURE/az/G.png", &g_pTextureRank[10]);
	D3DXCreateTextureFromFile(pDeviceRk, "data/TEXTURE/az/H.png", &g_pTextureRank[11]);
	D3DXCreateTextureFromFile(pDeviceRk, "data/TEXTURE/az/I.png", &g_pTextureRank[12]);
	D3DXCreateTextureFromFile(pDeviceRk, "data/TEXTURE/az/J.png", &g_pTextureRank[13]);
	D3DXCreateTextureFromFile(pDeviceRk, "data/TEXTURE/az/K.png", &g_pTextureRank[14]);
	D3DXCreateTextureFromFile(pDeviceRk, "data/TEXTURE/az/L.png", &g_pTextureRank[15]);
	D3DXCreateTextureFromFile(pDeviceRk, "data/TEXTURE/az/M.png", &g_pTextureRank[16]);
	D3DXCreateTextureFromFile(pDeviceRk, "data/TEXTURE/az/N.png", &g_pTextureRank[17]);
	D3DXCreateTextureFromFile(pDeviceRk, "data/TEXTURE/az/O.png", &g_pTextureRank[18]);
	D3DXCreateTextureFromFile(pDeviceRk, "data/TEXTURE/az/P.png", &g_pTextureRank[19]);
	D3DXCreateTextureFromFile(pDeviceRk, "data/TEXTURE/az/Q.png", &g_pTextureRank[20]);
	D3DXCreateTextureFromFile(pDeviceRk, "data/TEXTURE/az/R.png", &g_pTextureRank[21]);
	D3DXCreateTextureFromFile(pDeviceRk, "data/TEXTURE/az/S.png", &g_pTextureRank[22]);
	D3DXCreateTextureFromFile(pDeviceRk, "data/TEXTURE/az/T.png", &g_pTextureRank[23]);
	D3DXCreateTextureFromFile(pDeviceRk, "data/TEXTURE/az/U.png", &g_pTextureRank[24]);
	D3DXCreateTextureFromFile(pDeviceRk, "data/TEXTURE/az/V.png", &g_pTextureRank[25]);
	D3DXCreateTextureFromFile(pDeviceRk, "data/TEXTURE/az/W.png", &g_pTextureRank[26]);
	D3DXCreateTextureFromFile(pDeviceRk, "data/TEXTURE/az/X.png", &g_pTextureRank[27]);
	D3DXCreateTextureFromFile(pDeviceRk, "data/TEXTURE/az/Y.png", &g_pTextureRank[28]);
	D3DXCreateTextureFromFile(pDeviceRk, "data/TEXTURE/az/Z.png", &g_pTextureRank[29]);

	//create vertex buffer
	if (FAILED(pDeviceRk->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * (MAX_COMPONENT_RANK + RESULT_ROW * MAX_COMPONENT_RESULT + 30), D3DUSAGE_WRITEONLY, FVF_VERTEX_2D, D3DPOOL_MANAGED, &g_pVtxBuffRank, NULL))) return E_FAIL;

	g_pVtxBuffRank->Lock(0, 0, (void**)&pVtx, 0);
	for (int nCntRank = 0; nCntRank < MAX_COMPONENT_RANK; nCntRank++, pVtx += 4)
	{
		pVtx[0].pos = D3DXVECTOR3(g_aRank[nCntRank].pos.x - g_aRank[nCntRank].size.x, g_aRank[nCntRank].pos.y - g_aRank[nCntRank].size.y, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(g_aRank[nCntRank].pos.x + g_aRank[nCntRank].size.x, g_aRank[nCntRank].pos.y - g_aRank[nCntRank].size.y, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(g_aRank[nCntRank].pos.x - g_aRank[nCntRank].size.x, g_aRank[nCntRank].pos.y + g_aRank[nCntRank].size.y, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(g_aRank[nCntRank].pos.x + g_aRank[nCntRank].size.x, g_aRank[nCntRank].pos.y + g_aRank[nCntRank].size.y, 0.0f);
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
	}
	//score number
	for (int nCntResultRow = 0; nCntResultRow < RESULT_ROW; nCntResultRow++)
	{
		for (int nCntResultZero = 0; nCntResultZero < MAX_COMPONENT_RESULT; nCntResultZero++, pVtx += 4)
		{
			pVtx[0].pos = D3DXVECTOR3(g_aResult[nCntResultRow][nCntResultZero].pos.x - g_aResult[nCntResultRow][nCntResultZero].size.x, g_aResult[nCntResultRow][nCntResultZero].pos.y - g_aResult[nCntResultRow][nCntResultZero].size.y, 0.0f);
			pVtx[1].pos = D3DXVECTOR3(g_aResult[nCntResultRow][nCntResultZero].pos.x + g_aResult[nCntResultRow][nCntResultZero].size.x, g_aResult[nCntResultRow][nCntResultZero].pos.y - g_aResult[nCntResultRow][nCntResultZero].size.y, 0.0f);
			pVtx[2].pos = D3DXVECTOR3(g_aResult[nCntResultRow][nCntResultZero].pos.x - g_aResult[nCntResultRow][nCntResultZero].size.x, g_aResult[nCntResultRow][nCntResultZero].pos.y + g_aResult[nCntResultRow][nCntResultZero].size.y, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(g_aResult[nCntResultRow][nCntResultZero].pos.x + g_aResult[nCntResultRow][nCntResultZero].size.x, g_aResult[nCntResultRow][nCntResultZero].pos.y + g_aResult[nCntResultRow][nCntResultZero].size.y, 0.0f);
			pVtx[0].rhw = 1.0f;
			pVtx[1].rhw = 1.0f;
			pVtx[2].rhw = 1.0f;
			pVtx[3].rhw = 1.0f;
			pVtx[0].col = D3DCOLOR_RGBA(255, 255, 255, 255);
			pVtx[1].col = D3DCOLOR_RGBA(255, 255, 255, 255);
			pVtx[2].col = D3DCOLOR_RGBA(255, 255, 255, 255);
			pVtx[3].col = D3DCOLOR_RGBA(255, 255, 255, 255);
			pVtx[0].tex = D3DXVECTOR2(0.0, 0.0);
			pVtx[1].tex = D3DXVECTOR2(((float)0.1), 0.0);
			pVtx[2].tex = D3DXVECTOR2(0.0, 1.0);
			pVtx[3].tex = D3DXVECTOR2(((float)0.1), 1.0);
		}
	}
	//score name
	for (int nCntrow = 0; nCntrow < RESULT_ROW; nCntrow++)
	{
		for (int nCnt = 0; nCnt < 6; nCnt++, pVtx += 4)
		{
			pVtx[0].pos = D3DXVECTOR3(g_aName[nCntrow][nCnt].pos.x - g_aName[nCntrow][nCnt].size.x, g_aName[nCntrow][nCnt].pos.y - g_aName[nCntrow][nCnt].size.y, 0.0f);
			pVtx[1].pos = D3DXVECTOR3(g_aName[nCntrow][nCnt].pos.x + g_aName[nCntrow][nCnt].size.x, g_aName[nCntrow][nCnt].pos.y - g_aName[nCntrow][nCnt].size.y, 0.0f);
			pVtx[2].pos = D3DXVECTOR3(g_aName[nCntrow][nCnt].pos.x - g_aName[nCntrow][nCnt].size.x, g_aName[nCntrow][nCnt].pos.y + g_aName[nCntrow][nCnt].size.y, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(g_aName[nCntrow][nCnt].pos.x + g_aName[nCntrow][nCnt].size.x, g_aName[nCntrow][nCnt].pos.y + g_aName[nCntrow][nCnt].size.y, 0.0f);
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
		}
	}

	g_pVtxBuffRank->Unlock();
	return S_OK;
}

//release memory
void UninitRank(void)
{
	if (g_pVtxBuffRank != NULL)
	{
		g_pVtxBuffRank->Release();
		g_pVtxBuffRank = NULL;
	}
	for (int nCntTitle = 0; nCntTitle < MAX_COMPONENT_RANK; nCntTitle++)
	{
		if (g_pTextureRank[nCntTitle] != NULL)
		{
			g_pTextureRank[nCntTitle]->Release();
			g_pTextureRank[nCntTitle] = NULL;
		}
	}
}


void UpdateRank(void)
{
	VERTEX_2D *pVtx;
	PLAYER *pPlayer;
	pPlayer = GetPlayer();
	int nnscore = pPlayer->nScore;
	RANKPLAYER *pRankPlayer;
	pRankPlayer = &g_aRankPlayer[4];

	if ((GetkeyboardTrigger(DIK_RETURN) == true))
	{
		SetFade(FADE_OUT, MODE_MENU);
		UpdatePlayerFade();
	}
	pRankPlayer = &g_aRankPlayer[0];
	g_nCntAnimeRk++;
	//RESULT *pResult;
	int nSigma = 0;
	for (int nCntRow = 0; nCntRow < RESULT_ROW; nCntRow++, pRankPlayer++)
	{
		for (int nCnt = 0; nCnt < MAX_COMPONENT_RESULT; nCnt++)
		{
			g_aResult[nCntRow][nCnt].nMultipleNum = (unsigned int)pow(10.0, g_aResult[nCntRow][nCnt].nZeroNum);
			g_aResult[nCntRow][nCnt].nDisplayNum = pRankPlayer->nScore % (g_aResult[nCntRow][nCnt].nMultipleNum * 10) / g_aResult[nCntRow][nCnt].nMultipleNum;
			SetTextureResult(nSigma + nCnt, g_aResult[nCntRow][nCnt].nDisplayNum);
		}
		if (pRankPlayer->nHistory == 1)
		{
			g_aRank[1].bUse = true;
			g_aRank[1].pos.y = 300 + (float)(nCntRow * 120);
			g_pVtxBuffRank->Lock(0, 0, (void**)&pVtx, 0);
			pVtx[4].pos = D3DXVECTOR3(g_aRank[1].pos.x - g_aRank[1].size.x, g_aRank[1].pos.y - g_aRank[1].size.y, 0.0f);
			pVtx[5].pos = D3DXVECTOR3(g_aRank[1].pos.x + g_aRank[1].size.x, g_aRank[1].pos.y - g_aRank[1].size.y, 0.0f);
			pVtx[6].pos = D3DXVECTOR3(g_aRank[1].pos.x - g_aRank[1].size.x, g_aRank[1].pos.y + g_aRank[1].size.y, 0.0f);
			pVtx[7].pos = D3DXVECTOR3(g_aRank[1].pos.x + g_aRank[1].size.x, g_aRank[1].pos.y + g_aRank[1].size.y, 0.0f);
			g_pVtxBuffRank->Unlock();
		}
		nSigma += MAX_COMPONENT_RESULT;
	}

	pRankPlayer = &g_aRankPlayer[0];
	g_pVtxBuffRank->Lock(0, 0, (void**)&pVtx, 0);
	for (int nCntrow = 0; nCntrow < RESULT_ROW; nCntrow++, pRankPlayer++)
	{
		for (int nCnt = 0; nCnt < 6; nCnt++, pVtx += 4)
		{
			if (pRankPlayer->nHistory == 1 && g_aName[nCntrow][nCnt].NewsIn == false)
			{
				g_aName[nCntrow][nCnt].nType = 3;

				if (g_nCntAnimeRk % 50 == 0)
				{
					g_aName[nCntrow][nCnt].bUse = !g_aName[nCntrow][nCnt].bUse;
					if (bInput == true) g_aName[nCntrow][nCnt].bUse = false;
				}
				if (GetkeyboardTrigger(DIK_A) == true)
				{
					g_Input = 'A';
					g_aName[nCntrow][nCnt].nType = 4;
					g_aName[nCntrow][nCnt].NewsIn = true;
					if (nCnt == 0) strcpy(pRankPlayer->aName, &g_Input);
					else strcat(pRankPlayer->aName, &g_Input);
					if (nCnt == 5)
					{
						//PlaySound(SOUND_LABEL_SE_DECIDE);
						bInput = true;
						break;
					}
					break;
				}
				else if (GetkeyboardTrigger(DIK_B) == true)
				{
					g_Input = 'B';
					g_aName[nCntrow][nCnt].nType = 5;
					g_aName[nCntrow][nCnt].NewsIn = true;
					if (nCnt == 0) strcpy(pRankPlayer->aName, &g_Input);
					else strcat(pRankPlayer->aName, &g_Input);
					if (nCnt == 5)
					{
						//PlaySound(SOUND_LABEL_SE_DECIDE);
						bInput = true;
						break;
					}
					break;
				}
				else if (GetkeyboardTrigger(DIK_C) == true)
				{
					g_Input = 'C';
					g_aName[nCntrow][nCnt].nType = 6;
					g_aName[nCntrow][nCnt].NewsIn = true;
					if (nCnt == 0) strcpy(pRankPlayer->aName, &g_Input);
					else strcat(pRankPlayer->aName, &g_Input);
					if (nCnt == 5)
					{
						//PlaySound(SOUND_LABEL_SE_DECIDE);
						bInput = true;
						break;
					}
					break;
				}
				else if (GetkeyboardTrigger(DIK_D) == true)
				{
					g_Input = 'D';
					g_aName[nCntrow][nCnt].nType = 7;
					g_aName[nCntrow][nCnt].NewsIn = true;
					if (nCnt == 0) strcpy(pRankPlayer->aName, &g_Input);
					else strcat(pRankPlayer->aName, &g_Input);
					if (nCnt == 5)
					{
						//PlaySound(SOUND_LABEL_SE_DECIDE);
						bInput = true;
						break;
					}
					break;
				}
				else if (GetkeyboardTrigger(DIK_E) == true)
				{
					g_Input = 'E';
					g_aName[nCntrow][nCnt].nType = 8;
					g_aName[nCntrow][nCnt].NewsIn = true;
					if (nCnt == 0) strcpy(pRankPlayer->aName, &g_Input);
					else strcat(pRankPlayer->aName, &g_Input);
					if (nCnt == 5)
					{
						//PlaySound(SOUND_LABEL_SE_DECIDE);
						bInput = true;
						break;
					}
					break;
				}
				else if (GetkeyboardTrigger(DIK_F) == true)
				{
					g_Input = 'F';
					g_aName[nCntrow][nCnt].nType = 9;
					g_aName[nCntrow][nCnt].NewsIn = true;
					if (nCnt == 0) strcpy(pRankPlayer->aName, &g_Input);
					else strcat(pRankPlayer->aName, &g_Input);
					if (nCnt == 5)
					{
						//PlaySound(SOUND_LABEL_SE_DECIDE);
						bInput = true;
						break;
					}
					break;
				}
				else if (GetkeyboardTrigger(DIK_G) == true)
				{
					g_Input = 'G';
					g_aName[nCntrow][nCnt].nType = 10;
					g_aName[nCntrow][nCnt].NewsIn = true;
					if (nCnt == 0) strcpy(pRankPlayer->aName, &g_Input);
					else strcat(pRankPlayer->aName, &g_Input);
					if (nCnt == 5)
					{
						//PlaySound(SOUND_LABEL_SE_DECIDE);
						bInput = true;
						break;
					}
					break;
				}
				else if (GetkeyboardTrigger(DIK_H) == true)
				{
					g_Input = 'H';
					g_aName[nCntrow][nCnt].nType = 11;
					g_aName[nCntrow][nCnt].NewsIn = true;
					if (nCnt == 0) strcpy(pRankPlayer->aName, &g_Input);
					else strcat(pRankPlayer->aName, &g_Input);
					if (nCnt == 5)
					{
						//PlaySound(SOUND_LABEL_SE_DECIDE);
						bInput = true;
						break;
					}
					break;
				}
				else if (GetkeyboardTrigger(DIK_I) == true)
				{
					g_Input = 'I';
					g_aName[nCntrow][nCnt].nType = 12;
					g_aName[nCntrow][nCnt].NewsIn = true;
					if (nCnt == 0) strcpy(pRankPlayer->aName, &g_Input);
					else strcat(pRankPlayer->aName, &g_Input);
					if (nCnt == 5)
					{
						//PlaySound(SOUND_LABEL_SE_DECIDE);
						bInput = true;
						break;
					}
					break;
				}
				else if (GetkeyboardTrigger(DIK_J) == true)
				{
					g_Input = 'J';
					g_aName[nCntrow][nCnt].nType = 13;
					g_aName[nCntrow][nCnt].NewsIn = true;
					if (nCnt == 0) strcpy(pRankPlayer->aName, &g_Input);
					else strcat(pRankPlayer->aName, &g_Input);
					if (nCnt == 5)
					{
						//PlaySound(SOUND_LABEL_SE_DECIDE);
						bInput = true;
						break;
					}
					break;
				}
				else if (GetkeyboardTrigger(DIK_K) == true)
				{
					g_Input = 'K';
					g_aName[nCntrow][nCnt].nType = 14;
					g_aName[nCntrow][nCnt].NewsIn = true;
					if (nCnt == 0) strcpy(pRankPlayer->aName, &g_Input);
					else strcat(pRankPlayer->aName, &g_Input);
					if (nCnt == 5)
					{
						//PlaySound(SOUND_LABEL_SE_DECIDE);
						bInput = true;
						break;
					}
					break;
				}
				else if (GetkeyboardTrigger(DIK_L) == true)
				{
					g_Input = 'L';
					g_aName[nCntrow][nCnt].nType = 15;
					g_aName[nCntrow][nCnt].NewsIn = true;
					if (nCnt == 0) strcpy(pRankPlayer->aName, &g_Input);
					else strcat(pRankPlayer->aName, &g_Input);
					if (nCnt == 5)
					{
						//PlaySound(SOUND_LABEL_SE_DECIDE);
						bInput = true;
						break;
					}
					break;
				}
				else if (GetkeyboardTrigger(DIK_M) == true)
				{
					g_Input = 'M';
					g_aName[nCntrow][nCnt].nType = 16;
					g_aName[nCntrow][nCnt].NewsIn = true;
					if (nCnt == 0) strcpy(pRankPlayer->aName, &g_Input);
					else strcat(pRankPlayer->aName, &g_Input);
					if (nCnt == 5)
					{
						//PlaySound(SOUND_LABEL_SE_DECIDE);
						bInput = true;
						break;
					}
					break;
				}
				else if (GetkeyboardTrigger(DIK_N) == true)
				{
					g_Input = 'N';
					g_aName[nCntrow][nCnt].nType = 17;
					g_aName[nCntrow][nCnt].NewsIn = true;
					if (nCnt == 0) strcpy(pRankPlayer->aName, &g_Input);
					else strcat(pRankPlayer->aName, &g_Input);
					if (nCnt == 5)
					{
						//PlaySound(SOUND_LABEL_SE_DECIDE);
						bInput = true;
						break;
					}
					break;
				}
				else if (GetkeyboardTrigger(DIK_O) == true)
				{
					g_Input = 'O';
					g_aName[nCntrow][nCnt].nType = 18;
					g_aName[nCntrow][nCnt].NewsIn = true;
					if (nCnt == 0) strcpy(pRankPlayer->aName, &g_Input);
					else strcat(pRankPlayer->aName, &g_Input);
					if (nCnt == 5)
					{
					//	PlaySound(SOUND_LABEL_SE_DECIDE);
						bInput = true;
						break;
					}
					break;
				}
				else if (GetkeyboardTrigger(DIK_P) == true)
				{
					g_Input = 'P';
					g_aName[nCntrow][nCnt].nType = 19;
					g_aName[nCntrow][nCnt].NewsIn = true;
					if (nCnt == 0) strcpy(pRankPlayer->aName, &g_Input);
					else strcat(pRankPlayer->aName, &g_Input);
					if (nCnt == 5)
					{
					//	PlaySound(SOUND_LABEL_SE_DECIDE);
						bInput = true;
						break;
					}
					break;
				}
				else if (GetkeyboardTrigger(DIK_Q) == true)
				{
					g_Input = 'Q';
					g_aName[nCntrow][nCnt].nType = 20;
					g_aName[nCntrow][nCnt].NewsIn = true;
					if (nCnt == 0) strcpy(pRankPlayer->aName, &g_Input);
					else strcat(pRankPlayer->aName, &g_Input);
					if (nCnt == 5)
					{
						//PlaySound(SOUND_LABEL_SE_DECIDE);
						bInput = true;
						break;
					}
					break;
				}
				else if (GetkeyboardTrigger(DIK_R) == true)
				{
					g_Input = 'R';
					g_aName[nCntrow][nCnt].nType = 21;
					g_aName[nCntrow][nCnt].NewsIn = true;
					if (nCnt == 0) strcpy(pRankPlayer->aName, &g_Input);
					else strcat(pRankPlayer->aName, &g_Input);
					if (nCnt == 5)
					{
						//PlaySound(SOUND_LABEL_SE_DECIDE);
						bInput = true;
						break;
					}
					break;
				}
				else if (GetkeyboardTrigger(DIK_S) == true)
				{
					g_Input = 'S';
					g_aName[nCntrow][nCnt].nType = 22;
					g_aName[nCntrow][nCnt].NewsIn = true;
					if (nCnt == 0) strcpy(pRankPlayer->aName, &g_Input);
					else strcat(pRankPlayer->aName, &g_Input);
					if (nCnt == 5)
					{
						//PlaySound(SOUND_LABEL_SE_DECIDE);
						bInput = true;
						break;
					}
					break;
				}
				else if (GetkeyboardTrigger(DIK_T) == true)
				{
					g_Input = 'T';
					g_aName[nCntrow][nCnt].nType = 23;
					g_aName[nCntrow][nCnt].NewsIn = true;
					if (nCnt == 0) strcpy(pRankPlayer->aName, &g_Input);
					else strcat(pRankPlayer->aName, &g_Input);
					if (nCnt == 5)
					{
					//	PlaySound(SOUND_LABEL_SE_DECIDE);
						bInput = true;
						break;
					}
					break;
				}
				else if (GetkeyboardTrigger(DIK_U) == true)
				{
					g_Input = 'U';
					g_aName[nCntrow][nCnt].nType = 24;
					g_aName[nCntrow][nCnt].NewsIn = true;
					if (nCnt == 0) strcpy(pRankPlayer->aName, &g_Input);
					else strcat(pRankPlayer->aName, &g_Input);
					if (nCnt == 5)
					{
					//	PlaySound(SOUND_LABEL_SE_DECIDE);
						bInput = true;
						break;
					}
					break;
				}
				else if (GetkeyboardTrigger(DIK_V) == true)
				{
					g_Input = 'V';
					g_aName[nCntrow][nCnt].nType = 25;
					g_aName[nCntrow][nCnt].NewsIn = true;
					if (nCnt == 0) strcpy(pRankPlayer->aName, &g_Input);
					else strcat(pRankPlayer->aName, &g_Input);
					if (nCnt == 5)
					{
					//	PlaySound(SOUND_LABEL_SE_DECIDE);
						bInput = true;
						break;
					}
					break;
				}
				else if (GetkeyboardTrigger(DIK_W) == true)
				{
					g_Input = 'W';
					g_aName[nCntrow][nCnt].nType = 26;
					g_aName[nCntrow][nCnt].NewsIn = true;
					if (nCnt == 0) strcpy(pRankPlayer->aName, &g_Input);
					else strcat(pRankPlayer->aName, &g_Input);
					if (nCnt == 5)
					{
					//	PlaySound(SOUND_LABEL_SE_DECIDE);
						bInput = true;
						break;
					}
					break;
				}
				else if (GetkeyboardTrigger(DIK_X) == true)
				{
					g_Input = 'X';
					g_aName[nCntrow][nCnt].nType = 27;
					g_aName[nCntrow][nCnt].NewsIn = true;
					if (nCnt == 0) strcpy(pRankPlayer->aName, &g_Input);
					else strcat(pRankPlayer->aName, &g_Input);
					if (nCnt == 5)
					{
					//	PlaySound(SOUND_LABEL_SE_DECIDE);
						bInput = true;
						break;
					}
					break;
				}
				else if (GetkeyboardTrigger(DIK_Y) == true)
				{
					g_Input = 'Y';
					g_aName[nCntrow][nCnt].nType = 28;
					g_aName[nCntrow][nCnt].NewsIn = true;
					if (nCnt == 0) strcpy(pRankPlayer->aName, &g_Input);
					else strcat(pRankPlayer->aName, &g_Input);
					if (nCnt == 5)
					{
						//PlaySound(SOUND_LABEL_SE_DECIDE);
						bInput = true;
						break;
					}
					break;
				}
				else if (GetkeyboardTrigger(DIK_Z) == true)
				{
					g_Input = 'Z';
					g_aName[nCntrow][nCnt].nType = 29;
					g_aName[nCntrow][nCnt].NewsIn = true;
					if (nCnt == 0) strcpy(pRankPlayer->aName, &g_Input);
					else strcat(pRankPlayer->aName, &g_Input);
					if (nCnt == 5)
					{
					//	PlaySound(SOUND_LABEL_SE_DECIDE);
						bInput = true;
						break;
					}
					break;
				}

				if (GetkeyboardTrigger(DIK_BACKSPACE) == true)
				{
					for (int nCnt = 0; nCnt < 6; nCnt++)
					{
						bInput = false;
						g_aName[nCntrow][nCnt].bUse = true;
						g_aName[nCntrow][nCnt].nType = 3;
						g_aName[nCntrow][nCnt].NewsIn = false;
					}
				}
				if (GetkeyboardTrigger(DIK_F1) == true)
				{
					g_Input = '1';
					if (nCnt == 0) strcpy(pRankPlayer->aName, &g_Input);
					else strcat(pRankPlayer->aName, &g_Input);
					g_aName[nCntrow][nCnt].NewsIn = true;
					bInput = true;
				//	PlaySound(SOUND_LABEL_SE_DECIDE);
					break;
					//}
				}
			}
			else
			{
				g_aName[nCntrow][nCnt].bUse = true;
				if (g_aRankPlayer[nCntrow].aName[nCnt] != NULL)
				{
					switch (g_aRankPlayer[nCntrow].aName[nCnt])
					{
					case 'A':
						g_aName[nCntrow][nCnt].nType = 4;
						break;
					case 'B':
						g_aName[nCntrow][nCnt].nType = 5;
						break;
					case 'C':
						g_aName[nCntrow][nCnt].nType = 6;
						break;
					case 'D':
						g_aName[nCntrow][nCnt].nType = 7;
						break;
					case 'E':
						g_aName[nCntrow][nCnt].nType = 8;
						break;
					case 'F':
						g_aName[nCntrow][nCnt].nType = 9;
						break;
					case 'G':
						g_aName[nCntrow][nCnt].nType = 10;
						break;
					case 'H':
						g_aName[nCntrow][nCnt].nType = 11;
						break;
					case 'I':
						g_aName[nCntrow][nCnt].nType = 12;
						break;
					case 'J':
						g_aName[nCntrow][nCnt].nType = 13;
						break;
					case 'K':
						g_aName[nCntrow][nCnt].nType = 14;
						break;
					case 'L':
						g_aName[nCntrow][nCnt].nType = 15;
						break;
					case 'M':
						g_aName[nCntrow][nCnt].nType = 16;
						break;
					case 'N':
						g_aName[nCntrow][nCnt].nType = 17;
						break;
					case 'O':
						g_aName[nCntrow][nCnt].nType = 18;
						break;
					case 'P':
						g_aName[nCntrow][nCnt].nType = 19;
						break;
					case 'Q':
						g_aName[nCntrow][nCnt].nType = 20;
						break;
					case 'R':
						g_aName[nCntrow][nCnt].nType = 21;
						break;
					case 'S':
						g_aName[nCntrow][nCnt].nType = 22;
						break;
					case 'T':
						g_aName[nCntrow][nCnt].nType = 23;
						break;
					case 'U':
						g_aName[nCntrow][nCnt].nType = 24;
						break;
					case 'V':
						g_aName[nCntrow][nCnt].nType = 25;
						break;
					case 'W':
						g_aName[nCntrow][nCnt].nType = 26;
						break;
					case 'X':
						g_aName[nCntrow][nCnt].nType = 27;
						break;
					case 'Y':
						g_aName[nCntrow][nCnt].nType = 28;
						break;
					case 'Z':
						g_aName[nCntrow][nCnt].nType = 29;
						break;
					default:
						g_aName[nCntrow][nCnt].bUse = false;
						break;
					}
				}
				else
				{
					g_aName[nCntrow][nCnt].bUse = false;
					if (nCnt != 5) 
					{
						nCnt++;
						g_aName[nCntrow][nCnt].bUse = false;
						if (nCnt != 5)
						{
							nCnt++;
							g_aName[nCntrow][nCnt].bUse = false;
							if (nCnt != 5)
							{
								nCnt++;
								g_aName[nCntrow][nCnt].bUse = false;
								if (nCnt != 5)
								{
									nCnt++;
									g_aName[nCntrow][nCnt].bUse = false;
								}
							}
						}
					}
				}
			}
			pVtx[168].tex = D3DXVECTOR2(0.0, 0.0);
			pVtx[169].tex = D3DXVECTOR2(1.0, 0.0);
			pVtx[170].tex = D3DXVECTOR2(0.0, 1.0);
			pVtx[171].tex = D3DXVECTOR2(1.0, 1.0);
		}

	}
	g_pVtxBuffRank->Unlock();
}

void DrawRank(void)
{
	LPDIRECT3DDEVICE9 pDeviceRk;
	pDeviceRk = GetDevice();
	RANK *pRank;
	pRank = &g_aRank[0];

	pDeviceRk->SetStreamSource(0, g_pVtxBuffRank, 0, sizeof(VERTEX_2D));
	pDeviceRk->SetFVF(FVF_VERTEX_2D);

	for (int nCntRk = 0; nCntRk < MAX_COMPONENT_RANK; nCntRk++, pRank++)
	{
		if (pRank->bUse == true)
		{
			pDeviceRk->SetTexture(0, g_pTextureRank[nCntRk]);
			pDeviceRk->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntRk * 4, 2);
		}
	}

	int nSigma = 0;
	for (int nCntResultRow = 1; nCntResultRow <= RESULT_ROW; nCntResultRow++)
	{
		pDeviceRk->SetTexture(0, g_pTextureRank[2]);
		for (int nCntResultZero = 1; nCntResultZero <= MAX_COMPONENT_RESULT; nCntResultZero++)
		{
			pDeviceRk->DrawPrimitive(D3DPT_TRIANGLESTRIP, (MAX_COMPONENT_RANK - 1) * 4 + nSigma + nCntResultZero * 4, 2);//描画(1,ploygonの種類　2,始める頂点　3,polygonの数)
		}
		nSigma = nSigma + MAX_COMPONENT_RESULT * 4;
	}

	nSigma = 0;
	for (int nCntResultRow = 0; nCntResultRow < RESULT_ROW; nCntResultRow++)
	{
		for (int nCntResultZero = 0; nCntResultZero < 6; nCntResultZero++)
		{
			if (g_aName[nCntResultRow][nCntResultZero].bUse == true)
			{
				pDeviceRk->SetTexture(0, g_pTextureRank[g_aName[nCntResultRow][nCntResultZero].nType]);
				pDeviceRk->DrawPrimitive(D3DPT_TRIANGLESTRIP, 168 + nSigma + nCntResultZero * 4, 2);
			}
		}
		nSigma = nSigma + 6 * 4;
	}
	OutPutRanking(&g_aRankPlayer[0]);
}

void SetTextureResult(int nIdx, int nDisplayNumber)
{
	VERTEX_2D *pVtx;
	g_pVtxBuffRank->Lock(0, 0, (void**)&pVtx, 0);
	pVtx += (nIdx * 4);
	pVtx[8].tex = D3DXVECTOR2((float)(nDisplayNumber * 0.1), 0.0);
	pVtx[9].tex = D3DXVECTOR2((float)(nDisplayNumber * 0.1 + 0.1), 0.0);
	pVtx[10].tex = D3DXVECTOR2((float)(nDisplayNumber * 0.1), 1.0);
	pVtx[11].tex = D3DXVECTOR2((float)(nDisplayNumber * 0.1 + 0.1), 1.0);
	g_pVtxBuffRank->Unlock();
}

char InputName(void)
{
	return g_Input;
}