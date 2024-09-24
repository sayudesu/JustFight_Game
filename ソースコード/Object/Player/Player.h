#pragma once
#include <DxLib.h>

#include "../CharacterBase.h"

#include "../../Util/DifficultyData.h"

#include "../../Util/Tips.h"

class Player : public CharacterBase
{
public:
	Player(DifficultyData data,VECTOR pos);
	virtual ~Player();
	
	// �{�^������
	void Input()override;
	void InputTutorial()override;

private:
	// �L�����N�^�[�̌���
	void Direction()override;
	void InputMove();
	void InputAttack();
	void InputGuard();
	
	// �L�����N�^�[�̈ړ�����
	void MoveCharacter(VECTOR moveVector);
	// �ړ��p�֐�
	VECTOR AddMoving(const VECTOR RelativePos, const MATRIX rotMtx, const VECTOR pos);
	VECTOR SubMoving(const VECTOR RelativePos, const MATRIX rotMtx, const VECTOR pos);
	// ���p�֐�
	void MoveAway(float x, float z);
private:
	// �����o�֐��|�C���^
	void(Player::* m_pMyFunc)();
	// ���p�̃x�N�g��
	VECTOR m_awayVec;
	// ����̈ړ��p���Έʒu
	VECTOR m_awayRelativePos;

	bool m_isUp;
	bool m_isDown;
	bool m_isLeft;
	bool m_isRight;

	// �J�������쑀��̕ύX
	bool m_isCameraLockon;

	// �R���g���[���[�p�ϐ�
	DINPUT_JOYSTATE m_padInput;

	// �v���C���[�̉�]�s��
	MATRIX m_platerRotMtx;

	// �����Ă��邩�ǂ���
	bool m_isMove;

	// ��������Ă��玟�̉�����\�ɂȂ�܂ł̃t���[�����J�E���g����
	int m_awayFrameCount;
};

