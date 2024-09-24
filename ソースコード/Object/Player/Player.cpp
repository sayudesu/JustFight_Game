#include <corecrt_math.h>

#include "Player.h"

#include "../../Util/Pad.h"
#include "../../Util/SoundName.h"

#include "../../CSVData/SoundManager.h"

namespace
{
	// プレイヤーの移動量
	constexpr VECTOR kVecZ{ 0.0f,0.0f,-10.0f };
	constexpr VECTOR kVecZP{ 0.0f,0.0f,10.0f };
	constexpr VECTOR kVecX{ -10.0f,0.0f,0.0f };
	constexpr VECTOR kVecXP{ 10.0f,0.0f,0.0f };

	// 回避
	constexpr float kVecAway = 60.0f;

	// 回避が次可能になるまでのフレーム
	constexpr int kAwayDelayFrame = 10;

	// 移動速度
	constexpr float kMoveSpeed = 10.0f;

	// モデルのパス
	const char* const kModelPath = "Data/Model/Knight_W.mv1";

	// スティックでカメラを操作する場合の角度
	constexpr int kStickRightRata = 30;
	// スティックでカメラを操作する場合のカメラ移動速度
	constexpr float kStickRightCameraSpeed = 0.05f;

	// プレイヤーのパラメーター調整
	constexpr float kattackFrameMax = 10;
	constexpr float kattackFrameGapMax = 20;
	constexpr float kattackTotalFrame = kattackFrameMax + kattackFrameGapMax;
	constexpr float kattackAfterStopFrameMax = 60;
	constexpr float kstrongAttackFrameMax = 60;
	constexpr float kstrongAttackFrameGapMax = 25;
	constexpr float kStrongAttackTotalFrame = kstrongAttackFrameMax + kstrongAttackFrameGapMax;
	constexpr float kGuardFrameMax = 5;
	constexpr float kJustGuardFrameMax = 10;
	constexpr float kStunFrameMax = 60 * 3;
	constexpr float kHpMax = 6;
	constexpr float kFightingMeterMax = 100;
	const VECTOR kWeaponRelativePos = { -80.0f, 100.0f, 0.0f };
	const VECTOR kShieldRelativePos = { 100.0f, 100.0f, 10.0f };
	constexpr float kWeaponAttackRadius = 100.0f;
	constexpr float kShieldRadius = 50.0f;
	constexpr float kModelRadius = 180.0f;
	const VECTOR kWeaponAttackPos = { 0.0f, 0.0f, -210.0f };
	const VECTOR kKnockBackPos = { 0.0f,0.0f ,-20.0f };
	constexpr float kWeaponBackSpeed = 30.0f;
	constexpr float kShieldBackSpeed = 30.0f;
}

Player::Player(DifficultyData data,VECTOR pos):
	CharacterBase(data,pos),
	m_awayVec(VGet(0,0,0)),
	m_awayRelativePos(VGet(0,0,0)),
	m_isUp(false),
	m_isDown(false),
	m_isLeft(false),
	m_isRight(false),
	m_isCameraLockon(false),
	m_isMove(false),
	m_awayFrameCount(0)
{
	// 初期待機状態で停止
	m_pFunc = &Player::Idle;

	m_isAttack = false;
	m_isGuard  = false;
	m_isJustGuard = false;

	m_isAway = false;

	m_angle = 0.0f;

	// 自身がプレイヤーであると決める
	m_myId = CharacterName::PLAYER;

	for (int i = 0; i < static_cast<int>(Tips::MAX); i++)
	{
		m_isTipsMove[i] = false;
	}
	m_isTipsMove[static_cast<int>(Tips::MOVE)] = true;

	m_parameter.fileName = kModelPath;
	// パラメーター調整
	m_parameter.attackFrameMax          = kattackFrameMax;
	m_parameter.attackFrameGapMax       = kattackFrameGapMax;
	m_parameter.attackTotalFrame        = kattackTotalFrame;
	m_parameter.attackAfterStopFrameMax = kattackAfterStopFrameMax;
	m_parameter.strongAttackFrameMax    = kstrongAttackFrameMax;
	m_parameter.strongAttackFrameGapMax = kstrongAttackFrameGapMax;
	m_parameter.strongAttackTotalFrame  = kStrongAttackTotalFrame;
	m_parameter.guardFrameMax           = kGuardFrameMax;
	m_parameter.justGuardFrameMax       = kJustGuardFrameMax;
	m_parameter.stunFrameMax            = kStunFrameMax;
	m_parameter.hpMax                   = kHpMax;
	m_parameter.fightingMeterMax        = kFightingMeterMax;
	m_parameter.weaponRelativePos       = kWeaponRelativePos;
	m_parameter.shieldRelativePos       = kShieldRelativePos;
	m_parameter.weaponAttackRadius      = kWeaponAttackRadius;
	m_parameter.shieldRadius            = kShieldRadius;
	m_parameter.modelRadius             = kModelRadius;
	m_parameter.weaponAttackPos         = kWeaponAttackPos;
	m_parameter.knockBackPos            = kKnockBackPos;
	m_parameter.weaponBackSpeed         = kWeaponBackSpeed;
	m_parameter.shieldBackSpeed         = kShieldBackSpeed;
}

