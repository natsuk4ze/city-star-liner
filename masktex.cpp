//=============================================================================
//
// �}�X�N���� [masktex.cpp]
// Author : �n糗Ǒ�
//
//=============================================================================
#include "main.h"
#include "masktex.h"
#include "UIdrawer.h"
#include "Stencil.h"
#include "Game.h"

Clip::Stencil clip;
OBJECT	masktex;
OBJECT	testtitle;

//�g��k�����n�܂�t���O
bool sizechange;
//�t�F�[�h�C���A�A�E�g�̂ǂ��炩�𔻒肷��t���O
bool isFadeIn;
//�t�F�[�h���s��������
bool active;
//�V�[���؂�ւ��ׂ̈̃E�F�C�g�^�C��
int wait;

Scene nextscene;

//�e�N�X�`��������
HRESULT InitMask(float size_x, float size_y, float size_z)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	LoadTexture(pDevice, MASK_TEXTURE, &masktex);
	InitialTexture(&masktex);
	MakeVertexObject(&masktex);

	masktex.size = D3DXVECTOR3(size_x, size_y, size_z);
	masktex.position = D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT/2, 0.0f);
	masktex.rotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	SetColorObject(&masktex, SET_COLOR_NOT_COLORED);

	active = false;
	sizechange = false;
	isFadeIn = false;
	wait = 0;

	return S_OK;
}

//�e�N�X�`���I������
void UninitMask(void)
{
	ReleaseTexture(&masktex);

}

void UpdateMask(void) {

	if (active) {
		//isFadeIn��true�̏ꍇ�Ƀt�F�[�h�C��
		if (isFadeIn) {
			MaskFadeIn();
		}
		else {
			MaskFadeOut();
		}
	}

}

//�}�X�N�p�e�N�X�`���X�V����
void MaskFadeOut(void) {

	//�T�C�Y�������Ȃ�ɂ��ʂ������Ȃ�
	if (masktex.size.x <= 0) {
		masktex.size = D3DXVECTOR3(0,0,0);
		SceneChangeFlag(true, nextscene);

		//�T�C�Y0�ȉ��ŃV�[���؂�ւ�
		wait++;

		if (wait > 60) {

			ChangeScene(nextscene);
			isFadeIn = true;
			//active = false;
			wait = 0;

		}

	}

	if (sizechange) {
		masktex.size -= D3DXVECTOR3(10.0f, 10.0f, 0.0f);

	}

}

void MaskFadeIn(void) {

	//�T�C�Y���傫���Ȃ�ɂ�Q�[����ʕ\��
	if (masktex.size.x >= MASK_SIZE) {

		SceneChangeFlag(false,nextscene);
		active = false;
	}

	if (sizechange) {
		masktex.size += D3DXVECTOR3(10.0f, 10.0f, 0.0f);

	}
	

}

//�}�X�N�p�e�N�X�`���`��
void DrawMaskTexSet(void) {

	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, true);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	//�}�X�N����
	clip.setWriteMaskColor(Clip::Stencil::MaskColor_Trans);
	clip.regionBegin(Clip::Stencil::MaskColor_Fill);

	DrawObject(pDevice, masktex);
	SetVertexObject(&masktex);

	clip.regionEnd();

	clip.setRefMaskColor(Clip::Stencil::MaskColor_Fill);
	clip.drawBegin();

}

void DrawMaskTexEnd(void) {

	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	clip.drawEnd();

	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, false);

}

void SceneChangeFlag(bool fadeflag,Scene next) {

	if (fadeflag) {
		sizechange = true;
		nextscene = next;
		active = true;
		isFadeIn = false;
	}
	else {
		sizechange = false;
		active = false;
	}
	
}