//=====================================
//
//�x�[�X�G�~�b�^����[BaseEmitter.cpp]
//Author:GP12B332 21 ���ԗY��
//
//=====================================
#include "BaseEmitter.h"

/**************************************
�}�N����`
***************************************/

/**************************************
�\���̒�`
***************************************/

/**************************************
�O���[�o���ϐ�
***************************************/

/**************************************
�R���X�g���N�^
***************************************/
BaseEmitter::BaseEmitter(D3DXVECTOR3 *pos)
{
	this->transform.pos = *pos;
	active = true;
}

/**************************************
�R���X�g���N�^
***************************************/
BaseEmitter::BaseEmitter()
{
	active = false;
}

/**************************************
�f�X�g���N�^
***************************************/
BaseEmitter::~BaseEmitter()
{

}