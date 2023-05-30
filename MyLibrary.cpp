//=============================================================================
//
// マイライブラリ処理 [myLibrary.cpp]
// Author : GP12B332 21 立花雄太
//
//=============================================================================
#include "main.h"
#include "MyLibrary.h"

/*****************************************************************************
マクロ定義
*****************************************************************************/
#define LENGTH_STARTSLOW			(100*100)			//減速が始まる距離（距離を2乗で計算するので200*200)
#define SPEEDMAGNI_MIN				(0.15f)				//減速率の最低値
#define LENGTH_STOPMOVE				(10*10)				//移動を止める目標との距離

#define YELLOWNUMBER_TEXTURE_NAME _T("data/TEXTURE/GUI/scoreNum.png")	//黄色い数字テクスチャ
#define NUMTEXTURE_DEVIDE_X			(10)								//数字テクスチャの分割数

/*****************************************************************************
マクロ定義
*****************************************************************************/
enum TEX_INDEX
{
	YELLOWNUM,
	TEX_MAX
};

/*****************************************************************************
グローバル変数
*****************************************************************************/
static LPDIRECT3DTEXTURE9 texture[TEX_MAX];				//黄色い数字テクスチャへのポインタ

/*****************************************************************************
初期化処理
*****************************************************************************/

/*****************************************************************************
終了処理
*****************************************************************************/
void UninitMyLibrary(void)
{
	for (int i = 0; i < TEX_MAX; i++)
	{
		if (texture[i] != NULL)
		{


			texture[i]->Release();
			texture[i] = NULL;
		}
	}
}

/******************************************************************************
//関数名	：float RandomRangef(float x, float y)
//引数1		：float x	：
//引数2		：float y	：
//戻り値	：x～yの範囲内の実数
//説明		：xからyの範囲内でランダムに実数を返す関数
******************************************************************************/
float RandomRangef(float x, float y)
{
	return rand() % 100 * (y - x) / 100.0f + x;

	/*
	float frand(unsigned v) {
		unsigned res = (v >> 9) | 0x3f800000;
		return (*(float*)&res) - 1.0f;
	}
	*/
}

/******************************************************************************
//関数名	：int RandomRange(int x, int y)
//引数1		：int x	：
//引数2		：int y	：
//戻り値	：x～yの範囲内の実数
//説明		：xからyの範囲内でランダムに整数を返す関数
******************************************************************************/
int RandomRange(int x, int y)
{
	return rand() % (y - x) + x;
}

/******************************************************************************
//関数名	：void MoveToTarget(D3DXVECTOR3 *currentPos, D3DXVECTOR3 *targetPos, float speed)
//引数1		：D3DXVECTOR3 *currentPos	：現在の座標（実行後、更新される）
//引数2		：D3DXVECTOR3 *targetPos	：目標座標
//引数3		：float speed				：移動速度
//戻り値	：目標座標との距離が一定以下の場合：true	それ以外：false
//説明		：目標座標へ移動する関数（目標座標に近づくにつれ減速する）
******************************************************************************/
bool MoveToTarget(D3DXVECTOR3 *currentPos, D3DXVECTOR3 *targetPos, float speed)
{
	//現在座標から目標座標へのベクトルを生成
	D3DXVECTOR3 tmpVec = *targetPos - *currentPos;


	//現在座標と目標座標の距離から減速率を計算
	float length = D3DXVec3LengthSq(&tmpVec);

	if (length < LENGTH_STOPMOVE)
	{
		//距離が規定値以下なら移動を止める
		return true;
	}

	float magni = (length > LENGTH_STARTSLOW) ? 1.0f : length / LENGTH_STARTSLOW;
	if (magni < SPEEDMAGNI_MIN)
	{
		magni = SPEEDMAGNI_MIN;
	}

	//現在座標から目標座標へのベクトルを正規化
	D3DXVec3Normalize(&tmpVec, &tmpVec);

	//現在座標を更新
	*currentPos += tmpVec * speed * magni;

	return false;
}

/******************************************************************************
//関数名	：bool StartAccel(D3DXVECTOR3 *velocity, float speed)
//引数1		：D3DXVECTOR3 *currentPos	：現在座標
//引数2		：D3DXVECTOR3 *direction	：目標方向
//引数3		：float speed				：目標スピード
//引数4		：int frame					：加速にかけるフレーム
//引数5		：int currentFrame			：現在の経過フレーム
//戻り値	：速度が目標に達している：true	それ以外：false
//説明		：目標速度へ加速していく関数
******************************************************************************/
bool StartAccel(D3DXVECTOR3 *currentPos, D3DXVECTOR3 *direction, float speed, int frame, int currentFrame)
{
	D3DXVECTOR3 vec;
	D3DXVec3Normalize(&vec, direction);

	//経過フレームが加速に必要なフレームより大きい場合
	if (currentFrame > frame)
	{
		*currentPos += vec * speed;
		return true;
	}
	//小さい場合
	else
	{
		*currentPos += vec * (speed * currentFrame / frame);
		return false;
	}
}

