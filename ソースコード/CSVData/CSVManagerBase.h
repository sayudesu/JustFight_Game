#pragma once

#include <fstream>
#include <vector>
#include <string>

class CSVManagerBase
{
public:
	CSVManagerBase() {};
	virtual ~CSVManagerBase() {};
	
	// CSV�t�@�C���������ꍇ�͍쐬�A����ꍇ�͓ǂݍ���
	virtual std::ifstream CreateOrOpen(const char* fileNamePath, std::vector<std::vector<std::string>> data);

	// CSV�t�@�C���Ƀf�[�^���������ފ֐�
	virtual void WriteCSV(const std::string& filename, const std::vector<std::vector<std::string>>& data);

	// CSV�f�[�^�̒��g��z��ŕ�����
	std::vector<std::string> Split(std::string& input, char delimiter);
};

