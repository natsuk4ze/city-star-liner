//=====================================
//
//エネミーバレットトレイルコントローラ処理[EnemyBulletTrailController.cpp]
//Author:GP12A332 21 立花雄太
//
//=====================================
#include "EnemyBulletTrailController.h"
#include "EnemyBulletTrail.h"

/**************************************
マクロ定義
***************************************/
#define ENEMYBULLETTRAIL_TEX_NAME		"data/TEXTURE/Effect/EnemyBulletTrail.png"
#define ENEMYBULLETTRAIL_NUM_MAX		(1024)
#define ENEMYBULLETTRAIL_EMITTER_MAX	(32)
#define ENEMYBULLETTRAIL_EMIT_NUM		(5)

typedef BaseParticleController Base;
/**************************************
初期化処理
***************************************/
void EnemyBulletTrailController::Init()
{
	D3DXVECTOR2 ENEMYBULLETTRAIL_SIZE = D3DXVECTOR2(10.0f, 10.0f);
	D3DXVECTOR2 ENEMYBULLETTRAIL_TEX_DIV	= D3DXVECTOR2(8.0f, 8.0f);
	//頂点バッファ設定、テクスチャ読み込み
	Base::MakeUnitBuffer(&ENEMYBULLETTRAIL_SIZE, &ENEMYBULLETTRAIL_TEX_DIV);
	Base::LoadTexture(ENEMYBULLETTRAIL_TEX_NAME);

	//パーティクルコンテナ作成
	particleContainer.resize(ENEMYBULLETTRAIL_NUM_MAX);
	for (auto& particle : particleContainer)
	{
		particle = new EnemyBulletTrail();
		static_cast<AnimationParticle*>(particle)->SetAnimParameter(&ENEMYBULLETTRAIL_TEX_DIV);
	}

	//エミッターコンテナ作成
	emitterContainer.resize(ENEMYBULLETTRAIL_EMITTER_MAX);
	for(auto& emitter : emitterContainer)
	{ 
		emitter = new EnemyBulletTrailEmitter();
	}
}

/**************************************
放出処理
***************************************/
void EnemyBulletTrailController::Emit()
{
	//放出
	Base::ForEachEmitter(ENEMYBULLETTRAIL_EMIT_NUM, [](BaseEmitter *emitter, BaseParticle *particle)
	{
		//座標設定
		particle->transform.pos = emitter->transform.pos;

		//移動方向設定
		EnemyBulletTrail *entityParticle = static_cast<EnemyBulletTrail*>(particle);
		EnemyBulletTrailEmitter *entityEmitter = static_cast<EnemyBulletTrailEmitter*>(emitter);
		entityParticle->moveDir = entityEmitter->prevPos - entityEmitter->transform.pos;
		D3DXVec3Normalize(&entityParticle->moveDir, &entityParticle->moveDir);

		//初期化
		particle->Init();
	});

	//現在位置を保存
	for (auto& emitter : emitterContainer)
	{
		if (!emitter->active)
			continue;

		EnemyBulletTrailEmitter *entity = static_cast<EnemyBulletTrailEmitter*>(emitter);
		entity->prevPos = entity->transform.pos;
	}
}