/******************************************************************************
//関数名	：float SnipePlayer(D3DXVECTOR3 *pos, D3DXVECTOR3 *targetPos)
//引数1		：D3DXVECTOR3 *pos			：設定座標
//引数2		：D3DXVECTOR3 *targetPos	：目標座標
//戻り値	：設定座標から目標座標への角度（ラジアン）
//説明		：設定座標から目標座標への角度を求める関数
******************************************************************************/
float SnipeTarget(D3DXVECTOR3 *pos, D3DXVECTOR3 *targetPos)
{
	float angle = atan2f(targetPos->y - pos->y, targetPos->x - pos->x);
	return angle;
}

/******************************************************************************
//関数名	：float RotateToPlayer(D3DXVECTOR3 *pos, D3DXVECTOR3 *targetPos, float currentRot, float angle)
//引数1		：D3DXVECTOR3 *pos			：設定座標
//引数2		：D3DXVECTOR3 *targetPos	：目標座標
//引数3		：float currentRot			：現在の角度(ラジアン）
//引数4		：float angle				：回転角度(ラジアン)
//戻り値	：目標座標の方へ回転させる場合：回転角度*方向		させない場合：0.0f
//説明		：設定座標から目標座標の方向へ回転させる関数
******************************************************************************/
float RotateToTarget(D3DXVECTOR3 *pos, D3DXVECTOR3 *targetPos, float currentRot, float angle)
{

	float baseAngle = D3DXToRadian(90.0f);

	D3DXVECTOR3 vec1 = D3DXVECTOR3(cosf(baseAngle + currentRot), sinf(baseAngle + currentRot), 0.0f);
	D3DXVECTOR3 vec2 = *targetPos - *pos;

	//各ベクトルを正規化
	D3DXVec3Normalize(&vec1, &vec1);
	D3DXVec3Normalize(&vec2, &vec2);

	//正規化したベクトル同士で内積を計算して回転の必要or不必要を計算
	float dot = D3DXVec3Dot(&vec1, &vec2);
	float tmp = cosf(angle);
	if (dot >= tmp)
	{
		return 0.0f;		//2つのベクトルの角度が回転角より小さければ0.0fをリターン
	}

	//回転させる方向の判定
	int sign = (vec1.y*vec2.x - vec1.x*vec2.y > 0) ? -1 : 1;

	//回転角度*方向をリターン
	return angle * sign;
}

/*******************************************************************
//関数名	：void DrawYellowNum(VERTEX_2D vertexWk[], int num)
//引数1		：VERTEX_2D vertexWk[]		：描画で使用する頂点情報
//引数2		：int num					：描画したい数字
//戻り値	：void
//説明		：黄色い数字を描画する関数
******************************************************************************/
void DrawYellowNum(VERTEX_2D *vertexWk, int num)
{
	float sizeX = 1.0f / NUMTEXTURE_DEVIDE_X;
	float x = sizeX * num;

	vertexWk[0].tex = D3DXVECTOR2(x, 0.0f);
	vertexWk[1].tex = D3DXVECTOR2(x + sizeX, 0.0f);
	vertexWk[2].tex = D3DXVECTOR2(x, 1.0f);
	vertexWk[3].tex = D3DXVECTOR2(x + sizeX, 1.0f);

	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	pDevice->SetTexture(0, texture[YELLOWNUM]);
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, NUM_POLYGON, vertexWk, sizeof(VERTEX_2D));
}

/*******************************************************************
//関数名	：WrapAround(int min, int max, int value)
//引数1		：int min		：最小値
//引数2		：int max		：最大値
//引数3		：int value		：対象の値
//戻り値	：void
//説明		：対象の値を最小から最大の範囲でラップさせる関数
******************************************************************************/
int WrapAround(int min, int max, int value)
{
	int n = (value - min) % (max - min);
	return (n >= 0) ? (n + min) : (n + max);
}

