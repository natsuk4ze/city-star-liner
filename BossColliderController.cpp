//=====================================
//
//ボスコライダーコントローラ処理[BossColliderController.cpp]
//Author:GP12A332 21 立花雄太
//
//=====================================
#include "BossColliderController.h"
#include "Framework\ResourceManager.h"
#include "BossColliderGuide.h"
#include "BossEnemyModel.h"
#include "GameParticleManager.h"

using namespace std;
/**************************************
マクロ定義
***************************************/

/**************************************
コンストラクタ
***************************************/
BossColliderController::BossColliderController(BossEnemyModel& model) : posZ(500.0f), model(model)
{
	//ポリゴン準備
	ResourceManager::Instance()->MakePolygon("BossColliderEdge", "data/TEXTURE/Enemy/BossColliderEdge.png", D3DXVECTOR2(40.0f, 40.0f));
	ResourceManager::Instance()->MakePolygon("BossColliderLine", "data/TEXTURE/Enemy/BossCollider.png", D3DXVECTOR2(0.5f, 20.0f));
}

/**************************************
デストラクタ
***************************************/
BossColliderController::~BossColliderController()
{
	for (auto&& guide : guideMap)
	{
		SAFE_DELETE(guide.second);
	}
	guideMap.clear();

	for (auto&& collider : colliderList)
	{
		SAFE_DELETE(collider);
	}
	colliderList.clear();
}

/**************************************
更新処理
***************************************/
void BossColliderController::Update()
{
	for (auto&& guide : guideMap)
	{
		guide.second->Update();
	}
}

/**************************************
描画処理
***************************************/
void BossColliderController::Draw()
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);
	pDevice->SetRenderState(D3DRS_ZENABLE, false);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	pDevice->SetRenderState(D3DRS_LIGHTING, false);
	pDevice->SetRenderState(D3DRS_CULLMODE, false);

	for (auto&& guide : guideMap)
	{
		guide.second->Draw();
	}

	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);
	pDevice->SetRenderState(D3DRS_ZENABLE, true);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pDevice->SetRenderState(D3DRS_LIGHTING, true);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

/**************************************
セット処理
***************************************/
void BossColliderController::SetCollider(const std::vector<int>& edgeList)
{
	DeleteAll();

	const UINT EdgeMax = edgeList.size() - 1;
	for (UINT i = 0; i < EdgeMax; i++)
	{
		int left = Min(edgeList[i], edgeList[i + 1]);
		int right = Max(edgeList[i], edgeList[i + 1]);
		LineTrailModel model = LineTrailModel(left, right);

		TrailCollider *collider = new TrailCollider(TrailColliderTag::Enemy);
		collider->SetTrailIndex(model);
		collider->SetAddressZ(&posZ);
		collider->AddObserver(this);

		guideMap[collider] = new BossColliderGuide(model);

		modelMap[collider] = model;

		colliderList.push_back(collider);
	}

	cntHit = 0;
}

/**************************************
全消去処理
***************************************/
void BossColliderController::DeleteAll()
{
	for (auto&& pair : guideMap)
	{
		SAFE_DELETE(pair.second);
	}
	guideMap.clear();

	for (auto&& collider : colliderList)
	{
		SAFE_DELETE(collider);
	}
	colliderList.clear();
}

/**************************************
当たり判定通知処理
***************************************/
void BossColliderController::OnNotified(ObserveSubject* notifier)
{
	model.OnDamage();

	LineTrailModel model = modelMap[notifier];

	D3DXVECTOR3 right, left;
	model.GetEdgePos(&right, &left);
	right.z = left.z = posZ;
	D3DXVECTOR3 offset = right - left;
	offset /= 10.0f;
	D3DXVECTOR3 temp = left + offset;
	for (int i = 0; i < 10; i++)
	{
		GameParticleManager::Instance()->SetBossHit(&temp);
		left += offset;
	}

	modelMap.erase(notifier);

	SAFE_DELETE(guideMap[notifier]);
	guideMap.erase(notifier);

	auto itr = std::find(colliderList.begin(), colliderList.end(), notifier);
	(*itr)->active = false;

	//
	cntHit++;
	if (cntHit == colliderList.size())
	{
		this->model.ChangeState(BossEnemyModel::State::LargeDamage);
	}
}