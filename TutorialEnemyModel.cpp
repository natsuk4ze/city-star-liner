//=====================================
//
//TutorialEnemyModel.cpp
//機能:
//Author:GP12B332 21 立花雄太
//
//=====================================
#include "TutorialEnemyModel.h"

/**************************************
マクロ定義
***************************************/
#define TUTORIALENEMY_INIT_OFFSET				(200.0f)	//目標座標から初期座標への距離
#define TUTORIALENEMY_DEST_FRAME				(60)		//エネミーがライン上に到達するのにかかる時間

/**************************************
コンストラクタ
***************************************/
TutorialEnemyModel::TutorialEnemyModel(LineTrailModel model, int enemyNum):
	EnemyModel()
{
	EnemyModel::Init(model);

	cntFrame = 0;
	collider->active = true;

	//Enemyを生成
	for (int i = 0; i < enemyNum; i++)
	{
		enemyList.push_back(std::make_shared<EnemyTutorial>());
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
	D3DXVECTOR3 initOffset = cross * TUTORIALENEMY_INIT_OFFSET;

	//エネミーをセット
	for (auto& enemy : enemyList)
	{
		enemy->VInit();
		enemy->VSet(edgeR + initOffset, edgeR, TUTORIALENEMY_DEST_FRAME);
		edgeR += enemyLength;
	}
}

/**************************************
更新処理
***************************************/
int TutorialEnemyModel::Update()
{
	for (auto&& enemy : enemyList)
	{
		enemy->VUpdate();
	}

	return StateContinuous;

}
