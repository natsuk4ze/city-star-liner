//=====================================
//
//�v���C���[�{���o�[�w�b�_[PlayerBomber.h]
//Author:GP12B332 12 ����
//
//=====================================
#ifndef _PLAYERBOMBER_H_
#define _PLAYERBOMBER_H_

#include "main.h"

/**************************************
�}�N����`
***************************************/

/**************************************
�\���̒�`
***************************************/


/**************************************
�v���g�^�C�v�錾
***************************************/
class PlayerBomber
{
public:
	PlayerBomber();				//�R���X�g���N�^
	~PlayerBomber();				//�f�X�g���N�^

	void Init();				//����������
	void Uninit();				//�I������
	void Update();				//�X�V����
	void Draw();				//�`�揈��

	void Set(D3DXVECTOR3 *pos,D3DXVECTOR3 initpos);
	void CalcBomber(void);


	Transform transform;
	D3DXVECTOR3 *targetPos;
	D3DXVECTOR3 velocity;
	bool active;
	int cntFrame;
	int reachFrame;

	static int instanceCount;

};





#endif