//=============================================================================
//
// [collision.h]　外積を使用しての当たり判定
// Author : Yo Jitaku
//
//=============================================================================
#pragma once
#include "DirectX.h"

#define PLAYER_VTX 12//playerの辺で判断したい点は全8個(四つの頂点と上下左右の真ん中の点)
#define BLOCK_VTX 4//判定させる目標(block)、四つの頂点と辺vector
#define ENEMY_VTX 3//Enemyの辺で判断したい点は全3個(下左右の真ん中の点)
//当たり判定した結果
typedef enum
{
	CS_UP = 0,//乗る
	CS_DOWN,
	CS_LEFT,
	CS_RIGHT,
	CS_MAX
}COLLISION_STATE;

bool Vtx_Vector_ColDet(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2, D3DXVECTOR2 size2);
COLLISION_STATE Player_ColDet(D3DXVECTOR3 Pos1, D3DXVECTOR2 Size1, D3DXVECTOR3 Pos2, D3DXVECTOR2 Size2);//player pos and size と　判定目標(block)のpos and size
COLLISION_STATE Enemy_ColDet(D3DXVECTOR3 Pos1, D3DXVECTOR2 Size1, D3DXVECTOR3 Pos2, D3DXVECTOR2 Size2);