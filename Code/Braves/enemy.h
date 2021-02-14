//=============================================================================
//
// [Enemy.h]Å@
// Author : Yo Jitaku
//
//=============================================================================
#pragma once
#include "DirectX.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define WALK_SPEED 3.0f

typedef enum
{
	E_IDLE = 0,
	E_WALK,
	E_ATTACK,
	E_DIED,
	E_HURT,
	E_ALERT,
	E_MAX,
}ENEMYSTATE;

typedef struct
{
	D3DXVECTOR3 pos;
	D3DXVECTOR2 size;
	D3DXVECTOR3 move;
	bool bUse, bBlock, bHurt, bAlert, bAnimeChange;
	int nDirection, nState, nAnime, nLife, nType, nMoveRange, nDeadTime;
}ENEMY;

void SetEnemy(void);
HRESULT InitEnemy(int nStage);
void UninitEnemy(int nStage);
void UpdateEnemy(int nStage);
void DrawEnemy(int nStage);
int EnemyNum(int nStage);
ENEMY *GetEnemy(void);