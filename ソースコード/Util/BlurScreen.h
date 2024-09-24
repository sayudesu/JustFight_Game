#pragma once

class EffectScreen
{
private:
	EffectScreen() = default;
	static EffectScreen* m_pInstance;
public:
	virtual~EffectScreen() = default;
	// �R�s�[�R���X�g���N�^�̋֎~
	EffectScreen(const EffectScreen&)            = delete;
	EffectScreen& operator=(const EffectScreen&) = delete;
	EffectScreen(EffectScreen&&)                 = delete;
	EffectScreen& operator=(EffectScreen&&)      = delete;

	// �C���X�^���X�̍쐬
	static EffectScreen& GetInstance()
	{
		if (!m_pInstance)
		{
			m_pInstance = new EffectScreen();
		}
		return *m_pInstance;
	}

	// �������
	static void Destroy()
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
public:

	// MakeScreen���ĉ�ʃT�C�Y���擾����
	void Load();
	void Unload();
	//	��ʂɕ`���ꂽ���̂���������
	void ClearScreen();
	// �`��𗠂̉�ʂɂ���
	void ScreenBack();

	// �u���[
	void BlurIReplayInit();
	void BlurRelease();
	void BlurPreRenderBlurScreen();
	void BlurPostRenderBlurScreen(bool isBlurDraw);

	// �h�炵
	void QuakeReplayInit();
	void QuakeUpdate();

private:
	enum class DamageEffectNo
	{
		QUAKE,
		COLOR,
		MAX,
	};
	// �U�������ꂽ�ꍇ�̉�ʏ���
	int m_damageScreen[static_cast<int>(DamageEffectNo::MAX)]{};

	// ��ʗh��p
	float m_shake = 0.0f;
	int m_shakeFrame = 0;

	// �u���[��ς���ꍇ�̏d�˂��ʂ̐�
	enum class ScreenEffectNo
	{
		ONE,
		TWO,

		MAX
	};

	// �U�������ꂽ�ꍇ�̉�ʏ���
	int m_blurScreen[static_cast<int>(ScreenEffectNo::MAX)]{};

	// �u���[��p
	int m_current = 0;
	int m_alpha   = 0;
	int m_notBlendDraw = 0;
	int m_blendAddRate = 0;
};

