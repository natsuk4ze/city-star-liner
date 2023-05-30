//=====================================
//
//プレイヤートレイル処理[PlayerTrail.cpp]
//Author:GP12B332 21 立花雄太
//
//=====================================
#include "PlayerTrail.h"

/**************************************
マクロ定義
***************************************/
#define PLAYERTRAIL_TEXTURE_NAME		"data/TEXTURE/Effect/PlayerTrail.png"

/**************************************
構造体定義
***************************************/

/**************************************
グローバル変数
***************************************/

/**************************************
コンストラクタ
***************************************/
PlayerTrail::PlayerTrail()
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	MakeLineBuffer();
	MakeEdgeBuffer();
	
	texture = CreateTextureFromFile((LPSTR)PLAYERTRAIL_TEXTURE_NAME, pDevice);

	bool active = false;
}

/**************************************
デストラクタ
***************************************/
PlayerTrail::~PlayerTrail()
{
	SAFE_RELEASE(lineBuff);
	SAFE_RELEASE(edgeBuff);
	SAFE_RELEASE(texture);
}

/**************************************
初期化処理
***************************************/
void PlayerTrail::Init(D3DXVECTOR3 *pPos)
{
	rightPos = leftPos = *pPos;
	playerPos = pPos;
	scale = 1.0f;
	cntFrame = 0;
	flgFadeout = false;
	active = true;
}

/**************************************
終了処理
***************************************/
void PlayerTrail::Uninit()
{
	playerPos = NULL;
	flgFadeout = true;
}

/**************************************
更新処理
***************************************/
void PlayerTrail::Update()
{
	if (!active)
		return;

	if (flgFadeout)
	{
		const int DurationFadeout = 20;
		cntFrame++;
		scale = 1.0f - (float)cntFrame / DurationFadeout;

		if (cntFrame == DurationFadeout)
			active = false;
	}

	if(playerPos != NULL)
		leftPos = *playerPos;

	SetLine();
}

/**************************************
描画処理
***************************************/
void PlayerTrail::Draw()
{
	if (!active)
		return;

	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	pDevice->SetFVF(FVF_VERTEX_3D);
	pDevice->SetStreamSource(0, lineBuff, 0, sizeof(VERTEX_3D));
	pDevice->SetTexture(0, texture);

	pDevice->SetRenderState(D3DRS_ZENABLE, false);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pDevice->SetRenderState(D3DRS_LIGHTING, false);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	D3DXMATRIX mtxWorld;
	D3DXMatrixIdentity(&mtxWorld);
	pDevice->SetTransform(D3DTS_WORLD, &mtxWorld);

	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, NUM_POLYGON);

	pDevice->SetRenderState(D3DRS_ZENABLE, true);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pDevice->SetRenderState(D3DRS_LIGHTING, true);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

/**************************************
ラインバッファー作成処理
***************************************/
void PlayerTrail::MakeLineBuffer()
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * NUM_VERTEX,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&lineBuff,
		0);

	VERTEX_3D *pVtx;
	lineBuff->Lock(0, 0, (void**)&pVtx, 0);

	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	pVtx[0].nor =
		pVtx[1].nor =
		pVtx[2].nor =
		pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	pVtx[0].diffuse =
		pVtx[1].diffuse =
		pVtx[2].diffuse =
		pVtx[3].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	lineBuff->Unlock();

}

/**************************************
エッジバッファー作成処理
***************************************/
void PlayerTrail::MakeEdgeBuffer()
{

}

/**************************************
ライン設定処理
***************************************/
void PlayerTrail::SetLine()
{
	//右端と左端を結ぶ線分を計算し正規化
	D3DXVECTOR3 diff = leftPos - rightPos;
	D3DXVec3Normalize(&diff, &diff);

	//線分に垂直なベクトルを求める
	D3DXVECTOR3 vtxUp;
	D3DXVECTOR3 pos = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	D3DXVec3Cross(&vtxUp, &pos, &diff);
	D3DXVec3Normalize(&vtxUp, &vtxUp);

	//求めたベクトルから頂点座標を計算
	const float Length = 1.0f;
	VERTEX_3D *pVtx;
	lineBuff->Lock(0, 0, (void**)&pVtx, 0);
	pVtx[0].vtx = leftPos + vtxUp * Length * scale;
	pVtx[1].vtx = rightPos + vtxUp * Length * scale;
	pVtx[2].vtx = leftPos - vtxUp * Length * scale;
	pVtx[3].vtx = rightPos - vtxUp * Length * scale;
	lineBuff->Unlock();


}