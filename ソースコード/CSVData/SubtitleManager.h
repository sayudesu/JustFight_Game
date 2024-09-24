#pragma once
#include <DxLib.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <iostream>
#include <map>

#include "CSVManagerBase.h"

#include "../Util/SubtitleData.h"

class SubtitleManager : public CSVManagerBase
{
public:
	struct SubtitleData
	{
		int no;
		std::string sub;
	};
private:
	SubtitleManager() = default;
	static SubtitleManager* m_pInstance;
public:
	virtual~SubtitleManager() = default;
	// �R�s�[�R���X�g���N�^�̋֎~
	SubtitleManager(const SubtitleManager&) = delete;
	SubtitleManager& operator=(const SubtitleManager&) = delete;
	SubtitleManager(SubtitleManager&&) = delete;
	SubtitleManager& operator=(SubtitleManager&&) = delete;

	// �C���X�^���X�̍쐬
	static SubtitleManager& GetInstance()
	{
		if (!m_pInstance)
		{
			m_pInstance = new SubtitleManager();
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
	
	// �w�肵��������n��
	std::string SubtitleStringData(Subtitle data);
private:
	std::map<int, SubtitleData>m_data;
};

