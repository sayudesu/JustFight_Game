#include <cmath>

#include "CharacterBase.h"

#include "../Util/EffekseerDrawer.h"

#include "../CSVData/ModelManager.h"
#include "../CSVData/SoundManager.h"
#include "../CSVData/CharactorDataManager.h"

#include "../Util/GameObject.h"
#include "../Util/SoundName.h"
#include "../Util/BlurScreen.h"
#include "../Util/ParameterData.h"
#include "../Util/ModelName.h"
#include "../Util/Easing.h"
#include "../Util/Pad.h"

namespace
{
	// �J�v�Z���`��̔��a
	float kCapsuleRadius = 64.0f;

	// �d��
	float kGravity = 32.0f;

	constexpr float kStrongAttackPowerMax = 100.0f;

	// �U���\���t���[�����̕���̐U��
	constexpr float kShakeRate = 15.0f;

	// ���f���̊p�x
	constexpr float kAngleZ = 90 * DX_PI_F / 180.0f;

	// �ǐՑ��x
	constexpr float kTargetMoveingSpeed = 5.0f;

	// �I�u�W�F�N�g�T�C�Y
	const VECTOR kModelSize = VGet(40.0f, 40.0f, 40.0f);
	const VECTOR kWeaponSize = VGet(3.0f, 3.0f, 3.0f);
	const VECTOR kShieldSize = VGet(3.0f, 3.0f, 3.0f);
}

CharacterBase::CharacterBase(DifficultyData data ,VECTOR pos):
	m_pFunc(nullptr),
	m_effectHandle(-1),
	m_strongAttackPower(0),
	m_rotMtx({}),
	m_targetRotMtx({}),
	m_pos(pos),
	m_targetPos    (VGet(0,0,0)),
	m_vecWeapon    (VGet(0, 0, 0)),
	m_vecShield    (VGet(0, 0, 0)),
	m_tempWeaponPos(VGet(0, 0, 0)),
	m_tempFightingMeter(0),
	m_angle         (0.0f),
	m_isAttack      (false),
	m_isStrongAttack(false),
	m_isGuard       (false),
	m_isJustGuard   (false),
	m_isJustGuardCounter(false),
	m_isAway	    (false),
	m_isResultGuard (false),
	m_isResultDamage(false),
	m_isWeaponAttacksShield(false),
	m_isStun        (false),
	m_isAttackRange (false),
	m_targetHp      (1),
	m_attackFrame   (0),
	m_attackGapFrame(0),
	m_guardFrame    (0),
	m_justGuardFrame(0),
	m_justGuardCounterFrame(0),
	m_stunFrame     (0),
	m_recoilFrame   (0),
	m_guardOneFrame (0),
	m_attackAfterStopFrame(0),
	m_isSceneChange(false),
	m_comboAttack   (0),
	m_difficultyData(data),
	m_isHit(false)
{
	// �����o�֐��̏���
	m_pFunc = &CharacterBase::Idle;

	// ���g�̔��ʗpID��������
	m_myId = CharacterName::NONE;

	// ���݂̍s�����L�^
	m_battleState       = BattleState::NONE;// ���g
	m_targetBattleState = BattleState::NONE;// �^�[�Q�b�g
	m_tempTargetBattleState = BattleState::NONE;// �^�[�Q�b�g
	
	// ���g�̃p���[���[�^�[
	m_parameter = {};
}

CharacterBase::~CharacterBase()
{
}

void CharacterBase::Init()
{
	// ���f���̑��Έʒu
	m_vecWeapon = m_parameter.weaponRelativePos;
	m_vecShield = m_parameter.shieldRelativePos;

	// �L�������Ƃ̃X�e�[�^�X�����擾
	m_hp = m_parameter.hpMax;
	m_vec = m_parameter.knockBackPos;
	m_fightingMeter = m_parameter.fightingMeterMax;

	const VECTOR move = VTransform(m_vecWeapon, m_rotMtx);
	m_weaponPos = VAdd(m_pos, move);

	// �I�u�W�F�N�g�̐���
	m_pCharactor = new GameObject(
		m_parameter.fileName.c_str(),
		m_pos,
		VGet(0.0f, m_angle, kAngleZ),
		kModelSize);

	// ����I�u�W�F�N�g�̐���
	m_pWeapon = new GameObject(
		ModelManager::GetInstance().ModelType(ModelName::WEAPON),
		m_weaponPos,
		VGet(0.0f, m_angle, kAngleZ),
		kWeaponSize);

	// ���I�u�W�F�N�g�̐���
	m_pShield = new GameObject(
		ModelManager::GetInstance().ModelType(ModelName::SHIELD),
		VGet(100, m_weaponPos.y, 100),
		VGet(0.0f, m_angle, kAngleZ),
		kShieldSize);

	// �ʒu���̍X�V
	UpdatePos();
}

void CharacterBase::End()
{
	// �������
	delete m_pCharactor;
	m_pCharactor = nullptr;
	delete m_pWeapon;
	m_pWeapon = nullptr;
	delete m_pShield;
	m_pShield = nullptr;
}

void CharacterBase::Update()
{
	// �����o�֐��|�C���^
	(this->*m_pFunc)();
}

void CharacterBase::Draw()
{
	// �L�����N�^�[���f��
	m_pCharactor->Draw();
	// ���탂�f��
	m_pWeapon->Draw();
	// �����f��
	m_pShield->Draw();
}

