//=====================================
//
//ボンバーファイア処理[BomberFire.cpp]
//Author:GP12A332 21 立花雄太
//
//=====================================
#include "BomberFire.h"
#include "Framework\Easing.h"

/**************************************
マクロ定義
***************************************/

/**************************************
BomberFireコンストラクタ
***************************************/
BomberFire::BomberFire()
{
	D3DXVECTOR2 temp = D3DXVECTOR2(8.0f, 8.0f);
	SetAnimParameter(&temp);

	moveDir.x = RandomRangef(-1.0f, 1.0f);
	moveDir.y = RandomRangef(-1.0f, 1.0f);
	moveDir.z = RandomRangef(0.5f, 5.0f);
	D3DXVec3Normalize(&moveDir, &moveDir);

	speed = RandomRangef(5.0f, 10.0f);

	lifeFrame = RandomRange(20, 40);
}

/**************************************
BomberFire初期化処理
***************************************/
void BomberFire::Init()
{
	cntFrame = 0;
	active = true;
}

/**************************************
BomberFire更新処理
***************************************/
void BomberFire::Update()
{
	cntFrame++;
	
	float t = (float)cntFrame / lifeFrame;
	float currentSpeed = Easing::EaseValue(t, speed, 0.0f, EaseType::InCubic);
	transform.pos += moveDir * currentSpeed;

	Animation(t);

	if (cntFrame == lifeFrame)
	{
		active = false;
	}
}

/**************************************
BomberFireEmitter初期化処理
***************************************/
void BomberFireEmitter::Init()
{
	cntFrame = 0;
	duration = 3;
	active = true;
}

/**************************************
BomberFireEmitter更新処理
***************************************/
void BomberFireEmitter::Update()
{
	cntFrame++;

	if (cntFrame == duration)
	{
		active = false;
	}
}