//=====================================
//
//スコアパーティクルコントローラ処理[ScoreParticleController.cpp]
//Author:GP12B332 21 立花雄太
//
//=====================================
#include "ScoreParticleController.h"
#include "ScoreParticle.h"
#include <algorithm>

using namespace std;

/**************************************
マクロ定義
***************************************/
#define SCOREPARTICLE_NUM_MAX		(256)
#define SCOREPARTICLE_EMITTER_MAX	(32)

#define SCOREPARTICLE_TEX_NAME		("data/TEXTURE/Effect/ScoreParticle.png")

#define SCOREPARTICLE_LIFEFRAME		(40)
#define SCOREPARTICLE_LIFE_RANGE	(10)
#define SCOREPARTICLE_SPEED_INIT	(2.0f)
#define SCOREPARTICLE_SPEED_RANGE	(2.0f)
#define SCOREPARTICLE_SPEED_TIME	(10.0f)

#define SCOREPARTICLE_DURATION		(5)
#define SCOREPARTICLE_EMIT_NUM		(10)

/**************************************
構造体定義
***************************************/
typedef BaseParticleController Base;

/**************************************
グローバル変数
***************************************/

/**************************************
プロトタイプ宣言
***************************************/

/**************************************
初期化処理
***************************************/
void ScoreParticleController::Init()
{
	D3DXVECTOR2 SCOREPARTICLE_SIZE = D3DXVECTOR2(10.0f, 10.0f);
	D3DXVECTOR2 SCOREPARTICLE_TEX_DIV = D3DXVECTOR2(1.0f, 1.0f);
	//テクスチャ読み込み、単位頂点バッファ作成
	LoadTexture(SCOREPARTICLE_TEX_NAME);
	MakeUnitBuffer(&SCOREPARTICLE_SIZE, &SCOREPARTICLE_TEX_DIV);

	//パーティクルコンテナを準備
	particleContainer.resize(SCOREPARTICLE_NUM_MAX);
	for (int i = 0; i < SCOREPARTICLE_NUM_MAX; i++)
	{
		particleContainer[i] = new ScoreParticle();
	}

	//エミッタコンテナ準備
	emitterContainer.resize(SCOREPARTICLE_EMITTER_MAX);
	for (int i = 0; i < SCOREPARTICLE_EMITTER_MAX; i++)
	{
		emitterContainer[i] = new ScoreParticleEmitter();
	}
}

/**************************************
放出処理
***************************************/
void ScoreParticleController::Emit()
{

	ForEachEmitter(SCOREPARTICLE_EMIT_NUM, [](BaseEmitter *emitter, BaseParticle *particle)
	{
		particle->transform.pos = emitter->transform.pos;
		particle->Init();
	});
}