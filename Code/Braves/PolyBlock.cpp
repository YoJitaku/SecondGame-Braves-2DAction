//=============================================================================
//
// [PolyBlock.cpp]　 
// Author : Yo Jitaku
//
//=============================================================================
#define _CRT_SECURE_NO_WARNINGS
#include "PolyBlock.h"
#include "Player.h"
#include "background.h"
#include "Input.h"
#include "Fade.h"
#include "enemy.h"

LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffBlock = NULL;
LPDIRECT3DTEXTURE9 g_pTextureBlock[BLOCK_TEXTURE] = {};
POLYGON *pPolygon;
int nCntTime, g_aPolygon[6] = {2, 39, 1, 421, 441, 1};//各ステージpolygonの数　title menu47 tutorial stage1 stage2 rank
int g_nCntRow, g_nCntColumn;//行列計算用
int g_aMapData[20][500];//csv file読み込た配置情報
bool g_bAllMove;//スクロール移動
D3DXVECTOR2 g_AllMove;//スクロール移動速度
int g_aDoorNum[2] = { 0 , 0 };//doorの番号を保存する　スクロール判定用
//fileを選択
void ReadFile(int nStage)
{
	FILE *pFile;
	switch (nStage)
	{
	case 0://Title　fileなし
		//ゲーム名
		pPolygon[0].pos = D3DXVECTOR3(SCREEN_WIDTH / 2, -200, 0);
		pPolygon[0].size = D3DXVECTOR2(600, 300);
		pPolygon[0].move = D3DXVECTOR3(0, 10, 0);
		pPolygon[0].nMoveType = 0;
		pPolygon[0].nAnime = 0;
		pPolygon[0].nTextureID = 0;
		pPolygon[0].bBlock = false;
		pPolygon[0].bAnime = false;
		pPolygon[0].bUse = true;

		//start button
		pPolygon[1].pos = D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 200, 0);
		pPolygon[1].size = D3DXVECTOR2(150, 30);
		pPolygon[1].move = D3DXVECTOR3(0, 0, 0);
		pPolygon[1].nMoveType = 0;
		pPolygon[1].nAnime = 0;
		pPolygon[1].nTextureID = 1;
		pPolygon[1].bBlock = false;
		pPolygon[1].bAnime = false;
		pPolygon[1].bUse = false;
		break;

	case 1://Menu
		pFile = fopen("MenuPolygon.csv", "r");
		LOOPTOREAD(pFile);
		fclose(pFile);
		SetPolygon(nStage);
		break;

	case 2://tutorial fileなし
		pPolygon[0].bUse = true;
		pPolygon[0].nTextureID = 0;
		pPolygon[0].pos = D3DXVECTOR3(SCREEN_WIDTH / 2, 150, 0);
		pPolygon[0].size = D3DXVECTOR2(400, 150);
		break;

	case 3://stage1
		pFile = fopen("Stage1Polygon.csv", "r");
		LOOPTOREAD(pFile);
		fclose(pFile);
		SetPolygon(nStage);
		break;

	case 4://stage2
		pFile = fopen("Stage2Polygon.csv", "r");
		LOOPTOREAD(pFile);
		fclose(pFile);
		SetPolygon(nStage);
		break;

	case 5://ranking fileなし
		pPolygon[0].bUse = true;
		pPolygon[0].nTextureID = 0;
		pPolygon[0].pos = D3DXVECTOR3(SCREEN_WIDTH / 2, 150, 0);
		pPolygon[0].size = D3DXVECTOR2(400, 150);
		break;

	default:
		break;
	}
}

//fileを読み込む
void LOOPTOREAD(FILE *pFile)
{
	char aline[1000];//行保存
	while (fgets(aline, 1000, pFile) != NULL)
	{
		g_nCntColumn = 0;//行開始
		char *pToken = strtok(aline, ",");
		while (pToken != NULL)//行最後
		{
			g_aMapData[g_nCntRow][g_nCntColumn] = (int)atoi(pToken);//char -> int
			pToken = strtok(NULL, ",");
			g_nCntColumn++;
		}
		g_nCntRow++;
	}
}

