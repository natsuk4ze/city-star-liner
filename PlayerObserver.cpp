//=====================================
//
//プレイヤーオブザーバー処理[PlayerObserver.cpp]
//Author:GP12B332 21 立花雄太
//
//=====================================
#include "PlayerObserver.h"
#include "InputController.h"
#include "InputGuide.h"
#include "ScoreManager.h"

#include "PlayerMove.h"
#include "PlayerWait.h"
#include "PlayerReturn.h"
#include "PlayerIdle.h"
#include "PlayerFalldown.h"

#include "GameParticleManager.h"
#include "sound.h"

#include <algorithm>

using namespace std;

/**************************************
マクロ定義
***************************************/
#define MOVETARGET_DEFAULT	(5)

/**************************************
構造体定義
***************************************/

/**************************************
コンストラクタ
***************************************/
PlayerObserver::PlayerObserver() :
	InvalidInputID(5)
{
	player = new Player();
	model = new PlayerModel();
	trailEffect = new PlayerTrail();
	bomberController = new PlayerBomberController();
	bulletController = new PlayerBulletController();
	inputGuide = new InputGuide();

	fsm[PlayerState::Idle] = new PlayerIdle();
	fsm[PlayerState::Wait] = new PlayerWait();
	fsm[PlayerState::Move] = new PlayerMove();
	fsm[PlayerState::Return] = new PlayerReturn();
	fsm[PlayerState::Falldown] = new PlayerFalldown();

	//移動先確保
	targetPos.resize(5);
	for (UINT i = 0; i < targetPos.size(); i++)
	{
		targetPos[i] = LineTrailModel::GetEdgePos(i);
	}

	//moveTarget初期化
	moveTarget = MOVETARGET_DEFAULT;

	//ロジック更新有効化
	enableUpdateLogic = true;
}

/**************************************
デストラクタ
***************************************/
PlayerObserver::~PlayerObserver()
{
	SAFE_DELETE(player);
	SAFE_DELETE(model);
	SAFE_DELETE(trailEffect);
	SAFE_DELETE(inputGuide);

	SAFE_DELETE(bomberController);
	SAFE_DELETE(bulletController);

	for (auto&& pair : fsm)
	{
		SAFE_DELETE(pair.second);
	}
	fsm.clear();
}

/**************************************
初期化処理
***************************************/
void PlayerObserver::Init()
{
	player->Init();
	ChangeStatePlayer(PlayerState::Idle);
}

/**************************************
終了処理
***************************************/
void PlayerObserver::Uninit()
{
	player->Uninit();

	bomberController->Uninit();
	bulletController->Uninit();
}

/**************************************
更新処理
***************************************/
void PlayerObserver::Update()
{
	if (enableUpdateLogic)
	{
		int stateResult = player->Update();

		if (stateResult != STATE_CONTINUOUS)
			OnPlayerStateFinish();

		bulletController->Update();
	}

	trailEffect->Update();

	bomberController->Update();

	player->Animation();
}

/**************************************
描画処理
***************************************/
void PlayerObserver::Draw()
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	player->Draw();

	trailEffect->Draw();

	bomberController->Draw();
	bulletController->Draw();
}

/**************************************
描画処理
***************************************/
void PlayerObserver::DrawInputGuide()
{
	inputGuide->Draw();
}

/**************************************
入力確認処理
***************************************/
void PlayerObserver::CheckInput()
{
	//入力間隔をカウント
	player->inputInterval++;

	//入力を確認
	const int InvalidInput = 5;
	int inputID = GetMoveInput();
	int stickInput = InvalidInputID;

	if (current == PlayerState::Wait || current == PlayerState::Idle)
	{
		stickInput = GetStickInput(moveTarget);

		if (stickInput != InvalidInputID)
		{
			inputGuide->Set(player->transform.pos, LineTrailModel::GetEdgePos(stickInput));
		}
	}

	//キーボードとマウスを優先的に使用
	if(inputID < InvalidInput)
		PushInput(inputID);
	//パッドのスティックで移動
	else if (stickInput < InvalidInputID && IsAnyButtonTriggerd())
	{
		PushInput(stickInput);
	}
}

