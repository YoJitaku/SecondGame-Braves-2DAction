//=============================================================================
//
// [collision.cpp]　外積を使用しての当たり判定
// Author : Yo Jitaku
//
//=============================================================================
#include "collision.h"

bool Vtx_Vector_ColDet(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2, D3DXVECTOR2 size2)
{
	bool bColDet = false;
	int nCntTrue = 0;
	D3DXVECTOR3 *pPos;//Polygonの全座標の定義
	D3DXVECTOR3 aCoVec1[4];//Polygon四つの辺のvector定義
	D3DXVECTOR3 CoVec2;//外積計算し必要Vector

	pPos = (D3DXVECTOR3 *)malloc(sizeof(D3DXVECTOR3) * (BLOCK_VTX));

	pPos[0] = D3DXVECTOR3(pos2.x - size2.x, pos2.y - size2.y, 0.0f);
	pPos[1] = D3DXVECTOR3(pos2.x + size2.x, pos2.y - size2.y, 0.0f);
	pPos[2] = D3DXVECTOR3(pos2.x + size2.x, pos2.y + size2.y, 0.0f);//2，3逆順 計算しやすいように
	pPos[3] = D3DXVECTOR3(pos2.x - size2.x, pos2.y + size2.y, 0.0f);//2，3逆順 計算しやすいように

	//Polygonの四つ辺のVectorを計算する
	for (int nCnt1 = 0; nCnt1 < BLOCK_VTX; nCnt1++)
	{
		if (nCnt1 == BLOCK_VTX - 1)//最後の頂点から初めの頂点へ、組み合わせたVector
			aCoVec1[nCnt1] = pPos[0] - pPos[BLOCK_VTX - 1];
		else
			aCoVec1[nCnt1] = pPos[nCnt1 + 1] - pPos[nCnt1];
	}

	//判断したい点座標と四つのvectorの外積計算
	for (int nCnt = 0; nCnt < BLOCK_VTX; nCnt++)//四つの辺
	{
		CoVec2 = pPos[nCnt] - pos1;
		if (aCoVec1[nCnt].y * CoVec2.x - aCoVec1[nCnt].x * CoVec2.y > 0)
			nCntTrue++;
	}
	if (nCntTrue == BLOCK_VTX)
		bColDet = true;

	free(pPos);
	return bColDet;
}

COLLISION_STATE Player_ColDet(D3DXVECTOR3 Pos1, D3DXVECTOR2 Size1, D3DXVECTOR3 Pos2, D3DXVECTOR2 Size2)
{	
	bool *bPlayerCol;
	COLLISION_STATE PlayerCol_State = CS_MAX;
	D3DXVECTOR3 *pPos;//player全座標
	pPos = (D3DXVECTOR3 *)malloc(sizeof(D3DXVECTOR3) * (PLAYER_VTX));
	bPlayerCol = (bool *)malloc(sizeof(bool) * (PLAYER_VTX));
	//TOP
	pPos[0] = D3DXVECTOR3(Pos1.x - Size1.x * 0.8f, Pos1.y - Size1.y, 0.0f);
	pPos[1] = D3DXVECTOR3(Pos1.x , Pos1.y - Size1.y, 0.0f);
	pPos[2] = D3DXVECTOR3(Pos1.x + Size1.x * 0.8f, Pos1.y - Size1.y, 0.0f);

	//LOW
	pPos[3] = D3DXVECTOR3(Pos1.x - Size1.x * 0.8f, Pos1.y + Size1.y, 0.0f);
	pPos[4] = D3DXVECTOR3(Pos1.x, Pos1.y + Size1.y, 0.0f);
	pPos[5] = D3DXVECTOR3(Pos1.x + Size1.x * 0.8f, Pos1.y + Size1.y, 0.0f);

	//LEFT
	pPos[6] = D3DXVECTOR3(Pos1.x - Size1.x, Pos1.y - Size1.y, 0.0f);
	pPos[7] = D3DXVECTOR3(Pos1.x - Size1.x, Pos1.y, 0.0f);
	pPos[8] = D3DXVECTOR3(Pos1.x - Size1.x, Pos1.y + Size1.y * 0.9f, 0.0f);

	//RIGHT
	pPos[9] = D3DXVECTOR3(Pos1.x + Size1.x, Pos1.y - Size1.y, 0.0f);
	pPos[10] = D3DXVECTOR3(Pos1.x + Size1.x, Pos1.y, 0.0f);
	pPos[11] = D3DXVECTOR3(Pos1.x + Size1.x, Pos1.y + Size1.y * 0.9f, 0.0f);

	for (int nCnt = 0; nCnt < PLAYER_VTX; nCnt++)
	{
		bPlayerCol[nCnt] = false;//初期化
		bPlayerCol[nCnt] = Vtx_Vector_ColDet(pPos[nCnt], Pos2, Size2);//playerの各頂点とcollisionの当たり判定
	}

	if (bPlayerCol[0] == true || bPlayerCol[1] == true || bPlayerCol[2] == true)
		PlayerCol_State = CS_DOWN;
	else if (bPlayerCol[3] == true || bPlayerCol[4] == true || bPlayerCol[5] == true)
		PlayerCol_State = CS_UP;
	else if (bPlayerCol[6] == true || bPlayerCol[7] == true || bPlayerCol[8] == true)
		PlayerCol_State = CS_LEFT;
	else if (bPlayerCol[9] == true || bPlayerCol[10] == true || bPlayerCol[11] == true)
		PlayerCol_State = CS_RIGHT;

	free(pPos);
	free(bPlayerCol);
	return PlayerCol_State;
}

COLLISION_STATE Enemy_ColDet(D3DXVECTOR3 Pos1, D3DXVECTOR2 Size1, D3DXVECTOR3 Pos2, D3DXVECTOR2 Size2)
{
	bool *bEnemyCol;
	COLLISION_STATE EnemyCol_State = CS_MAX;
	D3DXVECTOR3 *pPos;
	pPos = (D3DXVECTOR3 *)malloc(sizeof(D3DXVECTOR3) * (ENEMY_VTX));
	bEnemyCol = (bool *)malloc(sizeof(bool) * (ENEMY_VTX));

	//Low
	pPos[0] = D3DXVECTOR3(Pos1.x, Pos1.y + Size1.y, 0.0f);

	//Left
	pPos[1] = D3DXVECTOR3(Pos1.x - Size1.x, Pos1.y, 0.0f);

	//Right
	pPos[2] = D3DXVECTOR3(Pos1.x + Size1.x, Pos1.y, 0.0f);

	for (int nCnt = 0; nCnt < PLAYER_VTX; nCnt++)
	{
		bEnemyCol[nCnt] = false;//初期化
		bEnemyCol[nCnt] = Vtx_Vector_ColDet(pPos[nCnt], Pos2, Size2);//playerの各頂点とcollisionの当たり判定
	}

	if (bEnemyCol[0] == true) EnemyCol_State = CS_UP;
	if (bEnemyCol[1] == true) EnemyCol_State = CS_LEFT;
	if (bEnemyCol[2] == true) EnemyCol_State = CS_RIGHT;

	free(pPos);
	free(bEnemyCol);
	return EnemyCol_State;
}