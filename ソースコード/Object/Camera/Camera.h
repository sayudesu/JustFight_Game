#pragma once

#include <DxLib.h>
#include <deque>

// カメラ管理クラス
class Camera
{
public:
	Camera();
	virtual ~Camera();
	void Setting();

	void Update();
public:
	// カメラ位置を取得する
	void SetPos(VECTOR pos);
	// ターゲット位置を取得する
	void SetTargetPos(VECTOR pos);

	void SetPlayerAngle(float angle);
private:
	// 位置
	VECTOR m_pos;
	// ターゲット位置
	VECTOR m_targetPos;
	// 角度
	float m_angle;
	float m_playerAngle;

	// カメラの位置にディレイを入れるための変数
	std::deque<VECTOR> m_delayFrameTargetPos;
	std::deque<float> m_delayFrameAngle;
};

