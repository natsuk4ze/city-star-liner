//=====================================
//
//MidiumEnemyModel.cpp
//機能:中型エネミーモデル
//Author:GP12B332 21 立花雄太
//
//=====================================
#include "MidiumEnemyModel.h"
#include "GameParticleManager.h"
#include "Framework\CameraShakePlugin.h"
#include "sound.h"

/**************************************
グローバル変数
***************************************/

/**************************************
コンストラクタ
***************************************/
MidiumEnemyModel::MidiumEnemyModel() :
	step(0),
	hp(3)
{
	cntFrame = 0;
	active = true;
	pos.z = PosZ;

	explosionScale = 5.0f;
}

/**************************************
初期化処理
***************************************/
void MidiumEnemyModel::Init(std::vector<LineTrailModel>& moveList)
{
	moveTargetList.assign(moveList.begin(), moveList.end());

	D3DXVECTOR3 edgeR, edgeL;
	moveTargetList[step].GetEdgePos(&edgeR, &edgeL);

	for (int i = 0; i < EnemyNum; i++)
	{
		enemyList.push_back(std::make_shared<EnemyMidium>());
		const D3DXVECTOR3 InitPos = D3DXVECTOR3(0.0f, -400.0f, PosZ);
		D3DXVECTOR3 moveTarget = GetMoveTarget(i);
		enemyList[i]->VInit();
		dynamic_pointer_cast<EnemyMidium>(enemyList[i])->Set(InitPos);

	}


	StartMove();
}

/**************************************
終了処理
***************************************/
void MidiumEnemyModel::Uninit()
{
	collider->active = false;
	active = false;
}

/**************************************
更新処理
***************************************/
int MidiumEnemyModel::Update()
{
	int res = EnemyModelResult::StateContinuous;

	if (state == State::Wait)
		res = UpdateOnWait();
	else if (state == State::Moving)
		res = UpdateOnMoving();
	else if (state == State::Damage)
		res = UpdateOnDamage();

	for (auto&& enemy : enemyList)
	{
		enemy->VUpdate();
	}

	return res;
}

/**************************************
ショット座標取得処理
***************************************/
void MidiumEnemyModel::GetShotPos(std::vector<D3DXVECTOR3>& out)
{
	for (auto&& enemy : enemyList)
	{
		const int LoopMax = 5;
		for (int i = 0; i < LoopMax; i++)
		{
			out.push_back(enemy->m_Pos + ShotPosOffset);
		}
	}
}

/**************************************
衝突判定通知処理
***************************************/
void MidiumEnemyModel::OnNotified(ObserveSubject * notifier)
{
	//ダメージ演出中は判定しない
	if (state == State::Damage)
		return;

	hp--;

	if (hp == 0)
	{
		for (auto&& enemy : enemyList)
		{
			enemy->m_FlgDestroyed = true;
			Sound::GetInstance()->SetPlaySE(ENEMYDOWN2, true, (Sound::GetInstance()->changevol / 10.0f));
		}

		//カメラを大きく揺らす
		const D3DXVECTOR3 ShakeAmplitude = D3DXVECTOR3(5.0f, 5.0f, 0.0f);
		const int ShakeDuration = 180;
		Camera::ShakePlugin::Instance()->Set(ShakeAmplitude, ShakeDuration);

	}
	else {
		Sound::GetInstance()->SetPlaySE(MIDENEMYDAMAGE, true, (Sound::GetInstance()->changevol / 2.0f));

		//カメラを小さく揺らす
		const D3DXVECTOR3 ShakeAmplitude = D3DXVECTOR3(0.75f, 0.75f, 0.0f);
		const int ShakeDuration = 120;
		Camera::ShakePlugin::Instance()->Set(ShakeAmplitude, ShakeDuration);
	}

	for (auto&& emitter : chageEffectList)
	{
		if (emitter != NULL)
			emitter->active = false;
	}

	for (auto&& enemy : enemyList)
	{
		const float explosionScale = 2.5f;
		GameParticleManager::Instance()->SetEnemyExplosion(&enemy->m_Pos, explosionScale);
	}

	cntFrame = 0;
	state = State::Damage;
}

/**************************************
移動目標取得処理
***************************************/
D3DXVECTOR3 MidiumEnemyModel::GetMoveTarget(int i)
{
	D3DXVECTOR3 edgeR, edgeL;
	moveTargetList[step].GetEdgePos(&edgeR, &edgeL);
	edgeR.z = edgeL.z = PosZ;

	D3DXVECTOR3 offset = (edgeR - edgeL) / 5.0f;

	return i == 0 ? edgeL + offset : edgeR - offset;
}

/**************************************
Wait更新処理
***************************************/
int MidiumEnemyModel::UpdateOnWait()
{
	const int WaitDuration = 180;
	const int AttackTiming = 120;

	cntFrame++;

	if (cntFrame == WaitDuration)
	{
		cntFrame = 0;
		StartMove();
	}

	if (cntFrame == AttackTiming)
	{
		return EnemyModelResult::AttackTiming;
	}

	return StateContinuous;
}

/**************************************
Moving更新処理
***************************************/
int MidiumEnemyModel::UpdateOnMoving()
{
	cntFrame++;

	if (cntFrame == MoveDuration)
	{
		cntFrame = 0;
		state = State::Wait;

		//移動先が残っていなければ終了、それ以外はWaitへ遷移してチャージ開始
		if (step <= moveTargetList.size())
		{
			collider->active = true;
			return ChargeTiming;
		}
		else
		{
			Uninit();
		}
	}

	return StateContinuous;
}

/**************************************
Damage更新処理
***************************************/
int MidiumEnemyModel::UpdateOnDamage()
{
	cntFrame++;

	for (auto&& enemy : enemyList)
	{
		dynamic_pointer_cast<EnemyMidium>(enemy)->HitAnimation();
	}

	const int DamageDuration = 19;
	if (cntFrame == DamageDuration)
	{
		cntFrame = 0;
		StartMove();
	}

	return StateContinuous;
}

/**************************************
移動開始処理
***************************************/
void MidiumEnemyModel::StartMove()
{
	cntFrame = 0;
	state = State::Moving;

	int cntLoop = 0;
	for (auto&& enemy : enemyList)
	{
		D3DXVECTOR3 moveTarget = step < moveTargetList.size() ? GetMoveTarget(cntLoop) : D3DXVECTOR3(0.0f, 400.0f, PosZ);
		dynamic_pointer_cast<EnemyMidium>(enemy)->Move(moveTarget, MoveDuration);
		cntLoop++;
	}

	if (step < moveTargetList.size())
	{
		model = moveTargetList[step];
		collider->SetTrailIndex(model);
	}
	collider->active = false;

	step++;
}






