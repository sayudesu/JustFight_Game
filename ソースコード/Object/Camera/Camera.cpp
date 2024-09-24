#include <math.h>

#include "Camera.h"

#include "../../Util/Pad.h"

namespace
{
	// ����p
	constexpr float kFov = 80.0f * DX_PI_F / 180.0f;
	// Near
	constexpr float kNear = 1.0f;
	// Far
	constexpr float kFar = 30000.0f;

	// �J�����̏����ʒu
	constexpr VECTOR kCameraPos{ -50.0f, 500.0f, 350.0f };
	constexpr VECTOR kCameraTarget{ 0.0f, 150.0f, -280.0f };

	// �J�����̒ǐՏ����Ƀf�B���C������t���[����
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
	// �ǂ�����A�ǂ��܂Ō����邩
	SetCameraNearFar(kNear, kFar);
	// �J�����̈ʒu�A�ǂ������Ă��邩��ݒ肷��
	SetCameraPositionAndTarget_UpVecY(VGet(m_pos.x, m_pos.y, m_pos.z ), m_targetPos);
	// FOV
	SetupCamera_Perspective(kFov);
}

void Camera::Update()
{
	// �J�����̒ǐՏ����Ƀf�B���C������
	m_delayFrameTargetPos.push_front(m_targetPos);
	m_delayFrameAngle.push_front(m_playerAngle);

	// �z��̐����w��̃t���[���𒴂���Ɩ����̔z����폜����
	if (m_delayFrameTargetPos.size() > kCameraDelayFrame &&
		m_delayFrameAngle.size()  > kCameraDelayFrame)
	{
		// �z��̍폜
		m_delayFrameTargetPos.pop_back();
		m_delayFrameAngle.pop_back();
	}

	m_angle = (m_angle * 0.0f) + (m_delayFrameAngle.back() * 1.0f);
	const MATRIX cameraRotMtx = MGetRotY(m_angle);

	// �W�����v���͒P���Ƀv���C���[�ɕ��]����̂ł͂Ȃ�
	//�v���C���[�̗����Ă����ʒu������悤�ɂ���
	VECTOR cameraTrans = m_delayFrameTargetPos.back();
	cameraTrans.y = m_delayFrameTargetPos.back().y * 1.0f;
	const MATRIX playerTransMtx = MGetTranslate(cameraTrans);

	// �v���C���[�̉�]�ɍ��킹�ăJ�����ʒu�A�����_����]������
	// �v���C���[�̉�]���ƕ��s�ړ���������
	const MATRIX cameraMtx = MMult(cameraRotMtx, playerTransMtx);

	// �s����g�������W�ϊ�
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
