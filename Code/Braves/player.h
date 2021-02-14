//=============================================================================
//
// [player.h]Å@
// Author : Yo Jitaku
//
//=============================================================================
#pragma once
#include "DirectX.h"
#include "PolyBlock.h"
#define MAX_PLAYER_TEXTURE (9)

typedef struct
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 prepos;
	D3DXVECTOR3 move;
	D3DXVECTOR2 size;
	D3DXCOLOR color;
	bool bUse, bJump, bAnimeChange;
	float fGravity, fSave;
	int nDirection, nState, nAnime, nLife, nLastDoorSave, nScore, nTime, nJump, nDoor;
}PLAYER;

typedef enum
{
	P_IDLE = 0,
	P_WALK,
	P_ATTACK1,
	P_ATTACK2,
	P_ATTACK3,
	P_JUMP,//5
	P_DEFEND,
	P_REFLECTION,
	P_DIED,
	P_HURT,
	P_MAX,
}PLAYERSTATE;

void SetPlayer(int nStage);
HRESULT InitPlayer(int nStage);
void UninitPlayer(void);
void UpdatePlayer(int nStage);
void DrawPlayer(void);
PLAYER *GetPlayer(void);
void UpdatePlayerFade(void);
void ResetPlayer(int nStage);
void DrawPlayerDead(void);