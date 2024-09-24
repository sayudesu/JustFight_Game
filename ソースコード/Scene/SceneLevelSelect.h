#pragma once
#include <DxLib.h>
#include <memory>

#include "SceneBase.h"

class GameObject;
class SlideSelect;
class GameObject;
class Camera;
class SceneLevelSelect : public SceneBase
{
public:
	SceneLevelSelect();
	virtual ~SceneLevelSelect();

	virtual void Init();
	virtual void End();
	virtual SceneBase* Update();
	virtual void Draw();
private:
	// カメラ移動処理
	void CameraMoveing();
	// モデル移動処理
	void ModelMoveing();
	// 入力処理
	void Input();
	// 選択した背景関連の処理
	void SelectBg();
private:
	int m_hTitle;
	// 背景画像
	std::shared_ptr<GameObject> m_hBg;
	std::shared_ptr<GameObject> m_hSelect;
	std::shared_ptr<GameObject> m_hDecoration;
	// 難易度
	std::shared_ptr<GameObject> m_hNovice;       // 簡単
	std::shared_ptr<GameObject> m_hIntermediate; // 中級
	std::shared_ptr<GameObject> m_hExpert;       // 難しい
	// 敵のモデル
	std::shared_ptr<GameObject> m_hModel[3];
	// 敵の背景
	std::shared_ptr<GameObject> m_hImageDifficultyBg;
	// 選択用矢印
	std::shared_ptr<GameObject> m_hArrow[2];
	// オプションボタンの背景
	std::shared_ptr<GameObject>m_hOptionBack;
	// オプションボタン
	std::shared_ptr<GameObject>m_hOptionBotton;

	std::unique_ptr<Camera> m_pCamera;

	std::unique_ptr<SlideSelect> m_pSelect;

	std::unique_ptr<GameObject>m_pStage;

	// コントローラーを接続してるかどうか
	bool m_isInputController;

	// カメラが指定の位置に停止したかどうかx.y.zそれぞれが
	bool m_isCameraStop[4];

	// 背景画像位置
	VECTOR m_bgPos;

	// 敵の背景画像
	int m_tempSelect;
	bool m_isEnemyBgShake;
	int m_enemyBgshakeCount;
	float m_firstEnemyBgX;
	float m_firstEnemyBgY;
	float m_enemyBgShakeX;
	float m_enemyBgShakeY;

	// 矢印
	float m_arrowSize[2];

	float m_arrowPosX[2];
	float m_arrowPosY[2];

	float m_firstArrowPosX[2];
	float m_firstArrowPosY[2];

	float m_arrowShakeX[2];
	float m_arrowShakeY[2];

	// 敵モデル
	float m_modelRot[3];
	float m_modelSlidePosY;

	// カメラの座標
	float m_cameraPosX;
	float m_cameraPosY;
	float m_cameraPosZ;

	// カメラの移動スピード
	float m_cameraSpeed;

	// 難易度文字の大きさ
	float m_difficulty;

	int m_arrowTimer;

	// カメラ停止用
	enum CameraStopData
	{
		X,
		Y,
		Z,
		ALL,
	};
};

