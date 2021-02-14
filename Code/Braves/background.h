//=============================================================================
//
// [BackGround.h]
// Author : Yo Jitaku
//
//=============================================================================
#include "DirectX.h"

#define BACKGROUND_NUM (6)//StageÇÃêî

typedef struct
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 move;
	D3DXVECTOR2 size;
	int nType;
	float fSpeed, fTexture;
}BACKGROUND;

void SetBackground(int nStage);
HRESULT InitBackground(int nStage);
void UninitBackground(int nStage);
void UpdateBackground(int nStage);
void DrawBackground(int nStage);
BACKGROUND GetBackground(void);