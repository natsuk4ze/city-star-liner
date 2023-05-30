//=====================================
//
//プレイヤーバレットパーティクルコントローラ処理[PlayerBulletParticleController.cpp]
//Author:GP12B332 21 立花雄太
//
//=====================================
#include "PlayerBulletParticleController.h"
#include "PlayerBulletParticle.h"
#include "Framework\Easing.h"

#include <algorithm>

/**************************************
マクロ定義
***************************************/
#define PLAYERBULLETPARTICLE_TEXTURE_NAME	"data/TEXTURE/Effect/PlayerBulletParticle.png"
#define PLAYERBULLETPARTCILE_NUM_MAX		(1024)
#define PLAYERBULLETPARTICLE_EMITTER_MAX	(8)

#define PLAYERBULLETPARTICLE_LIFEFRAME		(10)
#define PLAYERBULLETPARTTCLE_LIFE_RANGE		(3)

/**************************************
構造体定義
***************************************/
typedef BaseParticleController Base;

/**************************************
グローバル変数
***************************************/

/**************************************
初期化処理
***************************************/
void PlayerBulletParticleController::Init()
{
	D3DXVECTOR2 PLAYERBULLETPARTICLE_SIZE = D3DXVECTOR2(2.0f, 2.0f);
	D3DXVECTOR2 PLAYERBULLETPARTICLE_TEX_DIV = D3DXVECTOR2(1.0f, 1.0f);
	//単位頂点バッファ作成、テクスチャ読み込み
	Base::MakeUnitBuffer(&PLAYERBULLETPARTICLE_SIZE, &PLAYERBULLETPARTICLE_TEX_DIV);
	Base::LoadTexture(PLAYERBULLETPARTICLE_TEXTURE_NAME);

	//パーティクルコンテナを準備
	particleContainer.resize(PLAYERBULLETPARTCILE_NUM_MAX);
	for (UINT i = 0; i < particleContainer.size(); i++)
	{
		particleContainer[i] = new PlayerBulletParticle();
	}

	//エミッタコンテナを準備
	emitterContainer.resize(PLAYERBULLETPARTICLE_EMITTER_MAX);
	for (UINT i = 0; i < emitterContainer.size(); i++)
	{
		emitterContainer[i] = new PlayerBulletParticleEmitter();
	}
}

/**************************************
放出処理
***************************************/
void PlayerBulletParticleController::Emit()
{
	for (BaseEmitter* emitter : emitterContainer)
	{
		if (!emitter->active)
			continue;

		const int EmitNum = 10;			//1フレームの放出するパーティクル数
		const float InitSpeed = 2.0f;	//パーティクルの初期スピード

		int emitCount = 0;
		for (BaseParticle *p : particleContainer)
		{
			if (p->active)
				continue;

			PlayerBulletParticleEmitter *entity = static_cast<PlayerBulletParticleEmitter*>(emitter);
			PlayerBulletParticle *particle = static_cast<PlayerBulletParticle*>(p);

			//移動方向設定
			particle->moveDir.x = RandomRangef(-1.0f, 1.0f);
			particle->moveDir.y = RandomRangef(-1.0f, 1.0f);
			particle->moveDir.z = -2.0f;
			D3DXVec3Normalize(&particle->moveDir, &particle->moveDir);

			//寿命、スピード設定
			particle->lifeFrame = PLAYERBULLETPARTICLE_LIFEFRAME + RandomRange(-PLAYERBULLETPARTTCLE_LIFE_RANGE, PLAYERBULLETPARTTCLE_LIFE_RANGE);
			particle->speed = InitSpeed;

			//座標設定
			float t = RandomRangef(0.0f, 1.0f);
			particle->transform.pos = Easing::EaseValue(t, entity->edgeLeft, entity->edgeRight, EaseType::Linear);
			particle->transform.pos.z = entity->transform.pos.z;

			//スケール設定
			particle->transform.scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
			particle->transform.scale.x += fabsf(particle->moveDir.x) * 10.0f;
			particle->transform.scale.y += fabsf(particle->moveDir.y) * 10.0f;

			//初期化してカウント
			particle->Init();
			emitCount++;

			//決まった数だけ放出していればbreak
			if (emitCount == EmitNum)
				break;
		}

		//放出できるパーティクルが無いのでリターン
		if (emitCount < EmitNum)
			return;

	}
}

/**************************************
エミッタセット処理
***************************************/
void PlayerBulletParticleController::SetEmitter(D3DXVECTOR3 *pPos, bool *pActive, D3DXVECTOR3 *edgeRight, D3DXVECTOR3 *edgeLeft)
{
	//非アクティブのエミッタを検索
	auto itr = find_if(emitterContainer.begin(), emitterContainer.end(),
		[](BaseEmitter* p)
	{
		return !p->active;
	});

	//無かったのでリターン
	if (itr == emitterContainer.end())
		return;

	//初期化
	PlayerBulletParticleEmitter *emitter = static_cast<PlayerBulletParticleEmitter*>(*itr);
	emitter->parentActive = pActive;
	emitter->parentPos = pPos;
	emitter->edgeLeft = *edgeLeft;
	emitter->edgeRight = *edgeRight;
	emitter->Init();
}