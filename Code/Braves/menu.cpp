//=============================================================================
//
// [menu.cpp]Å@
// Author : Yo Jitaku
//
//=============================================================================
#include "menu.h"
#include "background.h"
#include "PolyBlock.h"
#include "player.h"

HRESULT InitMenu(void)
{
	InitBackground(1);
	InitPolyBlock(1);
	InitPlayer(1);
	return S_OK;
}

void UninitMenu(void)
{
	UninitBackground(1);
	UninitPolyBlock(1);
}

void UpdateMenu(void)
{
	UpdateBackground(1);
	UpdatePolyBlock(1);
	UpdatePlayer(1);
}

void DrawMenu(void)
{
	DrawBackground(1);
	DrawPolyBlock(1);
	DrawPlayer();
}