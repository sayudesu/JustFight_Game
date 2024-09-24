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
		m_data[mapKey].fileName = strvec[1].c_str();
		m_data[mapKey].name = strvec[2].c_str();
		m_data[mapKey].size = stoi(strvec.at(3));
		m_data[mapKey].sizeNo = stoi(strvec.at(4));
		m_data[mapKey].extension = strvec[5].c_str();

		// ファイル位置
		std::string name = kFile + strvec[1] + strvec[5];
		// フォントデータの読み込み
		AddFontResourceEx(name.c_str(), FR_PRIVATE, NULL);

		// mapにキーを決める
		mapKey++;
	}

	if (mapKey != 0)
	{
		for (int i = 0; i < mapKey; i++)
		{
			// フォントハンドルを作成
			m_fontHandle[m_data[i].sizeNo] = CreateFontToHandle(m_data[i].name.c_str(), m_data[i].size, 3, DX_FONTTYPE_NORMAL, DX_CHARSET_DEFAULT);
		}
	}
}

// メモリの解放
void FontManager::Unload()
{
	for (size_t i = 0; i < m_data.size(); i++)
	{
		DeleteFontToHandle(m_fontHandle[i]);
	}
}

// 文字列描画
void FontManager::DrawString(int x, int y, std::string text, int color, FontSize size)
{
	DrawStringToHandle(x, y, text.c_str(), color, m_fontHandle[static_cast<int>(size)]);
}

// 整数描画
void FontManager::DrawFormatString(int x, int y, int text, int color, FontSize size)
{
	DrawFormatStringToHandle(x, y, color, m_fontHandle[static_cast<int>(size)], "%d", text);
}