/*******************************************************************
//関数名	：WrapAroundf(float min, float max, float value)
//引数1		：float min		：最小値
//引数2		：float max		：最大値
//引数3		：float value		：対象の値
//戻り値	：void
//説明		：対象の値を最小から最大の範囲でラップさせる関数
******************************************************************************/
float WrapAroundf(float min, float max, float value)
{
	float mod = fmodf(value - min, max - min);
	return mod >= 0.0f ? mod + min : mod + max;
}

/*******************************************************************
//関数名	：Clamp(int min, int max, int value)
//引数1		：int min		：最小値
//引数2		：int max		：最大値
//引数3		：int value		：対象の値
//戻り値	：void
//説明		：対象の値を最小から最大の範囲に制限する関数
******************************************************************************/
int Clamp(int min, int max, int value)
{
	return Max(min, Min(max, value));
}

/*******************************************************************
//関数名	：Clampf(float min, float max, float value)
//引数1		：float min		：最小値
//引数2		：float max		：最大値
//引数3		：float value		：対象の値
//戻り値	：void
//説明		：対象の値を最小から最大の範囲に制限する関数
******************************************************************************/
float Clampf(float min, float max, float value)
{
	return Maxf(min, Minf(max, value));
}

/*******************************************************************
//関数名	：Min(int val1, int val2)
//引数1		：int val1		：対象値1
//引数2		：int val2		：対象値2
//戻り値	：小さい方の対象値
//説明		：2つの対象から小さい方の値を返す関数
******************************************************************************/
int Min(int val1, int val2)
{
	return (val1 < val2) ? val1 : val2;
}

/*******************************************************************
//関数名	：Max(int val1, int val2)
//引数1		：int val1		：対象値1
//引数2		：int val2		：対象値2
//戻り値	：大きい方の対象値
//説明		：2つの対象から大きい方の値を返す関数
******************************************************************************/
int Max(int val1, int val2)
{
	return (val1 > val2) ? val1 : val2;
}

/*******************************************************************
//関数名	：Minf(float val1, float val2)
//引数1		：float val1		：対象値1
//引数2		：float val2		：対象値2
//戻り値	：小さい方の対象値
//説明		：2つの対象から小さい方の値を返す関数
******************************************************************************/
float Minf(float val1, float val2)
{
	return (val1 < val2) ? val1 : val2;
}

/*******************************************************************
//関数名	：Maxf(float val1, float val2)
//引数1		：float val1		：対象値1
//引数2		：float val2		：対象値2
//戻り値	：大きい方の対象値
//説明		：2つの対象から大きい方の値を返す関数
******************************************************************************/
float Maxf(float val1, float val2)
{
	return (val1 > val2) ? val1 : val2;
}

/*******************************************************************
//関数名	：void ArrayShiftDown(int *array, int startNum, int size)
//引数1		：int *array		：シフトする配列の先頭へのポインタ
//引数2		：int startNum	：シフトを開始する位置
//引数3		：int size		：配列のサイズ
//戻り値	：void
//説明		：配列を開始位置から後ろにシフトする関数
******************************************************************************/
void ArrayShiftDown(int *array, int startNum, int size)
{
	if (startNum > size)
	{
		return;
	}

	for (int i = size - 1; i > startNum; i--)
	{
		array[i] = array[i - 1];
	}
}

/*******************************************************************
//関数名	：void VectorArrayShiftDown(int *array, int startNum, int size)
//引数1		：D3DXVECTOR3 *array	：シフトする配列の先頭へのポインタ
//引数2		：int startNum	：シフトを開始する位置
//引数3		：int size		：配列のサイズ
//戻り値	：void
//説明		：ベクトル配列を開始位置から後ろにシフトする関数
******************************************************************************/
void VectorArrayShiftDown(D3DXVECTOR3 *array, int startNum, int size)
{
	if (startNum > size)
	{
		return;
	}

	for (int i = size - 1; i > startNum; i--)
	{
		array[i] = array[i - 1];
	}
}

/*******************************************************************
//関数名	：void ArrayShiftUp(int *array, int startNum, int size)
//引数1		：int *array		：シフトする配列の先頭へのポインタ
//引数2		：int startNum		：シフトを開始する位置
//引数3		：int size			：配列のサイズ
//戻り値	：void
//説明		：配列を開始位置から上にシフトする関数
******************************************************************************/
void ArrayShiftUp(int *array, int startNum, int size)
{
	if (startNum > size)
	{
		return;
	}

	for (int i = 0; i < startNum; i++)
	{
		array[i] = array[i + 1];
	}
}

