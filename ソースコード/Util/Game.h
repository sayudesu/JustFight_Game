#pragma once

namespace Game
{
#if _DEBUG
	// �E�C���h�E���[�h�ݒ�
	constexpr bool kWindowMode = true;
#else
	// �E�C���h�E���[�h�ݒ�
	constexpr bool kWindowMode = false;
#endif

	// �E�C���h�E��
	const char* const kTitleText = "JustFight";
	// �E�C���h�E�T�C�Y
	constexpr int kScreenWidth  = 1920;
	constexpr int kScreenHeight = 1080;
	// �J���[���[�h
	constexpr int kColorDepth = 32;
	// window���[�h����window�T�C�Y��ύX���邩�ǂ���
	constexpr bool kWindowSizeChange = true;
	// ���O�e�L�X�g���쐬���邩�ǂ���
	constexpr bool kCreateLogText = false;
};