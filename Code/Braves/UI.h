//=============================================================================
//
// [UI.h]Å@
// Author : Yo Jitaku
//
//=============================================================================
#pragma once
#include "DirectX.h"

#define UI_COMPONENT (13)
#define UI_TEXTURE (2)
typedef struct
{
	D3DXVECTOR3 pos;
	D3DXVECTOR2 size;
	bool bUse;
	int nPatternAnime, nTexture;
	int nDisplayNumber, nZeroNumber, nMultipleNumber; //score Ç…Ç¬Ç¢Çƒ
}UI;

void SetUI(void);
HRESULT InitUI(void);
void UninitUI(void);
void UpdateUI(void);
void DrawUI(void);
