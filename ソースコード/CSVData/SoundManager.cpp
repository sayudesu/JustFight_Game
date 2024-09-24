#include <DxLib.h>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <iostream>

#include "SoundManager.h"

SoundManager* SoundManager::m_pInstance = nullptr;

namespace
{
	const std::string kFile = "Data/Sound/";
}

void SoundManager::Load(const char* fileNamePath, std::vector<std::vector<std::string>> data)
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

		// 文字列でSEかBGMを分ける
		if (strvec.at(1) == "SE")
		{
			m_data[mapKey].type = Type::SE;
		}
		else if(strvec.at(1) == "BGM")
		{
			m_data[mapKey].type = Type::BGM;
		}

		// データを読み込む
		m_data[mapKey].name      = strvec[2].c_str();  // 名前
		m_data[mapKey].volume    = stoi(strvec.at(3)); // 音量
		m_data[mapKey].extension = strvec[4].c_str();  // 拡張子

		// ファイル位置
		std::string name = kFile + strvec[2] + strvec[4];

		// サウンドのメモリ読み込み
		m_handle.push_back(LoadSoundMem(name.c_str()));
		// サウンドの音量を指定
		ChangeVolumeSoundMem(m_data[mapKey].volume, m_handle.back());

		// map用のにキーを決める
		mapKey++;
	}
}

void SoundManager::Unload()
{
	// 読み込んだサウンドを削除処理
	InitSoundMem();
}

// サウンドを再生
void SoundManager::Play(SoundName name, bool repeat)
{
	// 再生する
	if (!repeat)
	{
		PlaySoundMem(m_handle[name], DX_PLAYTYPE_BACK);
	}
	else
	{
		// 再生していなかったら
		if (!CheckSoundMem(m_handle[name]))
		{
			PlaySoundMem(m_handle[name], DX_PLAYTYPE_BACK);
		}
	}
}

void SoundManager::Stop(SoundName name)
{
	StopSoundMem(m_handle[name]);
}
