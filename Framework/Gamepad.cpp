//=====================================
//
//ゲームパッド処理[Gamepad.cpp]
//Author:GP12A332 21 立花雄太
//
//=====================================
#include "Gamepad.h"

/**************************************
マクロ定義
***************************************/
#define DEADZONE		1500			// 各軸の25%を無効ゾーンとする
#define RANGE_MAX		1000			// 有効範囲の最大値
#define RANGE_MIN		-1000			// 有効範囲の最小値

#define INPUT_SHORTWAIT (6)

/**************************************
クラス定義
***************************************/

/**************************************
グローバル変数
***************************************/
//スティックのRepeat状態検出用
static int		padAxisXRepeat[GAMEPADMAX];
static int		padAxisYRepeat[GAMEPADMAX];
static int		lastAxisX[GAMEPADMAX];
static int		lastAxisY[GAMEPADMAX];
static int		axisXRepeatCnt[GAMEPADMAX];
static int		axisYRepeatCnt[GAMEPADMAX];

static float	padAxislRx[GAMEPADMAX];
static float	padAxislRy[GAMEPADMAX];
static float	padAxisLx[GAMEPADMAX];
static float	padAxisLy[GAMEPADMAX];

static LPDIRECTINPUTDEVICE8	pGamePad[GAMEPADMAX] = { NULL,NULL,NULL,NULL };// パッドデバイス

static DWORD	padState[GAMEPADMAX];	// パッド情報（複数対応）
static DWORD	padTrigger[GAMEPADMAX];
static DWORD	padRelease[GAMEPADMAX];
static int		padCount = 0;			// 検出したパッドの数
static LPDIRECTINPUT8 pInput = NULL;

bool isAnyKeyTriggerd = false;

/**************************************
パッド検査コールバック
***************************************/
BOOL CALLBACK SearchPadCallback(LPDIDEVICEINSTANCE lpddi, LPVOID)
{
	HRESULT result;

	result = pInput->CreateDevice(lpddi->guidInstance, &pGamePad[padCount++], NULL);
	return DIENUM_CONTINUE;	// 次のデバイスを列挙

}

/**************************************
初期化処理
***************************************/
HRESULT InitializePad(LPDIRECTINPUT8 inputInterface)			// パッド初期化
{
	HRESULT		result;
	int			i;
	pInput = inputInterface;

	padCount = 0;
	// ジョイパッドを探す
	inputInterface->EnumDevices(DI8DEVCLASS_GAMECTRL, (LPDIENUMDEVICESCALLBACK)SearchPadCallback, NULL, DIEDFL_ATTACHEDONLY);
	// セットしたコールバック関数が、パッドを発見した数だけ呼ばれる。

	for (i = 0; i < padCount; i++) {
		// ジョイスティック用のデータ・フォーマットを設定
		result = pGamePad[i]->SetDataFormat(&c_dfDIJoystick);
		if (FAILED(result))
			return false; // データフォーマットの設定に失敗

	  // モードを設定（フォアグラウンド＆非排他モード）
	  //		result = pGamePad[i]->SetCooperativeLevel(hWindow, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	  //		if ( FAILED(result) )
	  //			return false; // モードの設定に失敗

	  // 軸の値の範囲を設定
	  // X軸、Y軸のそれぞれについて、オブジェクトが報告可能な値の範囲をセットする。
	  // (max-min)は、最大10,000(?)。(max-min)/2が中央値になる。
	  // 差を大きくすれば、アナログ値の細かな動きを捕らえられる。(パッドの性能による)
		DIPROPRANGE				diprg;
		ZeroMemory(&diprg, sizeof(diprg));
		diprg.diph.dwSize = sizeof(diprg);
		diprg.diph.dwHeaderSize = sizeof(diprg.diph);
		diprg.diph.dwHow = DIPH_BYOFFSET;
		diprg.lMin = RANGE_MIN;
		diprg.lMax = RANGE_MAX;
		// X軸の範囲を設定
		diprg.diph.dwObj = DIJOFS_X;
		pGamePad[i]->SetProperty(DIPROP_RANGE, &diprg.diph);
		// Y軸の範囲を設定
		diprg.diph.dwObj = DIJOFS_Y;
		pGamePad[i]->SetProperty(DIPROP_RANGE, &diprg.diph);

		// 各軸ごとに、無効のゾーン値を設定する。
		// 無効ゾーンとは、中央からの微少なジョイスティックの動きを無視する範囲のこと。
		// 指定する値は、10000に対する相対値(2000なら20パーセント)。
		DIPROPDWORD				dipdw;
		dipdw.diph.dwSize = sizeof(DIPROPDWORD);
		dipdw.diph.dwHeaderSize = sizeof(dipdw.diph);
		dipdw.diph.dwHow = DIPH_BYOFFSET;
		dipdw.dwData = DEADZONE;
		//X軸の無効ゾーンを設定
		dipdw.diph.dwObj = DIJOFS_X;
		pGamePad[i]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
		//Y軸の無効ゾーンを設定
		dipdw.diph.dwObj = DIJOFS_Y;
		pGamePad[i]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);

		//ジョイスティック入力制御開始
		pGamePad[i]->Acquire();
	}

	return true;

}