//読み込んだdataの配置情報
void SetPolygon(int nStage)
{
	srand((int)time(0));//時間seed
	int nCntPoly = 0;//描画polygon数count
	int nCntDoor = 1;//doorのpolygonに番号を付ける
	int nCntEnemy = 0;//描画enemy数count
	
	for (int nCntR = 0; nCntR < g_nCntRow; nCntR++)
	{
		for (int nCntC = 0; nCntC < g_nCntColumn; nCntC++)
		{
			//描画のなし以外の場合　全polygon
			if (g_aMapData[nCntR][nCntC] != 0 && g_aMapData[nCntR][nCntC] != 50)
			{
				pPolygon[nCntPoly].pos = D3DXVECTOR3(nCntC * DEFAULT_WIDTH + 0.5f * DEFAULT_WIDTH, nCntR * DEFAULT_HEIGHT + 0.5f * DEFAULT_HEIGHT, 0);
				pPolygon[nCntPoly].size = D3DXVECTOR2(0.5f * DEFAULT_WIDTH, 0.5f * DEFAULT_HEIGHT);
				pPolygon[nCntPoly].move = D3DXVECTOR3(0, 0, 0);
				//全polygon初期化
				pPolygon[nCntPoly].nMoveType = 0;//移動床type 0 stop, 1と3と５ 左右移動 速度違う, 2と4と6 上下移動 向き,速度違う
				pPolygon[nCntPoly].nMoveRange = 0;//移動床の最大移動距離
				pPolygon[nCntPoly].nAnime = 0;
				pPolygon[nCntPoly].nTextureID = g_aMapData[nCntR][nCntC];
				pPolygon[nCntPoly].bBlock = false;//当たり判定を付けるか
				pPolygon[nCntPoly].bAnime = false;
				pPolygon[nCntPoly].bUse = true;
				pPolygon[nCntPoly].nDoorID = 0;
				switch (g_aMapData[nCntR][nCntC])
				{
				case 1://静止block
					pPolygon[nCntPoly].bBlock = true;
					break;
				case 2://静止block
					pPolygon[nCntPoly].bBlock = true;
					break;
				case 3://静止block
					pPolygon[nCntPoly].bBlock = true;
					break;
				case 4://DOOR
					pPolygon[nCntPoly].pos = D3DXVECTOR3((float)nCntC * DEFAULT_WIDTH + DEFAULT_WIDTH, (float)nCntR * DEFAULT_HEIGHT + 0.3f * DEFAULT_HEIGHT, 0);
					pPolygon[nCntPoly].size = D3DXVECTOR2(DEFAULT_WIDTH, DEFAULT_HEIGHT);
					pPolygon[nCntPoly].nDoorID = nCntDoor;
					nCntDoor++;
					break;
				case 5://静止block
					pPolygon[nCntPoly].bBlock = true;
					break;
				case 6://静止block
					pPolygon[nCntPoly].bBlock = true;
					break;
				case 7://静止block
					pPolygon[nCntPoly].bBlock = true;
					break;
				case 8://環境
					if (nStage == 3 || nStage == 1) //ACID
					{
						pPolygon[nCntPoly].pos = D3DXVECTOR3((float)nCntC * DEFAULT_WIDTH + DEFAULT_WIDTH, (float)nCntR * DEFAULT_HEIGHT, 0);
						pPolygon[nCntPoly].size = D3DXVECTOR2(DEFAULT_WIDTH, 0.7f * DEFAULT_HEIGHT);
						pPolygon[nCntPoly].bAnime = true;
						pPolygon[nCntPoly].nAnime = rand() % 4;//開始anime random
					}
					else if (nStage == 4) //LAVA TOP
					{
						pPolygon[nCntPoly].pos = D3DXVECTOR3((float)nCntC * DEFAULT_WIDTH + DEFAULT_WIDTH, (float)nCntR * DEFAULT_HEIGHT + 50, 0);
						pPolygon[nCntPoly].size = D3DXVECTOR2(DEFAULT_WIDTH, 0.7f * DEFAULT_HEIGHT);
						pPolygon[nCntPoly].bAnime = true;
						pPolygon[nCntPoly].nAnime = rand() % 2;//開始anime random
					}
					break;
				case 9://静止block
					pPolygon[nCntPoly].bBlock = true;
					break;
				case 10:
					if (nStage == 1) //tutorial polygon
					{
						pPolygon[nCntPoly].pos = D3DXVECTOR3((float)nCntC * DEFAULT_WIDTH + DEFAULT_WIDTH, (float)nCntR * DEFAULT_HEIGHT, 0);
						pPolygon[nCntPoly].size = D3DXVECTOR2(DEFAULT_WIDTH, 0.5 * DEFAULT_HEIGHT);
					}
					else if (nStage == 4) //lava base
					{
						pPolygon[nCntPoly].pos = D3DXVECTOR3((float)nCntC * DEFAULT_WIDTH + DEFAULT_WIDTH, (float)nCntR * DEFAULT_HEIGHT + 50, 0);
						pPolygon[nCntPoly].size = D3DXVECTOR2(DEFAULT_WIDTH, 0.7f * DEFAULT_HEIGHT);
						pPolygon[nCntPoly].bAnime = true;
						pPolygon[nCntPoly].nAnime = rand() % 2;//開始anime random
					}
					break;
				case 11://開始polygon
					pPolygon[nCntPoly].pos = D3DXVECTOR3((float)nCntC * DEFAULT_WIDTH + DEFAULT_WIDTH, (float)nCntR * DEFAULT_HEIGHT, 0);
					pPolygon[nCntPoly].size = D3DXVECTOR2(DEFAULT_WIDTH, 0.5 * DEFAULT_HEIGHT);
					break;
				case 12://ranking　polygon
					pPolygon[nCntPoly].pos = D3DXVECTOR3((float)nCntC * DEFAULT_WIDTH + DEFAULT_WIDTH, (float)nCntR * DEFAULT_HEIGHT, 0);
					pPolygon[nCntPoly].size = D3DXVECTOR2(DEFAULT_WIDTH, 0.5 * DEFAULT_HEIGHT);
					break;
				case 13://heart
					pPolygon[nCntPoly].size = D3DXVECTOR2(0.25f * DEFAULT_WIDTH, 0.25f * DEFAULT_HEIGHT);
					pPolygon[nCntPoly].bAnime = true;
					break;
				case 14://静止block
					pPolygon[nCntPoly].bBlock = true;
					break;
				case 15://静止block
					pPolygon[nCntPoly].bBlock = true;
					break;
				case 16://Coin
					pPolygon[nCntPoly].size = D3DXVECTOR2(0.25f * DEFAULT_WIDTH, 0.25f * DEFAULT_HEIGHT);
					pPolygon[nCntPoly].bAnime = true;
					break;
				case 17://UPkey
					pPolygon[nCntPoly].pos = D3DXVECTOR3((float)nCntC * DEFAULT_WIDTH + DEFAULT_WIDTH, (float)nCntR * DEFAULT_HEIGHT - 0.3f * DEFAULT_HEIGHT, 0);
					pPolygon[nCntPoly].size = D3DXVECTOR2(DEFAULT_WIDTH * 0.4f, DEFAULT_HEIGHT * 0.4f);
					pPolygon[nCntPoly].bUse = false;
					pPolygon[nCntPoly].bBlock = false;
					break;
				//左右移動床1
				case 21://leftright 1 left
					pPolygon[nCntPoly].bBlock = true;
					pPolygon[nCntPoly].nMoveType = 1;
					pPolygon[nCntPoly].nTextureID = 1;
					pPolygon[nCntPoly].move = D3DXVECTOR3(3, 0, 0);
					break;
				case 22://leftright 1 mid
					pPolygon[nCntPoly].bBlock = true;
					pPolygon[nCntPoly].nMoveType = 1;
					pPolygon[nCntPoly].nTextureID = 2;
					pPolygon[nCntPoly].move = D3DXVECTOR3(3, 0, 0);
					break;
				case 23://leftright 1 right
					pPolygon[nCntPoly].bBlock = true;
					pPolygon[nCntPoly].nMoveType = 1;
					pPolygon[nCntPoly].nTextureID = 3;
					pPolygon[nCntPoly].move = D3DXVECTOR3(3, 0, 0);
					break;

				//左右移動床2
				case 24://leftright 2 left
					pPolygon[nCntPoly].bBlock = true;
					pPolygon[nCntPoly].nMoveType = 3;
					pPolygon[nCntPoly].nTextureID = 1;
					pPolygon[nCntPoly].move = D3DXVECTOR3(4, 0, 0);
					break;
				case 25://leftright 2 mid
					pPolygon[nCntPoly].bBlock = true;
					pPolygon[nCntPoly].nMoveType = 3;
					pPolygon[nCntPoly].nTextureID = 2;
					pPolygon[nCntPoly].move = D3DXVECTOR3(4, 0, 0);
					break;
				case 26://leftright 2 right
					pPolygon[nCntPoly].bBlock = true;
					pPolygon[nCntPoly].nMoveType = 3;
					pPolygon[nCntPoly].nTextureID = 3;
					pPolygon[nCntPoly].move = D3DXVECTOR3(4, 0, 0);
					break;

				//左右移動床3
				case 27://leftright 3 left
					pPolygon[nCntPoly].bBlock = true;
					pPolygon[nCntPoly].nMoveType = 5;
					pPolygon[nCntPoly].nTextureID = 1;
					pPolygon[nCntPoly].move = D3DXVECTOR3(5, 0, 0);
					break;
				case 28://leftright 3 mid
					pPolygon[nCntPoly].bBlock = true;
					pPolygon[nCntPoly].nMoveType = 5;
					pPolygon[nCntPoly].nTextureID = 2;
					pPolygon[nCntPoly].move = D3DXVECTOR3(5, 0, 0);
					break;
				case 29://leftright 3 right
					pPolygon[nCntPoly].bBlock = true;
					pPolygon[nCntPoly].nMoveType = 5;
					pPolygon[nCntPoly].nTextureID = 3;
					pPolygon[nCntPoly].move = D3DXVECTOR3(5, 0, 0);
					break;

				//上下移動床1
				case 31://updown 1 left
					pPolygon[nCntPoly].bBlock = true;
					pPolygon[nCntPoly].nMoveType = 2;
					pPolygon[nCntPoly].nTextureID = 1;
					pPolygon[nCntPoly].move = D3DXVECTOR3(0, -2, 0);
					break;
				case 32://updown 1 mid
					pPolygon[nCntPoly].bBlock = true;
					pPolygon[nCntPoly].nMoveType = 2;
					pPolygon[nCntPoly].nTextureID = 2;
					pPolygon[nCntPoly].move = D3DXVECTOR3(0, -2, 0);
					break;
				case 33://updown 1 right
					pPolygon[nCntPoly].bBlock = true;
					pPolygon[nCntPoly].nMoveType = 2;
					pPolygon[nCntPoly].nTextureID = 3;
					pPolygon[nCntPoly].move = D3DXVECTOR3(0, -2, 0);
					break;

				//上下移動床2
				case 34://updown 2 left 
					pPolygon[nCntPoly].bBlock = true;
					pPolygon[nCntPoly].nMoveType = 4;
					pPolygon[nCntPoly].nTextureID = 1;
					pPolygon[nCntPoly].move = D3DXVECTOR3(0, -2, 0);
					break;
				case 35://updown 2 mid
					pPolygon[nCntPoly].bBlock = true;
					pPolygon[nCntPoly].nMoveType = 4;
					pPolygon[nCntPoly].nTextureID = 2;
					pPolygon[nCntPoly].move = D3DXVECTOR3(0, -2, 0);
					break;
				case 36://updown 2 right
					pPolygon[nCntPoly].bBlock = true;
					pPolygon[nCntPoly].nMoveType = 4;
					pPolygon[nCntPoly].nTextureID = 3;
					pPolygon[nCntPoly].move = D3DXVECTOR3(0, -2, 0);
					break;

				//上下移動床3
				case 37://updown 3 left
					pPolygon[nCntPoly].bBlock = true;
					pPolygon[nCntPoly].nMoveType = 6;
					pPolygon[nCntPoly].nTextureID = 1;
					pPolygon[nCntPoly].move = D3DXVECTOR3(0, 2, 0);
					break;
				case 38://updown 3 mid
					pPolygon[nCntPoly].bBlock = true;
					pPolygon[nCntPoly].nMoveType = 6;
					pPolygon[nCntPoly].nTextureID = 2;
					pPolygon[nCntPoly].move = D3DXVECTOR3(0, 2, 0);
					break;
				case 39://updown 3 right
					pPolygon[nCntPoly].bBlock = true;
					pPolygon[nCntPoly].nMoveType = 6;
					pPolygon[nCntPoly].nTextureID = 3;
					pPolygon[nCntPoly].move = D3DXVECTOR3(0, 2, 0);
					break;
				default:
					break;
				}
				nCntPoly++;
			}
		}
	}
}

