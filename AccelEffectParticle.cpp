//=====================================
//
//アクセルエフェクトパーティクル処理[AccelEffectParticle.cpp]
//Author:GP12A332 21 立花雄太
//
//=====================================
#include "AccelEffectParticle.h"
#include "Framework\Easing.h"

/**************************************
マクロ定義
***************************************/
#define ACCELEFFECT_LIFE_MIN		(10)
#define ACCELEFFECT_LIFE_MAX		(20)
#define ACCELEFFECT_SCALE			(5.0f)
#define ACCELEFFECT_POS_OFFSET		(5.0f)
#define ACCELEFFECT_SPEED			(8.0f)
#define ACCELEFFECT_MOVE_Z			(-2.0f)

/**************************************
AccelEffectParticle初期化処理
***************************************/
void AccelEffectParticle::Init()
{
	cntFrame = 0;
	active = true;

	//移動方向決定
	moveDir.x = RandomRangef(-1.0f, 1.0f);
	moveDir.y = RandomRangef(-1.0f, 1.0f);
	moveDir.z = -0.0f;
	D3DXVec3Normalize(&moveDir, &moveDir);

	//回転
	float dir = moveDir.x > 0.0f ? 1.0f : -1.0f;
	D3DXVECTOR3 vecUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	float dot = D3DXVec3Dot(&moveDir, &vecUp);
	transform.Rotate(0.0f, 0.0f, D3DXToDegree(acosf(dot)) * dir);

	//座標オフセット
	transform.pos.x += moveDir.x * ACCELEFFECT_POS_OFFSET;
	transform.pos.y += moveDir.y * ACCELEFFECT_POS_OFFSET;

	//寿命決定
	lifeFrame = RandomRange(ACCELEFFECT_LIFE_MIN, ACCELEFFECT_LIFE_MAX);
	
	moveDir.z = ACCELEFFECT_MOVE_Z;
}

/**************************************
AccelEffectParticle更新処理
***************************************/
void AccelEffectParticle::Update()
{
	cntFrame++;
	float t = (float)cntFrame / lifeFrame;

	//スピードイージング
	float speed = Easing::EaseValue(t, ACCELEFFECT_SPEED, 0.3f, EaseType::OutCubic);
	transform.pos += moveDir * speed;

	//スケールイージング
	transform.scale.x = Easing::EaseValue(t, 1.0f, 0.0f, EaseType::InCubic);

	if (cntFrame == lifeFrame)
		active = false;

}

/**************************************
AccelEffectParticleEmitter初期化処理
***************************************/
void AccelEffectParticleEmitter::Init()
{
	cntFrame = 0;
	duration = 5;
	active = true;
}

/**************************************
AccelEffectParticleEmitter更新処理
**************************************/
void AccelEffectParticleEmitter::Update()
{
	cntFrame++;

	if (cntFrame == duration)
		active = false;
}