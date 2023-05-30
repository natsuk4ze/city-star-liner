//=====================================
//
//ラインレンダラー処理[LineRenderer.cpp]
//Author:GP12B332 21 立花雄太
//
//=====================================
#include "LineRenderer.h"

/**************************************
マクロ定義
***************************************/

/**************************************
構造体定義
***************************************/

/**************************************
グローバル変数
***************************************/

/**************************************
コンストラクタ
***************************************/
LineRenderer::LineRenderer()
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	//頂点バッファ作成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * NUM_VERTEX,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&vtxBuff,
		0);
	VERTEX_3D *pVtx;
	vtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	//UV座標設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	//色初期化
	pVtx[0].diffuse =
		pVtx[1].diffuse =
		pVtx[2].diffuse =
		pVtx[3].diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	vtxBuff->Unlock();
}

/**************************************
デストラクタ
***************************************/
LineRenderer::~LineRenderer()
{
	SAFE_RELEASE(vtxBuff);
}

/**************************************
初期化処理
***************************************/
void LineRenderer::Init(D3DXVECTOR3 *right, D3DXVECTOR3 *left, float width)
{
	//太さを保存
	this->width = width;

	//始点と終点を結ぶ線分を計算し、長さを半分にする
	D3DXVECTOR3 diff = *left - *right;
	diff /= 2.0f;
	D3DXVECTOR3 pos = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	//線分に垂直なベクトルを求める
	D3DXVec3Cross(&vtxUp, &pos, &diff);
	D3DXVec3Normalize(&vtxUp, &vtxUp);

	//求めたベクトルから頂点座標を計算
	VERTEX_3D *pVtx;
	vtxBuff->Lock(0, 0, (void**)&pVtx, 0);
	pVtx[0].vtx = -diff + vtxUp * width;
	pVtx[1].vtx = diff + vtxUp * width;
	pVtx[2].vtx = -diff - vtxUp * width;
	pVtx[3].vtx = diff - vtxUp * width;
	vtxBuff->Unlock();
}

/**************************************
描画処理
***************************************/
void LineRenderer::Draw()
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	//もし描画されなかったカリングをオフにする
	pDevice->SetRenderState(D3DRS_LIGHTING, false);

	pDevice->SetStreamSource(0, vtxBuff, 0, sizeof(VERTEX_3D));
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, NUM_POLYGON);

	pDevice->SetRenderState(D3DRS_LIGHTING, true);
}

/**************************************
右端設定処理
***************************************/
void LineRenderer::SetRightPos(D3DXVECTOR3 *pos)
{
	VERTEX_3D *pVtx;
	vtxBuff->Lock(0, 0, (void**)&pVtx, 0);
	pVtx[1].vtx = *pos + vtxUp * width;
	pVtx[3].vtx = *pos - vtxUp * width;
	vtxBuff->Unlock();
}

/**************************************
左端設定処理
***************************************/
void LineRenderer::SetLeftPos(D3DXVECTOR3 *pos)
{
	VERTEX_3D *pVtx;
	vtxBuff->Lock(0, 0, (void**)&pVtx, 0);
	pVtx[0].vtx = *pos + vtxUp * width;
	pVtx[2].vtx = *pos - vtxUp * width;
	vtxBuff->Unlock();
}

/**************************************
色設定処理
***************************************/
void LineRenderer::SetColor(D3DXCOLOR color)
{
	VERTEX_3D *pVtx;
	vtxBuff->Lock(0, 0, (void**)&pVtx, 0);
	pVtx[0].diffuse =
		pVtx[1].diffuse =
		pVtx[2].diffuse =
		pVtx[3].diffuse = color;
	vtxBuff->Unlock();
}