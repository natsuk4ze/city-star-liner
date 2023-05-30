//=====================================
//
//ボスヒットパーティクル処理[BossHitParticle.cpp]
//Author:GP12A332 21 立花雄太
//
//=====================================
#include "BossHitParticle.h"
#include "Framework\Easing.h"
#include "Framework\Vector3.h"

/**************************************
マクロ定義
***************************************/

/**************************************
BossHitParticleコンストラクタ
***************************************/
BossHitParticle::BossHitParticle() :
	speed(RandomRangef(2.0f, 10.5f))
{
	moveDir.x = RandomRangef(-1.0f, 1.0f);
	moveDir.y = RandomRangef(-1.0f, 1.0f);
	moveDir.z = RandomRangef(-1.0f, 1.0f);

	D3DXVec3Normalize(&moveDir, &moveDir);

	lifeFrame = RandomRange(10, 30);
}

/**************************************
BossHitParticle初期化処理
***************************************/
void BossHitParticle::Init()
{
	transform.scale = Vector3::One * RandomRangef(0.2f, 1.0f);
	active = true;
	cntFrame = 0;
}

/**************************************
BossHitParticle更新処理
***************************************/
void BossHitParticle::Update()
{
	cntFrame++;

	float t = (float)cntFrame / lifeFrame;
	float currentSpeed = Easing::EaseValue(t, speed, 0.0f, EaseType::InSine);
	transform.pos += moveDir * currentSpeed;

	transform.scale = Easing::EaseValue(t, 1.0f, 0.0f, EaseType::InCubic) * Vector3::One;

	if (cntFrame == lifeFrame)
	{
		active = false;
	}
}

/**************************************
BossHitParticleEmitter初期化処理
***************************************/
void BossHitParticleEmitter::Init()
{
	cntFrame = 0;
	duration = 5;
	active = true;
}

/**************************************
BossHitParticleEmitter更新処理
***************************************/
void BossHitParticleEmitter::Update()
{
	cntFrame++;

	if (cntFrame == duration)
	{
		active = false;
	}
}

/**************************************
BossHitParticleController初期化処理
***************************************/
void BossHitParticleController::Init()
{
	D3DXVECTOR2 temp = D3DXVECTOR2(5.0f, 5.0f);
	D3DXVECTOR2 temp2= D3DXVECTOR2(1.0f, 1.0f);

	MakeUnitBuffer(&temp, &temp2);
	LoadTexture("data/TEXTURE/Effect/EnemyBulletFire.png");

	particleContainer.resize(1024);
	for (auto&& particle : particleContainer)
	{
		particle = new BossHitParticle();
	}

	emitterContainer.resize(64);
	for (auto&& emitter : emitterContainer)
	{
		emitter = new BossHitParticleEmitter();
	}
}

/**************************************
BossHitParticleController放出処理
***************************************/
void BossHitParticleController::Emit()
{
	ForEachEmitter(15, [](BaseEmitter* emitter, BaseParticle* particle)
	{
		particle->transform.pos = emitter->transform.pos;
		particle->Init();
	});
}