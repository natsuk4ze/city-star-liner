//=====================================
//
//プレイヤーバレット処理[PlayerBullet.cpp]
//Author:GP12B332 21 立花雄太
//
//=====================================
#include "PlayerBullet.h"
#include "Framework/Easing.h"
#include "GameParticleManager.h"

/**************************************
マクロ定義
***************************************/
#define PLAYERBULLET_LIFE_COUNT		(60)
#define PLAYERBULLET_FADE_FRAME		(30)
#define PLAYERBULLET_FADE_START		(PLAYERBULLET_LIFE_COUNT-PLAYERBULLET_FADE_FRAME)
#define PLAYERBULLET_VTX_LENGTH		(2.0f)
#define PLAYERBLLET_VTX_DELTA		(PLAYERBULLET_VTX_LENGTH / PLAYERBULLET_FADE_FRAME)
#define PLAYERBULLET_COLLIDER_SIZE	(D3DXVECTOR3(5.0f, 5.0f, 5.0f))

/**************************************
構造体定義
***************************************/

/**************************************
staticメンバ
***************************************/

/****************************************
初期化処理
****************************************/
void PlayerBullet::Init(LineTrailModel model)
{
	cntFrame = 0;
	collider->SetTrailIndex(model);
	SetEdgePos(model);
	collider->active = true;
	colliderR->active = true;
	colliderL->active = true;
	active = true;
}

/****************************************
終了処理
*****************************************/
void PlayerBullet::Uninit()
{
	collider->active = false;
	colliderL->active = false;
	colliderR->active = false;
	isDestroyed = false;
	active = false;
}

/****************************************
更新処理
*****************************************/
void PlayerBullet::Update()
{
	if (!active)
		return;

	//移動処理
	const float Speed = 30.0f;
	pos.z += Speed;
	edgeL.z += Speed;
	edgeR.z += Speed;

	cntFrame++;

	//縮小処理
	if (cntFrame > PLAYERBULLET_FADE_START)
	{
		VERTEX_3D *pVtx;
		vtxBuff->Lock(0, 0, (void**)&pVtx, 0);
		pVtx[0].vtx -= vtxUp * PLAYERBLLET_VTX_DELTA;
		pVtx[1].vtx -= vtxUp * PLAYERBLLET_VTX_DELTA;
		pVtx[2].vtx += vtxUp * PLAYERBLLET_VTX_DELTA;
		pVtx[3].vtx += vtxUp * PLAYERBLLET_VTX_DELTA;
		vtxBuff->Unlock();
	}

	//寿命判定
	if (cntFrame > PLAYERBULLET_LIFE_COUNT)
	{
		Uninit();
	}
}

/****************************************
描画処理
*****************************************/
void PlayerBullet::Draw()
{
	if (!active)
		return;

	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	pDevice->SetStreamSource(0, vtxBuff, 0, sizeof(VERTEX_3D));

	D3DXMATRIX mtxWorld;
	D3DXMatrixIdentity(&mtxWorld);
	D3DXMatrixTranslation(&mtxWorld, pos.x, pos.y, pos.z);

	pDevice->SetTransform(D3DTS_WORLD, &mtxWorld);

	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, NUM_POLYGON);
}

/****************************************
コンストラクタ
****************************************/
PlayerBullet::PlayerBullet()
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	
	//頂点バッファ作成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * NUM_VERTEX, D3DUSAGE_WRITEONLY, FVF_VERTEX_3D, D3DPOOL_MANAGED, &vtxBuff, 0);

	//頂点座標以外の情報を埋める
	VERTEX_3D *pVtx;

	vtxBuff->Lock(0, 0, (void**)&pVtx, 0);
	
	const float texSize = 1.0f;
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(texSize, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(texSize, 1.0f);

	pVtx[0].diffuse =
		pVtx[1].diffuse =
		pVtx[2].diffuse =
		pVtx[3].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	pVtx[0].nor =
		pVtx[1].nor =
		pVtx[2].nor =
		pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	vtxBuff->Unlock();

	//コライダーインスタンス作成
	collider = new TrailCollider(TrailColliderTag::PlayerBullet);

	//TrailColliderのZ座標アドレスを設定
	collider->SetAddressZ(&pos.z);

	//コライダーの観測者に自身を追加
	collider->AddObserver(this);

	//端点のコライダーインスタンス作成
	colliderR = new BoxCollider3D(BoxCollider3DTag::PlayerBullet, &edgeR, PLAYERBULLET_COLLIDER_SIZE);
	colliderL = new BoxCollider3D(BoxCollider3DTag::PlayerBullet, &edgeL, PLAYERBULLET_COLLIDER_SIZE);
	colliderR->AddObserver(this);
	colliderL->AddObserver(this);
}

/****************************************
デストラクタ
****************************************/
PlayerBullet::~PlayerBullet()
{
	SAFE_RELEASE(vtxBuff);
	SAFE_DELETE(collider);
	SAFE_DELETE(colliderR);
	SAFE_DELETE(colliderL);
}

/****************************************
端点セット処理
*****************************************/
void PlayerBullet::SetEdgePos(LineTrailModel model)
{
	//始点と終点を結ぶ線分を計算し、長さを半分にする
	model.GetEdgePos(&edgeR, &edgeL);
	D3DXVECTOR3 diff = edgeR - edgeL;
	diff /= 2.0f;

	//線分に垂直なベクトルを求める
	D3DXVECTOR3 pos = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	D3DXVec3Cross(&vtxUp, &pos, &diff);
	D3DXVec3Normalize(&vtxUp, &vtxUp);

	//求めたベクトルから頂点座標を計算
	VERTEX_3D *pVtx;
	vtxBuff->Lock(0, 0, (void**)&pVtx, 0);
	pVtx[0].vtx = -diff + vtxUp * PLAYERBULLET_VTX_LENGTH;
	pVtx[1].vtx = diff + vtxUp * PLAYERBULLET_VTX_LENGTH;
	pVtx[2].vtx = -diff - vtxUp * PLAYERBULLET_VTX_LENGTH;
	pVtx[3].vtx = diff - vtxUp * PLAYERBULLET_VTX_LENGTH;
	vtxBuff->Unlock();

	//ワールド座標を始点と終点の真ん中に設定
	pos = edgeL + diff;

	//パーティクルセット
	GameParticleManager::Instance()->SetPlayerBulletParticle(&pos, &active, &edgeR, &edgeL);
}
/****************************************
衝突判定通知レシーバー
*****************************************/
void PlayerBullet::OnNotified(ObserveSubject *notifier)
{
	//バレットは貫通する仕様に変更
	//isDestroyed = true;
}

void PlayerBullet::OnNotified(BoxCollider3DTag other)
{
	//端点が当たった場合はなにもしない
}