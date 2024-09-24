#pragma once
#include <DxLib.h>
#include <string>

// �L�����N�^�[�̃p�����[�^�[
const struct CharacterParameter
{
	std::string fileName; // �t�@�C���l�[��

	int attackFrameMax;    // �U���ő�t���[��
	int attackFrameGapMax; // �U���\���̍ő�t���[��
	int attackTotalFrame;  // �U���t���[���̍��v

	int attackAfterStopFrameMax; // �U����̍d���t���[��

	int strongAttackFrameMax;    // ���U���ő�t���[��
	int strongAttackFrameGapMax; // ���U���\���̍ő�t���[��
	int strongAttackTotalFrame;  // ���U���t���[���̍��v

	int guardFrameMax;     // �K�[�h�ő�t���[��
	int justGuardFrameMax; // �W���X�g�K�[�h�ő�t���[��

	int stunFrameMax; // �X�^����Ԃ̍ő�t���[��

	int hpMax;            // �ő�̗�
	int fightingMeterMax; // �ő�퓬�ɕK�v�ȃ��[�^�[�̍ő�l

	VECTOR weaponRelativePos; // ����
	VECTOR shieldRelativePos; // ��

	float weaponAttackRadius; // ����̔��a
	float shieldRadius;       // ���̔��a
	float modelRadius;        // ���f���̓����蔻��p�͈�

	VECTOR weaponAttackPos; // ����̍U�����̔���̑��Έʒu
	VECTOR knockBackPos;    // �K�[�h���Ă��鎞�ɍU�����󂯂��ꍇ�̃m�b�N�o�b�N�ňړ����鑊�Έʒu

	float weaponBackSpeed; // �������̈ʒu�ɖ߂鑬�x
	float shieldBackSpeed; // �������̈ʒu�ɖ߂鑬�x

	int justGuardRate; // �W���X�g�K�[�h���Ă��銄��
}; 