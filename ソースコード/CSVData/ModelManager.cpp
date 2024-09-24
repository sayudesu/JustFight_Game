#include <DxLib.h>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <iostream>

#include "ModelManager.h"

namespace
{
	std::string kFileNamePath = "Data/Model/";
}

ModelManager* ModelManager::m_pInstance = nullptr;

void ModelManager::Load(const char* fileNamePath, std::vector<std::vector<std::string>> data)
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
		// �f�[�^��ǂݍ���
		m_data[mapKey].name      = strvec[1].c_str();  // ���O
		m_data[mapKey].extension = strvec[2].c_str();  // �g���q

		// �t�@�C���������߂�
		std::string fileName = kFileNamePath + m_data[mapKey].name + m_data[mapKey].extension;
		m_filePath.push_back(fileName);

		// map�p�̂ɃL�[�����߂�
		mapKey++;
	}
}

void ModelManager::Unload()
{

}

std::string ModelManager::ModelType(ModelName type)
{
	return m_filePath[static_cast<int>(type)];
}
