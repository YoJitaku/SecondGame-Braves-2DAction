//=============================================================================
//
// [collision.h]�@�O�ς��g�p���Ă̓����蔻��
// Author : Yo Jitaku
//
//=============================================================================
#pragma once
#include "DirectX.h"

#define PLAYER_VTX 12//player�̕ӂŔ��f�������_�͑S8��(�l�̒��_�Ə㉺���E�̐^�񒆂̓_)
#define BLOCK_VTX 4//���肳����ڕW(block)�A�l�̒��_�ƕ�vector
#define ENEMY_VTX 3//Enemy�̕ӂŔ��f�������_�͑S3��(�����E�̐^�񒆂̓_)
//�����蔻�肵������
typedef enum
{
	CS_UP = 0,//���
	CS_DOWN,
	CS_LEFT,
	CS_RIGHT,
	CS_MAX
}COLLISION_STATE;

bool Vtx_Vector_ColDet(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2, D3DXVECTOR2 size2);
COLLISION_STATE Player_ColDet(D3DXVECTOR3 Pos1, D3DXVECTOR2 Size1, D3DXVECTOR3 Pos2, D3DXVECTOR2 Size2);//player pos and size �Ɓ@����ڕW(block)��pos and size
COLLISION_STATE Enemy_ColDet(D3DXVECTOR3 Pos1, D3DXVECTOR2 Size1, D3DXVECTOR3 Pos2, D3DXVECTOR2 Size2);