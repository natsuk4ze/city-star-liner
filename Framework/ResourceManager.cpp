//=====================================
//
//リソースマネージャ処理[ResourceManager.cpp]
//Author:GP12B332 21 立花雄太
//
//=====================================
#include "ResourceManager.h"
#include "MeshContainer.h"
#include "BoardPolygon.h"

using namespace std;

/**************************************
マクロ定義
***************************************/

/**************************************
メッシュ読み込み処理
***************************************/
void ResourceManager::LoadMesh(const char* tag, const char* path)
{
	string tagStr = string(tag);

	//タグの重複確認
	if (meshPool.count(tagStr) != 0)
		SAFE_DELETE(meshPool[tagStr]);

	//メッシュコンテナを生成してロード
	meshPool[tagStr] = new MeshContainer();
	meshPool[tagStr]->Load(path);
}

/**************************************
メッシュ解放処理
***************************************/
void ResourceManager::ReleaseMesh(const char* tag)
{
	string tagStr = string(tag);

	//タグの登録確認
	if (meshPool.count(tagStr) == 0)
		return;

	//メッシュコンテナを削除
	SAFE_DELETE(meshPool[tagStr]);
	meshPool.erase(tagStr);
}

/**************************************
メッシュ取得処理
***************************************/
bool ResourceManager::GetMesh(const char* tag, MeshContainer*& pOut)
{
	string tagStr = string(tag);

	//登録確認
	if (meshPool.count(tagStr) == 0)
		return false;

	//メッシュへの参照を格納
	pOut = meshPool[tagStr];
	return true;
}

/**************************************
テクスチャ読み込み処理
***************************************/
void ResourceManager::LoadTexture(const char* path)
{
	string tagStr = string(path);

	//重複確認
	if (texturePool.count(tagStr) != 0)
		SAFE_RELEASE(texturePool[tagStr]);

	//読み込み
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	D3DXCreateTextureFromFile(pDevice, path, &texturePool[tagStr]);
}

/**************************************
テクスチャ解放処理
***************************************/
void ResourceManager::ReleaseTexture(const char* tag)
{
	string tagStr = string(tag);

	//登録確認
	if (texturePool.count(tagStr) == 0)
		return;

	SAFE_RELEASE(texturePool[tagStr]);
}

/**************************************
テクスチャ参照処理
***************************************/
bool ResourceManager::GetTexture(const char* path, LPDIRECT3DTEXTURE9& pOut)
{
	string tagStr = string(path);

	//登録確認
	if (texturePool.count(tagStr) == 0)
	{
		LoadTexture(path);
	}

	pOut = texturePool[tagStr];
	return true;
}

/**************************************
板ポリゴン作成処理
***************************************/
void ResourceManager::MakePolygon(const char* tag, const char* path, const D3DXVECTOR2& size, const D3DXVECTOR2& uv)
{
	string tagStr = string(tag);

	//登録確認
	if (polygonPool.count(tagStr) == 0)
		SAFE_DELETE(polygonPool[tagStr]);

	//BoardPolygonクラスを生成して登録
	polygonPool[tagStr] = new BoardPolygon();;
	polygonPool[tagStr]->SetSize(size);
	polygonPool[tagStr]->SetTexDiv(uv);
	polygonPool[tagStr]->LoadTexture(path);
}

/**************************************
板ポリゴン解放処理
***************************************/
void ResourceManager::ReleasePolygon(const char* tag)
{
	string tagStr = string(tag);

	//登録確認
	if (polygonPool.count(tagStr) == 0)
		return;

	SAFE_DELETE(polygonPool[tagStr]);
}

/**************************************
板ポリゴン参照処理
***************************************/
bool ResourceManager::GetPolygon(const char* tag, BoardPolygon*& pOut)
{
	string tagStr = string(tag);

	//登録確認
	if (polygonPool.count(tagStr) == 0)
		return false;

	pOut = polygonPool[tagStr];
	return true;
}

/**************************************
全リソース解放処理
***************************************/
void ResourceManager::AllRelease()
{
	for (auto& pair : meshPool)
	{
		SAFE_DELETE(pair.second);
	}
	meshPool.clear();

	for (auto& pair : texturePool)
	{
		SAFE_RELEASE(pair.second);
	}
	texturePool.clear();

	for (auto& pair : polygonPool)
	{
		SAFE_DELETE(pair.second);
	}
	polygonPool.clear();
}