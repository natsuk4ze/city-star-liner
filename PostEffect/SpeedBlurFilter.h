//=====================================
//
//スピードブラーフィルターヘッダ[SpeedBlurFilter.h]
//Author:GP12B332 21 立花雄太
//
//=====================================
#ifndef _SPEEDBLURFILTER_H_
#define _SPEEDBLURFILTER_H_

#include "../main.h"
#include "ScreenObject.h"

/**************************************
マクロ定義
***************************************/

/**************************************
クラス定義
***************************************/
class SpeedBlur : public ScreenObject
{
public:
	SpeedBlur();
	~SpeedBlur();
	void DrawEffect();
	void SetPower(float power);
	void SetCenterPos(D3DXVECTOR3 pos);
	void SetSurfaceSize(float width, float height);
	void SetStartLength(float length);
	void SetAspectRatio(float width, float height);

private:
	LPD3DXEFFECT effect;
	D3DXHANDLE hPower, hCenter;
	D3DXHANDLE hTU, hTV;
	D3DXHANDLE hStartLength;
	D3DXHANDLE hAspectRatio;
};

#endif