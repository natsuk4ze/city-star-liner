//=====================================
//
//ボスバトルボンバーシーケンス処理[GameBossBombSequence.cpp]
//Author:GP12B332 21 立花雄太
//
//=====================================
#include "GameBossBombSequence.h"
#include "PlayerObserver.h"
#include "GameParticleManager.h"
#include "BossController.h"

/**************************************
グローバル変数
***************************************/

/**************************************
入場処理
***************************************/
void GameScene::GameBossBombSequence::OnStart(GameScene * entity)
{
	entity->playerObserver->OnStartBomberSequence();
	entity->useDarkMask = true;
	cntFrame = 0;
}

/**************************************
更新処理
***************************************/
int GameScene::GameBossBombSequence::OnUpdate(GameScene * entity)
{
	const int BomberFireTiming = 80;
	const int StateDuration = 120;

	cntFrame++;

	int result = GameScene::State::BossBombSequence;

	//プレイヤーのアニメーションだけさせる
	entity->playerObserver->Update();
	
	//ボンバーのエフェクトだけ更新する
	entity->particleManager->UpdateBombParticle();

	//発射タイミングであれば発射
	if (cntFrame == BomberFireTiming)
	{
		//鉄骨へのボム発射
		std::list<std::shared_ptr<RebarObstacle>> reberList;
		entity->bossController->GetRebarList(reberList);
		entity->playerObserver->FirePlayerBomber(reberList);

		//ボスへのボム発射
		std::shared_ptr<BossEnemyModel> target = entity->bossController->GetBoss();
		entity->playerObserver->FirePlayerBomber(target);
	}

	//終了判定
	if (cntFrame == StateDuration)
	{
		entity->playerObserver->OnFinishBomberSequence();
		
		entity->useDarkMask = false;

		result = GameScene::State::BossBattle;
	}

	return result;
}
