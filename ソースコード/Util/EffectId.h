#pragma once
enum Id
{
	// �ʏ�K�[�h�����ꍇ
	Guard,
	// �W���X�g�K�[�h�����ꍇ
	JustGuard,
	// �X�^����ԂɂȂ����ꍇ
	Stun,

	Max
};

enum EffectPlayType
{
	NORMAL,
	LOOP,
};

struct EffectData
{
	// �Đ��p�n���h��
	int *playingHandle;
	//
	Id id;
	// �Đ��^�C�v
	EffectPlayType type;
	// �ʒu
	VECTOR pos;
	// �T�C�Y
	VECTOR size;
	// �p�x
	VECTOR angle;
	// �Đ��t���[��
	int playFrameMax;
	int playFrameCount;
};