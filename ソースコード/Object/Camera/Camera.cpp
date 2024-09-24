#include <math.h>

#include "Camera.h"

#include "../../Util/Pad.h"

namespace
{
	// 視野角
	constexpr float kFov = 80.0f * DX_PI_F / 180.0f;
	// Near
	constexpr float kNear = 1.0f;
	// Far
	constexpr float kFar = 30000.0f;

	// カメラの初期位置
	constexpr VECTOR kCameraPos{ -50.0f, 500.0f, 350.0f };
	constexpr VECTOR kCameraTarget{ 0.0f, 150.0f, -280.0f };

	// カメラの追跡処理にディレイを入れるフレーム数
	constexpr int kCameraDelayFrame = 6 + 6;
}

Camera::Camera():
	m_pos(VGet(0,300,-1000)),
	m_targetPos(VGet(0,0,0)),
	m_angle(0.0f),
	m_playerAngle(0.0f)
{
}

Camera::~Camera()
{
}

void Camera::Setting()
{
	// どこから、どこまで見えるか
	SetCameraNearFar(kNear, kFar);
	// カメラの位置、どこを見ているかを設定する
	SetCameraPositionAndTarget_UpVecY(VGet(m_pos.x, m_pos.y, m_pos.z ), m_targetPos);
	// FOV
	SetupCamera_Perspective(kFov);
}

void Camera::Update()
{
	// カメラの追跡処理にディレイを入れる
	m_delayFrameTargetPos.push_front(m_targetPos);
	m_delayFrameAngle.push_front(m_playerAngle);

	// 配列の数が指定のフレームを超えると末尾の配列を削除する
	if (m_delayFrameTargetPos.size() > kCameraDelayFrame &&
		m_delayFrameAngle.size()  > kCameraDelayFrame)
	{
		// 配列の削除
		m_delayFrameTargetPos.pop_back();
		m_delayFrameAngle.pop_back();
	}

	m_angle = (m_angle * 0.0f) + (m_delayFrameAngle.back() * 1.0f);
	const MATRIX cameraRotMtx = MGetRotY(m_angle);

	// ジャンプ時は単純にプレイヤーに服従するのではなく
	//プレイヤーの立っていた位置を見るようにする
	VECTOR cameraTrans = m_delayFrameTargetPos.back();
	cameraTrans.y = m_delayFrameTargetPos.back().y * 1.0f;
	const MATRIX playerTransMtx = MGetTranslate(cameraTrans);

	// プレイヤーの回転に合わせてカメラ位置、注視点を回転させる
	// プレイヤーの回転情報と平行移動情報を合成
	const MATRIX cameraMtx = MMult(cameraRotMtx, playerTransMtx);

	// 行列を使った座標変換
	m_pos       = VTransform(kCameraPos, cameraMtx);
	m_targetPos = VTransform(kCameraTarget, cameraMtx);
}

void Camera::SetPos(VECTOR pos)
{
	m_pos = pos;
}
void Camera::SetTargetPos(VECTOR pos)
{
	m_targetPos = pos;
}
void Camera::SetPlayerAngle(float angle)
{
	m_playerAngle = angle;
}
