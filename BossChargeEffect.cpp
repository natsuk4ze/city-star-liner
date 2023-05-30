//=====================================
//
//ボスチャージパーティクル処理[BossChargeParticle.cpp]
//Author:GP12A332 21 立花雄太
//
//=====================================
#include "BossChargeEffect.h"

/**************************************
マクロ定義
***************************************/

/**************************************
BossChargeParticleコンストラクタ
***************************************/
BossChargeParticle::BossChargeParticle()
{
	D3DXVECTOR2 temp = D3DXVECTOR2(8.0f, 8.0f);
	SetAnimParameter(&temp);
}

/**************************************
BossChargeParticle初期化
***************************************/
void BossChargeParticle::Init()
{
	cntFrame = 0;
	active = true;

	transform.Rotate(0.0f, 0.0f, RandomRangef(0.0f, 360.0f));

	const float PosRange = 5.0f * transform.scale.x;
	transform.pos.x += RandomRangef(-PosRange, PosRange);
	transform.pos.y += RandomRangef(-PosRange, PosRange);

	lifeFrame = 60;
}

/**************************************
BossChargeParticle更新処理
***************************************/
void BossChargeParticle::Update()
{
	cntFrame++;

	float t = (float)cntFrame / lifeFrame;
	Animation(t);

	if (cntFrame == lifeFrame)
	{
		active = false;
	}
}

/**************************************
BossChargeEmitter初期化
***************************************/
void BossChargeEmitter::Init()
{
	cntFrame = 0;
	active = true;
	transform.scale = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	duration = 90;
}

/**************************************
BossChargeEmitter更新処理
***************************************/
void BossChargeEmitter::Update()
{
	cntFrame++;

	const float DeltaScale = 0.02f;
	transform.scale += D3DXVECTOR3(1.0f, 1.0f, 1.0f) * DeltaScale;

	if (cntFrame == duration)
	{
		active = false;
	}
}

/**************************************
BossChargeParticleController初期化
***************************************/
void BossChargeParticleController::Init()
{
	D3DXVECTOR2 temp1 = D3DXVECTOR2(30.0f, 30.0f);
	D3DXVECTOR2 temp2 = D3DXVECTOR2(8.0f, 8.0f);

	MakeUnitBuffer(&temp1, &temp2);
	MakeUnitBuffer(&temp1, &temp2);
	LoadTexture("data/TEXTURE/EFFECT/EnemyBulletCharge.png");

	particleContainer.resize(1023);
	for (auto&& particle : particleContainer)
	{
		particle = new BossChargeParticle();
	}

	emitterContainer.resize(4);
	for (auto&& emitter : emitterContainer)
	{
		emitter = new BossChargeEmitter();
	}
}

/**************************************
BossChargeParticlrController放出処理
***************************************/
void BossChargeParticleController::Emit()
{
	ForEachEmitter(5, [](BaseEmitter* emitter, BaseParticle* particle)
	{
		particle->transform = emitter->transform;
		particle->Init();
	});
}