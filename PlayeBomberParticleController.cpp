//=====================================
//
//プレイヤーボンバーパーティクルコントローラ処理[PlayerBomberParticleController.cpp]
//Author:GP12B332 12 権頭
//
//=====================================
#include "PlayerBomberParticleController.h"
#include "PlayerBomberParticle.h"
#include "Framework/Easing.h"

#include <algorithm>

/***************************************
マクロ定義
***************************************/
#define PLAYERBOMBERPARTICLE_TEXTURE_NAME "data/TEXTURE/Effect/PlayerBomberParticle.png"
#define PLAYERBOMBERPARTICLE_NUM_MAX		(1024)
#define PLAYERBOMBERPARTICLE_EMITTER_MAX	(128)

/************************************************
構造体定義
*************************************************/
typedef BaseParticleController Base;
/************************************************
グローバル変数
*************************************************/

/************************************************
初期化処理
*************************************************/
void PlayerBomberParticleController::Init()
{
	D3DXVECTOR2 PLAYERBOMBERPARTICLE_SIZE = D3DXVECTOR2(7.5f, 7.5f);
	D3DXVECTOR2 PLAYERBOMBERPARTICLE_TEX_DIV = D3DXVECTOR2(1.0f, 1.0f);
	//単位頂点バッファ作成、テクスチャ読み込み
	Base::MakeUnitBuffer(&PLAYERBOMBERPARTICLE_SIZE, &PLAYERBOMBERPARTICLE_TEX_DIV);
	Base::LoadTexture(PLAYERBOMBERPARTICLE_TEXTURE_NAME);

	//パーティクルコンテナを準備
	particleContainer.resize(PLAYERBOMBERPARTICLE_NUM_MAX);
	for (UINT i = 0; i < particleContainer.size(); i++)
	{
		particleContainer[i] = new PlayerBomberParticle();
	}
	//エミッタコンテナを準備
	emitterContainer.resize(PLAYERBOMBERPARTICLE_EMITTER_MAX);
	for (UINT i = 0; i < emitterContainer.size(); i++)
	{
		emitterContainer[i] = new PlayerBomberParticleEmitter();

	}
}
/************************************************
放出処理
*************************************************/
void PlayerBomberParticleController::Emit()
{
	for (BaseEmitter* emitter : emitterContainer)
	{
		if (!emitter->active)
			continue;

		const int EmitNum = 4;		//１フレームの放出するパーティクル数
		const float InitSpeed = 0.5f;

		int emitCount = 0;
		for (BaseParticle *p : particleContainer)
		{
			if (p->active)
				continue;

			PlayerBomberParticleEmitter *entity = static_cast<PlayerBomberParticleEmitter*>(emitter);
			PlayerBomberParticle *particle = static_cast<PlayerBomberParticle*>(p);

			////移動方向設定
			D3DXVECTOR3 dir = entity->prevPos - entity->transform.pos;
			D3DXVec3Normalize(&dir, &dir);
			particle->SetMoveDir(dir);

			//座標設定
			particle->transform.pos = entity->transform.pos;

			//初期化してカウント
			particle->Init();
			emitCount++;

			//決まった数だけ放出していればbreak
			if (emitCount == EmitNum)
				break;

		}

		//決まった数だけ放出していればbreak
		if (emitCount < EmitNum)
			return;
	}
}
