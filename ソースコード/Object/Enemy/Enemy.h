#pragma once
#include <DxLib.h>
#include <deque>

#include "EnemyBase.h"

#include "../../Util/DifficultyData.h"

class Enemy : public EnemyBase
{
public:
	Enemy(DifficultyData data,VECTOR pos);
	virtual ~Enemy();

	void Input()override;
	void InputTutorial()override;

private:

	// ��O�ɗ����Ȃ��p�ɂ���
	void FieldLimit();

	// �^�[�Q�b�g�ɋ߂Â����ꍇ���E�ړ�������
	void MoveLeftAndRight(MATRIX mtxRot);

	// �퓬�Ԑ������߂�
	void BattleType();
	void BattleTypeGurad();
	void BattleTypeT();

	// �L�����N�^�[�̌���
	void Direction()override;
private:
	// �v���C���[�̕���������ׂ̃A���O���ɒx��������
	std::deque<float> m_delayFrameAngle;

	// �h��
	bool m_isCheckGuard;
	bool m_isResultGuard;
	// �ʏ�U��
	bool m_isCheckAttack;
	bool m_isAttackResult;

	// ���U��
	bool m_isCheckStrongAttack;
	bool m_isStrongAttackResult;
	
	// ���E�ړ��p
	bool m_isMoveLeft;
	bool m_isMoveRight;

	// ���p�̃x�N�g��
	VECTOR m_awayVec;
	// ����̈ړ��p���Έʒu
	VECTOR m_awayRelativePos;

	int m_guardFrameCount = 0;

	// �G��p�̉�]�s��
	MATRIX m_enemyRotMtx;

	// �`���[�g���A���p
	bool m_targetGuard;
};