/**************************************
入力処理
***************************************/
void PlayerObserver::PushInput(int num)
{
	//同じところへは移動しない
	if (num == moveTarget)
		return;

	//プレイヤーの入力間隔をリセット
	player->inputInterval = 0;

	//Wait状態であればMoveに遷移
	if (current == PlayerState::Wait || current == PlayerState::Idle)
	{
		if (current == PlayerState::Wait)
		{
			player->collider->SetTrailIndex(LineTrailModel(moveTarget, num));

			//無敵状態でなければ当たり判定を有効化
			if (!player->flgInvincible)
			{
				player->collider->active = true;
			}

			player->ChangeAnim(PlayerAnimID::Attack);
			trailEffect->Init(&player->transform.pos);
		}

		moveTarget = num;
		player->goalpos = targetPos[moveTarget];
		ChangeStatePlayer(PlayerState::Move);
	}
	//先行入力を保存して何もしない
	else
	{
		model->PushInput(num);
	}
}

/**************************************
Player状態遷移
***************************************/
void PlayerObserver::ChangeStatePlayer(PlayerState next)
{
	prevState = current;
	current = next;
	player->ChangeState(fsm[current]);
}

/**************************************
プレイヤーの状態終了時の処理
***************************************/
void PlayerObserver::OnPlayerStateFinish()
{
	switch (current)
	{
	case PlayerState::Move:
		OnFinishPlayerMove();
		break;

	case PlayerState::Wait:
		OnFinishPlayerWait();
		break;

	case PlayerState::Return:
		OnFinishPlayerReturn();
		break;

	}
}
/**************************************
Moveコールバック
***************************************/
void PlayerObserver::OnFinishPlayerMove()
{
	//当たり判定を無効化(移動履歴がある場合のみ)
	if(!model->Empty())
		player->collider->active = true;

	//移動履歴をプッシュ（ボンバーのストックインターバルが終了していたら）
	model->PushMoveStack(moveTarget, bomberController->CanStock());

	//トレイルを終了
	trailEffect->Uninit();

	//WaitかｒMoveからの移動であればバレット発射
	if (prevState == PlayerState::Wait || prevState == PlayerState::Move)
	{
		LineTrailModel modelTrail;
		model->GetPlayerTrail(&modelTrail);
		bulletController->SetPlayerBullet(modelTrail);
	}

	//ボンバーストック可能かつ一筆書きが成立したか判定
	TryStockBomber();

	//先行入力確認
	int prevMoveTarget = moveTarget;
	if (model->IsExistPrecedInput(&moveTarget))
	{
		//当たり判定を更新
		player->collider->SetTrailIndex(LineTrailModel(prevMoveTarget, moveTarget));

		player->goalpos = targetPos[moveTarget];
		trailEffect->Init(&player->transform.pos);
		player->ChangeAnim(PlayerAnimID::Attack);
		ChangeStatePlayer(PlayerState::Move);
	}
	//無ければ待機状態へ遷移
	else
	{
		player->ChangeAnim(PlayerAnimID::Flying);
		ChangeStatePlayer(PlayerState::Wait);
	}
}

/**************************************
Waitコールバック
***************************************/
void PlayerObserver::OnFinishPlayerWait()
{
	//TODO:初期位置に戻るので色々リセット
	model->Clear();
	moveTarget = 5;
	player->EnableCollider(false);

	//Return状態へ遷移し初期位置へ
	ChangeStatePlayer(PlayerState::Return);
}

/**************************************
Returnコールバック
***************************************/
void PlayerObserver::OnFinishPlayerReturn()
{
	//プレイヤーをIdle状態へ遷移
	ChangeStatePlayer(PlayerState::Idle);

	//履歴クリア
	model->Clear();
}

