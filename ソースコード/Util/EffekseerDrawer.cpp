#include "EffekseerDrawer.h"
#include "EffekseerForDXLib.h"

EffekseerDrawer* EffekseerDrawer::m_pInstance = nullptr;

namespace
{	
	struct EffekseerData
	{
		const char* fileName;
		float size;
	};

	// �G�t�F�N�g�Ǘ��p�ϐ�
	const EffekseerData kEffectNo[Id::Max] =
	{
		// �ʏ�K�[�h�����ꍇ
		{
			"Data/Guard.efk",
			30.0f,
		},
		// �W���X�g�K�[�h�����ꍇ
		{
			"Data/Guard2.efk",
			130.0f,
		},
		// �X�^����Ԃ̏ꍇ
		{
			"Data/Hiyoko.efk",
			30.0f,
		},
	};
}

void EffekseerDrawer::Load()
{
	for (int i = 0; i < Id::Max; i++)
	{
		m_handle[i] = LoadEffekseerEffect(kEffectNo[i].fileName, kEffectNo[i].size);
	}
}

void EffekseerDrawer::Unload()
{
	for (int i = 0; i < Id::Max; i++)
	{
		// �G�t�F�N�g���\�[�X���폜����B
		DeleteEffekseerEffect(m_handle[i]);
	}
}

void EffekseerDrawer::Update()
{
	// DX���C�u�����̃J������Effekseer�̃J�����𓯊�����B
	Effekseer_Sync3DSetting();

	for(auto& effect : m_effectData)
	{
		// �Đ��t���[���̃J�E���g
		effect.playFrameCount++;
		if (effect.playFrameCount < effect.playFrameMax)
		{
			// �Đ�����Ă��Ȃ���Ԃ������������x�Đ��J�n
			if (IsEffekseer3DEffectPlaying(*effect.playingHandle) == -1)
			{			
				*effect.playingHandle = PlayEffekseer3DEffect(m_handle[effect.id]);
			}
		}
		// Effekseer�ɂ��Đ����̃G�t�F�N�g���X�V����B
		UpdateEffekseer3D();
	}

	// �d�l���Ȃ��G�t�F�N�g��z�񂩂�폜
	// �f�[�^�����݂���ꍇ
	if (m_effectData.size() != 0)
	{
		for (int i = 0; i < m_effectData.size(); i++)
		{
			// �Đ����I����Ă�ꍇ
			if (IsEffekseer3DEffectPlaying(*m_effectData[i].playingHandle) == -1)
			{
				// �Đ��t���[�����ő吔�𒴂��Ă���ꍇ
				if (m_effectData[i].playFrameCount >= m_effectData[i].playFrameMax)
				{
					m_effectData.erase(m_effectData.begin() + i);
				}
			}
		}
	}
}

void EffekseerDrawer::Draw()
{
	// Effekseer�ɂ��Đ����̃G�t�F�N�g��`�悷��B
	DrawEffekseer3D();
}

void EffekseerDrawer::EffekseerSync()
{
	// DX���C�u�����̃J������Effekseer�̃J�����𓯊�����B
	Effekseer_Sync3DSetting();
}

void EffekseerDrawer::Play(int& playingEffectHandle, Id id, EffectPlayType type, VECTOR pos, VECTOR size, VECTOR angle, int playFrame)
{
	EffectData data{};

	playingEffectHandle = PlayEffekseer3DEffect(m_handle[id]);

	// �ʒu�𒲐�
	SetPosPlayingEffekseer3DEffect(playingEffectHandle, pos.x, pos.y, pos.z);
	// �p�x�𒲐�
	SetRotationPlayingEffekseer3DEffect(playingEffectHandle, angle.x, angle.y, angle.z);

	data.playingHandle = &playingEffectHandle;
	data.id    = id;
	data.type  = type;
	data.pos   = pos;
	data.size  = size;
	data.angle = angle;
	data.playFrameMax   = playFrame;
	data.playFrameCount = 0;

	m_effectData.push_back(data);
}