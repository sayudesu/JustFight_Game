#pragma once
#include <vector>
#include <string>
#include <map>

#include "CSVManagerBase.h"

#include "../Util/SoundName.h"
#include "../Util/ModelName.h"

// サウンドマネージャーのシングルトーンクラス

class ModelManager final : public CSVManagerBase
{
private:
	ModelManager() = default;
	static ModelManager* m_pInstance;
public:
	virtual~ModelManager() = default;
	// コピーコンストラクタの禁止
	ModelManager(const ModelManager&) = delete;
	ModelManager& operator=(const ModelManager&) = delete;
	ModelManager(ModelManager&&) = delete;
	ModelManager& operator=(ModelManager&&) = delete;

	// インスタンスの作成
	static ModelManager& GetInstance()
	{
		if (!m_pInstance)
		{
			m_pInstance = new ModelManager();
		}
		return *m_pInstance;
	}

	// 解放処理
	static void Destroy()
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}

public:
	// ロード
	void Load(const char* fileNamePath, std::vector<std::vector<std::string>> data);
	// アンロード
	void Unload();
public:
	std::string ModelType(ModelName type);
private:
	// サウンド保存形式データ
	struct ModelData
	{
		int no;		
		std::string name;		
		std::string extension;
	};

	// サウンドのデータを格納
	std::map<int, ModelData> m_data;
	// ハンドル
	std::vector<int> m_handle;
	std::vector<std::string> m_filePath;
};

