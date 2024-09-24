#pragma once
#include <DxLib.h>

#include "../Util/CharacterName.h"
#include "../Util/BattleState.h"
#include "../Util/CharacterParameter.h"

#include "../Util/DifficultyData.h"

#include "../Util/Tips.h"

// �Q�[���I�u�W�F�N�g
class GameObject;

// �L�����N�^�[��p���N���X
class CharacterBase
{
public:
	CharacterBase(DifficultyData data,VECTOR pos);
	virtual ~CharacterBase();

	virtual void Init();
	virtual void End();
	virtual void Update();
	virtual void Draw();

	// ����
	virtual void Input() = 0;
	// �`���[�g���A���p
	virtual void InputTutorial() = 0;

protected:

	// �A�C�h�����
	virtual void Idle();
	// �U��
	virtual void Attack();
	virtual void AttackTwo();
	// ���U��
	virtual void StrongAttack();
	// �h��
	virtual void Guard();

private:
	// �ʒu���̍X�V
	void UpdatePos(int shiftX = 0, int shiftY = 0, int shiftZ = 0);
	// �t���[���Ō������������W�܂œ�����
	float MoveByFrame(const float relativePos, const float EndPos, const int nowFrame, const int maxFrame);

	// �W���X�g�K�[�h
	virtual void JustGuard();
	// �W���X�g�K�[�h�����ꂽ�ꍇ
	virtual void Stun();

	// �̗͂̒���
	virtual void HitPoint();
	// �m�b�N�o�b�N
	virtual void KnockBack();

	// ���������̈ʒu�ɖ߂�����
	virtual void WeaponDefaultPos();
	virtual void ShieldDefaultPos();

	// �d��
	virtual void Gravity();

protected:
	// �p�����[�^�[�̒���
	virtual void InputParamType(int type);
	// �p�x���擾
	virtual void SetAngle(float angle);
	virtual void SetRotMtx(MATRIX rotMtx);

	// �^�[�Q�b�g�ʒu�ǐՈړ�
	virtual void TargetMove();

	// �L�����N�^�[�̌����̒���
	virtual void Direction() = 0;

	// ���U���ׂ̈ɗ��߂��͂����Z�b�g����
	void SetStrongPowerReset();

	// �ǂɓ����������ǂ���
	bool GetCheckHitWall();
public:
	// ���g���N����Ԃ�
	CharacterName GetMyId();

	// ���݂̍s���̏���n��
	BattleState GetBattleState();

	// �ʒu
	VECTOR GetPos      ()const;// ���g
	VECTOR GetCollPos      ()const;// ���g
	VECTOR GetCollWeaponPos() const;
	VECTOR GetShieldPos ()const;// ��

	// �J�v�Z���^����p�̈ʒu��n��
	VECTOR GetCapsulePosDown();
	VECTOR GetCapsulePosUp();
	float GetCapsuleRadius();

	// �p�x
	MATRIX GetRot ()const;// �s��
	float GetAngle()const;// ���W�A��

	// ���f���̓����蔻��p���a
	float GetModelRadius()const;
	
	// �����̔���p���a
	float GetWeaponAttackRadius()const;
	float GetShieldRadius()const;

	// �����̑��Έʒu
	VECTOR GetWeaponAttackRelative()const;
	VECTOR GetShieldRelative()const;

	// �t���[���֌W
	int GetAttackFrame      ()const;// ���݂̍U���t���[��
	int GetGuardFrame       ()const;// ���݂̖h��t���[��
	int GetJustGuardFrame   ()const;// ���݂̃W���X�g�K�[�h�t���[��
	int GetAttackFrameMax   ()const;// �ő�U���t���[��
	int GetGuardFrameMax    ()const;// �ő�h��t���[��
	int GetJustGuardFrameMax()const;// �ő�W���X�g�K�[�h�t���[��
	int GetStunFrameMax     ()const;// �ő�X�^���̃t���[����

	// �Q�[�W�֌W
	int GetHp             ()const;// ���݂̃q�b�g�|�C���g
	int GetMaxHp          ()const;// ���݂̃q�b�g�|�C���g
	float GetFightingMeter()const;// �퓬�ɕK�v�ȃ��[�^�[

	bool IsAway()const;

	bool IsHitDamage()const;

	bool IsGuard()const;

	// ���U�������邽�߂̗͂��m�F
	float GetStrongPower();
	// ���U���̍ő�l
	float GetkStrongAttackPowerMax();

	// �U�����\���ǂ���
	bool IsAttack()const;

	// �`���[�g���A���p
	// ���݂ł���s����n��
	bool GetTipsMove(Tips tips);

	// �W���X�g�K�[�h�ł������ǂ���
	bool IsJustGuard()const;

	// �X�^����Ԃ��ǂ���
	bool IsStun()const;

	// �U���͈͂ɓ����Ă��邩�ǂ���
	bool IsAttackRange()const;

	// �n�ʂɓ������Ă��邩�ǂ����A�������Ă���ꍇ�Ăяo���Ă�������
	void SetFieldHit();

	// �U�������ɓ����������ǂ���
	void SetWeaponAttacksShield(const bool isShieldHit);

	// ���݂̍s���̏����󂯎��
	void SetBattleState(BattleState state);

	// ����̊p�x
	void SetTargetMtxRota(const MATRIX rot);

