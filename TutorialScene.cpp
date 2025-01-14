//=====================================
//
//TutorialScene.cpp
//機能:チュートリアルシーン
//Author:GP12B332 21 立花雄太
//
//=====================================
#include "TutorialScene.h"
#include "TutorialBG.h"
#include "GameSceneUIManager.h"
#include "PlayerObserver.h"
#include "PlayerController.h"
#include "InputController.h"
#include "GameParticleManager.h"
#include "TutorialController.h"
#include "TutorialEnemyController.h"
#include "ScoreManager.h"
#include "Framework\Polygon2D.h"
#include "input.h"
#include "masktex.h"

#include "TutorialIdle.h"
#include "TutorialBomber.h"

/**************************************
グローバル変数
***************************************/

/**************************************
初期化処理
***************************************/
void TutorialScene::Init()
{
	//FSM作成
	fsm.resize(State::Max, NULL);
	fsm[State::Idle] = new TutorialIdle();
	fsm[State::Bomber] = new TutorialBomber();
	
	//UIインスタンス作成
	container = new GameSceneUIManager();

	//☆ボタンの位置からワールド座標を計算
	std::vector<D3DXVECTOR3> starPositionContainer;
	container->GetStarPosition(starPositionContainer);
	LineTrailModel::CalcEdgePosition(starPositionContainer);

	//他のインスタンス作成
	bg = new TutorialBG();
	playerObserver = new PlayerObserver();
	controller = new TutorialController();
	enemyController = new TutorialEnemyController();

	//暗転用ポリゴン作成
	darkMask = new Polygon2D();
	darkMask->SetSize((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT);
	darkMask->SetColor(D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.5f));
	useDarkMask = false;

	//PlayerControllerにPlayerObserverをセット
	SetPlayerObserverAdr(playerObserver);

	//インプットコントローラにUIManagerのインスタンスを渡す
	SetInstanceUIManager(container);

	//スコアマネージャにインスタンスを渡す
	SetGameScneeUIManagerInstance(container);
	
	//フォグを有効化
	FLOAT StartPos = 10000;
	FLOAT EndPos = 50000;

	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	pDevice->SetRenderState(D3DRS_FOGENABLE, true);
	pDevice->SetRenderState(D3DRS_FOGCOLOR, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));
	pDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);
	pDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
	pDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD*)(&StartPos));
	pDevice->SetRenderState(D3DRS_FOGEND, *(DWORD*)(&EndPos));

	//初期化処理
	GameParticleManager::Instance()->Init();
	playerObserver->Init();

	current = State::Idle;
}

/**************************************
終了処理
***************************************/
void TutorialScene::Uninit()
{
	//スコアマネージャクリア
	ClearScoreManager();

	//インスタンス削除
	SAFE_DELETE(bg);
	SAFE_DELETE(container);
	SAFE_DELETE(controller);
	SAFE_DELETE(enemyController);
	SAFE_DELETE(darkMask);
	SAFE_DELETE(playerObserver);
	
	//FSM削除
	for (auto&& statemachine : fsm)
	{
		SAFE_DELETE(statemachine);
	}
	fsm.clear();

	//パーティクル終了
	GameParticleManager::Instance()->Uninit();

	//フォグを無効化
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	if(pDevice != NULL)
		pDevice->SetRenderState(D3DRS_FOGENABLE, false);
}

/**************************************
更新処理
***************************************/
void TutorialScene::Update(HWND hWnd)
{
	int result = fsm[current]->OnUpdate(this);

	if (result != current)
	{
		current = (State)result;
		fsm[current]->OnStart(this);
	}

	//UI更新
	container->Update(hWnd);

	//パーティクル更新
	GameParticleManager::Instance()->Update();

	//衝突判定
	TrailCollider::UpdateCollision();
	BoxCollider3D::UpdateCollision();

	//エンターキーが押されたらチュートリアル終了
	if (GetKeyboardTrigger(DIK_RETURN))
	{
		SceneChangeFlag(true, Scene::SceneGame);
	}
}

/**************************************
描画処理
***************************************/
void TutorialScene::Draw()
{
	bg->Draw();

	//暗転用ポリゴンの描画
	if (useDarkMask)
	{
		LPDIRECT3DDEVICE9 pDevice = GetDevice();
		pDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);

		darkMask->Draw();

		pDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);
	}

	playerObserver->Draw();
	enemyController->Draw();

	GameParticleManager::Instance()->Draw();

	container->SetBomberStock(playerObserver->GetBomberStockNum(), playerObserver->IsBomberCharging());
	container->Draw();
	playerObserver->DrawInputGuide();

	controller->Draw();
}

/**************************************
ボンバーを撃つべきかどうか
***************************************/
bool TutorialScene::ShouldFireBomber()
{
	return playerObserver->ShouldFireBomber() && enemyController->IsExistEnemy();
}