void CharacterBase::TargetMove()
{
	// Y���͕ύX�������Ȃ��̂Ŏn�߂̈ʒu���L�^
	const float tempPosY = m_pos.y;

	// �������Z�o
	VECTOR m_dir = VSub(m_targetPos, m_pos);

	// �v���C���[����G�l�~�[�܂ł̊p�x�����߂�
	const float angle = atan2(m_dir.y, m_dir.x);

	// ���ݓG�������Ă�������̃x�N�g���𐶐�����
	const MATRIX enemyRotMtx = MGetRotY(angle);

	// �΂߂ɂȂ����Ƃ�((1, 1, 0)�Ȃ�)�ɂ������񒷂��P�ɖ߂�(���K��)
	if (VSquareSize(m_dir) > 0)
	{
		m_dir = VNorm(m_dir);
	}

	// ���x�����߂�
	const VECTOR velecity = VScale(m_dir, kTargetMoveingSpeed);

	// �ʒu��ς���
	m_pos = VAdd(m_pos, velecity);

	m_pos.y = tempPosY;
}

void CharacterBase::Idle()
{
	// �X�e�[�^�X�̍X�V
	if (m_battleState != BattleState::IDLE)
	{
		m_battleState = BattleState::IDLE;

		m_isHit = false;
	}	

	m_isSceneChange = false;

	m_isJustGuardCounter = false;
	m_isJustGuard = false;
	m_isGuard = false;
	// 
	SetFightingMeter(0.09f);

	// �R���{������
	m_comboAttack = 0;

	// �U���J�E���g�̏�����
	m_attackFrame = 0;

	// ��������̈ʒu�ɖ߂�
	WeaponDefaultPos();
	// �������̈ʒu�ɖ߂�
	ShieldDefaultPos();

	// �̗͂��Ȃ��Ȃ����ꍇ
	if (m_hp <= 0)
	{
		m_vecWeapon.y = 100.0f;
	}

	// ���g���X�^�������ꍇ
	if (m_isStun)
	{
		m_pFunc = &CharacterBase::Stun;
	}

	// ����̊p�x��ς���
	if (m_weaponRotaY > 0.0f)
	{
		m_isAttack = true;
		m_weaponRotaY -= 0.5f;
	}
	else
	{
		m_isAttack = false;
		// ���݂̍s�����L�^
		m_battleState = BattleState::IDLE;
		m_weaponRotaY = 0.0f;
	}

	// �����Y���p�x���ꎞ�ۊ�
	m_tempWeaponRotaY = m_weaponRotaY;

	// HP�X�V����
	HitPoint();

	// �ʒu���̍X�V
	UpdatePos();

	// �V�[���h�̊p�x���Œ肷��
	m_shieldRotaY = (90) * DX_PI_F / 180.0f;
}

// �U�������ꍇ
void CharacterBase::Attack()
{
	// �X�e�[�^�X�̍X�V
	if (m_battleState != BattleState::ATTACK)
	{
		// ���݂̍s�����L�^
		m_battleState = BattleState::ATTACK;
		// �U���T�E���h�̍Đ�
		SoundManager::GetInstance().Play(SoundName::ATTACK);

		m_isHit = false;
	}

	// ���̃R���{�U���ɐ؂�ւ���
	if (m_comboAttack == 2)
	{
		m_weaponRotaY = -(90.0f * 3.0f) * DX_PI_F / 180.0f;
		m_tempWeaponRotaY = m_weaponRotaY;
		// �U�����J�n����
		m_isAttack = true;
		// �d����ԗp�t���[�������Z�b�g
		m_attackAfterStopFrame = 0;
		// �U���\���t���[�������Z�b�g
		m_attackGapFrame = 0;
		// �U���t���[�������Z�b�g
		m_attackFrame = 0;
		// �V�[���J�ڗp
		m_isSceneChange = false;
		// ���̍U���Ɉڍs����
		m_pFunc = &CharacterBase::AttackTwo;

		// ����̐U���̃��Z�b�g
		m_weaponShakeRate.x = 0.0f;
		m_weaponShakeRate.y = 0.0f;
		m_weaponShakeRate.z = 0.0f;

		return;
	}

	// ���퓮����
	if (!m_isSceneChange)
	{
		// �U���\���t���[��
		if (m_attackGapFrame < m_parameter.attackFrameGapMax)
		{
			// ����̐U���p�ʒu
			m_weaponShakeRate.x = static_cast<float>(GetRand(kShakeRate));
			m_weaponShakeRate.y = static_cast<float>(GetRand(kShakeRate));
			m_weaponShakeRate.z = static_cast<float>(GetRand(kShakeRate));
		}
		else
		{
			// �C�[�W���O���g�p�������U�鑬�x�����X�ɑ�������
			m_weaponRotaY = Easing::InSine(m_attackFrame, m_parameter.attackFrameMax, (90 * 3) * DX_PI / 180.0f,0.0f);
			
			m_vecWeapon.z = MoveByFrame(m_tempWeaponPos.z, -30.0f, m_attackFrame, m_parameter.attackFrameMax);
			m_vecWeapon.x = MoveByFrame(m_parameter.weaponRelativePos.x, 0.0f, m_attackFrame, m_parameter.attackFrameMax);

			// �U�����̃t���[������Z
			m_attackFrame++;
		}

		// �U���\���t���[������Z
		m_attackGapFrame++;
	}

	// �ő�t���[���ɓ��B������
	if (m_attackFrame == m_parameter.attackFrameMax)
	{
		m_isSceneChange = true;
	}
	
	// �V�[����؂�ւ��鎖���ł���Ȃ�
	if (m_isSceneChange)
	{	
		// �U�����Ă��Ȃ�
		m_isAttack = false;

		// �U����̍d�����
		m_attackAfterStopFrame++;

		// �U���p�t���[�����U���ő�t���[���ɂȂ����ꍇ
		if (m_attackAfterStopFrame == m_parameter.attackAfterStopFrameMax)
		{
			// �d����ԗp�t���[�������Z�b�g
			m_attackAfterStopFrame = 0;			
			//�U���\���t���[���̃��Z�b�g
			m_attackGapFrame = 0;
			// �U���t���[�������Z�b�g
			m_attackFrame = 0;
			// �R���{�I��
			m_comboAttack = 0;
			// �V�[���J�ڗp
			m_isSceneChange = false;
			// �V�[���@��
			m_pFunc = &CharacterBase::Idle;

			// ����̐U���̃��Z�b�g
			m_weaponShakeRate.x = 0.0f;
			m_weaponShakeRate.y = 0.0f;
			m_weaponShakeRate.z = 0.0f;
		}
	}

	// �X�^�����
	if (m_isStun)
	{
		m_pFunc = &CharacterBase::Stun;
	}

	// HP�X�V����
	HitPoint();
	// �ʒu���̍X�V
	UpdatePos(m_weaponShakeRate.x, m_weaponShakeRate.y, m_weaponShakeRate.z);

	m_pCharactor->SetPos(m_pos);
	m_pCharactor->SetRotate(VGet(0.0f, m_angle + ((90) * DX_PI_F / 180.0f), 0.0f));
	m_pCharactor->Update();
}