	// �U���͈͂ɓ����Ă��邩�ǂ���
	void SetAttackRange(const bool isRange);

	// �U�����󂯂����ǂ���
	void SetDamage(const bool isDamage);

	// �K�[�h�������������ǂ���
	void SetGuardKnockBack(bool isGuard, float vec);

	// �W���X�g�K�[�h�������������ǂ���
	void SetJustGuard(const bool isJustGuard);

	// �킢�ɕK�v�ȓ���ȃ��[�^�[��ύX����
	void SetFightingMeter(const float fightingMeter);

	// �킢�ɕK�v�ȓ���ȃ��[�^�[���Ǘ�����
	void FightingMeter();

	// �^�[�Q�b�g�̈ʒu���擾����
	void SetTargetPos(const VECTOR pos);

	// �^�[�Q�b�g�̗̑͂��擾
	void SetTargetHp(const int hp);

	// �G�t�F�N�g���Ăяo��
	void SetCollGuardEffect    ();// �ʏ�h��
	void SetCollJustGuardEffect();// �W���X�g�K�[�h

	// ���U�����o�����߂̗͂𗭂߂�
	void SetStrongPower(int power);

	// �U�������ꂽ���ǂ���
	void IsHit();
private:
	// �G�t�F�N�g�n���h��
	int m_effectHandle;

	// ���U�����o�����߂̗͂𗭂߂�
	int m_strongAttackPower;

	// �m�b�N�o�b�N�p�x�N�g��
	float m_vecKnockBack;

	// �p�x
	MATRIX m_rotMtx;
	MATRIX m_targetRotMtx;
	// ����
	VECTOR m_vecWeapon;
	VECTOR m_vecShield;
	VECTOR m_weaponPos;
	// �J�v�Z���̏�̈ʒu
	VECTOR m_capsuleUpPos;
	VECTOR m_capsuleUpDown;

	// �ꎞ�I�ȕ���̈ʒu
	VECTOR m_tempWeaponPos;

	// �̗�
	// �킢�ɕK�v�ȓ���ȃ��[�^�[
	int m_hp;
	float m_tempFightingMeter;
	float m_fightingMeter;

	// �t���[���֘A
	int m_attackGapFrame;// �U�����̗V�т̃t���[��
	int m_attackFrame;   // �V�т���U���̃t���[��
	int m_guardFrame;    // �h�䂷��܂ł̃t���[��
	int m_justGuardFrame;// �W���X�g�K�[�h�̃t���[��
	int m_stunFrame;     // �X�^����Ԃ̃t���[��
	int m_recoilFrame;   // �U����e����Ώꍇ�̃t���[��
	int m_attackAfterStopFrame; // �U����̍d���t���[��
	int m_justGuardCounterFrame;// �W���X�g�K�[�h�𐬌���������̃J�E���g

	// �h�䂵���ꍇ��1�t���[������������Ƃ�p�t���[���J�E���g
	int m_guardOneFrame;

	// �U���p�V�[���@�ۂ̈ړ����\���ǂ���
	int m_isSceneChange;

	// �G����̍U�����K�[�h�ł������ǂ���
	bool m_isResultGuard;
	bool m_isResultDamage;

	// �����U���������ǂ���
	bool m_isWeaponAttacksShield;

	// �X�^����Ԃ��ǂ���
	bool m_isStun;

	// �U���͈͂ɂ��邩�ǂ���
	bool m_isAttackRange;

	// �^�[�Q�b�g�̗̑�
	int m_targetHp;

	// �ǂɓ����������ǂ���
	bool m_isHItWall;

	// �U���̗\���t���[�����̐U��
	VECTOR m_weaponShakeRate;

	// ���݂̍s�����L�^
	BattleState m_battleState;

	GameObject* m_pCharactor;
	GameObject* m_pWeapon;
	GameObject* m_pShield;

	bool m_isHitNow = false;
	bool m_isHitResult = false;

	float m_shieldRotaY = 0.0f;
	float m_weaponRotaY = 0.0f;
	float m_tempWeaponRotaY = 0.0f;

	// �X�^�������ꍇ�̉�](���W�A��)
	float m_stunRota = 0.0f;

	// �U���������������ǂ���
	bool m_isHit;

protected:
	
	// �����o�֐��|�C���^
	void(CharacterBase::*m_pFunc)();

	// �d��
	bool m_isGravity;

	// �ʒu
	VECTOR m_pos;
	VECTOR m_vec;
	VECTOR m_targetPos;

	// �p�x
	float m_angle;

	// ���
	bool m_isAttack;
	bool m_isStrongAttack;
	bool m_isGuard;
	bool m_isJustGuard;
	bool m_isJustGuardCounter;

	// ���
	bool m_isAway;

	// �^�[�Q�b�g�̋���
	VECTOR m_targetRange;

	// ���g��ID���擾����
	CharacterName m_myId;

	// �^�[�Q�b�g�̍s�����L�^
	BattleState m_targetBattleState;
	BattleState m_tempTargetBattleState;

	// �L�����N�^�[�S�Ẵp�����[�^�[
	CharacterParameter m_parameter;

	// �R���{�Z�p
	int m_comboAttack;

	// ��Փx
	DifficultyData m_difficultyData;

	// �`���[�g���A���p
	// �ǂ̍s�����ł��邩���߂�
	bool m_isTipsMove[static_cast<int>(Tips::MAX)];
};

