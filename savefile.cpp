//=============================================================================
//
// セーブデータ処理 [savefile.cpp]
// Author : 渡邉良則
//
//=============================================================================

/***************************************************************************
* インクルードファイル****
*******************************************************************************/

#include "savefile.h"

/*******************************************************************************
* グローバル変数
*******************************************************************************/
SAVEDATA	save;		// セーブデータ作成場所

void SaveData(void)
{
	SCORERANK *rank = GetRank();	// スコア０番のアドレスを取得する

	// スコア分セーブする
	for (int i = 0; i < RANK_MAX; i++, rank++)
	{
		save.scorerank[i] = *rank;
	}

	// SAVEDATA構造体ごと全部をファイルに出力する
	FILE* fp;

	//printf("\nセーブ開始・・・");
	fp = fopen("data/SAVEDATA/savedata.bin", "wb");			// ファイルをバイナリ書き込みモードでOpenする

	if (fp != NULL)								// ファイルがあれば書き込み、無ければ無視
	{
		fwrite(&save, 1, sizeof(SAVEDATA), fp);	// 指定したアドレスから指定したバイト数分ファイルへ書き込む
		fclose(fp);								// Openしていたファイルを閉じる
		//printf("終了！\n");
	}
	//else
	//{
	//	printf("ファイルエラー！\n");
	//}

	rewind(stdin);
	getchar();

}


/*******************************************************************************
関数名:	void LoadData( void )
引数:	void
戻り値:	void
説明:	セーブデータをファイルから読み込む
*******************************************************************************/
void LoadData(void)
{
	SCORERANK *rank = GetRank();	// スコア０番のアドレスを取得する

	// ファイルからセーブデータを読み込む
	FILE* fp;

	//printf("\nロード開始・・・");
	fp = fopen("data/SAVEDATA/savedata.bin", "rb");			// ファイルをバイナリ読み込みモードでOpenする

	if (fp != NULL)								// ファイルがあれば書き込み、無ければ無視
	{
		fread(&save, 1, sizeof(SAVEDATA), fp);	// 指定したアドレスへ指定したバイト数分ファイルから読み込む
		fclose(fp);								// Openしていたファイルを閉じる
		//printf("終了！\n");
	}
	//else
	//{
	//	printf("ファイルエラー！\n");
	//}

	// スコア分、rankワークへ戻す
	for (int i = 0; i < RANK_MAX; i++, rank++)
	{
		*rank = save.scorerank[i];				// ファイルから読み込んだデータを現在のワークへ戻す
	}

	rewind(stdin);
	getchar();

}



