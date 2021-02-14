//=============================================================================
//
// [stage2.cpp]@
// Author : Yo Jitaku
//
//=============================================================================
#include "stage2.h"
#include "background.h"
#include "polyblock.h"
#include "player.h"
#include "UI.h"
#include "enemy.h"
HRESULT InitStage2(void)
{
	InitBackground(4);
	InitPolyBlock(4);
	InitEnemy(4);
	return S_OK;
}
void UninitStage2(void)
{
	UninitBackground(4);
	UninitEnemy(4);
	UninitPolyBlock(4);
}
void UpdateStage2(void)
{
	PLAYER *pPlayer = GetPlayer();

	if (pPlayer->nState == P_DIED)
		DrawPlayerDead();
	else
	{
		UpdatePlayer(4);
		UpdateBackground(4);
		UpdatePolyBlock(4);
		UpdateEnemy(4);
		UpdateUI();
	}
}
void DrawStage2(void)
{
	DrawBackground(4);
	DrawPolyBlock(4);
	DrawPlayer();
	DrawEnemy(4);
	DrawUI();
}