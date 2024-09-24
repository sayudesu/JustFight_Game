#include "Enemy.h"

#include "../../Util/Pad.h"

#include "../../CSVData/CharactorDataManager.h"
#include "../../Util/ParameterData.h"

#include "../../CSVData/ModelManager.h"
#include "../../Util/ModelName.h"

namespace
{
	// �p�x�����t���[���P�ʂŒx�点��
	constexpr int kDelayFrameAngle = 18;
	constexpr int kGuardFrameCountMax = 60 * 2;
}

namespace
{
	// �v���C���[�̈ړ���
	constexpr VECTOR kVecZ{ 0.0f,0.0f,-10.0f };
	constexpr VECTOR kVecX{ -10.0f,0.0f,0.0f };

	// ���
	constexpr VECTOR kVecAwayZ{ 0.0f,0.0f,-25.0f };
}


Enemy::Enemy(DifficultyData data,VECTOR pos) :
	EnemyBase(data,pos),
	m_isCheckGuard(false),
	m_isResultGuard(false),
	m_isCheckAttack(false),
	m_isAttackResult(false),
	m_isCheckStrongAttack(false),
	m_isStrongAttackResult(false),
	m_isMoveLeft(false),
	m_isMoveRight(false),
	m_awayVec(VGet(0,0,0)),
	m_awayRelativePos(VGet(0,0,0)),
	m_guardFrameCount(0),
	m_targetGuard(false),
	m_enemyRotMtx({})
{
	// �����ҋ@��ԂŒ�~
	m_pFunc = &Enemy::Idle;

	m_isAttack = false;
	m_isGuard = false;

	// ���g���G�l�~�[�ł���ƌ��߂�
	m_myId = CharacterName::ENEMY;

	// �I��������Փx�ɂ���ă��f����ύX
	if (data == DifficultyData::NOIVE)
	{		
		m_parameter.fileName = ModelManager::GetInstance().ModelType(ModelName::Pawn_B);
		InputParamType(0);
	}
	else if (data == DifficultyData::INTERMEDIATE)
	{
		m_parameter.fileName = ModelManager::GetInstance().ModelType(ModelName::Knight_B);
		InputParamType(1);
	}
	else if (data == DifficultyData::EXPERT)
	{
		m_parameter.fileName = ModelManager::GetInstance().ModelType(ModelName::Queen_B);
		InputParamType(2);
	}
}

Enemy::~Enemy()
{
}

