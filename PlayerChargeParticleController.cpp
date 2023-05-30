//=====================================
//
//プレイヤーチャージパーティクルコントローラ処理[PlayerChargeParticleController.cpp]
//Author:GP12A332 21 立花雄太
//
//=====================================
#include "PlayerChargeParticleController.h"
#include "PlayerChargeParticle.h"

/**************************************
マクロ定義
***************************************/
#define PLAYERCHARGEPARTICLE_TEX_NAME		"data/TEXTURE/Effect/PlayerBulletParticle.png"


#define PLAYERCHARGEPARTICLE_NUM_MAX		(512)
#define PLAYERCHARGEPARTICLE_EMITTER_MAX	(4)
#define PLAYERCHARGEPARTICLE_EMIT_NUM		(20)

/**************************************
初期化処理
***************************************/
void PlayerChargeParticleController::Init()
{
	D3DXVECTOR2 PLAYERCHARGEPARTICLE_SIZE = D3DXVECTOR2(2.0f, 2.0f);
	D3DXVECTOR2 PLAYERCHARGEPARTICLE_TEX_DIV = D3DXVECTOR2(1.0f, 1.0f);
	//単位頂点バッファ作成、テクスチャ読み込み
	MakeUnitBuffer(&PLAYERCHARGEPARTICLE_SIZE, &PLAYERCHARGEPARTICLE_TEX_DIV);
	LoadTexture(PLAYERCHARGEPARTICLE_TEX_NAME);

	//パーティクルコンテナ作成
	particleContainer.resize(PLAYERCHARGEPARTICLE_NUM_MAX);
	for (auto& particle : particleContainer)
	{
		particle = new PlayerChargeParticle();
	}

	//エミッターコンテナ作成
	emitterContainer.resize(PLAYERCHARGEPARTICLE_EMITTER_MAX);
	for (auto& emitter : emitterContainer)
	{
		emitter = new PlayerChargeParticleEmitter();
	}
}

/**************************************
放出処理
***************************************/
void PlayerChargeParticleController::Emit()
{
	ForEachEmitter(PLAYERCHARGEPARTICLE_EMIT_NUM, [](BaseEmitter* emitter, BaseParticle* particle)
	{
		particle->transform.pos = emitter->transform.pos;
		particle->Init();
	});
}
