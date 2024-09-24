#pragma once
#include <memory>
#include <vector>

#include "SceneBase.h"

#include "../Util/CharacterName.h"

#include "../Util/DifficultyData.h"
#include "../Util/GameResultData.h"

namespace
{
	constexpr int kCharacterNum = 2;

	constexpr int kEffectNum = 3;
}

class Camera;
class CharacterBase;
class BloodDrawer;
class FieldDrawer;
class UIDrawer;
class EffectScreen;
class TutorialDrawer;

class SceneMain final : public SceneBase
{
public:
	SceneMain(DifficultyData data);
	virtual ~SceneMain();

	virtual void Init() override;
	virtual void End() override;

	virtual SceneBase* Update() override;
	virtual void Draw() override;
private:
	// シーン遷移
	// ゲームメイン
	SceneBase* UpdateGamePlay();
	// ゲームオーバー
	SceneBase* UpdateGameResult();
private:
	// 操作系
	void InputCharacter();
	void UpdateCharacter();
	// パラメーター調整用
	void CheckParameter(bool isDamage, bool isHit);
	// 勝敗の確認
	void CheckResult();

	// 当たり判定
	// 武器と体の判定
	bool CheckWeaponAndBodyHit(std::shared_ptr<CharacterBase> chara1, std::shared_ptr<CharacterBase> chara2);
	// 武器と盾の判定
	bool CheckWeaponAndShieldHIt(std::shared_ptr<CharacterBase> chara1, std::shared_ptr<CharacterBase> chara2);
	// 武器と体範囲の判定
	bool CheckWeaponAndModelAboutHIt(std::shared_ptr<CharacterBase> chara1, std::shared_ptr<CharacterBase> chara2);
	// 体範囲と体範囲の判定
	bool CheckModelAboutHIt(std::shared_ptr<CharacterBase> chara1, std::shared_ptr<CharacterBase> chara2);
	// 地面のモデルとプレイヤーの判定
	bool CheckCollMap(std::shared_ptr<CharacterBase> character);

	// キャラクターの更新処理
	void UpdateCharacter(std::shared_ptr<CharacterBase> chara1, std::shared_ptr<CharacterBase> chara2,bool isPlayer);
private:
	// メンバー関数ポインタ
	SceneBase* (SceneMain::* m_pUpdateFunc)() = &SceneMain::UpdateGamePlay;
	// カメラクラス
	std::unique_ptr<Camera> m_pCamera;
	// キャラクタークラス
	std::shared_ptr<CharacterBase> m_pCharacter[kCharacterNum];

	// 血描画クラス
	std::vector<BloodDrawer*> m_pBlood;
	// フィールドクラス
	std::unique_ptr<FieldDrawer>m_pField; 
	// UIクラス
	std::unique_ptr<UIDrawer> m_pUi;
	// チュートリアル用クラス
	std::unique_ptr<TutorialDrawer> m_pTutorial;

	// 勝敗はついた場合の画像
	int m_hCheckmate;

	// 勝敗の結果
	GameResultData m_resultData;

	// クリア後カウント
	int m_frameCount;

	// 難易度データ
	DifficultyData m_difficultyData;

	// チェックメイト時に表示する画像用
	float m_checkmateSize;
	float m_checkmateRota;
	// チェックメイト時背景のブレンドアルファ用
	int m_checkmateBgBlendRate;
	float m_checkmatePosY;

	// ブラーをかけるかどうか
	bool m_isBlur;

	// 現在攻撃されている
	bool m_isHit;
	// 攻撃が当たった場合のカウント
	int m_hitFrameCount;
	// ヒットストップするフレーム
	int m_hitStopFrame;
};