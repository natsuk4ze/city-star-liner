//=============================================================================
//
// �����L���O���� [rank.h]
// Author : �n糗Ǒ�
//
//=============================================================================
#ifndef _RANK_H
#define _RANK_H

#include "main.h"
#include "UIdrawer.h"

#define	ADRESS_TEXTURE_NUMBER_RANK	"data/TEXTURE/UI/number.png"	// �ǂݍ��ރe�N�X�`���t�@�C����
#define	ADRESS_TEXTURE_BG_RANK1	"data/TEXTURE/UI/Result/rank1.jpg"	// �ǂݍ��ރe�N�X�`���t�@�C����
#define	ADRESS_TEXTURE_BG_RANK2	"data/TEXTURE/UI/Result/rank2.jpg"	// �ǂݍ��ރe�N�X�`���t�@�C����
#define	ADRESS_TEXTURE_BG_RANK3	"data/TEXTURE/UI/Result/rank3.jpg"	// �ǂݍ��ރe�N�X�`���t�@�C����
#define	ADRESS_TEXTURE_BG_RANK4	"data/TEXTURE/UI/Result/rank4.jpg"	// �ǂݍ��ރe�N�X�`���t�@�C����
#define	ADRESS_TEXTURE_BG_RANK5	"data/TEXTURE/UI/Result/rank5.jpg"	// �ǂݍ��ރe�N�X�`���t�@�C����
#define	ADRESS_TEXTURE_BG_RANK6	"data/TEXTURE/UI/Result/myrank.jpg"	// �ǂݍ��ރe�N�X�`���t�@�C����

#define MYSCORE (5)
#define RANK_MAX (6)
#define ARRAY_MAX (7)		//�ő�z��
#define SIZE_RANK		(D3DXVECTOR3(30.0f,40.0f,0.0f))
#define SIZE_BG_RANK		(D3DXVECTOR3(210.0f,150.0f,0.0f))
#define POSITION_BG_RANK_TOP			(D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 6, 0.0))
#define POSITION_BG_RANK_MIDDLE_LEFT	(D3DXVECTOR3(SCREEN_WIDTH / 5, SCREEN_HEIGHT / 5*2, 0.0f))
#define POSITION_BG_RANK_LOWER_LEFT		(D3DXVECTOR3(SCREEN_WIDTH / 5, SCREEN_HEIGHT / 5*4, 0.0f))
#define POSITION_BG_RANK_LOWER_RIGHT	(D3DXVECTOR3(SCREEN_WIDTH / 1.3, SCREEN_HEIGHT / 5*4, 0.0f))
#define POSITION_BG_RANK_MIDDLE_RIGHT	(D3DXVECTOR3(SCREEN_WIDTH / 1.3, SCREEN_HEIGHT / 5*2, 0.0f))
#define POSITION_BG_RANK_CENTER			(D3DXVECTOR3(SCREEN_WIDTH / 2 ,SCREEN_HEIGHT / 1.8, 0.0f))

#define POSITION_RANK_TOP			(D3DXVECTOR3(SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 6 + 1000, 0.0f))
#define POSITION_RANK_MIDDLE_LEFT	(D3DXVECTOR3(SCREEN_WIDTH / 5 - 1500, SCREEN_HEIGHT / 5*2, 0.0f))
#define POSITION_RANK_LOWER_LEFT	(D3DXVECTOR3(SCREEN_WIDTH / 5 - 1500, SCREEN_HEIGHT / 5*4, 0.0f))
#define POSITION_RANK_LOWER_RIGHT	(D3DXVECTOR3(SCREEN_WIDTH / 1.3 + 1500, SCREEN_HEIGHT / 5*4, 0.0f))
#define POSITION_RANK_MIDDLE_RIGHT	(D3DXVECTOR3(SCREEN_WIDTH / 1.3 + 1500, SCREEN_HEIGHT / 5*2, 0.0f))
#define POSITION_RANK_CENTER		(D3DXVECTOR3(SCREEN_WIDTH / 2 - 80 ,SCREEN_HEIGHT / 1.8 + 10, 0.0f))

typedef struct
{
	int g_scorerank; //����

} SCORERANK;


HRESULT InitRank(void);

void UninitRank(void);

void UpdateRank(void);

void DrawRank(void);

void RankSort(void);

SCORERANK *GetRank();

#endif