//=====================================
//
//3次元ベクトルヘッダ[Vector3.h]
//Author:GP12B332 21 立花雄太
//
//=====================================
#ifndef _VECTOR3_H_
#define _VECTOR3_H_

#include "../main.h"

/**************************************
マクロ定義
***************************************/

/**************************************
D3DXVECTOR3ユーティリティクラス
***************************************/
class Vector3
{
public:
	//前後左右上下
	static const D3DXVECTOR3 Up, Down;
	static const D3DXVECTOR3 Right, Left;
	static const D3DXVECTOR3 Forward, Back;
	static const D3DXVECTOR3 One, Zero;

	//2つのうち大きいベクトルを返す
	static D3DXVECTOR3 Max(const D3DXVECTOR3& arg1, const D3DXVECTOR3& arg2);

	//2つのうち小さいベクトルを返す
	static D3DXVECTOR3 Min(const D3DXVECTOR3& arg1, const D3DXVECTOR3& arg2);

	//argをmin〜maxの範囲に制限して返す
	static D3DXVECTOR3 Clamp(const D3DXVECTOR3& min, const D3DXVECTOR3& max, const D3DXVECTOR3& arg);

	//min〜maxの範囲でtで線形補間して返す
	static D3DXVECTOR3 Lerp(float t, D3DXVECTOR3& min, const D3DXVECTOR3& max);

	//min〜maxの範囲でtで球面線形補間して返す
	static D3DXVECTOR3 Slerp(float t, const D3DXVECTOR3& from, const D3DXVECTOR3& to);

	//2つのベクトルがなす角度をオイラー角で返す
	static float Angle(const D3DXVECTOR3& from, const D3DXVECTOR3& to);

	//2つのベクトルと直行する軸を返す
	static D3DXVECTOR3 Axis(const D3DXVECTOR3& from, const D3DXVECTOR3& to);

	//2つのベクトルの距離を返す
	static float Distance(const D3DXVECTOR3& from, const D3DXVECTOR3& to);
		
private:
	Vector3();
};

#endif