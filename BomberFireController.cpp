//=====================================
//
//ボンバーファイアコントローラ処理[BomberFireController.cpp]
//Author:GP12A332 21 立花雄太
//
//=====================================
#include "BomberFireController.h"
#include "BomberFire.h"

/**************************************
マクロ定義
***************************************/
#define BOBMERFIRE_TEXTURE_NAME		"data/TEXTURE/Effect/BomberFire.png"

#define BOMBERFIRE_NUM_MAX			(512)
#define BOMBERFIRE_EMITTER_NUM		(1)
#define BOMBERFIRE_EMIT_NUM			(170)

/**************************************
初期化処理
***************************************/
void BomberFireController::Init()
{
	D3DXVECTOR2 BOMBERFIRE_TEXTURE_DIV = D3DXVECTOR2(8.0f, 8.0f);
	D3DXVECTOR2 BOMBERFIRE_SIZE = D3DXVECTOR2(5.0f, 5.0f);
	//単位頂点バッファ作成、テクスチャロード
	MakeUnitBuffer(&BOMBERFIRE_SIZE, &BOMBERFIRE_TEXTURE_DIV);
	LoadTexture(BOBMERFIRE_TEXTURE_NAME);

	//パラメータコンテナ作成
	particleContainer.resize(BOMBERFIRE_NUM_MAX);
	for (auto& particle : particleContainer)
	{
		particle = new BomberFire();
	}

	//エミッタコンテナ作成
	emitterContainer.resize(BOMBERFIRE_EMITTER_NUM);
	for (auto& emitter : emitterContainer)
	{
		emitter = new BomberFireEmitter();
	}
}

/**************************************
放出処理
***************************************/
void BomberFireController::Emit()
{
	ForEachEmitter(BOMBERFIRE_EMIT_NUM, [](BaseEmitter* emitter, BaseParticle* particle)
	{
		particle->transform.pos = emitter->transform.pos;
		particle->Init();
	});
}

