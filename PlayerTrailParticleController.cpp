//=====================================
//
//�v���C���[�g���C���p�[�e�B�N���R���g���[������[PlayerTrailParticleController.cpp]
//Author:GP12B332 21 ���ԗY��
//
//=====================================
#include "PlayerTrailParticleController.h"
#include "PlayerTrailParticle.h"

using namespace std;

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
PlayerTrailParticleController����������
***************************************/
void PlayerTrailParticleController::Init()
{
	const D3DXVECTOR2 Size = D3DXVECTOR2(2.0f, 2.0f);
	const D3DXVECTOR2 TexDiv = D3DXVECTOR2(1.0f, 1.0f);
	const char* TextureName = "data/TEXTURE/Effect/PlayerBulletParticle.png";

	MakeUnitBuffer(&Size, &TexDiv);
	LoadTexture(TextureName);

	const int ParticleMax = 256;
	const int EmitterMax = 1;

	particleContainer.resize(ParticleMax);
	for (UINT i = 0; i < ParticleMax; i++)
	{
		particleContainer[i] = new PlayerTrailParticle();
	}

	emitterContainer.resize(EmitterMax);
	emitterContainer[0] = new PlayerTrailEmitter();
}

/**************************************
PlayerTrailParticleController�I������
***************************************/
void PlayerTrailParticleController::Uninit()
{
	vector<BaseParticle*>().swap(particleContainer);
	vector<BaseEmitter*>().swap(emitterContainer);
}

/**************************************
PlayerTrailParticleController�X�V����
***************************************/
void PlayerTrailParticleController:: Emit()
{
	PlayerTrailEmitter *emitter = static_cast<PlayerTrailEmitter*>(emitterContainer[0]);

	if (!emitter->active)
		return;

	const int EmitNum = 15;
	const float PosRange = 12.0f;

	int emitCount = 0;
	for (BaseParticle *particle : particleContainer)
	{
		if (particle->active)
			continue;

		PlayerTrailParticle *entity = static_cast<PlayerTrailParticle*>(particle);

		//�������W�ݒ�
		entity->transform.pos = emitter->transform.pos;
		entity->transform.pos.x += RandomRangef(-PosRange, PosRange);
		entity->transform.pos.y += RandomRangef(-PosRange, PosRange);
		entity->transform.pos.z += RandomRangef(-PosRange, PosRange);

		//���������ăJ�E���g
		entity->Init();
		
		emitCount++;

		if (emitCount == EmitNum)
			return;
	}
}

/**************************************
PlayerTrailParticleController�G�~�b�^�Z�b�g����
***************************************/
void PlayerTrailParticleController::SetEmitter(D3DXVECTOR3 *pPos, bool *pActive)
{
	PlayerTrailEmitter *emitter = static_cast<PlayerTrailEmitter*>(emitterContainer[0]);
	emitter->SetParent(pPos, pActive);
	emitter->active = true;
}