/**************************************
終了処理
***************************************/
void UninitPad(void)
{
	for (int i = 0; i < GAMEPADMAX; i++) {
		if (pGamePad[i])
		{
			pGamePad[i]->Unacquire();
			pGamePad[i]->Release();
		}
	}

}

#include "../debugWindow.h"
/**************************************
更新処理
***************************************/
void UpdatePad(void)
{
	HRESULT			result;
	DIJOYSTATE2		dijs;
	int				i;

	for (i = 0; i < padCount; i++)
	{
		DWORD lastPadState;
		lastPadState = padState[i];
		padState[i] = 0x00000000l;	// 初期化

		result = pGamePad[i]->Poll();	// ジョイスティックにポールをかける
		if (FAILED(result)) {
			result = pGamePad[i]->Acquire();
			while (result == DIERR_INPUTLOST)
				result = pGamePad[i]->Acquire();
		}

		result = pGamePad[i]->GetDeviceState(sizeof(DIJOYSTATE), &dijs);	// デバイス状態を読み取る
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED) {
			result = pGamePad[i]->Acquire();
			while (result == DIERR_INPUTLOST)
				result = pGamePad[i]->Acquire();
		}

		//右スティック用の処理
		if (dijs.lZ != 0 && dijs.lRx != 0)
		{
			padAxislRx[i] = (float)(dijs.lRx);
			padAxislRy[i] = (float)(dijs.lRy);

		}
		else
		{
			padAxislRx[i] = (float)(dijs.lZ + dijs.lRx);
			padAxislRy[i] = (float)(dijs.lRz + dijs.lRy);

		}

		padAxisLx[i] = dijs.lX / 1000.0f;
		padAxisLy[i] = dijs.lY / -1000.0f;

		BeginDebugWindow("Input");
		DebugText("X:%d, Y:%d", dijs.lX, dijs.lY);
		DebugText("X:%f, Y:%f", padAxisLx[i], padAxisLy[i]);
		EndDebugWindow("Input");

		// ３２の各ビットに意味を持たせ、ボタン押下に応じてビットをオンにする
		//* y-axis (forward)
		if (dijs.lY < 0)					padState[i] |= BUTTON_UP;
		//* y-axis (backward)
		if (dijs.lY > 0)					padState[i] |= BUTTON_DOWN;
		//* x-axis (left)
		if (dijs.lX < 0)					padState[i] |= BUTTON_LEFT;
		//* x-axis (right)
		if (dijs.lX > 0)					padState[i] |= BUTTON_RIGHT;
		//* Ａボタン
		if (dijs.rgbButtons[0] & 0x80)	padState[i] |= BUTTON_A;
		//* Ｂボタン
		if (dijs.rgbButtons[1] & 0x80)	padState[i] |= BUTTON_B;
		//* Ｃボタン
		if (dijs.rgbButtons[2] & 0x80)	padState[i] |= BUTTON_C;
		//* Ｘボタン
		if (dijs.rgbButtons[3] & 0x80)	padState[i] |= BUTTON_X;
		//* Ｙボタン
		if (dijs.rgbButtons[4] & 0x80)	padState[i] |= BUTTON_Y;
		//* Ｚボタン
		if (dijs.rgbButtons[5] & 0x80)	padState[i] |= BUTTON_Z;
		//* Ｌボタン
		if (dijs.rgbButtons[6] & 0x80)	padState[i] |= BUTTON_L;
		//* Ｒボタン
		if (dijs.rgbButtons[7] & 0x80)	padState[i] |= BUTTON_R;
		//* ＳＴＡＲＴボタン
		if (dijs.rgbButtons[8] & 0x80)	padState[i] |= BUTTON_START;
		//* Ｍボタン
		if (dijs.rgbButtons[9] & 0x80)	padState[i] |= BUTTON_M;

		// Trigger設定
		padTrigger[i] = ((lastPadState ^ padState[i])	// 前回と違っていて
			& padState[i]);					// しかも今ONのやつ

											// Release設定
		padRelease[i] = ((lastPadState ^ padState[i]))	//前回と違っていて
			& (~padState[i]);				//今OFFのやつ

											//スティックのRepeat判定処理
		padAxisYRepeat[i] = GetPadAxisYTriggered(i);

		if (BUTTON_UP & padState[i])
		{
			axisYRepeatCnt[i]++;
			if (axisYRepeatCnt[i] >= 20 && axisYRepeatCnt[i] % INPUT_SHORTWAIT == 0)
			{
				padAxisYRepeat[i] = 1;
			}
		}
		else if (BUTTON_DOWN & padState[i])
		{
			axisYRepeatCnt[i]++;
			if (axisYRepeatCnt[i] >= 20 && axisYRepeatCnt[i] % INPUT_SHORTWAIT == 0)
			{
				padAxisYRepeat[i] = -1;
			}
		}
		else
		{
			axisYRepeatCnt[i] = 0;
			padAxisYRepeat[i] = 0;
		}

		padAxisXRepeat[i] = GetPadAxisXTriggered(i);

		if (BUTTON_RIGHT & padState[i])
		{
			axisXRepeatCnt[i]++;
			if (axisXRepeatCnt[i] >= 20 && axisXRepeatCnt[i] % INPUT_SHORTWAIT == 0)
			{
				padAxisXRepeat[i] = 1;
			}
		}
		else if (BUTTON_LEFT & padState[i])
		{
			axisXRepeatCnt[i]++;
			if (axisXRepeatCnt[i] >= 20 && axisXRepeatCnt[i] % INPUT_SHORTWAIT == 0)
			{
				padAxisXRepeat[i] = -1;
			}
		}
		else
		{
			axisXRepeatCnt[i] = 0;
			padAxisXRepeat[i] = 0;
		}

	}

}

