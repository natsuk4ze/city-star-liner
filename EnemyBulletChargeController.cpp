//=====================================
//
//エネミーバレットチャージコントローラ処理[EnemyBulletChargeController.cpp]
//Author:GP12A332 21 立花雄太
//
//=====================================
#include "EnemyBulletChargeController.h"
#include "EnemyBulletCharge.h"

/**************************************
マクロ定義
***************************************/
#define ENEMYBULLETCHARGE_TEX_NAME		"data/TEXTURE/EFFECT/EnemyBulletCharge.png"

#define ENEMYBULLETCHARGE_NUM_MAX		(1024)
#define ENEMYBULLETCHARGE_EMITTER_MAX	(64)

#define ENEMYBULLETCHARGE_EMIT_NUM		(2)

typedef BaseParticleController Base;
/**************************************
初期化処理
***************************************/
void EnemyBulletChargeController::Init()
{
	D3DXVECTOR2 ENEMYBULLETCHARGE_SIZE = D3DXVECTOR2(2.0f, 2.0f);
	D3DXVECTOR2 ENEMYBULLETCHARGE_TEX_DIV = D3DXVECTOR2(8.0f, 8.0f);
	//単位頂点バッファ作成、テクスチャ読み込み
	Base::LoadTexture(ENEMYBULLETCHARGE_TEX_NAME);
	Base::MakeUnitBuffer(&ENEMYBULLETCHARGE_SIZE,&ENEMYBULLETCHARGE_TEX_DIV);

	//パーティクルコンテナ作成
	particleContainer.resize(ENEMYBULLETCHARGE_NUM_MAX);
	for (auto& particle : particleContainer)
	{
		particle = new EnemyBulletCharge();
		static_cast<EnemyBulletCharge*>(particle)->SetAnimParameter(&ENEMYBULLETCHARGE_TEX_DIV);
	}

	//エミッターコンテナ作成
	emitterContainer.resize(ENEMYBULLETCHARGE_EMITTER_MAX);
	for (auto& emitter : emitterContainer)
	{
		emitter = new EnemyBulletChargeEmitter();
	}
}

/**************************************
放出処理
***************************************/
void EnemyBulletChargeController::Emit()
{
	ForEachEmitter(ENEMYBULLETCHARGE_EMIT_NUM, [](BaseEmitter *emitter, BaseParticle *particle)
	{
		particle->transform = emitter->transform;

		particle->Init();
	});
}
