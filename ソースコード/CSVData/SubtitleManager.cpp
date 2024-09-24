#include "SubtitleManager.h"

SubtitleManager* SubtitleManager::m_pInstance = nullptr;

void SubtitleManager::Load(const char* fileNamePath, std::vector<std::vector<std::string>> data)
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
		m_data[mapKey].sub = strvec[1].c_str();

		// map用のにキーを決める
		mapKey++;
	}
}

// 指定した文字を渡す
std::string SubtitleManager::SubtitleStringData(Subtitle data)
{
	return m_data[static_cast<int>(data)].sub;
}