// �U���R���{2
void CharacterBase::AttackTwo()
{	
	// �X�e�[�^�X�̍X�V
	if (m_battleState != BattleState::ATTACKTWO)
	{
		// ���݂̍s�����L�^
		m_battleState = BattleState::ATTACKTWO;
		// �U���T�E���h�̍Đ�
		SoundManager::GetInstance().Play(SoundName::ATTACK);

		m_isHit = false;
	}

	// ���퓮����
	if (!m_isSceneChange)
	{
		// �C�[�W���O���g�p�������U�鑬�x�����X�ɑ�������
		m_weaponRotaY = Easing::InSine(m_attackFrame, m_parameter.attackFrameMax, -((90 * 5) * DX_PI / 180.0f), m_tempWeaponRotaY);

		m_vecWeapon.z = MoveByFrame(m_tempWeaponPos.z, -30.0f, m_attackFrame, m_parameter.attackFrameMax);
		m_vecWeapon.x = MoveByFrame(m_parameter.weaponRelativePos.x, 0.0f, m_attackFrame, m_parameter.attackFrameMax);		

		// �U�����̃t���[������Z
		m_attackFrame++;
	}

	// �ő�t���[���ɓ��B������	
	if (m_attackFrame == m_parameter.attackFrameMax)
	{
		m_isSceneChange = true;
	}

	// �V�[����؂�ւ��鎖���ł���Ȃ�
	if (m_isSceneChange)
	{
		// �U����̍d�����
		m_attackAfterStopFrame++;
		if (m_attackAfterStopFrame == m_parameter.attackAfterStopFrameMax)
		{
			m_tempWeaponRotaY = 0;
			// �U���I��
			m_isAttack = false;
			// �d����ԗp�t���[�������Z�b�g
			m_attackAfterStopFrame = 0;
			// �U���t���[�������Z�b�g
			m_attackFrame = 0;
			// �R���{�I��
			m_comboAttack = 0;
			// �V�[���J�ڗp
			m_isSceneChange = false;
			// �V�[���@��
			m_pFunc = &CharacterBase::Idle;
		}
	}

	// �X�^�����
	if (m_isStun)
	{
		m_pFunc = &CharacterBase::Stun;
	}

	// �d�͌�ŏ����̈ʒu��ς��܂�
	if (m_isGravity)
	{
		m_pos.y -= kGravity;
	}
	m_isGravity = true;

	// ����
	{
		VECTOR move = VTransform(m_vecWeapon, m_rotMtx);
		m_weaponPos = VAdd(m_pos, move);
		m_pWeapon->SetPos(m_weaponPos);
		m_pWeapon->SetRotate(VGet((90 * 3) * DX_PI / 180.0f, m_angle - m_weaponRotaY, 0));
		m_pWeapon->Update();
	}
	// ��
	{
		VECTOR move = VTransform(m_vecShield, m_rotMtx);
		move = VAdd(m_pos, move);
		m_pShield->SetPos(move);
		m_pShield->SetRotate(VGet(0, m_angle - m_shieldRotaY, 0));
		m_pShield->Update();
	}

	m_pCharactor->SetPos(m_pos);
	m_pCharactor->SetRotate(VGet(0.0f, m_angle + ((90) * DX_PI_F / 180.0f), 0.0f));
	m_pCharactor->Update();

	m_capsuleUpDown = m_pos;
	m_capsuleUpDown.y = m_pos.y + 50.0f;

	m_capsuleUpPos = m_capsuleUpDown;
	m_capsuleUpPos.y = m_capsuleUpDown.y + 150.0f;

	// HP�X�V����
	HitPoint();
	// �m�b�N�o�b�N���ꂽ�ꍇ
	KnockBack();

	m_shieldRotaY = (90) * DX_PI_F / 180.0f;
}

