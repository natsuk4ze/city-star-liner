//=====================================
//
//エネミーコントローラー処理[EnemyController.cpp]
//Author:GP12B332 21 立花雄太
//
//=====================================
#include "EnemyController.h"
#include "TestEnemyModel.h"
#include "ChangeEnemyFactory.h"
#include "StraightEnemyFactory.h"
#include "SnakeEnemyFactory.h"
#include "MidiumEnemyFactory.h"
#include "EnemyBullet.h"
#include "GameParticleManager.h"
#include "BossEnemyModel.h"

#include "Framework\ResourceManager.h"
#include "picojson\picojson.h"
#include "debugWindow.h"

#include <algorithm>
#include <fstream>

#include "sound.h"
using namespace std;

#include "MidiumEnemyModel.h"

/**************************************
マクロ定義
***************************************/
#define USE_DEBUG_TESTENEMY (1)

#define ENEMY_SHOTPOS_OFFSET	(D3DXVECTOR3(0.0f, 0.0f, -50.0f))
#define ENEMY_GUIDE_TIMING		(10)

/**************************************
構造体定義
***************************************/

/**************************************
グローバル変数
***************************************/

/**************************************
コンストラクタ
***************************************/
EnemyController::EnemyController()
{
	//リソース読み込み
	//解放はシーン終了時にGame.cppで一括して開放する
	ResourceManager::Instance()->LoadMesh("Enemy", "data/MODEL/Enemy/drone.x");
	ResourceManager::Instance()->LoadMesh("MidiumEnemy", "data/MODEL/Enemy/midium.x");

	//各コントローラ作成
	bulletController = new EnemyBulletController();
	guideController = new EnemyGuideArrowController();

	//各ファクトリー作成
	factoryContainer["Change"] = new ChangeEnemyFactory();
	factoryContainer["Straight"] = new StraightEnemyFactory();
	factoryContainer["Snake"] = new SnakeEnemyFactory(); 
	factoryContainer["Midium"] = new MidiumEnemyFactory();
	//ステージデータ読み込み
	LoadStageData();
}

/**************************************
デストラクタ
***************************************/
EnemyController::~EnemyController()
{
	//モデルコンテナクリア
	for (auto &model : modelList)
	{
		SAFE_DELETE(model);
	}
	modelList.clear();
	
	//各コントローラ削除
	SAFE_DELETE(bulletController);
	SAFE_DELETE(guideController);

	//各ファクトリー削除
	for (auto& pair : factoryContainer)
	{
		SAFE_DELETE(pair.second);
	}
	factoryContainer.clear();

	//ステージデータクリア
	stageModelList.clear();
}

/**************************************
初期化処理
***************************************/
void EnemyController::Init()
{
	cntFrame = 0;

	//新しく作るEnemyの初期化テストはここに書く
#if USE_DEBUG_TESTENEMY
	test = new EnemyMidium;
	//test->VInit();

#endif
}

/**************************************
終了処理
***************************************/
void EnemyController::Uninit()
{
	for (auto &model : modelList)
	{
		model->Uninit();
	}

	bulletController->Uninit();

	//新しく作るEnemyの終了テストはここに書く
#if USE_DEBUG_TESTENEMY
	//test->VUninit();
	SAFE_DELETE(test);
#endif
}

/**************************************
更新処理
***************************************/
void EnemyController::Update()
{
	//新しく作るEnemyの更新テストはここに書く
#if USE_DEBUG_TESTENEMY
	//test->VUpdate();
#endif

	//モデル更新処理
	for (auto &model : modelList)
	{
		int updateResult = model->Update();

		if (updateResult == AttackTiming)
			EnemyAttack(model);
		else if (updateResult == ChargeTiming)
			SetChageEffect(model);
	}

	//バレット更新処理
	bulletController->Update();
	for (auto& model : modelList)
	{
		model->CheckDestroied();
	}

	//ガイド更新処理
	guideController->Update();

	//モデル内のエネミー撃墜確認

	//終了したモデルを削除する
	for (auto& model : modelList)
	{
		if (!model->active)
			SAFE_DELETE(model);
	}

	//削除したモデルをリストから削除
	modelList.remove_if([](EnemyModel* model)
	{
		return model == nullptr;
	});
}

/**************************************
描画処理
***************************************/
void EnemyController::Draw()
{
	//エネミーモデル描画
	for (auto &model : modelList)
	{
		model->Draw();
	}

	//バレット描画
	bulletController->Draw();

	//新しく作るEnemyの描画テストはここに書く
#if USE_DEBUG_TESTENEMY
	//test->VDraw();
#endif
}

/**************************************
ガイド描画処理
***************************************/
void EnemyController::DrawGuide()
{
	guideController->Draw();
}

