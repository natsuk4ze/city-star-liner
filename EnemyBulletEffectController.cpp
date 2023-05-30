//=====================================
//
//エネミーバレットエフェクトコントローラ処理[EnemyBulletEffectController.cpp]
//Author:GP12A332 21 立花雄太
//
//=====================================
#include "EnemyBulletEffectController.h"
#include "EnemyBulletEffect.h"
#include "Framework\Easing.h"

#include <algorithm>

using namespace std;
/**************************************
マクロ定義
***************************************/
#define ENEMYBULLETEFFECT_TEX_NAME	"data/TEXTURE/Effect/EnemyBulletEffect.png"
#define ENEMYBULLETEFFECT_NUM_MAX	(1024)

#define ENEMYBULLETEFFECT_EMITTER_MAX	(32)
#define ENEMYBULLETEFFECT_EMIT_NUM		(10)

#define ENEMYBULLETEFFECT_SHRINK_LENGTH	(10.0f)

typedef BaseParticleController Base;
/**************************************
初期化処理
***************************************/
void EnemyBulletEffectController::Init()
{
	D3DXVECTOR2 ENEMYBULLETEFFECT_SIZE = D3DXVECTOR2(5.0f, 5.0f);
	D3DXVECTOR2 ENEMYBULLETEFFECT_TEX_DIV = D3DXVECTOR2(8.0f, 8.0f);
	//単位頂点作成、テクスチャ読み込み
	Base::MakeUnitBuffer(&ENEMYBULLETEFFECT_SIZE, &ENEMYBULLETEFFECT_TEX_DIV);
	Base::LoadTexture(ENEMYBULLETEFFECT_TEX_NAME);

	//パーティクルコンテナ作成
	particleContainer.resize(ENEMYBULLETEFFECT_NUM_MAX);
	for (auto& particle : particleContainer)
	{
		particle = new EnemyBulletEffect();
		static_cast<EnemyBulletEffect*>(particle)->SetAnimParameter(&ENEMYBULLETEFFECT_TEX_DIV);
	}

	//エミッターコンテナ作成
	emitterContainer.resize(ENEMYBULLETEFFECT_EMITTER_MAX);
	for (auto& emitter : emitterContainer)
	{
		emitter = new EnemyBulletEffectEmitter();
	}
}

/**************************************
放出処理
***************************************/
void EnemyBulletEffectController::Emit()
{
	Base::ForEachEmitter(ENEMYBULLETEFFECT_EMIT_NUM, [](BaseEmitter* emitter, BaseParticle *particle)
	{
		EnemyBulletEffectEmitter *entity = static_cast<EnemyBulletEffectEmitter*>(emitter);
		
		
		float t = RandomRangef(0.0f, 1.0f);
		particle->transform.pos = Easing::EaseValue(t, entity->edgeL, entity->edgeR, EaseType::Linear);

		particle->Init();
		
	});
}

/**************************************
エミッターセット処理
***************************************/
BaseEmitter* EnemyBulletEffectController::SetEmitter(LineTrailModel model)
{
	auto itr = find_if(emitterContainer.begin(), emitterContainer.end(), [](BaseEmitter* emitter)
	{
		return !emitter->active;
	});

	if (itr == emitterContainer.end())
		return NULL;

	EnemyBulletEffectEmitter *entity = static_cast<EnemyBulletEffectEmitter*>(*itr);

	D3DXVECTOR3 edgeR, edgeL;
	model.GetEdgePos(&edgeR, &edgeL);

	D3DXVECTOR3 diff = edgeR - edgeL;
	D3DXVec3Normalize(&diff, &diff);
	entity->edgeL = edgeL + diff * ENEMYBULLETEFFECT_SHRINK_LENGTH;
	entity->edgeR = edgeR - diff * ENEMYBULLETEFFECT_SHRINK_LENGTH;

	entity->Init();

	return entity;
}