/**************************************
ボンバーシーケンス開始処理
***************************************/
void PlayerObserver::OnStartBomberSequence()
{
	//ボンバーSE
	Sound::GetInstance()->SetPlaySE(BOMB, true, (Sound::GetInstance()->changevol / 5.0f));

	enableUpdateLogic = false;
	player->ChangeAnim(PlayerAnimID::FireBomber);
	player->ChargeBomber();
}

/**************************************
ボンバーシーケンス終了処理
***************************************/
void PlayerObserver::OnFinishBomberSequence()
{
	enableUpdateLogic = true;
	player->ReturnPrevAnim();
}

/**************************************
ボンバー発射判定
***************************************/
bool PlayerObserver::ShouldFireBomber()
{
	//ボンバーのストックがなければfalse
	if (!bomberController->CanSet())
		return false;

	//発射の入力が行われていなければfalse
	if (!GetBomberInput())
		return false;

	return true;
}

/**************************************
ボンバー発射処理
***************************************/
void PlayerObserver::FirePlayerBomber(std::list<std::shared_ptr<Enemy>>& targetList)
{
	if (bomberController->CanSet())
		bomberController->SetPlayerBomber(targetList, player->transform.pos);
}

/**************************************
ボンバー発射処理
***************************************/
void PlayerObserver::FirePlayerBomber(std::shared_ptr<BossEnemyModel>& targetList)
{
	if (bomberController->CanSet())
		bomberController->SetPlayerBomber(targetList, player->transform.pos);
}

/**************************************
ボンバー発射処理
***************************************/
void  PlayerObserver::FirePlayerBomber(std::list<std::shared_ptr<RebarObstacle>>& targetList)
{
	if (bomberController->CanSet())
		bomberController->SetPlayerBomber(targetList, player->transform.pos);
}

/**************************************
加速演出処理
***************************************/
void PlayerObserver::OnStartAccel()
{
	const D3DXVECTOR3 EffectPos = D3DXVECTOR3(0.0f, 10.0f, 50.0f);
	D3DXVECTOR3 pos = (player->transform.pos + EffectPos);
	GameParticleManager::Instance()->SetAccelEffect(&pos);
}

/**************************************
ボンバーストック処理
***************************************/
void PlayerObserver::TryStockBomber()
{
	//ストックインターバルが経過しているか
	if (!bomberController->CanStock())
		return;

	//一筆書きが成立しているか
	if (!model->CheckOneStroke())
		return;

	//ボンバーをストック
	bomberController->AddStock();

	//エフェクト再生
	player->StockBomber();

	//スコアも加算する
	const int AddScoreValue = 100;
	SetAddScore(AddScoreValue);

	//SE再生
	//ストック追加時のSE
	Sound::GetInstance()->SetPlaySE(BOMBSTOCK, true, (Sound::GetInstance()->changevol / 5.0f));
}

/**************************************
PlayerTransform取得処理
***************************************/
const Transform& PlayerObserver::GetPlayerTransform() const
{
	return player->transform;
}

/**************************************
HP取得処理
***************************************/
float PlayerObserver::GetHpPercent()
{
	return player->GetHp() / Player::MaxHp;
}

/**************************************
生存判定
***************************************/
bool PlayerObserver::IsAlive()
{
	return player->IsAlive();
}

/**************************************
ボンバーストック数取得
***************************************/
int PlayerObserver::GetBomberStockNum()
{
	return bomberController->GetStockNum();
}

/**************************************
ボンバーチャージング判定
***************************************/
bool PlayerObserver::IsBomberCharging()
{
	return bomberController->IsCharging();
}

/**************************************
現在の位置取得
***************************************/
int PlayerObserver::GetCurrentPosition() const
{
	return moveTarget;
}

/**************************************
ゲームオーバー時処理
***************************************/
void PlayerObserver::OnGameOver()
{
	Sound::GetInstance()->SetPlaySE(GAMEOVER, true, Sound::GetInstance()->changevol / 2.0f);
	trailEffect->Uninit();
	ChangeStatePlayer(PlayerState::Falldown);  
}

/**************************************
プレイヤー回復処理
***************************************/
void PlayerObserver::HealPlayer(float value)
{
	player->AddHp(value);
}
