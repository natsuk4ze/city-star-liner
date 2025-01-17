//=============================================================================
//
// タイトルロゴ画面処理 [titleLogoUI.h]
// Author : Yu Oohama (bnban987@gmail.com)
//
//=============================================================================
#ifndef _TITLELOGO_H_
#define _TITLELOGO_H_

/**************************************
前方宣言
***************************************/
class Object;

//*****************************************************************************
// 構造体定義
//*****************************************************************************
class TitleLogo
{
public:
	TitleLogo();
	~TitleLogo();

	void Update(void);
	void Draw(void);

private:
	Object * titleLogo;
};

#endif