// ���U�������ꍇ
void CharacterBase::StrongAttack()
{
	// �X�e�[�^�X�̍X�V
	if (m_battleState != BattleState::STRONGATTACK)
	{
		// ���݂̍s�����L�^
		m_battleState = BattleState::STRONGATTACK;

		m_isHit = false;
	}

	// ���퓮����
	if (!m_isSceneChange)
	{
		if (m_attackGapFrame < m_parameter.strongAttackFrameGapMax)
		{
			m_vecWeapon.x = MoveByFrame(0.0f, 0.0f, m_attackGapFrame, m_parameter.strongAttackFrameGapMax);
			m_vecWeapon.y = MoveByFrame(0.0f, 150.0f, m_attackGapFrame, m_parameter.strongAttackFrameGapMax);
			m_vecWeapon.z = MoveByFrame(0.0f, -100.0f, m_attackGapFrame, m_parameter.strongAttackFrameGapMax);

			// �U�����̃t���[������Z
			m_attackGapFrame++;
		}
		if (m_attackGapFrame == m_parameter.strongAttackFrameGapMax)
		{
			m_shieldRotaY += 30.0f * DX_PI_F / 180.0f;
			//m_vecWeapon.y = MoveByFrame(m_parameter.weaponRelativePos.y, 500.0f, m_attackFrame, m_parameter.strongAttackFrameMax);
			m_vecWeapon.z = MoveByFrame(m_parameter.weaponRelativePos.z, -500.0f, m_attackFrame, m_parameter.strongAttackFrameMax);

			// �U�����̃t���[������Z
			m_attackFrame++;
		}
	}

	// �ő�t���[���ɓ��B������
	if (m_attackFrame == m_parameter.strongAttackFrameMax)
	{
		m_isSceneChange = true;
	}

	// �V�[����؂�ւ��鎖���ł���Ȃ�
	if (m_isSceneChange)
	{
		// ���U���̗͂��Ȃ���
		SetStrongPowerReset();
		m_shieldRotaY = 0.0f;
		// �U���t���[�������Z�b�g
		m_attackFrame = 0;
		m_attackGapFrame = 0;
		// �V�[���J�ڗp
		m_isSceneChange = false;
		// �V�[���@��
		m_pFunc = &CharacterBase::Idle;
	}

	// �d�͌�ŏ����̈ʒu��ς��܂�
	if (m_isGravity)
	{
		m_pos.y -= kGravity;
	}
	m_isGravity = true;

	// ����
	{
		VECTOR move = VTransform(m_vecWeapon, m_rotMtx);
		m_weaponPos = VAdd(m_pos, move);
		m_pWeapon->SetPos(m_weaponPos);
		m_pWeapon->SetRotate(VGet(m_shieldRotaY, m_angle - m_weaponRotaY + (90 * 3) * DX_PI_F / 180.0f, 0.0f));
		m_pWeapon->Update();
	}
	// ��
	{
		VECTOR move = VTransform(m_vecShield, m_rotMtx);
		move = VAdd(m_pos, move);
		m_pShield->SetPos(move);
		m_pShield->SetRotate(VGet(0, m_angle, 0));
		m_pShield->Update();
	}

	// �L�����N�^�[�̈ʒu
	m_pCharactor->SetPos(m_pos);
	m_pCharactor->SetRotate(VGet(0.0f, m_angle + ((90) * DX_PI_F / 180.0f), 0.0f));
	m_pCharactor->Update();

	m_capsuleUpDown = m_pos;
	m_capsuleUpDown.y = m_pos.y + 50.0f;

	m_capsuleUpPos = m_capsuleUpDown;
	m_capsuleUpPos.y = m_capsuleUpDown.y + 150.0f;

	// HP�X�V����
	HitPoint();
	// �m�b�N�o�b�N���ꂽ�ꍇ
	KnockBack();
}

// �K�[�h�����ꍇ
void CharacterBase::Guard()
{	
	m_attackFrame = 0;
	m_vecWeapon.x = -80.0f;
	m_weaponRotaY = 0;

	// �X�e�[�^�X�̍X�V
	if (m_battleState != BattleState::GUARD)
	{
		// ���݂̍s�����L�^
		m_battleState = BattleState::GUARD;
		// �K�[�h�T�E���h�̍Đ�
		SoundManager::GetInstance().Play(SoundName::GUARD);
	}


	// �ő�t���[�����ɖڕW�n�_�܂ňړ�����
	if (m_guardFrame < m_parameter.guardFrameMax)
	{
		// �C�[�W���O���g�p�������U�鑬�x�����X�ɑ�������
		m_shieldRotaY = Easing::InQuad(m_guardFrame, m_parameter.guardFrameMax, (90) * DX_PI_F / 180.0f, 0.0f);

		m_vecShield.x = MoveByFrame(m_parameter.shieldRelativePos.x, 0.0f, m_guardFrame, m_parameter.guardFrameMax);
		m_vecShield.z = MoveByFrame(m_parameter.shieldRelativePos.z, -50.0f, m_guardFrame, m_parameter.guardFrameMax);

		// �t���[���J�E���g
		m_guardFrame++;

		// �W���X�g�K�[�h�̃t���[��
		// + 1 �̓W���X�g�K�[�h�͈̔͊O������
		if (m_justGuardFrame < m_parameter.justGuardFrameMax + 1)
		{
			m_justGuardFrame++;
		}
		
	}
	else
	{
		// �t���[���J�E���g
		m_guardFrame++;
		m_vecShield.x = 0.0f;

		m_shieldRotaY = (0) * DX_PI_F / 180.0f;
	}

	// �K�[�h����߂��ꍇ
	if (!m_isGuard)
	{		
		m_guardFrame = 0;
		m_justGuardFrame = 0;
		m_pFunc = &CharacterBase::Idle;
	}

	if (m_isJustGuard)
	{
		m_isSceneChange = false;
		m_guardFrame = 0;
		m_pFunc = &CharacterBase::JustGuard;
	}

	if (m_isStun)
	{
		m_pFunc = &CharacterBase::Stun;
	}

	if (m_isWeaponAttacksShield)
	{
		m_guardOneFrame++;		
	}
	else
	{
		m_guardOneFrame = 0;
	}

	// �K�[�h�o�����ꍇ�̃T�E���h�̍Đ�
	if (m_guardOneFrame == 1)
	{
		SoundManager::GetInstance().Play(SoundName::GUARDRESULT);
	}

	// ���݂�HP�𒲐�
	HitPoint();
	// �ʒu���̍X�V
	UpdatePos();
}