/*******************************************************************
//関数名	：void VectorArrayShiftUp(int *array, int startNum, int size)
//引数1		：int *array		：シフトする配列の先頭へのポインタ
//引数2		：int startNum		：シフトを開始する位置
//引数3		：int size			：配列のサイズ
//戻り値	：void
//説明		：ベクトル配列を開始位置から上にシフトする関数
******************************************************************************/
void VectorArrayShiftUp(D3DXVECTOR3 *array, int startNum, int size)
{
	if (startNum > size)
	{
		return;
	}

	for (int i = 0; i < startNum; i++)
	{
		array[i] = array[i + 1];
	}
}

/*******************************************************************
//関数名	：void ArrayShiftUp(int *array, int startNum, int size)
//引数1		：int *dst		：コピー対象
//引数2		：int *src		：コピー元
//引数3		：int size		：配列のサイズ
//戻り値	：void
//説明		：配列をコピーする関数
******************************************************************************/
void ArrayCopy(int *dst, int *src, int size)
{
	for (int i = 0; i < size; i++)
	{
		dst[i] = src[i];
	}
}

/*******************************************************************
//関数名	：void RotateByQuaternion(D3DXVECTOR3 *initPos, D3DXVECTOR3 *axis, FLOAT deg, D3DXVECTOR3 *out)
//引数1		：D3DXVECTOR3 *initPos	回転させたい対象の座標
//引数2		：D3DXVECTOR3 *axis		回転軸
//引数3		：FLAOT deg				回転角度
//引数4		：D3DXVECTOR3 *out		回転結果を格納するポインタ
//戻り値	：void
//説明		：クォータニオンによる任意軸回転
********************************************************************/
void RotateByQuaternion(const D3DXVECTOR3 *initPos, const D3DXVECTOR3 *axis, const FLOAT rad, D3DXVECTOR3 *out)
{
	D3DXQUATERNION r, ans;		//r:回転クォータニオンQの共役クォータニオン　ans:回転後の座標

	D3DXQUATERNION p(initPos->x, initPos->y, initPos->z, 0.0f); //回転前の位置のクォータニオンp作成
	D3DXQUATERNION q(0.0f, 0.0f, 0.0f, 1.0f);					//回転用の単位クォータニオンq作成
	//FLOAT angle = D3DXToRadian(rad);

	D3DXQuaternionRotationAxis(&q, axis, rad);				//回転軸周りの回転クォータニオンqを作成
	D3DXQuaternionConjugate(&r, &q);							//共役クォータニオンrを作成
	D3DXQuaternionMultiply(&ans, &r, &p);						//r・pを計算
	D3DXQuaternionMultiply(&ans, &ans, &q);						//p・qを計算

	out->x = ans.x;
	out->y = ans.y;
	out->z = ans.z;
}

/********************************************************************
//関数名	：float GetAngleFromTwoVector(D3DXVECTOR3 *pV1, D3DXVECTOR3 *pV2)
//引数1		：D3DXVECTOR3 *pV1 ベクトル1
//引数2		：D3DXVECTOR3 *pV2 ベクトル2
//戻り値	：float 2つのベクトルがなす角度
//説明		：2つのベクトルがなす角度
********************************************************************/
float GetAngleFromTwoVector(D3DXVECTOR3 *pV1, D3DXVECTOR3 *pV2)
{
	D3DXVECTOR3 v1, v2;

	D3DXVec3Normalize(&v1, pV1);
	D3DXVec3Normalize(&v2, pV2);

	return acosf(D3DXVec3Dot(&v1, &v2));
}

/********************************************************************
//関数名	：void CopyVtxBuff(unsigned size, void** src, LPDIRECT3DVERTEXBUFFER9 buff)
//引数1		：unsigned size コピーするバイト数
//引数2		：void* src コピー元のアドレス
//引数3		：LPDIRECT3DVERTEXBUFFER9 buff コピー先の頂点バッファ
//戻り値	：void
//説明		：頂点バッファへ値をコピーする関数
********************************************************************/
void CopyVtxBuff(unsigned size, void* src, LPDIRECT3DVERTEXBUFFER9 buff)
{
	void *p = NULL;

	buff->Lock(0, 0, &p, 0);

	memcpy(p, src, size);

	buff->Unlock();
}

