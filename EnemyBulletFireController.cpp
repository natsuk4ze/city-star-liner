//=====================================
//
//エネミーバレットファイアコントローラ処理[EnemyBulletFireController.cpp]
//Author:GP12A332 21 立花雄太
//
//=====================================
#include "EnemyBulletFireController.h"
#include "EnemyBulletFire.h"

/**************************************
マクロ定義
***************************************/
#define ENEMYBULLETFIRE_TEX_NAME		"data/TEXTURE/Effect/EnemyBulletFire.png"

#define ENEMYBULLETFIRE_NUM_MAX			(1024)
#define ENEMYBULLETFIRE_EMITTER_MAX		(64)
#define ENEMYBULLERFIRE_EMIT_NUM		(10)

typedef BaseParticleController Base;
/**************************************
初期化処理
***************************************/
void EnemyBulletFireController::Init()
{
	D3DXVECTOR2 ENEMYBULLETFIRE_SIZE = D3DXVECTOR2(3.0f, 3.0f);
	D3DXVECTOR2 ENEMYBULLETFIRE_TEX_DIV = D3DXVECTOR2(1.0f, 1.0f);
	//単位頂点バッファ作成、テクスチャ読み込み
	Base::MakeUnitBuffer(&ENEMYBULLETFIRE_SIZE, &ENEMYBULLETFIRE_TEX_DIV);
	Base::LoadTexture(ENEMYBULLETFIRE_TEX_NAME);

	//パーティクルコンテナ作成
	particleContainer.resize(ENEMYBULLETFIRE_NUM_MAX);
	for (auto& particle : particleContainer)
	{
		particle = new EnemyBulletFire();
	}

	//エミッターコンテナ作成
	emitterContainer.resize(ENEMYBULLETFIRE_EMITTER_MAX);
	for (auto& emitter : emitterContainer)
	{
		emitter = new EnemyBulletFireEmitter();
	}

}

/**************************************
放出処理
***************************************/
void EnemyBulletFireController::Emit()
{
	Base::ForEachEmitter(ENEMYBULLERFIRE_EMIT_NUM, [](BaseEmitter* emitter, BaseParticle* particle)
	{
		particle->transform.pos = emitter->transform.pos;
		particle->Init();
	});
}
