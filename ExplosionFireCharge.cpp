//=====================================
//
//テンプレート処理[Template.cpp]
//Author:GP12A332 21 立花雄太
//
//=====================================
#include "ExplosionFireCharge.h"
#include "Framework\Easing.h"
#include "Framework\Vector3.h"

/**************************************
マクロ定義
***************************************/

/**************************************
ExplosionFireChargeコンストラクタ
***************************************/
ExplosionFireCharge::ExplosionFireCharge()
{
	lifeFrame = RandomRange(60, 180);

	D3DXVECTOR3 moveDir;
	moveDir.x = RandomRangef(-1.0f, 1.0f);
	moveDir.y = RandomRangef(-1.0f, 1.0f);
	moveDir.z = RandomRangef(-1.0f, 1.0f);

	D3DXVec3Normalize(&moveDir, &moveDir);

	const float Speed = RandomRangef(5.0f, 10.0f);
	offset = moveDir * (float)lifeFrame * Speed;	

}

/**************************************
ExplosionFireCharge初期化処理
***************************************/
void ExplosionFireCharge::Init()
{
	cntFrame = 0;
	active = true;
}

/**************************************
ExplosionFireCharge更新処理
***************************************/
void ExplosionFireCharge::Update()
{
	cntFrame++;

	float t = (float)cntFrame / lifeFrame;
	transform.pos = parent->pos - Easing::EaseValue(t, offset, Vector3::Zero, EaseType::InExpo);
	
	transform.scale = Vector3::One * Easing::EaseValue(t, 0.0f, 2.0f, EaseType::InSine);

	if (cntFrame == lifeFrame)
	{
		active = false;
	}
}

/**************************************
ExplosionFireChargeEmitter初期化処理
***************************************/
void ExplosionFireChargeEmitter::Init()
{
	duration = 120;
	cntFrame = 0;
	active = true;
}

/**************************************
ExplosionFireChargeEmitter更新処理
***************************************/
void ExplosionFireChargeEmitter::Update()
{
	cntFrame++;
	if (cntFrame == duration)
	{
		active = false;
	}
}

/**************************************
ExplosionFireChargeController初期化処理
***************************************/
void ExplosionFireChargeController::Init()
{
	D3DXVECTOR2 temp = D3DXVECTOR2(20.0f, 20.0f);
	D3DXVECTOR2 temp2 = D3DXVECTOR2(1.0f,1.0f);

	MakeUnitBuffer(&temp, &temp2);
	LoadTexture("data/TEXTURE/Effect/ExplosionFireCharge.png");

	particleContainer.resize(1024);
	for (auto&& particle : particleContainer)
	{
		particle = new ExplosionFireCharge();
	}

	emitterContainer.resize(2);
	for (auto&& emitter : emitterContainer)
	{
		emitter = new ExplosionFireChargeEmitter();
	}
}

/**************************************
ExplosionFireChargeController放出処理
***************************************/
void ExplosionFireChargeController::Emit()
{
	ForEachEmitter(3, [](BaseEmitter* emitter, BaseParticle *particle)
	{
		particle->transform = emitter->transform;
		static_cast<ExplosionFireCharge*>(particle)->parent = &emitter->transform;
		particle->Init();
	});
}

/**************************************
ExplosionFireChargeController描画処理
***************************************/
bool ExplosionFireChargeController::Draw()
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	//pDevice->SetRenderState(D3DRS_ZENABLE, false);

	bool res = BaseParticleController::Draw();

	pDevice->SetRenderState(D3DRS_ZENABLE, true);

	return res;
}
