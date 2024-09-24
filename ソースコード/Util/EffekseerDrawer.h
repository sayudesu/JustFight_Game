#pragma once
#include <DxLib.h>
#include <array>
#include <map>
#include <vector>

#include "EffectId.h"

class EffekseerDrawer
{
private:
	EffekseerDrawer() = default;
	static EffekseerDrawer* m_pInstance;
public:
	virtual~EffekseerDrawer() = default; 
	// コピーコンストラクタの禁止
	EffekseerDrawer(const EffekseerDrawer&) = delete;
	EffekseerDrawer& operator=(const EffekseerDrawer&) = delete;
	EffekseerDrawer(EffekseerDrawer&&) = delete;
	EffekseerDrawer& operator=(EffekseerDrawer&&) = delete;

	// インスタンスの作成
	static EffekseerDrawer& GetInstance()
	{
		if (!m_pInstance)
		{
			m_pInstance = new EffekseerDrawer;
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
	void Load();
	// アンロード
	void Unload();

	void Update();
	void Draw();

	// DxLibのカメラとの同期を行う
	void EffekseerSync();

	// エフェクシアの再生
	void Play(int& playingEffectHandle,Id id,EffectPlayType type, VECTOR pos,VECTOR size, VECTOR angle,int playFrame = -1);
private:
	// エフェクトのハンドル
	std::array<int, Id::Max> m_handle{};

	// エフェクト再生用ハンドル
	int m_playingEffectHandle = 0;

	// エフェクトのデータを管理する
	std::vector<EffectData> m_effectData{};
};

