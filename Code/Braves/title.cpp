//=============================================================================
//
// [title.cpp]Å@titleÇÃî‘çÜÇÕ0
// Author : Yo Jitaku
//
//=============================================================================
#include "title.h"
#include "background.h"
#include "PolyBlock.h"
#include "player.h"

HRESULT InitTitle(void)
{
	InitBackground(0);
	InitPolyBlock(0);
	return S_OK;
}

void UninitTitle(void)
{
	UninitBackground(0);
	UninitPolyBlock(0);
}

void UpdateTitle(void)
{
	UpdateBackground(0);
	UpdatePolyBlock(0);
}

void DrawTitle(void)
{
	DrawBackground(0);
	DrawPolyBlock(0);
}