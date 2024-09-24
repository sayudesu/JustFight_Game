#pragma once

#include <vector>
#include <memory>
#include <string>

#include "SceneBase.h"

#include "../Util/GameResultData.h"
#include "../Util/DifficultyData.h"

class GameObject;
class Camera;
class SceneResult final : public SceneBase
{
public:
	SceneResult(GameResultData resultData, DifficultyData data);
	virtual ~SceneResult();

	virtual void Init() override;
	virtual void End() override;

	virtual SceneBase* Update() override;
	virtual void Draw() override;
private:
	// ゲームのリザルトデーターを取得する
	GameResultData m_resultData;

	// 画像の読み込み
	int m_hImageResultBg;
	int m_hImageResult;

	// 画像の角度
	float m_imageAngle;

	// 3Dオブジェクト生成用
	std::unique_ptr<GameObject> m_pPlayer;
	std::unique_ptr<GameObject> m_pEnemy;

	// カメラ生成用
	std::unique_ptr<Camera> m_pCamera;

	// 敵パスの名前
	std::string m_enemyPath;

	// 勝敗結果画像位置調整用
	float m_resultY;
};