// �W���X�g�K�[�h�����ꍇ
void CharacterBase::JustGuard()
{	
	// �X�e�[�^�X�̍X�V
	if (m_battleState != BattleState::JUSTGUARD)
	{
		// ���݂̍s�����L�^
		m_battleState = BattleState::JUSTGUARD;
		// �W���X�g�K�[�h�T�E���h�̍Đ�
		SoundManager::GetInstance().Play(SoundName::JUSTGUARD);

		m_isHit = false;
	}

	m_isJustGuardCounter = true;

	// �ő�t���[�����ɖڕW�n�_�܂ňړ�����
	if (!m_isSceneChange)
	{
		if (m_justGuardCounterFrame > 10 && m_justGuardCounterFrame < 20)
		{
			m_shieldRotaY = MoveByFrame(0.0f, (static_cast<float>(90) / 2.0f) * DX_PI_F / 180.0f, m_justGuardCounterFrame, 10);
		}
		if (m_justGuardCounterFrame < 15)
		{
			m_vecShield.x = MoveByFrame(m_parameter.shieldRelativePos.x, 100.0f, m_justGuardCounterFrame, 15);
		}
		if (m_justGuardCounterFrame < 15)
		{
			m_vecShield.y = MoveByFrame(m_parameter.shieldRelativePos.y, 150.0f, m_justGuardCounterFrame, 15);
		}
		if (m_justGuardCounterFrame < 15)
		{
			m_vecShield.z = MoveByFrame(m_parameter.shieldRelativePos.z, -50.0f, m_justGuardCounterFrame, 15);
		}
		m_justGuardCounterFrame++;
	}

	// �ő�t���[���ɓ��B������
	if (m_justGuardCounterFrame == 30)
	{
		m_isSceneChange = true;
	}

	// �V�[����؂�ւ��鎖���ł���Ȃ�
	if (m_isSceneChange)
	{
		m_isJustGuard = false;
		m_isJustGuardCounter = false;
		m_justGuardCounterFrame = 0;
		m_vecShield = m_parameter.shieldRelativePos;
		m_shieldRotaY = 0.0f;
		m_isSceneChange = false;	
		m_pFunc = &CharacterBase::Idle;
	}

	// HP�X�V����
	HitPoint();
	// �ʒu���̍X�V
	UpdatePos();
}

// �X�^�������ꍇ
void CharacterBase::Stun()
{
	// �X�^����Ԃ̃t���[�����J�E���g
	m_stunFrame++;

	// �X�e�[�^�X�̍X�V
	if (m_battleState != BattleState::STUN)
	{
		// ���݂̍s�����L�^
		m_battleState = BattleState::STUN;

		// ���t���[���̃��Z�b�g
		m_attackFrame = 0;
		m_attackGapFrame = 0;
		m_guardFrame = 0;
		m_justGuardFrame = 0;
	}

	// �X�^����Ԃ̃T�E���h�Đ�
	SoundManager::GetInstance().Play(SoundName::STUN,true);

	// ��]�p�x��ύX����
	m_stunRota += 0.3f;

	if (m_vecWeapon.y < m_parameter.weaponRelativePos.y + 60.0f)
	{
		m_vecWeapon.y += 10.0f;
	}

	if (m_parameter.stunFrameMax < m_stunFrame)
	{
		m_fightingMeter = 30.0f;
		m_stunRota = 0.0f;
		m_stunFrame = 0;
		m_isStun = false;
		m_pFunc = &CharacterBase::Idle;
	}

	// HP�X�V����
	HitPoint();
	// �ʒu���̍X�V
	UpdatePos();
}

void CharacterBase::HitPoint()
{
	// �U�����󂯂���
	if (m_isResultDamage)
	{
		// �U�����󂯂Ă���ꍇ
		m_isHitNow = true;

		// ���݂̍U���ƑO��̍U�����Ⴄ�ꍇ����
		// ���g���K�[�h���Ă��Ȃ��ꍇ
		// �W���X�g�K�[�h���������Ă��Ȃ��ꍇ
		// ���肪�W���X�g�K�[�h�ł��Ȃ��ꍇ
		if (!m_isHitResult &&
			m_battleState != BattleState::GUARD && 
			m_battleState != BattleState::JUSTGUARD&&
			m_targetBattleState != BattleState::STUN)
		{
			// �_���[�W�T�E���h�̍Đ�
			SoundManager::GetInstance().Play(SoundName::DAMAGE);

			// �v���C���[���L�̏���
			if(m_myId == CharacterName::PLAYER)
			{
				// ��ʂɑ΂��ẴG�t�F�N�g
				EffectScreen::GetInstance().QuakeReplayInit();
			}

			// �̗͂����炷
			m_hp--;

			if (m_hp <= 0)
			{
				m_hp = 0;
			}

			// �ꎞ�I�ɍŌ�̍U���̎�ނ��擾
			m_tempTargetBattleState = m_targetBattleState;
		}

		// �U���𖳌���
		m_isResultDamage = false;
	}
	else
	{
		m_isHitNow = false;
	}

	if (m_isHitNow)
	{
		m_isHitResult = true;
	}
	else
	{
		m_isHitResult = false;
	}

	// �X�^����Ԃɂ���
	if (m_fightingMeter <= 0.0f)
	{
		m_isStun = true;
	}
}

