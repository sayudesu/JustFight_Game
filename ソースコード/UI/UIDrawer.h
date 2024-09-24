#pragma once
#include <vector>
#include <memory>

#include "../Util/CharacterParameter.h";
#include "../Util/CharacterName.h";

class GameObject;
class UIDrawer
{
public:
	UIDrawer();
	~UIDrawer();

	void Update();
	void Draw();
public:
	/// <summary>
	/// パラメーターを受け取ります
	/// </summary>
	/// <param name="name">名前を指定</param>
	/// <param name="param">名前で指定したキャラのパラメーター</param>
	void SetParam(CharacterName name, int hpNum, int hpMax, float skillNum, float skillMax, int fightMeterNum,bool damageEffect, bool isHit);
private:
	enum class HandleType
	{
		BG,// 背景

		CHARACTOR,// キャラクターの画像

		HP,   // 体力
		HP_BG,// 体力の背景

		SP,   // スキル
		SP_BG,// スキルの背景

		BAR_OUTSIDE,// 枠
		BAR_OUTSIDE2,// 枠

		FIGHT_POWER,       // 体幹ゲージ
		FIGHT_POWER_CENTER,// 体幹ゲージの中心
		FIGHT_POWER_BG,    // 体幹背景
		FIGHT_OUTSIDE,     // 体感ゲージ枠

		MAX,// 最大数
	};

	// 画像用ハンドル
	int m_handle[static_cast<int>(CharacterName::MAX)][static_cast<int>(HandleType::MAX)];

	// キャラクターごとのパラメーター
	int m_hpNum[static_cast<int>(CharacterName::MAX)];
	int m_hpMax[static_cast<int>(CharacterName::MAX)];
	int m_skillNum[static_cast<int>(CharacterName::MAX)];
	int m_skillMax[static_cast<int>(CharacterName::MAX)];
	int m_fightMeterNum[static_cast<int>(CharacterName::MAX)];

	// ダメージエフェクトがあるかどうか
	bool m_damageEffect;

	std::unique_ptr<GameObject> m_image[static_cast<int>(CharacterName::MAX)][static_cast<int>(HandleType::MAX)];

	VECTOR m_playerPos;
	VECTOR m_enemyPos;

	// ダメージエフェクト
	int m_hDamageEffect[5];

	// ダメージエフェクトを描画するフレームを管理
	int m_damageEffectFrame;

	int m_damageCount;

	bool m_isHit;

};

