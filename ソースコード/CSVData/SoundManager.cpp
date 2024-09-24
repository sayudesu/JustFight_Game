#include <DxLib.h>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <iostream>

#include "SoundManager.h"

SoundManager* SoundManager::m_pInstance = nullptr;

namespace
{
	const std::string kFile = "Data/Sound/";
}

void SoundManager::Load(const char* fileNamePath, std::vector<std::vector<std::string>> data)
{
	// �t�@�C������1�������ǂݍ���
	std::string line;

	std::ifstream ifs = CreateOrOpen(fileNamePath, data);

	int count = 0;
	int mapKey = 0;
	while (getline(ifs, line))
	{
		// �w�b�_�[�����͓ǂݍ��܂Ȃ�
		count++;

		if (count < 2) continue;

		// �ǂݍ��񂾍s���J���}�ŕ�������
		std::vector<std::string> strvec = Split(line, ',');

		// �f�[�^�ۊ�
		m_data[mapKey].no = stoi(strvec.at(0));

		// �������SE��BGM�𕪂���
		if (strvec.at(1) == "SE")
		{
			m_data[mapKey].type = Type::SE;
		}
		else if(strvec.at(1) == "BGM")
		{
			m_data[mapKey].type = Type::BGM;
		}

		// �f�[�^��ǂݍ���
		m_data[mapKey].name      = strvec[2].c_str();  // ���O
		m_data[mapKey].volume    = stoi(strvec.at(3)); // ����
		m_data[mapKey].extension = strvec[4].c_str();  // �g���q

		// �t�@�C���ʒu
		std::string name = kFile + strvec[2] + strvec[4];

		// �T�E���h�̃������ǂݍ���
		m_handle.push_back(LoadSoundMem(name.c_str()));
		// �T�E���h�̉��ʂ��w��
		ChangeVolumeSoundMem(m_data[mapKey].volume, m_handle.back());

		// map�p�̂ɃL�[�����߂�
		mapKey++;
	}
}

void SoundManager::Unload()
{
	// �ǂݍ��񂾃T�E���h���폜����
	InitSoundMem();
}

// �T�E���h���Đ�
void SoundManager::Play(SoundName name, bool repeat)
{
	// �Đ�����
	if (!repeat)
	{
		PlaySoundMem(m_handle[name], DX_PLAYTYPE_BACK);
	}
	else
	{
		// �Đ����Ă��Ȃ�������
		if (!CheckSoundMem(m_handle[name]))
		{
			PlaySoundMem(m_handle[name], DX_PLAYTYPE_BACK);
		}
	}
}

void SoundManager::Stop(SoundName name)
{
	StopSoundMem(m_handle[name]);
}