/********************************************************************
//関数名	：void MakeParticleUnitBuffer(D3DXVECTOR2 *size, D3DXVECTOR2 *tecDiv, LPDIRECT3DVERTEXBUFFER9 *buff)
//引数1		：D3DXVECTOR2 *size パーティクルサイズ
//引数2		：D3DXVECTOR2 *texDiv テクスチャの分割設定
//引数3		：LPDIRECT3DVERTEXBUFFER9 buff 作成した頂点バッファを指すインターフェイス
//戻り値	：void
//説明		：パーティクル単位頂点バッファを作成する関数
********************************************************************/
void MakeParticleUnitBuffer(D3DXVECTOR2 *size, D3DXVECTOR2 *texDiv, LPDIRECT3DVERTEXBUFFER9 *buff)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	pDevice->CreateVertexBuffer(sizeof(ParticleUnit) * NUM_VERTEX, 0, 0, D3DPOOL_MANAGED, buff, 0);
	
	ParticleUnit *p;
	(*buff)->Lock(0, 0, (void**)&p, 0);

	//単位サイズ設定
	p[0].vtx = D3DXVECTOR3(-size->x / 2.0f, size->y / 2.0f, 0.0f);
	p[1].vtx = D3DXVECTOR3(size->x / 2.0f, size->y / 2.0f, 0.0f);
	p[2].vtx = D3DXVECTOR3(-size->x / 2.0f, -size->y / 2.0f, 0.0f);
	p[3].vtx = D3DXVECTOR3(size->x / 2.0f, -size->y / 2.0f, 0.0f);
	
	//単位UV設定
	float u = 1.0f / texDiv->x;
	float v = 1.0f / texDiv->y;
	p[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	p[1].tex = D3DXVECTOR2(u, 0.0f);
	p[2].tex = D3DXVECTOR2(0.0f, v);
	p[3].tex = D3DXVECTOR2(u, v);

	(*buff)->Unlock();
}

/********************************************************************
//関数名	：void MakeTransformBuffer
//引数1		：DWORD maxNum パーティクルの最大数
//引数2		：LPDIRECT3DVERTEXBUFFER9 *buff 作成した頂点バッファを指すインターフェイス
//戻り値	：void
//説明		：SRT情報バッファを作成する関数
********************************************************************/
void MakeTransformBuffer(DWORD maxNum, LPDIRECT3DVERTEXBUFFER9 *buff)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	pDevice->CreateVertexBuffer(sizeof(Transform) * maxNum, 0, 0, D3DPOOL_MANAGED, buff, 0);
}

/********************************************************************
//関数名	：void MakeUVBuffer
//引数1		：DWORD maxNum パーティクルの最大数
//引数2		：LPDIRECT3DVERTEXBUFFER9 buff 作成した頂点バッファを指すインターフェイス
//戻り値	：void
//説明		：UV情報を作成する関数
********************************************************************/
void MakeUVBUffer(DWORD maxNum, LPDIRECT3DVERTEXBUFFER9 *buff)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	pDevice->CreateVertexBuffer(sizeof(ParticleUV) * maxNum, 0, 0, D3DPOOL_MANAGED, buff, 0);
}

/********************************************************************
//関数名	：void CalcScreenToWorld
//引数1		：D3DXVECTOR3 *pOut 計算結果の格納先
//引数2		：const D3DXVECTOR3 *screenPos 変換するスクリーン座標
//引数3		：float f Near～Farの位置指定(0.0f～1.0f）
//戻り値	：void
//説明		：スクリーン座標をワールド座標に変換する処理
********************************************************************/
void CalcScreenToWorld(D3DXVECTOR3 *pOut, const D3DXVECTOR3 *screenPos, float f)
{
	D3DXMATRIX view, proj, viewport, invView, invProj, invViewport;
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	//ビュー行列、プロジェクション行列取得
	pDevice->GetTransform(D3DTS_VIEW, &view);
	pDevice->GetTransform(D3DTS_PROJECTION, &proj);

	//ビューポート行列計算
	D3DXMatrixIdentity(&viewport);
	viewport._11 = SCREEN_WIDTH / 2.0f;
	viewport._22 = -SCREEN_HEIGHT / 2.0f;
	viewport._41 = SCREEN_WIDTH / 2.0f;
	viewport._42 = SCREEN_HEIGHT / 2.0f;

	//逆行列計算
	D3DXMatrixInverse(&invView, NULL, &view);
	D3DXMatrixInverse(&invProj, NULL, &proj);
	D3DXMatrixInverse(&invViewport, NULL, &viewport);

	//座標を変換
	D3DXMATRIX tmp = invViewport * invProj * invView;
	D3DXVECTOR3 pos = D3DXVECTOR3(screenPos->x, screenPos->y, f);
	D3DXVec3TransformCoord(pOut, &pos, &tmp);
}