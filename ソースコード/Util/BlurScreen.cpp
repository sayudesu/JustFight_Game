#include <DxLib.h>

#include "BlurScreen.h"

#include "Game.h"

EffectScreen* EffectScreen::m_pInstance = nullptr;

namespace
{
	// �h��
	constexpr float kShakeMaxPower = 20.0f;
	constexpr float kShakeRate = 0.95f;

	// �h��t���[��
	constexpr int kShakeFrame = 30;

	// �u�����h���[�h�����̏ꍇ�̒l
	constexpr int kNoBlendModeRate = 128;

	// �u���[��������ۂ̃A���t�@�l
	constexpr int kBlurAlphaRate = 150;
}

void EffectScreen::Load()
{
	// �U�����o�p
	m_shake = 0.0f;
	m_shakeFrame = 0;

	// ���H�p�̉�ʃn���h����ۑ�
	int sw = 0, sh = 0, bit = 0;      // ��ʕ�����ʍ����r�b�g��
	GetScreenState(&sw, &sh, &bit);   // ���ƍ������擾���Ă���

	for (int i = 0; i < static_cast<int>(DamageEffectNo::MAX); i++)
	{
		m_damageScreen[i] = MakeScreen(sw, sh);// ���H�p��ʂ�p�ӂ���
	}
	// ���[�V�����u���[�p
	m_notBlendDraw = 0;
	m_current = 0;
	m_alpha = kBlurAlphaRate;

	// �摜�n���h�����쐬����
	for (int i = 0; i < static_cast<int>(ScreenEffectNo::MAX); ++i)
	{
		m_blurScreen[i] = MakeScreen(Game::kScreenWidth, Game::kScreenHeight);
	}
}

void EffectScreen::Unload()
{
	// �摜�n���h�����폜����
	for (int i = 0; i < static_cast<int>(DamageEffectNo::MAX); ++i)
	{
		if (m_damageScreen[i] != -1)
		{
			DeleteGraph(m_damageScreen[i]);
		}
	}
}

void EffectScreen::ClearScreen()
{
	// ��ʂ��N���A�ɂ���
	ClearDrawScreen();
}

void EffectScreen::ScreenBack()
{
	SetDrawScreen(DX_SCREEN_BACK);
}

void EffectScreen::BlurIReplayInit()
{
	// �摜�n���h�������������A�V�����摜�n���h�����쐬����	
	for (int i = 0; i < static_cast<int>(ScreenEffectNo::MAX); ++i)
	{
		if (m_blurScreen[i] != -1)
		{
			DeleteGraph(m_blurScreen[i]);
		}

		m_blurScreen[i] = MakeScreen(Game::kScreenWidth, Game::kScreenHeight);
	}
	m_current      = 0;
	m_notBlendDraw = 0;
}

void EffectScreen::BlurRelease()
{
	// �摜�n���h�����폜����
	for (int i = 0; i < static_cast<int>(ScreenEffectNo::MAX); ++i)
	{
		if (m_blurScreen[i] != -1)
		{
			DeleteGraph(m_blurScreen[i]);
		}
	}
}

void EffectScreen::BlurPreRenderBlurScreen()
{
	// ��ʂ̍쐬
	SetDrawScreen(m_blurScreen[m_current]);
}

void EffectScreen::BlurPostRenderBlurScreen(bool isBlurDraw)
{
	// �A���t�@�l�ύX�p
	int alphaRate = 0;
	// �`�悷���ʂ̃t���[��
	int backFrameRate = 0;

	// �u���[�����o�����邩�ǂ���
	if (isBlurDraw)
	{
		alphaRate     = m_alpha;
		backFrameRate = 1;		
	}
	else
	{
		alphaRate     = 0;
		backFrameRate = 0;
	}

	// �A���t�@�̒l��ύX����
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alphaRate);

	// ��ʂ̕ۑ������J�E���g���������珈�������s����
	m_notBlendDraw++;
	if (m_notBlendDraw > static_cast<int>(ScreenEffectNo::MAX))
	{
		// backFrameRate�t���[���O�̉��
		DrawExtendGraph(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_blurScreen[backFrameRate - m_current], false);		
	}

	// �ȉ��̏����Ƀu�����h���[�h��K�p���Ȃ�
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// �o�C���j�A�@�ŕ`��
	SetDrawMode(DX_DRAWMODE_BILINEAR);

	// ��ʂ̐؂�ւ�
	SetDrawScreen(DX_SCREEN_BACK);

	// ��ʗh����ʂ����邩�ǂ���
	if (static_cast<int>(m_shake) != 0)
	{
		// ���Z��������
		SetDrawBlendMode(DX_BLENDMODE_ADD, 128);
	
		// �O���t�B�b�N�n���h����������ʂ�`�悳����
		DrawGraphF(m_shake, m_shake, m_blurScreen[m_current], false);

		// �K�E�X�łڂ���������
		GraphFilter(m_damageScreen[static_cast<int>(DamageEffectNo::QUAKE)], DX_GRAPH_FILTER_GAUSS, 32, 1400);
	}

	// �u���[���ʂ𓾂��摜��`�悷��
	DrawGraphF(m_shake, m_shake, m_blurScreen[m_current], false);

	// �u���[���ʗp�Ɏg�p���Ă��Ȃ������z��̔ԍ����v�Z����
	m_current = 1 - m_current;

	// ��ʗh����ʂ����邩�ǂ���
	if (static_cast<int>(m_shake) != 0)
	{
		// �u�����h���[�h��������Ԃɖ߂�
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, kNoBlendModeRate);
	}
}

void EffectScreen::QuakeReplayInit()
{	
	// �h��
	m_shake      = kShakeMaxPower;
	// �h���t���[����
	m_shakeFrame = kShakeFrame;
}

void EffectScreen::QuakeUpdate()
{	
	// ��ʂ̗h���̒���
	if (m_shakeFrame > 0) 
	{
		m_shake = -m_shake;
		m_shake *= kShakeRate;
		m_shakeFrame--;
	}
	else
	{
		m_shake = 0.0f;
	}
}