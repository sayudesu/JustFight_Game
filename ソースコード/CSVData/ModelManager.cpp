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
	// ファイルから1文字ずつ読み込む
	std::string line;

	std::ifstream ifs = CreateOrOpen(fileNamePath, data);

	int count = 0;
	int mapKey = 0;
	while (getline(ifs, line))
	{
		// ヘッダー部分は読み込まない
		count++;

		if (count < 2) continue;

		// 読み込んだ行をカンマで分割する
		std::vector<std::string> strvec = Split(line, ',');

		// データ保管
		m_data[mapKey].no = stoi(strvec.at(0));
		// データを読み込む
		m_data[mapKey].name      = strvec[1].c_str();  // 名前
		m_data[mapKey].extension = strvec[2].c_str();  // 拡張子

		// ファイル名を決める
		std::string fileName = kFileNamePath + m_data[mapKey].name + m_data[mapKey].extension;
		m_filePath.push_back(fileName);

		// map用のにキーを決める
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
