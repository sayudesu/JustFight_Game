#pragma once
#include <vector>
#include <string>
#include <map>

#include "CSVManagerBase.h"

#include "../Util/SoundName.h"

// サウンドマネージャーのシングルトーンクラス

class SoundManager final : public CSVManagerBase
{
private:
	SoundManager() = default;
	static SoundManager* m_pInstance;
public:
	virtual~SoundManager() = default;
	// コピーコンストラクタの禁止
	SoundManager(const SoundManager&) = delete;
	SoundManager& operator=(const SoundManager&) = delete;
	SoundManager(SoundManager&&) = delete;
	SoundManager& operator=(SoundManager&&) = delete;

	// インスタンスの作成
	static SoundManager& GetInstance()
	{
		if (!m_pInstance)
		{
			m_pInstance = new SoundManager();
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

	/// <summary>
	/// サウンドの再生
	/// </summary>
	/// <param name="name">サウンドの名前</param>
	/// <param name="repeat">繰り返し再生するかどうか</param>
	void Play(SoundName name,bool repeat = false);

	void Stop(SoundName name);

private:
	enum Type
	{
		SE,
		BGM
	};
	// サウンド保存形式データ
	struct SoundData
	{
		int no;
		Type type;
		std::string name;
		int volume;
		std::string extension;
	};

	// サウンドのデータを格納
	std::map<int, SoundData> m_data;
	// ハンドル
	std::vector<int> m_handle;
};

