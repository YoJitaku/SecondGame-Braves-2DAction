//=============================================================================
//
// [stage1.cpp]@
// Author : Yo Jitaku
//
//=============================================================================
#include "stage1.h"
#include "background.h"
#include "polyblock.h"
#include "player.h"
#include "UI.h"
#include "enemy.h"

HRESULT InitStage1(void)
{
	InitBackground(3);
	InitPolyBlock(3);
	InitEnemy(3);
	return S_OK;
}
void UninitStage1(void)
{
	UninitBackground(3);
	UninitEnemy(3);
	UninitPolyBlock(3);
	
}
void UpdateStage1(void)
{	
	PLAYER *pPlayer = GetPlayer();

	if (pPlayer->nState == P_DIED) 
		DrawPlayerDead();
	else 
	{
		UpdatePlayer(3);
		UpdateBackground(3);
		UpdatePolyBlock(3);
		UpdateEnemy(3);
		UpdateUI();
	}
}
void DrawStage1(void)
{
	DrawBackground(3);
	DrawPolyBlock(3);
	DrawPlayer();
	DrawEnemy(3);
	DrawUI();
}