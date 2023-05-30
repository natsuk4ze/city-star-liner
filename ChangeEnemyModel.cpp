//=====================================
//
//ストップエネミーモデル処理[StopEnemyModel.cpp]
//Author:GP12B332 21 立花雄太
//
//=====================================
#include "ChangeEnemyModel.h"
#include "enemy.h"

/**************************************
マクロ定義
***************************************/
//初期化されてから当たり判定がアクティブになるタイミング
#define CHANGEENEMY_TIME_COLLIDER_ACTIVATE	(30)
#define CHANGEENEMY_DEST_FRAME				(60)		//エネミーがライン上に到達するのにかかる時間

//攻撃タイミング
#define CHANGEENEMY_TIME_ATTACK				(180 + CHANGEENEMY_DEST_FRAME)

//当たり判定が無効になるタイミング
#define CHANGEENEMY_TIME_ESCAPE				(300 + CHANGEENEMY_DEST_FRAME)

//終了タイミング
#define CHANGEENEMY_TIME_UNINIT				(60 + CHANGEENEMY_TIME_ESCAPE)

#define CHANGEENEMY_GENERATE_NUM			(3)			//エネミーの生成数	
#define CHANGEENEMY_INIT_OFFSET				(200.0f)	//目標座標から初期座標への距離


typedef EnemyModel Base;

/**************************************
構造体定義
***************************************/

/**************************************
コンストラクタ
***************************************/
ChangeEnemyModel::ChangeEnemyModel()
{

}

/**************************************
デストラクタ
***************************************/
ChangeEnemyModel::~ChangeEnemyModel()
{

}

/**************************************
初期化処理
***************************************/
void ChangeEnemyModel::Init(LineTrailModel model, int enemyNum)
{
	Base::Init(model);

	cntFrame = 0;
	collider->active = false;

	//Enemyを生成
	for (int i = 0; i < enemyNum; i++)
	{
		enemyList.push_back(std::make_shared<EnemyChange>());
	}

	//ラインの端点を求める
	D3DXVECTOR3 edgeL, edgeR;
	model.GetEdgePos(&edgeR, &edgeL);
	edgeL.z = edgeR.z = 250.0f;

	//エネミー同士の距離を求める
	D3DXVECTOR3 enemyLength = (edgeL - edgeR) / ((float)enemyList.size() + 1);
	edgeR += enemyLength;

	//ラインに対して垂直なベクトルを求める
	D3DXVECTOR3 cross;
	D3DXVECTOR3 pos = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	D3DXVECTOR3 edgePos = (edgeL - edgeR);
	D3DXVec3Cross(&cross, &edgePos, &pos);
	D3DXVec3Normalize(&cross, &cross);

	//初期座標を計算
	D3DXVECTOR3 initOffset = cross * CHANGEENEMY_INIT_OFFSET;

	//エネミーをセット
	for (auto& enemy : enemyList)
	{
		enemy->VInit();
		enemy->VSetVec(edgeR + initOffset, edgeR, CHANGEENEMY_DEST_FRAME, CHANGEENEMY_TIME_ESCAPE - CHANGEENEMY_TIME_COLLIDER_ACTIVATE, D3DXVECTOR3(0.0f, 15.0f, 0.0));
		edgeR += enemyLength;
	}
}

/**************************************
更新処理
***************************************/
int ChangeEnemyModel::Update()
{
	cntFrame++;

	//60フレーム目で当たり判定をアクティブにする
	if (cntFrame == CHANGEENEMY_TIME_COLLIDER_ACTIVATE)
		collider->active = true;
	
	//アクティブになってから300フレームで離脱する
	if (cntFrame == CHANGEENEMY_TIME_ESCAPE)
		collider->active = false;

	//離脱開始してから60フレームで終了する
	if (cntFrame == CHANGEENEMY_TIME_UNINIT)
		Uninit();

	for (auto& enemy : enemyList)
	{
		enemy->VUpdate();
	}

	//アクティブになってから120フレームで攻撃
	if (cntFrame == CHANGEENEMY_TIME_ATTACK)
		return AttackTiming;
	//エネミーの到着と同時にチャージ演出
	else if (cntFrame == CHANGEENEMY_DEST_FRAME)
		return ChargeTiming;
	else
		return StateContinuous;
}