/**************************************
エネミー生成インターフェース
***************************************/
void EnemyController::SetEnemy()
{
	cntFrame++;

	//ガイド生成
	SetGuide();

	//現在のインデックスからステージデータを確認していく
	for (UINT i = currentIndex; i < stageModelList.size(); i++)
	{
		//生成タイミング前であればbreak
		if (cntFrame < stageModelList[i].frame)
			break;

		//typeに応じて生成処理をディスパッチ
		string type = stageModelList[i].type;
		EnemyModel* model = factoryContainer[type]->Create(stageModelList[i].data);
		modelList.push_back(model);

		currentIndex++;
	}

	//デバッグ用ループ処理
	//if (cntFrame > (*(stageModelList.end() - 1)).frame + 300)
	//{
	//	cntFrame = 0;
	//	currentIndex = 0;
	//}
}

/**************************************
ガイド生成処理
***************************************/
void EnemyController::SetGuide()
{
	//現在のインデックスからデータを確認していく
	for (UINT i = currentIndex; i < stageModelList.size(); i++)
	{
		//ガイドタイミングでなければContinue
		if (cntFrame + ENEMY_GUIDE_TIMING != stageModelList[i].frame)
			continue;

		//ガイドタイミングより遅いデータが来たらbreak
		if (cntFrame + ENEMY_GUIDE_TIMING < stageModelList[i].frame)
			break;

		//ガイド生成
		string type = stageModelList[i].type;
		factoryContainer[type]->CreateGuide(stageModelList[i].data, guideController);
	}
}

/**************************************
ステージデータ読み込み処理
***************************************/
bool EnemyController::LoadStageData()
{
	//JSONファイルを開く
	ifstream ifs;
	ifs.open("data/JSON/test.json", std::ios::binary);

	//成功確認
	if (!ifs.is_open())
		return false;

	//JSONデータを読み込み
	picojson::value val;
	ifs >> val;
	ifs.close();

	//データ配列をパースする
	picojson::array& dataList = val.get<picojson::object>()["StageData"].get<picojson::array>();

	//データを1個1個パースしてStageModelを作成する
	stageModelList.resize(dataList.size());
	for (UINT i = 0; i < dataList.size(); i++)
	{
		int frame = static_cast<int>(dataList[i].get<picojson::object>()["frame"].get<double>());
		string type = dataList[i].get<picojson::object>()["type"].get<string>();
		picojson::object data = dataList[i].get<picojson::object>()["data"].get<picojson::object>();

		stageModelList[i] = StageModel(frame, type, data);
	}

	//インデックス初期化
	currentIndex = 0;

	return true;
}

/**************************************
エネミー攻撃処理
***************************************/
void EnemyController::EnemyAttack(EnemyModel *enermyModel)
{
	vector<D3DXVECTOR3> emitPos;
	//emitPos.reserve(enermyModel->enemyList.size());

	//for (auto& enemy : enermyModel->enemyList)
	//{
	//	emitPos.push_back(enemy->m_Pos + ENEMY_SHOTPOS_OFFSET);
	//}
		//バレット発射SE
	Sound::GetInstance()->SetPlaySE(BOSSSHOT, true, (Sound::GetInstance()->changevol / 10.0f));

	enermyModel->GetShotPos(emitPos);
	bulletController->Set(emitPos, enermyModel->model);
}

/**************************************
エネミー攻撃チャージ開始処理
***************************************/
void EnemyController::SetChageEffect(EnemyModel *model)
{
	model->chageEffectList.clear();
	model->chageEffectList.resize(model->enemyList.size());

	UINT cntSet = 0;
	for (auto& enemey : model->enemyList)
	{
		D3DXVECTOR3 pos = (enemey->m_Pos + ENEMY_SHOTPOS_OFFSET);
		BaseEmitter* emitter = GameParticleManager::Instance()->SetEnemyBulletCharge(&pos);
		model->chageEffectList[cntSet] = emitter;
		cntSet++;
	}
}

/**************************************
エネミー座標取得処理
***************************************/
void EnemyController::GetEnemyList(std::list<std::shared_ptr<Enemy>>& out)
{
	for (auto& model : modelList)
	{
		model->GetEnemy(out);
	}
}

/**************************************
ボンバーシーケンスコールバック
***************************************/
void EnemyController::OnFinishBombSequence()
{
	bulletController->DisableAll();
}

/**************************************
エネミー存在判定
***************************************/
bool EnemyController::ExistsAcitveEnemy()
{
	return !modelList.empty();
}

/**************************************
通常バトル終了判定
***************************************/
bool EnemyController::IsFinishedEnemy()
{
	if (currentIndex < stageModelList.size())
		return false;

	if (!modelList.empty())
		return false;

	return true;
}

