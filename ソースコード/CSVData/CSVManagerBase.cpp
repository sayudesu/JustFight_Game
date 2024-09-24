#include <DxLib.h>
#include <sstream>
#include <windows.h>
#include <iostream>
#include <vector>

#include "CSVManagerBase.h"

std::ifstream CSVManagerBase::CreateOrOpen(const char* fileNamePath, std::vector<std::vector<std::string>> data)
{
	// �t�H���_���쐬����f�B���N�g���̃p�X
	const wchar_t* folderPath = L"Data/CSV";

	// �t�H���_���쐬
	if (CreateDirectoryW(folderPath, NULL) || ERROR_ALREADY_EXISTS == GetLastError())
	{
		// �t�H���_�̍쐬�Ɏ��s�����ꍇ�̏���		
	}

	// �t�@�C�����J��
	std::ifstream ifs(fileNamePath);

	// �t�@�C�������݂��Ȃ��ꍇ�͍쐬����
	if (!ifs.is_open())
	{
		std::ofstream ofs(fileNamePath);

		// �t�@�C���𑦍��ɕ���
		ofs.close();

		// �쐬�����t�@�C�����ēx�J��
		ifs.open(fileNamePath);

		// CSV�t�@�C���Ƀf�[�^����������
		WriteCSV(fileNamePath, data);
	}

	return ifs;
}

void CSVManagerBase::WriteCSV(const std::string& filename, const std::vector<std::vector<std::string>>& data)
{
	std::ofstream ofs(filename);

	// �t�@�C�����������J���ꂽ���m�F
	if (!ofs.is_open())
	{
		return;
	}

	// �f�[�^��CSV�`���Ńt�@�C���ɏ�������
	for (const auto& row : data)
	{
		for (size_t i = 0; i < row.size(); ++i)
		{
			ofs << row[i];
			if (i != row.size() - 1)
			{
				// �J���}�ŃZ������؂�
				ofs << ",";
			}
		}

		// ���s�ōs����؂�
		ofs << "\n";
	}

	// �t�@�C�������
	ofs.close();
}

std::vector<std::string> CSVManagerBase::Split(std::string& input, char delimiter)
{
	// ���͕�������X�g���[���ɕϊ�
	std::istringstream stream(input);

	// �������ꂽ�e�t�B�[���h���i�[���邽�߂̕ϐ�
	std::string field;

	// �������ꂽ��������i�[����x�N�^�[
	std::vector<std::string> result;

	// �X�g���[������1���t�B�[���h�����o���A�x�N�^�[�ɒǉ�
	while (getline(stream, field, delimiter))
	{
		result.push_back(field);
	}

	// �������ꂽ������̃x�N�^�[��Ԃ�
	return result;
}
