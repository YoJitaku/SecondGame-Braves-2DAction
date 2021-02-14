#include "Tutorial.h"
#include "background.h"
#include "polyblock.h"
//=============================================================================
//
// [Tutorial.cpp]Å@
// Author : Yo Jitaku
//
//=============================================================================
#include "Input.h"
#include "Fade.h"
#include "player.h"

HRESULT InitTutorial(void)
{
	InitBackground(2);
	return S_OK;
}
void UninitTutorial(void)
{
	UninitBackground(2);
}
void UpdateTutorial(void)
{
	UpdateBackground(2);
	if ((GetkeyboardTrigger(DIK_RETURN) == true))
	{
		SetFade(FADE_OUT, MODE_MENU);
		UpdatePlayerFade();
	}
}
void DrawTutorial(void)
{
	DrawBackground(2);
}