void Enemy::Input()
{
	DINPUT_JOYSTATE input;
	// ���͏�Ԃ��擾
	GetJoypadDirectInputState(DX_INPUT_PAD1, &input);

	// �������w��
	Direction();

	// �X�^�������Ă��Ȃ�������
	if (!IsStun())
	{
		// ��苗���߂Â��ƃ����_���ō��E�ړ����n�߂�
		if (m_targetRange.x + m_targetRange.z < 1000.0f)
		{
			MoveLeftAndRight(m_enemyRotMtx);

			// �s���͈͂𐧌�����
			FieldLimit();
		}

		// �U���\�͈͂ɓ����Ă��邩�ǂ���
		if (IsAttackRange())
		{
			// �U��or�h�䂵�Ă��Ȃ������玟�̍s�������߂�
			if (!m_isAttack && !m_isGuard && !m_isStrongAttack)
			{
				// �^�[�Q�b�g�̏�Ԃœ����ύX����
				BattleType();
			}

			// �U�����ǂ���
			if (m_isAttackResult)
			{
				m_isAttackResult = false;
				m_isAttack = true;
				m_pFunc = &Enemy::Attack;
			}
			// �U�������Ă��Ȃ��ꍇ
			if (!m_isAttack)
			{
				m_isAttack = false;
			}

			// ���U�����ǂ���
			if (m_isStrongAttackResult && !m_isGuard)
			{
				m_isStrongAttackResult = false;
				m_isCheckStrongAttack = true;
				m_pFunc = &Enemy::StrongAttack;
			}
			// ���U�������Ă��Ȃ��ꍇ
			if (!m_isCheckStrongAttack)
			{
				m_isCheckStrongAttack = false;
			}

			// �K�[�h�����čU�������Ă��Ȃ��ꍇ
			if (m_isResultGuard && !m_isAttack)
			{
				m_isGuard = true;
				m_guardFrameCount++;
				m_pFunc = &Enemy::Guard;		
			}

			// �w��̃t���[�����̓K�[�h����������
			if (m_guardFrameCount == kGuardFrameCountMax)
			{
				m_guardFrameCount = 0;
				m_isCheckGuard = false;
				m_isGuard = false;
				m_isResultGuard = false;
			}

			// �W���X�g�K�[�h�ɐ���������ʏ�K�[�h���s��Ȃ�
			if (IsJustGuard())
			{
				m_guardFrameCount = 0;
				m_isCheckGuard = false;
				m_isGuard = false;
				m_isResultGuard = false;
			}
		}
		else
		{
			m_isAttack = false;
			m_isCheckAttack = false;
			m_isAttackResult = false;

			TargetMove();

			// �K�[�h�����邩�ǂ����̑I�����s��
			BattleTypeGurad();

			// �K�[�h�����čU�������Ă��Ȃ��ꍇ
			if (m_isResultGuard && !m_isAttack)
			{
				m_isGuard = true;
				m_guardFrameCount++;
				m_pFunc = &Enemy::Guard;
			}
			else
			{
				m_guardFrameCount = 0;
			}

			// �w��̃t���[�����̓K�[�h����������
			if (m_guardFrameCount == kGuardFrameCountMax)
			{
				m_guardFrameCount = 0;
				m_isCheckGuard = false;
				m_isGuard = false;
				m_isResultGuard = false;
			}

			// �W���X�g�K�[�h�ɐ���������ʏ�K�[�h���s��Ȃ�
			if (IsJustGuard())
			{
				m_guardFrameCount = 0;
				m_isCheckGuard = false;
				m_isGuard = false;
				m_isResultGuard = false;
			}
		}
	}
	else
	{
		m_isGuard = false;
		m_isCheckGuard = false;
		m_isResultGuard = false;

		m_isAttack = false;
		m_isCheckAttack = false;
		m_isAttackResult = false;
	}
}

void Enemy::InputTutorial()
{
	// �������w��
	Direction();

	if (!IsStun())
	{
		// �U���\�͈͂ɓ����Ă��邩�ǂ���
		if (IsAttackRange())
		{
			// �U��or�h�䂵�Ă��Ȃ������玟�̍s�������߂�
			if (!m_isAttack && !m_isGuard && !m_isStrongAttack)
			{
				// �^�[�Q�b�g�̏�Ԃœ����ύX����
				BattleTypeT();
			}

			// �U�����ǂ���
			if (m_isAttackResult)
			{
				m_isAttackResult = false;
				m_isAttack = true;
				m_pFunc = &Enemy::Attack;
			}
			// �U�������Ă��Ȃ��ꍇ
			if (!m_isAttack)
			{
				m_isAttack = false;
			}

			// ���U�����ǂ���
			if (m_isStrongAttackResult && !m_isGuard)
			{
				m_isStrongAttackResult = false;
				m_isCheckStrongAttack = true;
				m_pFunc = &Enemy::StrongAttack;
			}
			// ���U�������Ă��Ȃ��ꍇ
			if (!m_isCheckStrongAttack)
			{
				m_isCheckStrongAttack = false;
			}

			// �K�[�h�����čU�������Ă��Ȃ��ꍇ
			if (m_isResultGuard && !m_isAttack)
			{
				m_isGuard = true;
				m_guardFrameCount++;
				m_pFunc = &Enemy::Guard;
			}
			// �w��̃t���[�����̓K�[�h����������
			if (m_guardFrameCount == kGuardFrameCountMax)
			{
				m_guardFrameCount = 0;
				m_isCheckGuard = false;
				m_isGuard = false;
				m_isResultGuard = false;
			}
		}
		else
		{
			TargetMove();

			m_isGuard = false;
			m_isCheckGuard = false;
			m_isResultGuard = false;

			m_isAttack = false;
			m_isCheckAttack = false;
			m_isAttackResult = false;
		}
	}
	else
	{
		m_isGuard = false;
		m_isCheckGuard = false;
		m_isResultGuard = false;

		m_isAttack = false;
		m_isCheckAttack = false;
		m_isAttackResult = false;
	}
}

