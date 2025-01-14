//=====================================
//
//ボスアイドルヘッダ[BossIdle.h]
//Author:GP12B332 21 立花雄太
//
//=====================================
#ifndef _BOSSIDLE_H_
#define _BOSSIDLE_H_

#include "main.h"
#include "IStateMachine.h"
#include "BossEnemyModel.h"

/**************************************
前方宣言
***************************************/

/**************************************
マクロ・列挙子定義
***************************************/

/**************************************
クラス定義
***************************************/
class BossEnemyModel::BossIdle : public IStateMachine<BossEnemyModel>
{
public:
	void OnStart(BossEnemyModel* entity);
	int OnUpdate(BossEnemyModel* entity);

private:
	int cntFrame;
};

#endif