void CharacterBase::KnockBack()
{
	// �K�[�h������������
	// ���Ƀm�b�N�o�b�N
	if (m_isResultGuard)
	{
		// �m�b�N�o�b�N�p�̃x�N�g���͑��Έʒu���猩��Z�}�C�i�X�����Ȃ̂�0.0f�ɂȂ�܂ő����Ă���
		if (m_vecKnockBack < 0.0f)
		{
			m_vecKnockBack += 1.0f;
			VECTOR move = VTransform(VGet(0, 0, m_vecKnockBack), m_targetRotMtx);
			m_pos = VAdd(m_pos, move);
		}
		else
		{			
			m_isResultGuard = false;
		}
	}
}

void CharacterBase::WeaponDefaultPos()
{
	// ��������̈ʒu�ɖ߂�
	{
		bool isEndX = false;
		bool isEndZ = false;
		if (m_vecWeapon.x > m_parameter.weaponRelativePos.x)
		{
			m_vecWeapon.x -= 10.0f;
		}
		else
		{
			isEndX = true;
			m_vecWeapon.x = m_parameter.weaponRelativePos.x;
		}

		if (m_vecWeapon.z < m_parameter.weaponRelativePos.z)
		{
			m_vecWeapon.z += 30.0f;
		}
		else
		{
			isEndZ = true;
			m_vecWeapon.z = m_parameter.weaponRelativePos.z;
		}

		if (isEndX && isEndZ)
		{
			m_isAttack = false;
			m_isStrongAttack = false;
		}

		if (m_vecWeapon.y < m_parameter.weaponRelativePos.y)
		{
			m_vecWeapon.y -= 10.0f;
		}
		else
		{
			m_vecWeapon.y = m_parameter.weaponRelativePos.y;
		}
	}
}

void CharacterBase::ShieldDefaultPos()
{
	// �������̈ʒu�ɖ߂�
	{
		bool isEndX = false;
		bool isEndY = false;
		if (m_vecShield.x < m_parameter.shieldRelativePos.x)
		{
			m_vecShield.x += 30.0f;
		}
		else
		{
			m_vecShield.x = m_parameter.shieldRelativePos.x;
			isEndX = true;
		}

		if (m_vecShield.y < m_parameter.shieldRelativePos.y)
		{
			m_vecShield.y -= 30.0f;
		}
		else
		{
			m_vecShield.y = m_parameter.shieldRelativePos.y;
			isEndY = true;
		}
		if (isEndX && isEndY)
		{
			m_isGuard = false;
		}
	}
}

void CharacterBase::Gravity()
{
	// �d�͌�ŏ����̈ʒu��ς��܂�
	if (m_isGravity)
	{
		m_pos.y -= kGravity;
	}
	m_isGravity = true;

}

void CharacterBase::UpdatePos(int shiftX, int shiftY, int shiftZ)
{
	// �d��
	Gravity();

	// ����
	{
		VECTOR move = VTransform(m_vecWeapon, m_rotMtx);
		m_weaponPos = VAdd(m_pos, move);
		VECTOR shakePos{ m_weaponPos.x + shiftX ,m_weaponPos.y + shiftY ,m_weaponPos.z + shiftZ };
		m_pWeapon->SetPos(shakePos);
		m_pWeapon->SetRotate(VGet(90 * DX_PI / 180.0f, m_angle - m_weaponRotaY, 0.0f));
		m_pWeapon->Update();
	}
	// ��
	{
		VECTOR move = VTransform(m_vecShield, m_rotMtx);
		move = VAdd(m_pos, move);
		m_pShield->SetPos(move);
		m_pShield->SetRotate(VGet(0, m_angle - m_shieldRotaY, 0));
		m_pShield->Update();
	}	

	// �L�����N�^�[�̈ʒu
	m_pCharactor->SetPos(m_pos);
	m_pCharactor->SetRotate(VGet(0.0f, m_stunRota + m_angle + ((90) * DX_PI_F / 180.0f), 0.0f));
	m_pCharactor->Update();

	m_capsuleUpDown = m_pos;
	m_capsuleUpDown.y = m_pos.y + 50.0f;

	m_capsuleUpPos = m_capsuleUpDown;
	m_capsuleUpPos.y = m_capsuleUpDown.y + 150.0f;

	// �m�b�N�o�b�N���ꂽ�ꍇ
	KnockBack();
}

float CharacterBase::MoveByFrame(const float relativePos, const float EndPos, const int nowFrame, const int maxFrame)
{
	// �ʒu = ���Έʒu + (�������������W - ���Έʒu) * (���݂̃t���[�� / �ő�t���[��)�@�@
	const float resultPos = relativePos + (EndPos - relativePos) * (static_cast<float>(nowFrame) / static_cast<float>(maxFrame));
	return resultPos;
}

