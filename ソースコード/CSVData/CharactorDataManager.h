#pragma once
#include <DxLib.h>
#include <map>
#include <vector>
#include <string>
#include <sstream>

#include "CSVManagerBase.h"

#include "../Util/ParameterData.h"


class CharactorDataManager final : public CSVManagerBase
{
private:
	// サウンド保存形式データ
	struct CharactorData
	{
		int no = 0;
		std::string name = {};
		int valueNum = 0;

		std::vector<float> element{};
	};

private:
	CharactorDataManager() = default;
	static CharactorDataManager* m_pInstance;
public:
	virtual~CharactorDataManager() = default;
	// コピーコンストラクタの禁止
	CharactorDataManager(const CharactorDataManager&) = delete;
	CharactorDataManager& operator=(const CharactorDataManager&) = delete;
	CharactorDataManager(CharactorDataManager&&) = delete;
	CharactorDataManager& operator=(CharactorDataManager&&) = delete;

	// インスタンスの作成
	static CharactorDataManager& GetInstance()
	{
		if (!m_pInstance)
		{
			m_pInstance = new CharactorDataManager();
		}
		return *m_pInstance;
	}

	// 解放処理
	static void Destroy()
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}	

	// ロード
	void Load(
		const char* fileNamePath1, std::vector<std::vector<std::string>> data1,
		const char* fileNamePath2, std::vector<std::vector<std::string>> data2,
		const char* fileNamePath3, std::vector<std::vector<std::string>> data3);
	// アンロード
	void Unload();
public:
	// キャラクターのパラメーターを渡す
	CharactorData GetParamData(ParamData id,int type);
private:
	void CSVLoad(std::ifstream& ifs, std::string line, int count, int mapkey, int type);
	// サウンドのデータを格納
	std::map<int, CharactorData> m_data[3]{};
};

