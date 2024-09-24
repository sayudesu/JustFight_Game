#include <DxLib.h>
#include <cassert>

#include "CharactorDataManager.h"

CharactorDataManager* CharactorDataManager::m_pInstance = nullptr;

void CharactorDataManager::Load(
	const char* fileNamePath1, std::vector<std::vector<std::string>> data1,
	const char* fileNamePath2, std::vector<std::vector<std::string>> data2,
	const char* fileNamePath3, std::vector<std::vector<std::string>> data3)
{
	// ファイルの確認
	std::ifstream ifs1 = CreateOrOpen(fileNamePath1, data1);
	std::ifstream ifs2 = CreateOrOpen(fileNamePath2, data2);
	std::ifstream ifs3 = CreateOrOpen(fileNamePath3, data3);

	// ファイルから1文字ずつ読み込む
	std::string line1;
	std::string line2;
	std::string line3;

	// ヘッダー部分をカウントする用
	int count1 = 0;
	int count2 = 0;
	int count3 = 0;

	// mapの為のKey
	int mapKey1 = 0;
	int mapKey2 = 0;
	int mapKey3 = 0;

	// 読み込み
	CSVLoad(ifs1, line1, count1, mapKey1, 0);
	CSVLoad(ifs2, line2, count2, mapKey2, 1);
	CSVLoad(ifs3, line3, count3, mapKey3, 2);
}

void CharactorDataManager::Unload()
{
	printfDx("End");
}

CharactorDataManager::CharactorData CharactorDataManager::GetParamData(ParamData id, int type)
{
	return m_data[type][id];
}

void CharactorDataManager::CSVLoad(std::ifstream& ifs,std::string line,int count,int mapkey, int type)
{
	while (getline(ifs, line))
	{
		// ヘッダー部分は読み込まない
		count++;

		if (count < 2) continue;

		// 読み込んだ行をカンマで分割する
		std::vector<std::string> strvec = Split(line, ',');

		//// データ保管
		m_data[type][mapkey].no = std::stoi(strvec.at(0));
		m_data[type][mapkey].name = strvec.at(1);

		// データを読み込む
		m_data[type][mapkey].valueNum = std::stoi(strvec[2].c_str());  // 名前

		// 要素数によって配列を追加する
		if (m_data[type][mapkey].valueNum == 1)
		{
			m_data[type][mapkey].element.push_back(std::stof(strvec[3].c_str()));
		}
		else if(m_data[type][mapkey].valueNum == 3)
		{
			m_data[type][mapkey].element.push_back(std::stof(strvec[3].c_str()));
			m_data[type][mapkey].element.push_back(std::stof(strvec[4].c_str()));
			m_data[type][mapkey].element.push_back(std::stof(strvec[5].c_str()));
		}
		else
		{
			assert(0 && "ファイルに誤りがあります");
		}
		
		// map用のにキーを決める
		mapkey++;
	}
}
