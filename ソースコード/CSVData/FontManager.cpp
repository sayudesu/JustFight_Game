#include <DxLib.h>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <iostream>

#include "FontManager.h"

FontManager* FontManager::m_pInstance = nullptr;

namespace
{
	const std::string kFile = "Data/Font/";
}

void FontManager::Load(const char* fileNamePath, std::vector<std::vector<std::string>> data)
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
		m_data[mapKey].fileName = strvec[1].c_str();
		m_data[mapKey].name = strvec[2].c_str();
		m_data[mapKey].size = stoi(strvec.at(3));
		m_data[mapKey].sizeNo = stoi(strvec.at(4));
		m_data[mapKey].extension = strvec[5].c_str();

		// �t�@�C���ʒu
		std::string name = kFile + strvec[1] + strvec[5];
		// �t�H���g�f�[�^�̓ǂݍ���
		AddFontResourceEx(name.c_str(), FR_PRIVATE, NULL);

		// map�ɃL�[�����߂�
		mapKey++;
	}

	if (mapKey != 0)
	{
		for (int i = 0; i < mapKey; i++)
		{
			// �t�H���g�n���h�����쐬
			m_fontHandle[m_data[i].sizeNo] = CreateFontToHandle(m_data[i].name.c_str(), m_data[i].size, 3, DX_FONTTYPE_NORMAL, DX_CHARSET_DEFAULT);
		}
	}
}

// �������̉��
void FontManager::Unload()
{
	for (size_t i = 0; i < m_data.size(); i++)
	{
		DeleteFontToHandle(m_fontHandle[i]);
	}
}

// ������`��
void FontManager::DrawString(int x, int y, std::string text, int color, FontSize size)
{
	DrawStringToHandle(x, y, text.c_str(), color, m_fontHandle[static_cast<int>(size)]);
}

// �����`��
void FontManager::DrawFormatString(int x, int y, int text, int color, FontSize size)
{
	DrawFormatStringToHandle(x, y, color, m_fontHandle[static_cast<int>(size)], "%d", text);
}