Player::~Player()
{
}

void Player::Input()
{
	// 入力状態を取得
	GetJoypadDirectInputState(DX_INPUT_PAD1, &m_padInput);

	// 向きを指定
	Direction();

	// スタンしていなかったら
	if (!IsStun())
	{
		// 移動
		InputMove();
		// 攻撃
		InputAttack();
		// 防御
		InputGuard();
	}

#if _DEBUG
	{
		// カメラの操作変更
		static int frameCount = 0;
		if (m_padInput.Buttons[9] == 128)
		{
			frameCount++;
			if (frameCount == 1)
			{
				m_isCameraLockon = (!m_isCameraLockon);
			}
		}
		else
		{
			frameCount = 0;
		}
	}
#endif
}

void Player::InputTutorial()
{
	// 向きを指定
	Direction();

	// 動いた場合攻撃を有効にする
	if (m_isMove)
	{
		m_isTipsMove[static_cast<int>(Tips::ATTACK)] = true;
	}

	// 動けるかどうか
	if (m_isTipsMove[static_cast<int>(Tips::MOVE)])
	{
		InputMove();
	}

	// 攻撃ができるかどうか
	if (m_isTipsMove[static_cast<int>(Tips::ATTACK)])
	{
		InputAttack();
		if (m_isAttack)
		{
			m_isTipsMove[static_cast<int>(Tips::GUARD)] = true;
		}
	}

	// ガードできるかどうか
	if (m_isTipsMove[static_cast<int>(Tips::GUARD)])
	{
		InputGuard();
	}

}

void Player::Direction()
{
	if (m_isCameraLockon)
	{
		// カメラの回転角度を調整
		if (m_padInput.Rx > kStickRightRata)
		{
			m_angle += kStickRightCameraSpeed;
		}
		if (m_padInput.Rx < -kStickRightRata)
		{
			m_angle -= kStickRightCameraSpeed;
		}
	}
	else
	{
		const VECTOR direction = VSub(m_targetPos, m_pos);
		m_angle = atan2f(-direction.x, -direction.z);
	}
	// angleを基底クラスに渡す
	SetAngle(m_angle);
	// プレイヤーの進行方向
	m_platerRotMtx = MGetRotY(m_angle);
	// 回転行列を基底クラスに渡す
	SetRotMtx(m_platerRotMtx);
}

