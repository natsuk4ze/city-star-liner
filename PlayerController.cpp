//=============================================================================
//
// �v���C���[�R���g���[���[���� [PlayerController.cpp]
// Author : �n糗Ǒ�
//
//=============================================================================
#include "main.h"
#include "player.h"
#include "star.h"
#include "PlayerController.h"
#include "InputController.h"
#include "debugWindow.h"

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void push(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************

//��莞�ԓ��͂��Ȃ��ꍇ�ɏ����|�W�V�����ɖ߂�ׂ̃^�C�}�[
int resetcount;

//����p�̔z��
int judgment[JUDG_LEN] = { TOP,LOWER_LEFT,MIDDLE_RIGHT,MIDDLE_LEFT,LOWER_RIGHT };

//����p�z��̌��݈ʒu
int judg_currentCCW;
int judg_currentCW;

//���݂̈ʒu
int currentCCW;
int currentCW;

//�ړ���̔ԍ��ۊǔz��
int move_stackCCW[MAX_LENGTH];
int move_stackCW[MAX_LENGTH];

// ���ԂɈړ����邩��ۊ�
int	movenum;

//����z��̓��e�ƍ����Ă��邩�̔���
bool matchingCCW;
bool matchingCW;

//�{�������e�X�g�p�t���O
bool flag;
int flagtimer;

HRESULT InitPlayerController(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	for (int i = 0; i < MAX_LENGTH; i++) {
		move_stackCCW[i] = 0;
		move_stackCW[i] = 0;
	}
	currentCCW = 0;
	currentCW = 0;

	judg_currentCCW = 0;
	judg_currentCW = 0;
	matchingCCW = false;
	matchingCW = false;

	movenum = CENTER;

	resetcount = 0;

	flag = false;
	flagtimer = 0;

	return S_OK;
}

//update�����̒ǉ�
void UpdatePlayerController(HWND hWnd)
{

	resetcount++;
	
	//�{�������p�̃t���O�A�J�E���^
	if (flag == true) {
		flagtimer++;
		if (flagtimer > RESETTIME) {
			flag = false;
			flagtimer = 0;
		}
	}

	for (int i = 0; i < STAR_MAX; i++) {
		if (IsEntered(i, hWnd)) {
			movenum = i;
			push();

			CheckCW();
			CheckCCW();
			resetcount = 0;
		}
	}
	//��莞�Ԍ�ɒ����֖߂鏈��
	if (resetcount >= RESETTIME) {
		movenum = 5;
		push();
		CheckCW();
		CheckCCW();
		resetcount = 0;
	}
	DebugText("move_stackCCW:%d,%d,%d,%d,%d,%d\n", move_stackCCW[0], move_stackCCW[1], move_stackCCW[2], move_stackCCW[3], move_stackCCW[4], move_stackCCW[5]);
	DebugText("move_stackCW:%d,%d,%d,%d,%d,%d\n", move_stackCW[0], move_stackCW[1], move_stackCW[2], move_stackCW[3], move_stackCW[4], move_stackCW[5]);

}

void SetPlayerTargetPosition(int *n) {

	*n = movenum;

}

void push() {

	//�ړ��ۊǔz��ɓ��͂��ꂽ�L�[�̔ԍ���ۊ�
	if (currentCCW < MAX_LENGTH) {
		move_stackCCW[currentCCW] = movenum;
		move_stackCW[currentCW] = movenum;

	}

}

//�E���p
void CheckCW() {

	//����ɔ���p�z��̉��Ԗڂɓ����������i�[����Ă��邩�m�F

	if (matchingCW == false) {
		for (judg_currentCW = 0; judg_currentCW < JUDG_LEN; judg_currentCW++) {

			//��v�����ꍇ
			if (move_stackCW[currentCW] == judgment[judg_currentCW]) {

				matchingCW = true;

			}
			if (matchingCW == true) {
				//�ړ��ۊǔz��Ɣ���p�z�񂪈�v�����ꍇ������ijudgment_current�̒l���m�ہj
				break;
			}

		}
	}
	//����p�z��̍ő�l�܂Ői�񂾏ꍇ�A�ŏ��ɖ߂�
	if (judg_currentCW < 0) {
		judg_currentCW = JUDG_LEN - 1;
	}
	//�ړ��ۊǔz��Ɣ���p�z��ƈႤ�ꍇ�i�E���j
	if (move_stackCW[currentCW] != judgment[judg_currentCW]) {

		matchingCW = false;
		currentCW = 0;
		//�ړ��ۊǔz���������
		for (int i = 0; i < MAX_LENGTH; i++) {
			move_stackCW[i] = 0;
		}
	}
	else {
		//�������ꍇ
		matchingCW = true;
		judg_currentCW--;
		currentCW++;

		//�ړ��ۊǔz��̍ő�l�܂ŒB�����ꍇ�{����������
		if (matchingCW == true && currentCW == MAX_LENGTH) {
			currentCW = 0;
			judg_currentCW = 0;
			matchingCW = false;
			flag = true;

			//�z��̏�����
			for (int i = 0; i < MAX_LENGTH; i++) {
				move_stackCW[i] = 0;
			}

			move_stackCW[currentCW] = movenum;
			currentCW++;
		}

	}
}

//�����p
void CheckCCW() {

//����ɔ���p�z��̉��Ԗڂɓ����������i�[����Ă��邩�m�F

	if (matchingCCW == false) {
		for (judg_currentCCW = 0; judg_currentCCW < JUDG_LEN; judg_currentCCW++) {

			//��v�����ꍇ
			if (move_stackCCW[currentCCW] == judgment[judg_currentCCW]) {
				matchingCCW = true;
			}
			if (matchingCCW == true) {
				//�ړ��ۊǔz��Ɣ���p�z�񂪈�v�����ꍇ������ijudgment_current�̒l���m�ہj
				break;
			}

		}
	}

	//����p�z��̍ő�l�܂Ői�񂾏ꍇ�A�ŏ��ɖ߂�
	if (judg_currentCCW >= JUDG_LEN) {
		judg_currentCCW = 0;
	}

	//�ړ��ۊǔz��Ɣ���p�z��ƈႤ�ꍇ�i�����j
	if (move_stackCCW[currentCCW] != judgment[judg_currentCCW]) {

		matchingCCW = false;
		currentCCW = 0;
		//�ړ��ۊǔz���������
		for (int i = 0; i < MAX_LENGTH; i++) {
			move_stackCCW[i] = 0;
		}

	}
	else {
		//�������ꍇ
		matchingCCW = true;
		judg_currentCCW++;
		currentCCW++;

		//�ړ��ۊǔz��̍ő�l�܂ŒB�����ꍇ�{����������
		if (matchingCCW == true && currentCCW == MAX_LENGTH) {
			currentCCW = 0;
			judg_currentCCW = 0;
			matchingCCW = false;
			flag = true;

			//�z��̏�����
			for (int i = 0; i < MAX_LENGTH; i++) {
				move_stackCCW[i] = 0;
			}

			move_stackCCW[currentCCW] = movenum;
			currentCCW++;
		}
	}

}

bool SetBomb() {
	if (flag == true) {
		return true;
	}
	return false;
}