void CharacterBase::InputParamType(int type)
{
	// �p�����[�^�[����
	m_parameter.attackFrameMax = CharactorDataManager::GetInstance().GetParamData(ParamData::ATTACK_FRAME_MAX, type).element[0];
	m_parameter.attackFrameGapMax = CharactorDataManager::GetInstance().GetParamData(ParamData::ATTACK_FRAME_GAP_MAX, type).element[0];
	m_parameter.attackTotalFrame = CharactorDataManager::GetInstance().GetParamData(ParamData::ATTACK_TOTAL_FRAME, type).element[0];

	m_parameter.attackAfterStopFrameMax = CharactorDataManager::GetInstance().GetParamData(ParamData::ATTACK_AFTER_STOP_FRAME_MAX, type).element[0];

	m_parameter.strongAttackFrameMax = CharactorDataManager::GetInstance().GetParamData(ParamData::STRONG_ATTACK_FRAME_MAX, type).element[0];
	m_parameter.strongAttackFrameGapMax = CharactorDataManager::GetInstance().GetParamData(ParamData::STRONG_ATTACK_FRAME_GAP_MAX, type).element[0];
	m_parameter.strongAttackTotalFrame = CharactorDataManager::GetInstance().GetParamData(ParamData::STRONG_ATTACK_TOTAL_FRAME, type).element[0];

	m_parameter.guardFrameMax = CharactorDataManager::GetInstance().GetParamData(ParamData::GUARD_FRAME_MAX, type).element[0];
	m_parameter.justGuardFrameMax = CharactorDataManager::GetInstance().GetParamData(ParamData::JUST_GUARD_FRAME_MAX, type).element[0];

	m_parameter.stunFrameMax = CharactorDataManager::GetInstance().GetParamData(ParamData::STUN_FRAME_MAX, type).element[0];

	m_parameter.hpMax = CharactorDataManager::GetInstance().GetParamData(ParamData::HP_MAX, type).element[0];
	m_parameter.fightingMeterMax = CharactorDataManager::GetInstance().GetParamData(ParamData::FIGHTING_METER_MAX, type).element[0];

	m_parameter.weaponRelativePos =
	{
		CharactorDataManager::GetInstance().GetParamData(ParamData::WEAPON_RELATIVE_POS, type).element[0],
		CharactorDataManager::GetInstance().GetParamData(ParamData::WEAPON_RELATIVE_POS, type).element[1],
		CharactorDataManager::GetInstance().GetParamData(ParamData::WEAPON_RELATIVE_POS, type).element[2]
	};
	m_parameter.shieldRelativePos =
	{
		CharactorDataManager::GetInstance().GetParamData(ParamData::SHIELD_RELATIVE_POS, type).element[0],
		CharactorDataManager::GetInstance().GetParamData(ParamData::SHIELD_RELATIVE_POS, type).element[1],
		CharactorDataManager::GetInstance().GetParamData(ParamData::SHIELD_RELATIVE_POS, type).element[2]
	};

	m_parameter.weaponAttackRadius = CharactorDataManager::GetInstance().GetParamData(ParamData::WEAPON_ATTACK_RADIUS, type).element[0];
	m_parameter.shieldRadius = CharactorDataManager::GetInstance().GetParamData(ParamData::SHIELD_RADIUS, type).element[0];
	m_parameter.modelRadius = CharactorDataManager::GetInstance().GetParamData(ParamData::MODEL_RADIUS, type).element[0];

	m_parameter.weaponAttackPos =
	{
		CharactorDataManager::GetInstance().GetParamData(ParamData::WEAPON_ATTACK_POS, type).element[0],
		CharactorDataManager::GetInstance().GetParamData(ParamData::WEAPON_ATTACK_POS, type).element[1],
		CharactorDataManager::GetInstance().GetParamData(ParamData::WEAPON_ATTACK_POS, type).element[2]
	};
	m_parameter.knockBackPos =
	{
		CharactorDataManager::GetInstance().GetParamData(ParamData::KNOCKBACK_POS, type).element[0],
		CharactorDataManager::GetInstance().GetParamData(ParamData::KNOCKBACK_POS, type).element[1],
		CharactorDataManager::GetInstance().GetParamData(ParamData::KNOCKBACK_POS, type).element[2]
	};

	m_parameter.weaponBackSpeed = CharactorDataManager::GetInstance().GetParamData(ParamData::WEAPON_BACK_SPEED, type).element[0];
	m_parameter.shieldBackSpeed = CharactorDataManager::GetInstance().GetParamData(ParamData::SHIELD_BACK_SPEED, type).element[0];

	m_parameter.justGuardRate = CharactorDataManager::GetInstance().GetParamData(ParamData::JUSTGUARDRATE, type).element[0];
}

void CharacterBase::SetAngle(float angle)
{
	m_angle = angle;
}

void CharacterBase::SetRotMtx(MATRIX rotMtx)
{
	m_rotMtx = rotMtx;
}


CharacterName CharacterBase::GetMyId()
{
	return m_myId;
}

BattleState CharacterBase::GetBattleState()
{
	return m_battleState;
}

VECTOR CharacterBase::GetPos()const
{
	return m_pos;
}

VECTOR CharacterBase::GetCollPos() const
{
	return VGet(m_pos.x, m_pos.y + 100.0f, m_pos.z);
}

VECTOR CharacterBase::GetCollWeaponPos() const
{
	return m_pWeapon->GetCollPos(3);
}

VECTOR CharacterBase::GetShieldPos() const
{
	VECTOR move = VTransform(m_vecShield, m_rotMtx);
	move = VAdd(m_pos, move);
	return move;
}

VECTOR CharacterBase::GetCapsulePosDown()
{
	return m_capsuleUpDown;
}

VECTOR CharacterBase::GetCapsulePosUp()
{	
	return m_capsuleUpPos;
}

float CharacterBase::GetCapsuleRadius()
{
	return kCapsuleRadius;
}

MATRIX CharacterBase::GetRot()const
{
	return m_rotMtx;
}

float CharacterBase::GetAngle() const
{
	return m_angle;
}

float CharacterBase::GetModelRadius() const
{
	return m_parameter.modelRadius ;
}

float CharacterBase::GetWeaponAttackRadius() const
{
	return m_parameter.weaponAttackRadius;
}

