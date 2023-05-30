//=====================================
//
//エネミーエクスプロージョンフレアコントローラ処理[EnemyExplosionFlareController.cpp]
//Author:GP12B332 21 立花雄太
//
//=====================================
#include "EnemyExplosionFlareController.h"
#include "EnemyExplosionFlare.h"

#include <algorithm>

/**************************************
マクロ定義
***************************************/
#define ENEMYEXPLOSIONFLARE_MAX			(1024)
#define ENEMYEXPLOSIONFLARE_EMIT_MAX	(64)

#define ENEMYEXPLOSIONFLARE_TEX_NAME	"data/TEXTURE/Effect/EnemyExplosionFlare.png"

#define ENEMYEXPLOSIONFLARE_EMIT_NUM	(10)

typedef BaseParticleController Base;

/**************************************
初期化処理
***************************************/
void EnemyExplosionFlareController::Init()
{
	D3DXVECTOR2 ENEMYEXPLOSIONFLARE_SIZE = D3DXVECTOR2(4.0f, 4.0f);
	D3DXVECTOR2 ENEMYEXPLOSIONFLARE_TEX_DIV = D3DXVECTOR2(1.0f, 1.0f);

	//単位頂点バッファ作成、テクスチャ読み込み
	Base::MakeUnitBuffer(&ENEMYEXPLOSIONFLARE_SIZE, &ENEMYEXPLOSIONFLARE_TEX_DIV);
	Base::LoadTexture(ENEMYEXPLOSIONFLARE_TEX_NAME);

	//パーティクルコンテナ準備
	particleContainer.resize(ENEMYEXPLOSIONFLARE_MAX);
	for (auto& particle : particleContainer)
	{
		particle = new EnemyExplosionFlare();
	}

	//エミッタコンテナ準備
	emitterContainer.resize(ENEMYEXPLOSIONFLARE_EMIT_MAX);
	for (auto& emitter : emitterContainer)
	{
		emitter = new EnemyExplosionFlareEmitter();
	}
}

/**************************************
放出処理 
***************************************/
void EnemyExplosionFlareController::Emit()
{
	Base::ForEachEmitter(ENEMYEXPLOSIONFLARE_EMIT_NUM, [](BaseEmitter* emitter, BaseParticle *particle)
	{
		//座標設定
		particle->transform.pos = emitter->transform.pos;

		//初期化してカウント
		particle->Init();
	});
}