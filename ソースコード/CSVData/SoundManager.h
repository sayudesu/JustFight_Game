#pragma once
#include <vector>
#include <string>
#include <map>

#include "CSVManagerBase.h"

#include "../Util/SoundName.h"

// �T�E���h�}�l�[�W���[�̃V���O���g�[���N���X

class SoundManager final : public CSVManagerBase
{
private:
	SoundManager() = default;
	static SoundManager* m_pInstance;
public:
	virtual~SoundManager() = default;
	// �R�s�[�R���X�g���N�^�̋֎~
	SoundManager(const SoundManager&) = delete;
	SoundManager& operator=(const SoundManager&) = delete;
	SoundManager(SoundManager&&) = delete;
	SoundManager& operator=(SoundManager&&) = delete;

	// �C���X�^���X�̍쐬
	static SoundManager& GetInstance()
	{
		if (!m_pInstance)
		{
			m_pInstance = new SoundManager();
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
	// ���[�h
	void Load(const char* fileNamePath, std::vector<std::vector<std::string>> data);
	// �A�����[�h
	void Unload();

	/// <summary>
	/// �T�E���h�̍Đ�
	/// </summary>
	/// <param name="name">�T�E���h�̖��O</param>
	/// <param name="repeat">�J��Ԃ��Đ����邩�ǂ���</param>
	void Play(SoundName name,bool repeat = false);

	void Stop(SoundName name);

private:
	enum Type
	{
		SE,
		BGM
	};
	// �T�E���h�ۑ��`���f�[�^
	struct SoundData
	{
		int no;
		Type type;
		std::string name;
		int volume;
		std::string extension;
	};

	// �T�E���h�̃f�[�^���i�[
	std::map<int, SoundData> m_data;
	// �n���h��
	std::vector<int> m_handle;
};