HRESULT InitPolyBlock(int nStage)
{
	LPDIRECT3DDEVICE9 pDeviceBk = GetDevice();
	VERTEX_2D *pVtx;
	nCntTime = 0;//時間count
	g_nCntColumn = 0;//列reset
	g_nCntRow = 0;//行reset
	g_bAllMove = true;//スクロール有効
	g_AllMove = D3DXVECTOR2(0, 0);//スクロール速度初期化

	//メモリ
	if (FAILED(pPolygon = (POLYGON *)malloc(g_aPolygon[nStage] * sizeof(POLYGON)))) return E_FAIL;

	ReadFile(nStage);

	switch (nStage)
	{
	case 0:
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//Logo.png", &g_pTextureBlock[0]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//Presskey.png", &g_pTextureBlock[1]);
		break;
	case 1: case 3://menuとstage1
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//Coin.png", &g_pTextureBlock[0]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//land1left.png", &g_pTextureBlock[1]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//land1mid.png", &g_pTextureBlock[2]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//land1right.png", &g_pTextureBlock[3]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//door.png", &g_pTextureBlock[4]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//5landleft.png", &g_pTextureBlock[5]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//6landmid.png", &g_pTextureBlock[6]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//7landright.png", &g_pTextureBlock[7]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//acid.png", &g_pTextureBlock[8]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//9landbase.png", &g_pTextureBlock[9]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//handbook.png", &g_pTextureBlock[10]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//start.png", &g_pTextureBlock[11]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//record.png", &g_pTextureBlock[12]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//heart.png", &g_pTextureBlock[13]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//14landbaseleft.png", &g_pTextureBlock[14]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//15landbaseright.png", &g_pTextureBlock[15]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//Coin.png", &g_pTextureBlock[16]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//UpKey.png", &g_pTextureBlock[17]);
		break;
	case 2:
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//TutorialLogo.png", &g_pTextureBlock[0]);
		break;
	case 4:
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//Coin.png", &g_pTextureBlock[0]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//landB1left.png", &g_pTextureBlock[1]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//landB1mid.png", &g_pTextureBlock[2]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//landB1right.png", &g_pTextureBlock[3]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//door.png", &g_pTextureBlock[4]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//landb1topleft.png", &g_pTextureBlock[5]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//landb1topmid.png", &g_pTextureBlock[6]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//landb1topright.png", &g_pTextureBlock[7]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//lavatop.png", &g_pTextureBlock[8]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//landb1basemid.png", &g_pTextureBlock[9]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//lavabase.png", &g_pTextureBlock[10]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//start.png", &g_pTextureBlock[11]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//record.png", &g_pTextureBlock[12]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//heart.png", &g_pTextureBlock[13]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//landb1baseleft.png", &g_pTextureBlock[14]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//landb1baseright.png", &g_pTextureBlock[15]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//Coin.png", &g_pTextureBlock[16]);
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//UpKey.png", &g_pTextureBlock[17]);
		break;
	case 5:
		D3DXCreateTextureFromFile(pDeviceBk, "data//TEXTURE//RankingLogo.png", &g_pTextureBlock[0]);
		break;
	default:
		break;
	}
	
	if (FAILED(pDeviceBk->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * g_aPolygon[nStage], D3DUSAGE_WRITEONLY, FVF_VERTEX_2D, D3DPOOL_MANAGED, &g_pVtxBuffBlock, NULL))) return E_FAIL;

	g_pVtxBuffBlock->Lock(0, 0, (void**)&pVtx, 0);
	for (int nCnt = 0; nCnt < g_aPolygon[nStage]; nCnt++, pVtx += 4)
	{
		pVtx[0].pos = D3DXVECTOR3(pPolygon[nCnt].pos.x - pPolygon[nCnt].size.x, pPolygon[nCnt].pos.y - pPolygon[nCnt].size.y, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(pPolygon[nCnt].pos.x + pPolygon[nCnt].size.x, pPolygon[nCnt].pos.y - pPolygon[nCnt].size.y, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(pPolygon[nCnt].pos.x - pPolygon[nCnt].size.x, pPolygon[nCnt].pos.y + pPolygon[nCnt].size.y, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(pPolygon[nCnt].pos.x + pPolygon[nCnt].size.x, pPolygon[nCnt].pos.y + pPolygon[nCnt].size.y, 0.0f);
		pVtx[0].rhw = 1.0f;
		pVtx[1].rhw = 1.0f;
		pVtx[2].rhw = 1.0f;
		pVtx[3].rhw = 1.0f;
		pVtx[0].col = D3DCOLOR_RGBA(255, 255, 255, 255);
		pVtx[1].col = D3DCOLOR_RGBA(255, 255, 255, 255);
		pVtx[2].col = D3DCOLOR_RGBA(255, 255, 255, 255);
		pVtx[3].col = D3DCOLOR_RGBA(255, 255, 255, 255);
		
		//animation設定
		switch (pPolygon[nCnt].nTextureID)
		{
		case 4://door
			pPolygon[nCnt].bAnime = false;
			pVtx[0].tex = D3DXVECTOR2(0.2f * pPolygon[nCnt].nAnime, 0.0);
			pVtx[1].tex = D3DXVECTOR2(0.2f + 0.2f * pPolygon[nCnt].nAnime, 0.0);
			pVtx[2].tex = D3DXVECTOR2(0.2f * pPolygon[nCnt].nAnime, 1.0);
			pVtx[3].tex = D3DXVECTOR2(0.2f + 0.2f * pPolygon[nCnt].nAnime, 1.0);
			break;
		case 8://acid
			pVtx[0].tex = D3DXVECTOR2(0.25f * pPolygon[nCnt].nAnime, 0.0);
			pVtx[1].tex = D3DXVECTOR2(0.25f + 0.25f * pPolygon[nCnt].nAnime, 0.0);
			pVtx[2].tex = D3DXVECTOR2(0.25f * pPolygon[nCnt].nAnime, 1.0);
			pVtx[3].tex = D3DXVECTOR2(0.25f + 0.25f * pPolygon[nCnt].nAnime, 1.0);
			break;
		case 16://coin
			pVtx[0].tex = D3DXVECTOR2(0.1f * pPolygon[nCnt].nAnime, 0.0);
			pVtx[1].tex = D3DXVECTOR2(0.1f + 0.1f * pPolygon[nCnt].nAnime, 0.0);
			pVtx[2].tex = D3DXVECTOR2(0.1f * pPolygon[nCnt].nAnime, 1.0);
			pVtx[3].tex = D3DXVECTOR2(0.1f + 0.1f * pPolygon[nCnt].nAnime, 1.0);
			break;
		case 13://heart
			pVtx[0].tex = D3DXVECTOR2(0.1f * pPolygon[nCnt].nAnime, 0.0);
			pVtx[1].tex = D3DXVECTOR2(0.1f + 0.1f * pPolygon[nCnt].nAnime, 0.0);
			pVtx[2].tex = D3DXVECTOR2(0.1f * pPolygon[nCnt].nAnime, 1.0);
			pVtx[3].tex = D3DXVECTOR2(0.1f + 0.1f * pPolygon[nCnt].nAnime, 1.0);
			break;
		case 17://UPkey
			pVtx[0].tex = D3DXVECTOR2(0.2f * pPolygon[nCnt].nAnime, 0.0);
			pVtx[1].tex = D3DXVECTOR2(0.2f + 0.2f * pPolygon[nCnt].nAnime, 0.0);
			pVtx[2].tex = D3DXVECTOR2(0.2f * pPolygon[nCnt].nAnime, 1.0);
			pVtx[3].tex = D3DXVECTOR2(0.2f + 0.2f * pPolygon[nCnt].nAnime, 1.0);
			break;
		default:
			pVtx[0].tex = D3DXVECTOR2(0.0, 0.0);
			pVtx[1].tex = D3DXVECTOR2(1.0, 0.0);
			pVtx[2].tex = D3DXVECTOR2(0.0, 1.0);
			pVtx[3].tex = D3DXVECTOR2(1.0, 1.0);
			break;
		}
	}
	g_pVtxBuffBlock->Unlock();
	return S_OK;
}

void UninitPolyBlock(int nStage)
{
	if (g_pVtxBuffBlock != NULL)
	{
		g_pVtxBuffBlock->Release();
		g_pVtxBuffBlock = NULL;
	}
	for (int nCnt = 0; nCnt < BLOCK_TEXTURE; nCnt++)
	{
		if (g_pTextureBlock[nCnt] != NULL)
		{
			g_pTextureBlock[nCnt]->Release();
			g_pTextureBlock[nCnt] = NULL;
		}
	}
	free(pPolygon);
	//free(ppEnemy);
}

void UpdatePolyBlock(int nStage)
{
	VERTEX_2D *pVtx;
	PLAYER *pPlayer = GetPlayer();
	nCntTime++;//時間count

	switch (nStage)
	{
	case 0://title
		if (pPolygon[0].pos.y < 300)
		{
			pPolygon[0].pos += pPolygon[0].move;
			g_pVtxBuffBlock->Lock(0, 0, (void**)&pVtx, 0);
			pVtx[0].pos = D3DXVECTOR3(pPolygon[0].pos.x - pPolygon[0].size.x, pPolygon[0].pos.y - pPolygon[0].size.y, 0.0f);
			pVtx[1].pos = D3DXVECTOR3(pPolygon[0].pos.x + pPolygon[0].size.x, pPolygon[0].pos.y - pPolygon[0].size.y, 0.0f);
			pVtx[2].pos = D3DXVECTOR3(pPolygon[0].pos.x - pPolygon[0].size.x, pPolygon[0].pos.y + pPolygon[0].size.y, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(pPolygon[0].pos.x + pPolygon[0].size.x, pPolygon[0].pos.y + pPolygon[0].size.y, 0.0f);
			g_pVtxBuffBlock->Unlock();
		}
		else
		{
			if (nCntTime % 30 == 0)
				pPolygon[1].bUse = !pPolygon[1].bUse;
		}
		//press enter
		if ((GetkeyboardTrigger(DIK_RETURN) == true))
		{
			SetFade(FADE_OUT, MODE_MENU);
			if (nCntTime % 10 == 0)
				pPolygon[1].bUse = !pPolygon[1].bUse;
		}
		break;

	case 1://menu
		g_pVtxBuffBlock->Lock(0, 0, (void**)&pVtx, 0);
		for (int nCnt = 0; nCnt < g_aPolygon[nStage]; nCnt++, pVtx += 4)
		{
			switch (pPolygon[nCnt].nTextureID)
			{
			case 4://door
				if (nCntTime % 5 == 0 && pPolygon[nCnt].bAnime == true && pPolygon[nCnt].nAnime < 4)
					pPolygon[nCnt].nAnime++;
				else if (nCntTime % 5 == 0 && pPolygon[nCnt].bAnime == false && pPolygon[nCnt].nAnime > 0)
					pPolygon[nCnt].nAnime--;
				pVtx[0].tex = D3DXVECTOR2(0.2f * pPolygon[nCnt].nAnime, 0.0);
				pVtx[1].tex = D3DXVECTOR2(0.2f + 0.2f * pPolygon[nCnt].nAnime, 0.0);
				pVtx[2].tex = D3DXVECTOR2(0.2f * pPolygon[nCnt].nAnime, 1.0);
				pVtx[3].tex = D3DXVECTOR2(0.2f + 0.2f * pPolygon[nCnt].nAnime, 1.0);
				break;
			case 8://acid
				if (nCntTime % 15 == 0)
				{
					pPolygon[nCnt].nAnime++;
					if (pPolygon[nCnt].nAnime > 3)  pPolygon[nCnt].nAnime = 0;
					pVtx[0].tex = D3DXVECTOR2(0.25f * pPolygon[nCnt].nAnime, 0.0);
					pVtx[1].tex = D3DXVECTOR2(0.25f + 0.25f * pPolygon[nCnt].nAnime, 0.0);
					pVtx[2].tex = D3DXVECTOR2(0.25f * pPolygon[nCnt].nAnime, 1.0);
					pVtx[3].tex = D3DXVECTOR2(0.25f + 0.25f * pPolygon[nCnt].nAnime, 1.0);
				}
				break;
			case 17://Upkey
				if (nCntTime % 15 == 0 && pPolygon[nCnt].bUse == true)
				{
					pPolygon[nCnt].nAnime++;
					pVtx[0].tex = D3DXVECTOR2(0.2f * pPolygon[nCnt].nAnime, 0.0);
					pVtx[1].tex = D3DXVECTOR2(0.2f + 0.2f * pPolygon[nCnt].nAnime, 0.0);
					pVtx[2].tex = D3DXVECTOR2(0.2f * pPolygon[nCnt].nAnime, 1.0);
					pVtx[3].tex = D3DXVECTOR2(0.2f + 0.2f * pPolygon[nCnt].nAnime, 1.0);
				}
				break;
			default:
				break;
			}
		}
		g_pVtxBuffBlock->Unlock();
		break;

	case 2://tutorial
		break;

	case 3://stage1
		g_pVtxBuffBlock->Lock(0, 0, (void**)&pVtx, 0);
		for (int nCnt = 0; nCnt < g_aPolygon[nStage]; nCnt++, pVtx += 4)//移動床
		{
			switch (pPolygon[nCnt].nMoveType)
			{
			case 1:
				pPolygon[nCnt].nMoveRange += (int)pPolygon[nCnt].move.x;
				if (abs(pPolygon[nCnt].nMoveRange) > 400)
				{
					pPolygon[nCnt].move.x = -pPolygon[nCnt].move.x;
					pPolygon[nCnt].nMoveRange = 0;
				}
				break;
			case 2:
				if (pPolygon[nCnt].pos.y < 0) pPolygon[nCnt].pos.y = SCREEN_HEIGHT;
				break;
			case 3:
				pPolygon[nCnt].nMoveRange += (int)pPolygon[nCnt].move.x;
				if (abs(pPolygon[nCnt].nMoveRange) > 600)
				{
					pPolygon[nCnt].move.x = -pPolygon[nCnt].move.x;
					pPolygon[nCnt].nMoveRange = 0;
				}
				break;
			case 4:
				if (pPolygon[nCnt].pos.y < 100) pPolygon[nCnt].move.y = -pPolygon[nCnt].move.y;
				if (pPolygon[nCnt].pos.y > 700) pPolygon[nCnt].move.y = -pPolygon[nCnt].move.y;
				break;
			default:
				break;
			}
			switch (pPolygon[nCnt].nTextureID)
			{
			case 4://door
				if (nCntTime % 5 == 0 && pPolygon[nCnt].bAnime == true && pPolygon[nCnt].nAnime < 4)
					pPolygon[nCnt].nAnime++;
				else if (nCntTime % 5 == 0 && pPolygon[nCnt].bAnime == false && pPolygon[nCnt].nAnime > 0)
					pPolygon[nCnt].nAnime--;
				pVtx[0].tex = D3DXVECTOR2(0.2f * pPolygon[nCnt].nAnime, 0.0);
				pVtx[1].tex = D3DXVECTOR2(0.2f + 0.2f * pPolygon[nCnt].nAnime, 0.0);
				pVtx[2].tex = D3DXVECTOR2(0.2f * pPolygon[nCnt].nAnime, 1.0);
				pVtx[3].tex = D3DXVECTOR2(0.2f + 0.2f * pPolygon[nCnt].nAnime, 1.0);
				break;
			case 8://acid
				if (nCntTime % 15 == 0)
				{
					pPolygon[nCnt].nAnime++;
					if (pPolygon[nCnt].nAnime > 3)  pPolygon[nCnt].nAnime = 0;
					pVtx[0].tex = D3DXVECTOR2(0.25f * pPolygon[nCnt].nAnime, 0.0);
					pVtx[1].tex = D3DXVECTOR2(0.25f + 0.25f * pPolygon[nCnt].nAnime, 0.0);
					pVtx[2].tex = D3DXVECTOR2(0.25f * pPolygon[nCnt].nAnime, 1.0);
					pVtx[3].tex = D3DXVECTOR2(0.25f + 0.25f * pPolygon[nCnt].nAnime, 1.0);
				}
				break;
			case 16://coin
				if (nCntTime % 5 == 0)
				{
					pPolygon[nCnt].nAnime++;
					pVtx[0].tex = D3DXVECTOR2(0.1f * pPolygon[nCnt].nAnime, 0.0);
					pVtx[1].tex = D3DXVECTOR2(0.1f + 0.1f * pPolygon[nCnt].nAnime, 0.0);
					pVtx[2].tex = D3DXVECTOR2(0.1f * pPolygon[nCnt].nAnime, 1.0);
					pVtx[3].tex = D3DXVECTOR2(0.1f + 0.1f * pPolygon[nCnt].nAnime, 1.0);
				}
				break;
			case 13://heart 
				if (nCntTime % 5 == 0)
				{
					pPolygon[nCnt].nAnime++;
					pVtx[0].tex = D3DXVECTOR2(0.1f * pPolygon[nCnt].nAnime, 0.0);
					pVtx[1].tex = D3DXVECTOR2(0.1f + 0.1f * pPolygon[nCnt].nAnime, 0.0);
					pVtx[2].tex = D3DXVECTOR2(0.1f * pPolygon[nCnt].nAnime, 1.0);
					pVtx[3].tex = D3DXVECTOR2(0.1f + 0.1f * pPolygon[nCnt].nAnime, 1.0);
				}
				break;
			case 17://Upkey
				if (nCntTime % 15 == 0 && pPolygon[nCnt].bUse == true)
				{
					pPolygon[nCnt].nAnime++;
					pVtx[0].tex = D3DXVECTOR2(0.2f * pPolygon[nCnt].nAnime, 0.0);
					pVtx[1].tex = D3DXVECTOR2(0.2f + 0.2f * pPolygon[nCnt].nAnime, 0.0);
					pVtx[2].tex = D3DXVECTOR2(0.2f * pPolygon[nCnt].nAnime, 1.0);
					pVtx[3].tex = D3DXVECTOR2(0.2f + 0.2f * pPolygon[nCnt].nAnime, 1.0);
				}
				break;
			default:
				break;
			}
		}
		g_pVtxBuffBlock->Unlock();

		//スクロール移動処理 doorの位置とplayerの位置でcontrol
		for (int nCnt = 0; nCnt < g_aPolygon[nStage]; nCnt++)
		{
			if (pPolygon[nCnt].nDoorID == 1) g_aDoorNum[0] = nCnt;//右側のdoor
			if (pPolygon[nCnt].nDoorID == 2) g_aDoorNum[1] = nCnt;//左側のdoor
		}
		g_AllMove.x = 0;//スクロールreset
		if (pPolygon[g_aDoorNum[1]].pos.x > 2 * DEFAULT_WIDTH && pPlayer->move.x < 0)//左側のスクロール限り
		{
			g_AllMove.x = 0;
			if ((pPlayer->pos.x >= SCREEN_WIDTH / 2 + 50) && pPlayer->move.x > 0)
			{
				g_AllMove.x = -pPlayer->move.x;
				pPlayer->move.x = 0;
			}
		}
		else if (pPolygon[g_aDoorNum[0]].pos.x < SCREEN_WIDTH - 2 * DEFAULT_WIDTH && pPlayer->move.x > 0)//右側のスクロール限り
		{
			g_AllMove.x = 0;
			if ((pPlayer->pos.x <= SCREEN_WIDTH / 2 - 100) && pPlayer->move.x < 0)
			{
				g_AllMove.x = -pPlayer->move.x;
				pPlayer->move.x = 0;
			}
		}
		else if ((pPlayer->pos.x >= SCREEN_WIDTH / 2 + 50) && pPlayer->move.x > 0)//player画面中心で移動のcontrol
		{
			g_AllMove.x = -pPlayer->move.x;
			pPlayer->move.x = 0;
		}
		else if ((pPlayer->pos.x <= SCREEN_WIDTH / 2 - 100) && pPlayer->move.x < 0)//player画面中心で移動のcontrol
		{
			g_AllMove.x = -pPlayer->move.x;
			pPlayer->move.x = 0;
		}

		//update vtx
		g_pVtxBuffBlock->Lock(0, 0, (void**)&pVtx, 0);
		for (int nCnt = 0; nCnt < g_aPolygon[nStage]; nCnt++, pVtx += 4)
		{
			pPolygon[nCnt].pos.x += pPolygon[nCnt].move.x + g_AllMove.x;
			pPolygon[nCnt].pos.y += pPolygon[nCnt].move.y;
			pVtx[0].pos = D3DXVECTOR3(pPolygon[nCnt].pos.x - pPolygon[nCnt].size.x, pPolygon[nCnt].pos.y - pPolygon[nCnt].size.y, 0.0f);
			pVtx[1].pos = D3DXVECTOR3(pPolygon[nCnt].pos.x + pPolygon[nCnt].size.x, pPolygon[nCnt].pos.y - pPolygon[nCnt].size.y, 0.0f);
			pVtx[2].pos = D3DXVECTOR3(pPolygon[nCnt].pos.x - pPolygon[nCnt].size.x, pPolygon[nCnt].pos.y + pPolygon[nCnt].size.y, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(pPolygon[nCnt].pos.x + pPolygon[nCnt].size.x, pPolygon[nCnt].pos.y + pPolygon[nCnt].size.y, 0.0f);
		}
		g_pVtxBuffBlock->Unlock();

		break;
	case 4://stage2
		g_pVtxBuffBlock->Lock(0, 0, (void**)&pVtx, 0);

		//移動床
		for (int nCnt = 0; nCnt < g_aPolygon[nStage]; nCnt++, pVtx += 4)
		{
			switch (pPolygon[nCnt].nMoveType)
			{
			case 1:
				pPolygon[nCnt].nMoveRange += (int)pPolygon[nCnt].move.x;
				if (abs(pPolygon[nCnt].nMoveRange) > 400)
				{
					pPolygon[nCnt].move.x = -pPolygon[nCnt].move.x;
					pPolygon[nCnt].nMoveRange = 0;
				}
				break;
			case 2:
				if (pPolygon[nCnt].pos.y < 0) pPolygon[nCnt].pos.y = SCREEN_HEIGHT;
				break;
			case 3:
				pPolygon[nCnt].nMoveRange += (int)pPolygon[nCnt].move.x;
				if (abs(pPolygon[nCnt].nMoveRange) > 400)
				{
					pPolygon[nCnt].move.x = -pPolygon[nCnt].move.x;
					pPolygon[nCnt].nMoveRange = 0;
				}
				break;
			case 4:
				if (pPolygon[nCnt].pos.y < 100) pPolygon[nCnt].move.y = -pPolygon[nCnt].move.y;
				if (pPolygon[nCnt].pos.y > 700) pPolygon[nCnt].move.y = -pPolygon[nCnt].move.y;
				break;
			case 6:
				if (pPolygon[nCnt].pos.y > SCREEN_HEIGHT) pPolygon[nCnt].pos.y = 0;
				break;
			case 5:
				pPolygon[nCnt].nMoveRange += (int)pPolygon[nCnt].move.x;
				if (abs(pPolygon[nCnt].nMoveRange) > 400)
				{
					pPolygon[nCnt].move.x = -pPolygon[nCnt].move.x;
					pPolygon[nCnt].nMoveRange = 0;
				}
				break;
			default:
				break;
			}
			switch (pPolygon[nCnt].nTextureID)
			{
			case 4://door
				if (nCntTime % 5 == 0 && pPolygon[nCnt].bAnime == true && pPolygon[nCnt].nAnime < 4)
					pPolygon[nCnt].nAnime++;
				else if (nCntTime % 5 == 0 && pPolygon[nCnt].bAnime == false && pPolygon[nCnt].nAnime > 0)
					pPolygon[nCnt].nAnime--;
				pVtx[0].tex = D3DXVECTOR2(0.2f * pPolygon[nCnt].nAnime, 0.0);
				pVtx[1].tex = D3DXVECTOR2(0.2f + 0.2f * pPolygon[nCnt].nAnime, 0.0);
				pVtx[2].tex = D3DXVECTOR2(0.2f * pPolygon[nCnt].nAnime, 1.0);
				pVtx[3].tex = D3DXVECTOR2(0.2f + 0.2f * pPolygon[nCnt].nAnime, 1.0);
				break;
			case 8: case 10: //lava
				if (nCntTime % 20 == 0)
				{
					pPolygon[nCnt].nAnime++;
					if (pPolygon[nCnt].nAnime > 2)  pPolygon[nCnt].nAnime = 0;
					pVtx[0].tex = D3DXVECTOR2(0.5f * pPolygon[nCnt].nAnime, 0.0);
					pVtx[1].tex = D3DXVECTOR2(0.5f + 0.5f * pPolygon[nCnt].nAnime, 0.0);
					pVtx[2].tex = D3DXVECTOR2(0.5f * pPolygon[nCnt].nAnime, 1.0);
					pVtx[3].tex = D3DXVECTOR2(0.5f + 0.5f * pPolygon[nCnt].nAnime, 1.0);
				}
				break;
			case 16://coin
				if (nCntTime % 5 == 0)
				{
					pPolygon[nCnt].nAnime++;
					pVtx[0].tex = D3DXVECTOR2(0.1f * pPolygon[nCnt].nAnime, 0.0);
					pVtx[1].tex = D3DXVECTOR2(0.1f + 0.1f * pPolygon[nCnt].nAnime, 0.0);
					pVtx[2].tex = D3DXVECTOR2(0.1f * pPolygon[nCnt].nAnime, 1.0);
					pVtx[3].tex = D3DXVECTOR2(0.1f + 0.1f * pPolygon[nCnt].nAnime, 1.0);
				}
				break;
			case 13://heart
				if (nCntTime % 5 == 0)
				{
					pPolygon[nCnt].nAnime++;
					pVtx[0].tex = D3DXVECTOR2(0.1f * pPolygon[nCnt].nAnime, 0.0);
					pVtx[1].tex = D3DXVECTOR2(0.1f + 0.1f * pPolygon[nCnt].nAnime, 0.0);
					pVtx[2].tex = D3DXVECTOR2(0.1f * pPolygon[nCnt].nAnime, 1.0);
					pVtx[3].tex = D3DXVECTOR2(0.1f + 0.1f * pPolygon[nCnt].nAnime, 1.0);
				}
				break;
			case 17://Upkey
				if (nCntTime % 15 == 0 && pPolygon[nCnt].bUse == true)
				{
					pPolygon[nCnt].nAnime++;
					pVtx[0].tex = D3DXVECTOR2(0.2f * pPolygon[nCnt].nAnime, 0.0);
					pVtx[1].tex = D3DXVECTOR2(0.2f + 0.2f * pPolygon[nCnt].nAnime, 0.0);
					pVtx[2].tex = D3DXVECTOR2(0.2f * pPolygon[nCnt].nAnime, 1.0);
					pVtx[3].tex = D3DXVECTOR2(0.2f + 0.2f * pPolygon[nCnt].nAnime, 1.0);
				}
				break;
			default:
				break;
			}
		}
		g_pVtxBuffBlock->Unlock();

		//スクロール移動処理 doorの位置とplayerの移動でcontrol
		for (int nCnt = 0; nCnt < g_aPolygon[nStage]; nCnt++)
		{
			if (pPolygon[nCnt].nDoorID == 1) g_aDoorNum[0] = nCnt;//左側のdoor
			if (pPolygon[nCnt].nDoorID == 2) g_aDoorNum[1] = nCnt;//右側のdoor
		}
		g_AllMove.x = 0;
		if (pPolygon[g_aDoorNum[0]].pos.x > 2 * DEFAULT_WIDTH && pPlayer->move.x < 0)//左側のスクロール限り
		{
			g_AllMove.x = 0;
			if ((pPlayer->pos.x >= SCREEN_WIDTH / 2 + 50) && pPlayer->move.x > 0)
			{
				g_AllMove.x = -pPlayer->move.x;
				pPlayer->move.x = 0;
			}
		}
		else if (pPolygon[g_aDoorNum[1]].pos.x < SCREEN_WIDTH - 2 * DEFAULT_WIDTH && pPlayer->move.x > 0)//右側のスクロール限り
		{
			g_AllMove.x = 0;
			if ((pPlayer->pos.x <= SCREEN_WIDTH / 2 - 100) && pPlayer->move.x < 0)
			{
				g_AllMove.x = -pPlayer->move.x;
				pPlayer->move.x = 0;
			}
		}
		else if ((pPlayer->pos.x >= SCREEN_WIDTH / 2 + 50) && pPlayer->move.x > 0)//player画面中心で移動のcontrol
		{
			g_AllMove.x = -pPlayer->move.x;
			pPlayer->move.x = 0;
		}
		else if ((pPlayer->pos.x <= SCREEN_WIDTH / 2 - 100) && pPlayer->move.x < 0)//player画面中心で移動のcontrol
		{
			g_AllMove.x = -pPlayer->move.x;
			pPlayer->move.x = 0;
		}

		//update vtx
		g_pVtxBuffBlock->Lock(0, 0, (void**)&pVtx, 0);
		for (int nCnt = 0; nCnt < g_aPolygon[nStage]; nCnt++, pVtx += 4)
		{
			pPolygon[nCnt].pos.x += pPolygon[nCnt].move.x + g_AllMove.x;
			pPolygon[nCnt].pos.y += pPolygon[nCnt].move.y;
			pVtx[0].pos = D3DXVECTOR3(pPolygon[nCnt].pos.x - pPolygon[nCnt].size.x, pPolygon[nCnt].pos.y - pPolygon[nCnt].size.y, 0.0f);
			pVtx[1].pos = D3DXVECTOR3(pPolygon[nCnt].pos.x + pPolygon[nCnt].size.x, pPolygon[nCnt].pos.y - pPolygon[nCnt].size.y, 0.0f);
			pVtx[2].pos = D3DXVECTOR3(pPolygon[nCnt].pos.x - pPolygon[nCnt].size.x, pPolygon[nCnt].pos.y + pPolygon[nCnt].size.y, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(pPolygon[nCnt].pos.x + pPolygon[nCnt].size.x, pPolygon[nCnt].pos.y + pPolygon[nCnt].size.y, 0.0f);
		}
		g_pVtxBuffBlock->Unlock();
		break;
	default:
		break;
	}
}

void DrawPolyBlock(int nStage)
{
	LPDIRECT3DDEVICE9 pDeviceBk = GetDevice();

	pDeviceBk->SetStreamSource(0, g_pVtxBuffBlock, 0, sizeof(VERTEX_2D));
	pDeviceBk->SetFVF(FVF_VERTEX_2D);
	for (int nCnt = 0; nCnt < g_aPolygon[nStage]; nCnt++)
	{
		if (pPolygon[nCnt].bUse == true)
		{
			pDeviceBk->SetTexture(0, g_pTextureBlock[pPolygon[nCnt].nTextureID]);
			pDeviceBk->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCnt * 4, 2);
		}
	}
}

POLYGON *GetPolyBlock(void)
{
	return &pPolygon[0];
}

int PolyNum(int nStage)
{
	return g_aPolygon[nStage];
}

D3DXVECTOR2 *GetAllMove(void)
{
	return &g_AllMove;
}

int *MapData(void)
{
	return &g_aMapData[0][0];
}