void Enemy::Direction()
{
	// ���g�̈ʒu����^�[�Q�b�g�̈ʒu�̊p�x���擾����
	const VECTOR direction = VSub(m_targetPos, m_pos);

	const float angle = atan2f(-direction.x, -direction.z);
	
	// �w��̃t���[�����������������炷
	m_delayFrameAngle.push_front(angle);
	if (m_delayFrameAngle.size() > kDelayFrameAngle)
	{
		m_delayFrameAngle.pop_back();
	}
	SetAngle(m_delayFrameAngle.back());

	// �v���C���[�̐i�s����
	m_enemyRotMtx = MGetRotY(m_delayFrameAngle.back());
	SetRotMtx(m_enemyRotMtx);
}

// �s���͈͂𐧌�����
void Enemy::FieldLimit()
{
	if (m_pos.x > 1000.0f + 100.0f)
	{
		m_pos.x = 1000.0f + 100.0f;
	}
	if (m_pos.x < -1000.0f - 100.0f)
	{
		m_pos.x = -1000.0f - 100.0f;
	}
	if (m_pos.z > 1000.0f + 100.0f)
	{
		m_pos.z = 1000.0f + 100.0f;
	}
	if (m_pos.z < -1000.0f - 100.0f)
	{
		m_pos.z = -1000.0f - 100.0f;
	}
}

void Enemy::MoveLeftAndRight(MATRIX mtxRot)
{
	if (GetRand(100) == 0)
	{
		m_isMoveRight = true;
		m_isMoveLeft = false;
	}
	if (GetRand(100) == 1)
	{
		m_isMoveLeft = true;
		m_isMoveRight = false;
	}
	if (GetRand(100) == 2)
	{
		m_isMoveLeft = false;
		m_isMoveRight = false;
	}
	int moveType = 0;
	if (m_isMoveLeft)
	{
		moveType = 1;
	}
	if (m_isMoveRight)
	{
		moveType = -1;
	}

	const VECTOR move = VTransform(VGet(static_cast<float>(moveType) * 10, 0, 0), mtxRot);
	m_pos = VAdd(m_pos, move);
}

void Enemy::BattleType()
{
	// �^�[�Q�b�g�̏�Ԃœ����ύX����
	// �^�[�Q�b�g���U�����Ă���ꍇ
	if (m_targetBattleState == BattleState::ATTACK)
	{
		if (GetRand(m_parameter.justGuardRate) == 0)
		{
			m_isCheckGuard = true;
			m_isResultGuard = true;	
		}
	}
	// �^�[�Q�b�g���X�^�����Ă���ꍇ
	if (m_targetBattleState == BattleState::STUN)
	{
		// ���U�����ł���ꍇ�͂���
		// �ł��Ȃ��ꍇ�͒ʏ�U��������
		if (GetStrongPower() == 100)
		{
			m_isCheckStrongAttack = true;
			m_isStrongAttackResult = true;
		}
		else
		{
			m_isCheckAttack = true;
			m_isAttackResult = true;
		}
	}

	// �^�[�Q�b�g���h�䂵�Ă���ꍇ
	if (m_targetBattleState == BattleState::GUARD)
	{
		if (GetRand(30) == 0)
		{
			m_isCheckAttack = true;
			m_isAttackResult = true;
		}
	}

	// �A�C�h����Ԃ̏ꍇ
	if (m_targetBattleState == BattleState::IDLE)
	{
		m_isCheckAttack = true;
		m_isAttackResult = true;
	}

}

void Enemy::BattleTypeGurad()
{
	if (GetRand(100) == 0)
	{
		m_isCheckGuard = true;
		m_isResultGuard = true;
	}
}

void Enemy::BattleTypeT()
{
	if (m_targetGuard)
	{
		// �^�[�Q�b�g���h�䂵�Ă���ꍇ
		if (m_targetBattleState == BattleState::GUARD)
		{
			m_isCheckAttack = true;
			m_isAttackResult = true;

			m_targetGuard = true;
		}
	}
}

