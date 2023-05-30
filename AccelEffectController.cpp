//=====================================
//
//アクセルエフェクトコントローラ処理[AccelEffectController.cpp]
//Author:GP12A332 21 立花雄太
//
//=====================================
#include "AccelEffectController.h"
#include "AccelEffectParticle.h"

/**************************************
マクロ定義
***************************************/
#define ACCELEEFFECT_TEX_NAME		"data/TEXTURE/Effect/AccelEffect.png"
#define ACCELEFFECT_NUM_MAX			(512)
#define ACCELEFFECT_EMITTER_MAX		(4)
#define ACCELEFFEFCT_EMIT_NUM		(50)

/**************************************
初期化処理
***************************************/
void AccelEffectController::Init()
{
	D3DXVECTOR2 ACCELEEFFECT_TEX_DIV = D3DXVECTOR2(1.0f, 1.0f);
	D3DXVECTOR2 ACCELEFFECT_SIZE = D3DXVECTOR2(3.0f, 25.0f);

	//単位頂点バッファ作成、テクスチャ読み込み
	MakeUnitBuffer(&ACCELEFFECT_SIZE, &ACCELEEFFECT_TEX_DIV);
	LoadTexture(ACCELEEFFECT_TEX_NAME);

	//パーティクルコンテナ作成
	particleContainer.resize(ACCELEFFECT_NUM_MAX);
	for (auto& particle : particleContainer)
	{
		particle = new AccelEffectParticle();
	}

	//エミッタコンテナ作成
	emitterContainer.resize(ACCELEFFECT_EMITTER_MAX);
	for (auto& emitter : emitterContainer)
	{
		emitter = new AccelEffectParticleEmitter();
	}
}

/**************************************
放出処理
***************************************/
void AccelEffectController::Emit()
{
	ForEachEmitter(ACCELEFFEFCT_EMIT_NUM, [](BaseEmitter *emitter, BaseParticle* particle)
	{
		particle->transform = emitter->transform;
		particle->Init();
	});
}