VECTOR CharacterBase::GetWeaponAttackRelative() const
{
	return m_parameter.weaponAttackPos;
}

float CharacterBase::GetShieldRadius() const
{
	return m_parameter.shieldRadius;
}

VECTOR CharacterBase::GetShieldRelative() const
{
	return m_parameter.shieldRelativePos;
}

void CharacterBase::SetTargetPos(VECTOR pos)
{
	m_targetPos = pos;
}

void CharacterBase::SetTargetHp(const int hp)
{
	m_targetHp = hp;
}

void CharacterBase::SetCollGuardEffect()
{
	// �K�[�h�ۂ̃G�t�F�N�g�Đ�
	EffekseerDrawer::GetInstance().Play(
		m_effectHandle, Id::Guard,
		EffectPlayType::NORMAL,
		GetShieldPos(),
		VGet(1, 1, 1),
		VGet(0, m_angle, 0));
}

void CharacterBase::SetCollJustGuardEffect()
{
	// �W���X�g�K�[�h�����ۂ̃G�t�F�N�g�Đ�
	EffekseerDrawer::GetInstance().Play(
		m_effectHandle, Id::JustGuard,
		EffectPlayType::NORMAL,
		VGet(GetShieldPos().x, GetShieldPos().y + 100.0f, GetShieldPos().z),
		VGet(1, 1, 1),
		VGet(0, 0, 0));
}

void CharacterBase::SetStrongPower(int power)
{
	m_strongAttackPower += power;
	if (m_strongAttackPower >= 100)
	{
		m_strongAttackPower = 100;
	}
}

void CharacterBase::IsHit()
{
	m_isHit = true;
}

void CharacterBase::SetStrongPowerReset()
{
	m_strongAttackPower = 0;
}

int CharacterBase::GetAttackFrame()const
{
	const int totalFrame = m_attackFrame + m_attackGapFrame;
	return totalFrame;
}

int CharacterBase::GetGuardFrame()const
{
	return m_guardFrame;
}

int CharacterBase::GetJustGuardFrame()const
{
	return m_justGuardFrame;
}

int CharacterBase::GetAttackFrameMax()const
{
	if (m_isAttack)
	{
		return m_parameter.attackTotalFrame;
	}
	else if (m_isStrongAttack)
	{
		return m_parameter.strongAttackTotalFrame;
	}

	return -1;
}

int CharacterBase::GetGuardFrameMax()const
{
	return m_parameter.guardFrameMax ;
}

int CharacterBase::GetJustGuardFrameMax()const
{
	return m_parameter.justGuardFrameMax;
}

int CharacterBase::GetStunFrameMax() const
{
	return m_parameter.stunFrameMax;
}

int CharacterBase::GetHp()const
{
	return m_hp;
}

int CharacterBase::GetMaxHp() const
{
	return m_parameter.hpMax;
}

float CharacterBase::GetFightingMeter()const
{
	return m_fightingMeter;
}

bool CharacterBase::IsAway() const
{
	return m_isAway;
}

bool CharacterBase::IsHitDamage() const
{
	return m_isHitNow;
}

bool CharacterBase::IsGuard() const
{
	return m_isGuard;
}

float CharacterBase::GetStrongPower()
{
	return m_strongAttackPower;
}

float CharacterBase::GetkStrongAttackPowerMax()
{
	return kStrongAttackPowerMax;
}

bool CharacterBase::IsAttack() const
{
	return m_isHit;
}

bool CharacterBase::GetTipsMove(Tips tips)
{
	return m_isTipsMove[static_cast<int>(tips)];
}

bool CharacterBase::GetCheckHitWall()
{
	return m_isHItWall;
}

bool CharacterBase::IsJustGuard() const
{
	return m_isJustGuard;
}

bool CharacterBase::IsStun() const
{
	return m_isStun;
}

bool CharacterBase::IsAttackRange() const
{
	return m_isAttackRange;
}

void CharacterBase::SetFieldHit()
{
	m_isGravity = false;	
}

void CharacterBase::SetWeaponAttacksShield(const bool isShieldHit)
{
	m_isWeaponAttacksShield = isShieldHit;
}

void CharacterBase::SetBattleState(BattleState state)
{	
	m_targetBattleState = state;
}

void CharacterBase::SetTargetMtxRota(MATRIX rot)
{
	m_targetRotMtx = rot;
}

void CharacterBase::SetAttackRange(const bool isRange)
{
	m_isAttackRange = isRange;
}

void CharacterBase::SetDamage(bool isDamage)
{
	m_isResultDamage = isDamage;
}

void CharacterBase::SetGuardKnockBack(bool isGuard,float vec)
{
	m_isResultGuard = isGuard;
	m_vecKnockBack = vec;
}

void CharacterBase::SetJustGuard(bool isJustGuard)
{
	m_isJustGuard = isJustGuard;
}

void CharacterBase::SetFightingMeter(const float fightingMeter)
{
	m_tempFightingMeter = fightingMeter;
#if true
	FightingMeter();
#endif
}

void CharacterBase::FightingMeter()
{
	m_fightingMeter += m_tempFightingMeter;

	// �킢�ɕK�v�ȃ��[�^�[���ő�l�ɂȂ�ƍő�l�ŌŒ�
	if (m_fightingMeter > m_parameter.fightingMeterMax)
	{
		m_fightingMeter = m_parameter.fightingMeterMax;
	}
	// �킢�ɕK�v�ȃ��[�^�[���ŏ��l�ɂȂ�ƍŏ��l�ŌŒ�
	if (m_fightingMeter < 0.0f)
	{
		m_isStun = true;
		m_fightingMeter = 0.0f;
	}
}
