//=============================================================================
//
// �T�E���h���� [sound.h]
// Author : AKIRA TANAKA
//
//=============================================================================
#ifndef _SOUND_H_
#define _SOUND_H_

#include "DirectX.h"

//*****************************************************************************
// �T�E���h�t�@�C��
//*****************************************************************************
typedef enum
{
	SOUND_LABEL_BGM = 0,
	SOUND_LABEL_SE_ATTACK,
	SOUND_LABEL_SE_COIN,
	SOUND_LABEL_SE_DEAD,
	SOUND_LABEL_SE_DOOR,
	SOUND_LABEL_SE_GETHIT,
	SOUND_LABEL_SE_HIT,
	SOUND_LABEL_SE_RELIVE,
	SOUND_LABEL_SE_HP,
	//SOUND_LABEL_SE_REFLECTION,
	SOUND_LABEL_MAX,
} SOUND_LABEL;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitSound(HWND hWnd);
void UninitSound(void);
HRESULT PlaySound(SOUND_LABEL label);
void StopSound(SOUND_LABEL label);
void StopSound(void);

#endif