void Player::InputMove()
{
	// 移動or回避
	if (m_isAway)
	{
		// 回避が可能かどうか
		if (m_awayFrameCount < kAwayDelayFrame)
		{
			float t = static_cast<float>(m_awayFrameCount) / kAwayDelayFrame;
			m_awayVec.x = m_awayRelativePos.x * t;
			m_awayVec.z = m_awayRelativePos.z * t;
			m_awayFrameCount++;
			// 移動ベクトルを正規化
			VECTOR moveVector = VTransform(m_awayVec, m_platerRotMtx);  // 例としてX方向に移動する場合
			// キャラクターの位置に速度を加える
			m_pos = VAdd(m_pos, moveVector);
		}
		else
		{
			m_isAway = false;
			m_awayFrameCount = 0;
		}
	}
	else
	{
		m_isUp = false;
		m_isDown = false;
		m_isRight = false;
		m_isLeft = false;

		// 移動操作
		if (Pad::IsPress(PAD_INPUT_UP))
		{
			m_isUp = true;
			MoveCharacter(VTransform(kVecZ, m_platerRotMtx));
			MoveAway(0.0f, -kVecAway);
		}
		else if (Pad::IsPress(PAD_INPUT_DOWN))
		{
			m_isDown = true;
			MoveCharacter(VTransform(kVecZP, m_platerRotMtx));
			MoveAway(0.0f, kVecAway);
		}
		if (Pad::IsPress(PAD_INPUT_RIGHT))
		{
			m_isRight = true;
			MoveCharacter(VTransform(kVecX, m_platerRotMtx));
			MoveAway(-kVecAway, 0.0f);
		}
		else if (Pad::IsPress(PAD_INPUT_LEFT))
		{
			m_isLeft = true;
			MoveCharacter(VTransform(kVecXP, m_platerRotMtx));
			MoveAway(kVecAway, 0.0f);
		}

		if (!(m_isUp) && !(m_isDown) && !(m_isLeft) && !(m_isRight))
		{
			MoveAway(0.0f, kVecAway);
		}
	}
}

void Player::InputAttack()
{
	// 通常攻撃
	if (Pad::IsTrigger(PAD_INPUT_6) &&
		!m_isAttack &&
		!m_isStrongAttack &&
		!m_isGuard &&
		!m_isJustGuard)
	{
		m_isAttack = true;
		m_comboAttack++;
		if (m_comboAttack == 1)
		{
			m_pFunc = &Player::Attack;
		}
	}

	if (m_difficultyData != DifficultyData::NOIVE)
	{
		// 強攻撃
		if (m_padInput.Z < -100 &&
			!m_isAttack &&
			!m_isStrongAttack &&
			!m_isGuard &&
			!m_isJustGuard &&
			(GetStrongPower() >= 100.0f))
		{
			m_isStrongAttack = true;
			m_pFunc = &Player::StrongAttack;
		}
	}
}

void Player::InputGuard()
{
	// 防御
	if (Pad::IsPress(PAD_INPUT_5))
	{
		if (!m_isAttack && !m_isStrongAttack && !m_isJustGuard && !m_isJustGuard)
		{
			m_isGuard = true;
			m_pFunc = &Player::Guard;
		}
	}
	else
	{
		m_isGuard = false;
	}
}
void Player::MoveCharacter(VECTOR moveVector)
{
	if (!m_isMove)
	{
		m_isMove = true;
	}

	// 正規化
	moveVector = VNorm(moveVector);

	// 速度係数
	float speedFactor = kMoveSpeed;
	if (m_isGuard)
	{
		// ガード中は速度を半分に
		speedFactor = speedFactor / 2.0f;
	}

	// 速度ベクトルに速度係数を掛ける
	VECTOR velocity = VScale(moveVector, speedFactor);

	// キャラクターの位置に速度を加える
	m_pos = VAdd(m_pos, velocity);
}

VECTOR Player::AddMoving(const VECTOR RelativePos, const MATRIX rotMtx, const VECTOR pos)
{
	VECTOR move = VTransform(RelativePos, rotMtx);
	move = VAdd(pos, move);
	return move;
}

VECTOR Player::SubMoving(const VECTOR RelativePos, const MATRIX rotMtx, const VECTOR pos)
{
	VECTOR move = VTransform(RelativePos, rotMtx);
	move = VSub(m_pos, move);
	return move;
}

void Player::MoveAway(float x, float z)
{
	// 回避
	if (Pad::IsTrigger(PAD_INPUT_3))
	{
		SoundManager::GetInstance().Play(SoundName::SPEEDMOVE);
		m_isAway = true;
		m_awayRelativePos.x = x;
		m_awayRelativePos.z = z;
		// 速度ベクトルを設定
		m_awayVec.x = 0.0f;
		m_awayVec.y = 0.0f;
		m_awayVec.z = 0.0f;
	}
}
