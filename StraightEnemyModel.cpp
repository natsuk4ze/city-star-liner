//=====================================
//
//ストレートエネミーモデル処理[StraightEnemyModel.cpp]
//Author:GP12B332 21 立花雄太
//
//=====================================
#include "StraightEnemyModel.h"
#include "enemy.h"
#include "Framework\Easing.h"
#include "GameParticleManager.h"

/**************************************
マクロ定義
***************************************/
typedef EnemyModel Base;

#define STRAIGHTENEMY_REACH_FRAME		(240)
#define STRAIGHTENEMY_ACTIVATE_FRAME	(30)
#define STRAIGHTENEMY_HITABLE_FRAME		(45)
//0805 BA
#define SHADOW_FALSE_FRAME				(200)

/**************************************
コンストラクタ
***************************************/
StraightEnemyModel::StraightEnemyModel() : StartPosZ(300.0f), DestPosZ(-200.0f)
{

}

/**************************************
デストラクタ
***************************************/
StraightEnemyModel::~StraightEnemyModel()
{

}

/**************************************
初期化処理
***************************************/
void StraightEnemyModel::Init(LineTrailModel model, int enemyNum)
{
	Base::Init(model);

	cntFrame = 0;
	collider->active = false;

	//エネミー生成
	for (int i = 0; i < enemyNum; i++)
	{
		enemyList.push_back(std::make_shared<EnemyStraight>());
	}

	//Enemyの初期化
	D3DXVECTOR3 edgeR, edgeL;
	model.GetEdgePos(&edgeR, &edgeL);
	edgeL.z = edgeR.z = StartPosZ;

	D3DXVECTOR3 offset = (edgeL - edgeR) / ((float)enemyList.size() + 1);
	edgeR += offset;

	D3DXVECTOR3 dest = edgeR;
	dest.z = DestPosZ;

	for (auto& enemy : enemyList)
	{
		enemy->VInit();
		enemy->VSet(edgeR, dest, STRAIGHTENEMY_REACH_FRAME);
		enemy->m_Active = false;
		edgeR += offset;
		dest += offset;

		enemy->m_Active = true;
	}

	//ワープエフェクトセット
	for (auto& enemy : enemyList)
	{
		D3DXVECTOR3 setPos = enemy->m_Pos + D3DXVECTOR3(0.0f, 0.0f, -50.0f);
		GameParticleManager::Instance()->SetEnemyWarpHole(&setPos);
	}

}

/**************************************
更新処理
***************************************/
int StraightEnemyModel::Update()
{
	cntFrame++;

	//エネミーのアクティベイト
	if (cntFrame == STRAIGHTENEMY_ACTIVATE_FRAME)
	{
		collider->active = true;
	}

	//当たり判定の移動処理
	if (cntFrame >= STRAIGHTENEMY_ACTIVATE_FRAME)
	{
		float t = (float)(cntFrame - STRAIGHTENEMY_ACTIVATE_FRAME) / (float)STRAIGHTENEMY_REACH_FRAME;
		pos.z = Easing::EaseValue(t, StartPosZ, DestPosZ, EaseType::InSine);
	}

	//終了判定
	if (cntFrame == STRAIGHTENEMY_REACH_FRAME + STRAIGHTENEMY_ACTIVATE_FRAME + SHADOW_FALSE_FRAME)
	{
		Uninit();
	}

	//所属するエネミーを更新
	for (auto& enemy : enemyList)
	{
		enemy->VUpdate();
	}

	return StateContinuous;
}