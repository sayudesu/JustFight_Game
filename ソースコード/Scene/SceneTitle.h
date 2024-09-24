#pragma once
#include <memory>

#include "SceneBase.h"

class GameObject;
class Camera;
class SceneTitle final : public SceneBase
{
public:
	SceneTitle();
	virtual ~SceneTitle();

	virtual void Init() override;
	virtual void End() override;

	virtual SceneBase* Update() override;
	virtual void Draw() override;
private:
	// 画像ハンドル
	int m_hTitle;
	int m_hBg;

	// アルファ値
	float m_blendAlpha;

	std::unique_ptr<GameObject> m_pPlayer;
	std::unique_ptr<GameObject> m_pEnemy;

	std::unique_ptr<Camera> m_pCamera;
};

