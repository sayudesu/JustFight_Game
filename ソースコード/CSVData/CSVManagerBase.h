#pragma once

#include <fstream>
#include <vector>
#include <string>

class CSVManagerBase
{
public:
	CSVManagerBase() {};
	virtual ~CSVManagerBase() {};
	
	// CSVファイルが無い場合は作成、ある場合は読み込み
	virtual std::ifstream CreateOrOpen(const char* fileNamePath, std::vector<std::vector<std::string>> data);

	// CSVファイルにデータを書き込む関数
	virtual void WriteCSV(const std::string& filename, const std::vector<std::vector<std::string>>& data);

	// CSVデータの中身を配列で分ける
	std::vector<std::string> Split(std::string& input, char delimiter);
};