/**************************************
プレス検出処理
***************************************/
BOOL IsButtonPressed(int padNo, DWORD button)
{
	if (padNo > padCount)
		return false;

	return (button & padState[padNo]);
}

/**************************************
トリガー検出処理
***************************************/
BOOL IsButtonTriggered(int padNo, DWORD button)
{
	if (padNo > padCount)
		return false;

	return (button & padTrigger[padNo]);
}

/**************************************
リリース検出処理
***************************************/
BOOL IsButtonReleased(int padNo, DWORD button)
{
	if (padNo > padCount)
		return false;

	return (button & padRelease[padNo]);
}

/**************************************
X軸トリガー検出処理
***************************************/
int GetPadAxisXTriggered(int padNo)
{
	if (IsButtonTriggered(padNo, BUTTON_RIGHT))
		return 1;

	else if (IsButtonTriggered(padNo, BUTTON_LEFT))
		return -1;

	else
		return 0;
}

/**************************************
Y軸トリガー検出処理
***************************************/
int GetPadAxisYTriggered(int padNo)
{
	if (IsButtonTriggered(padNo, BUTTON_UP))
		return 1;

	else if (IsButtonTriggered(padNo, BUTTON_DOWN))
		return -1;

	else
		return 0;
}

/**************************************
パッド数を返す処理
***************************************/
int GetPadCount(void)
{
	return padCount;
}

/**************************************
右スティックX軸入力計算処理
***************************************/
float GetStickAxisX(int padNo)
{
	if (padNo >= padCount)
	{
		return 0.0f;
	}

	return (padAxislRx[padNo] / 65535.0f) - 0.5f;
}


/**************************************
右スティックY軸入力計算処理
***************************************/
float GetStickAxisY(int padNo)
{
	if (padNo >= padCount)
	{
		return 0.0f;
	}


	return (padAxislRy[padNo] / 65535.0f) - 0.5f;
}

/**************************************
左スティックX軸取得処理
***************************************/
float GetAxisX(int padNo)
{
	return padAxisLx[padNo];
}

/**************************************
左スティックY軸入力計算処理
***************************************/
float GetAxisY(int padNo)
{
	return padAxisLy[padNo];
}