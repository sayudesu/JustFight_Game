#pragma once
#include <vector>
#include <string>
#include <map>

#include "../Util/FontSize.h"

#include "CSVManagerBase.h"

class FontManager final : public CSVManagerBase
{
private:
	FontManager() = default;
	static FontManager* m_pInstance;
public:
	virtual~FontManager() = default;
	// コピーコンストラクタの禁止
	FontManager(const FontManager&) = delete;
	FontManager& operator=(const FontManager&) = delete;
	FontManager(FontManager&&) = delete;
	FontManager& operator=(FontManager&&) = delete;

	// インスタンスの作成
	static FontManager& GetInstance()
	{
		if (!m_pInstance)
		{
			m_pInstance = new FontManager();
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
	// 文字列描画
	void DrawString(int x, int y, std::string text, int color, FontSize size);
	// 整数描画
	void DrawFormatString(int x, int y, int text, int color, FontSize size);
private:
	int m_fontHandle[static_cast<int>(FontSize::MAX)]{};

	struct FontData
	{
		int no;
		std::string fileName;
		std::string name;
		int size;
		int sizeNo;
		std::string extension;
	};

	std::map<int, FontData> m_data{};

	std::vector<int> m_handle{};
};

