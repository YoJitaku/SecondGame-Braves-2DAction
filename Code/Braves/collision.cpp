//=============================================================================
//
// [collision.cpp]�@�O�ς��g�p���Ă̓����蔻��
// Author : Yo Jitaku
//
//=============================================================================
#include "collision.h"

bool Vtx_Vector_ColDet(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2, D3DXVECTOR2 size2)
{
	bool bColDet = false;
	int nCntTrue = 0;
	D3DXVECTOR3 *pPos;//Polygon�̑S���W�̒�`
	D3DXVECTOR3 aCoVec1[4];//Polygon�l�̕ӂ�vector��`
	D3DXVECTOR3 CoVec2;//�O�όv�Z���K�vVector

	pPos = (D3DXVECTOR3 *)malloc(sizeof(D3DXVECTOR3) * (BLOCK_VTX));

	pPos[0] = D3DXVECTOR3(pos2.x - size2.x, pos2.y - size2.y, 0.0f);
	pPos[1] = D3DXVECTOR3(pos2.x + size2.x, pos2.y - size2.y, 0.0f);
	pPos[2] = D3DXVECTOR3(pos2.x + size2.x, pos2.y + size2.y, 0.0f);//2�C3�t�� �v�Z���₷���悤��
	pPos[3] = D3DXVECTOR3(pos2.x - size2.x, pos2.y + size2.y, 0.0f);//2�C3�t�� �v�Z���₷���悤��

	//Polygon�̎l�ӂ�Vector���v�Z����
	for (int nCnt1 = 0; nCnt1 < BLOCK_VTX; nCnt1++)
	{
		if (nCnt1 == BLOCK_VTX - 1)//�Ō�̒��_���珉�߂̒��_�ցA�g�ݍ��킹��Vector
			aCoVec1[nCnt1] = pPos[0] - pPos[BLOCK_VTX - 1];
		else
			aCoVec1[nCnt1] = pPos[nCnt1 + 1] - pPos[nCnt1];
	}

	//���f�������_���W�Ǝl��vector�̊O�όv�Z
	for (int nCnt = 0; nCnt < BLOCK_VTX; nCnt++)//�l�̕�
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
	D3DXVECTOR3 *pPos;//player�S���W
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
		bPlayerCol[nCnt] = false;//������
		bPlayerCol[nCnt] = Vtx_Vector_ColDet(pPos[nCnt], Pos2, Size2);//player�̊e���_��collision�̓����蔻��
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
		bEnemyCol[nCnt] = false;//������
		bEnemyCol[nCnt] = Vtx_Vector_ColDet(pPos[nCnt], Pos2, Size2);//player�̊e���_��collision�̓����蔻��
	}

	if (bEnemyCol[0] == true) EnemyCol_State = CS_UP;
	if (bEnemyCol[1] == true) EnemyCol_State = CS_LEFT;
	if (bEnemyCol[2] == true) EnemyCol_State = CS_RIGHT;

	free(pPos);
	free(bEnemyCol);
	return EnemyCol_State;
}