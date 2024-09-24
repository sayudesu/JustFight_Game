#include "SubtitleManager.h"

SubtitleManager* SubtitleManager::m_pInstance = nullptr;

void SubtitleManager::Load(const char* fileNamePath, std::vector<std::vector<std::string>> data)
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
		m_data[mapKey].sub = strvec[1].c_str();

		// map�p�̂ɃL�[�����߂�
		mapKey++;
	}
}

// �w�肵��������n��
std::string SubtitleManager::SubtitleStringData(Subtitle data)
{
	return m_data[static_cast<int>(data)].sub;
}
