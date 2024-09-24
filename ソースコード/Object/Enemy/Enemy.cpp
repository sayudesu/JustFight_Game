#include "Enemy.h"

#include "../../Util/Pad.h"

#include "../../CSVData/CharactorDataManager.h"
#include "../../Util/ParameterData.h"

#include "../../CSVData/ModelManager.h"
#include "../../Util/ModelName.h"

namespace
{
	// 角度情報をフレーム単位で遅らせる
	constexpr int kDelayFrameAngle = 18;
	constexpr int kGuardFrameCountMax = 60 * 2;
}

namespace
{
	// プレイヤーの移動量
	constexpr VECTOR kVecZ{ 0.0f,0.0f,-10.0f };
	constexpr VECTOR kVecX{ -10.0f,0.0f,0.0f };

	// 回避
	constexpr VECTOR kVecAwayZ{ 0.0f,0.0f,-25.0f };
}


Enemy::Enemy(DifficultyData data,VECTOR pos) :
	EnemyBase(data,pos),
	m_isCheckGuard(false),
	m_isResultGuard(false),
	m_isCheckAttack(false),
	m_isAttackResult(false),
	m_isCheckStrongAttack(false),
	m_isStrongAttackResult(false),
	m_isMoveLeft(false),
	m_isMoveRight(false),
	m_awayVec(VGet(0,0,0)),
	m_awayRelativePos(VGet(0,0,0)),
	m_guardFrameCount(0),
	m_targetGuard(false),
	m_enemyRotMtx({})
{
	// 初期待機状態で停止
	m_pFunc = &Enemy::Idle;

	m_isAttack = false;
	m_isGuard = false;

	// 自身がエネミーであると決める
	m_myId = CharacterName::ENEMY;

	// 選択した難易度によってモデルを変更
	if (data == DifficultyData::NOIVE)
	{		
		m_parameter.fileName = ModelManager::GetInstance().ModelType(ModelName::Pawn_B);
		InputParamType(0);
	}
	else if (data == DifficultyData::INTERMEDIATE)
	{
		m_parameter.fileName = ModelManager::GetInstance().ModelType(ModelName::Knight_B);
		InputParamType(1);
	}
	else if (data == DifficultyData::EXPERT)
	{
		m_parameter.fileName = ModelManager::GetInstance().ModelType(ModelName::Queen_B);
		InputParamType(2);
	}
}

Enemy::~Enemy()
{
}

void Enemy::Input()
{
	DINPUT_JOYSTATE input;
	// 入力状態を取得
	GetJoypadDirectInputState(DX_INPUT_PAD1, &input);

	// 向きを指定
	Direction();

	// スタンをしていなかったら
	if (!IsStun())
	{
		// 一定距離近づくとランダムで左右移動を始める
		if (m_targetRange.x + m_targetRange.z < 1000.0f)
		{
			MoveLeftAndRight(m_enemyRotMtx);

			// 行動範囲を制限する
			FieldLimit();
		}

		// 攻撃可能範囲に入っているかどうか
		if (IsAttackRange())
		{
			// 攻撃or防御していなかったら次の行動を決める
			if (!m_isAttack && !m_isGuard && !m_isStrongAttack)
			{
				// ターゲットの状態で動作を変更する
				BattleType();
			}

			// 攻撃かどうか
			if (m_isAttackResult)
			{
				m_isAttackResult = false;
				m_isAttack = true;
				m_pFunc = &Enemy::Attack;
			}
			// 攻撃をしていない場合
			if (!m_isAttack)
			{
				m_isAttack = false;
			}

			// 強攻撃かどうか
			if (m_isStrongAttackResult && !m_isGuard)
			{
				m_isStrongAttackResult = false;
				m_isCheckStrongAttack = true;
				m_pFunc = &Enemy::StrongAttack;
			}
			// 強攻撃をしていない場合
			if (!m_isCheckStrongAttack)
			{
				m_isCheckStrongAttack = false;
			}

			// ガードをして攻撃をしていない場合
			if (m_isResultGuard && !m_isAttack)
			{
				m_isGuard = true;
				m_guardFrameCount++;
				m_pFunc = &Enemy::Guard;		
			}

			// 指定のフレーム数はガードをし続ける
			if (m_guardFrameCount == kGuardFrameCountMax)
			{
				m_guardFrameCount = 0;
				m_isCheckGuard = false;
				m_isGuard = false;
				m_isResultGuard = false;
			}

			// ジャストガードに成功したら通常ガードを行わない
			if (IsJustGuard())
			{
				m_guardFrameCount = 0;
				m_isCheckGuard = false;
				m_isGuard = false;
				m_isResultGuard = false;
			}
		}
		else
		{
			m_isAttack = false;
			m_isCheckAttack = false;
			m_isAttackResult = false;

			TargetMove();

			// ガードをするかどうかの選択を行う
			BattleTypeGurad();

			// ガードをして攻撃をしていない場合
			if (m_isResultGuard && !m_isAttack)
			{
				m_isGuard = true;
				m_guardFrameCount++;
				m_pFunc = &Enemy::Guard;
			}
			else
			{
				m_guardFrameCount = 0;
			}

			// 指定のフレーム数はガードをし続ける
			if (m_guardFrameCount == kGuardFrameCountMax)
			{
				m_guardFrameCount = 0;
				m_isCheckGuard = false;
				m_isGuard = false;
				m_isResultGuard = false;
			}

			// ジャストガードに成功したら通常ガードを行わない
			if (IsJustGuard())
			{
				m_guardFrameCount = 0;
				m_isCheckGuard = false;
				m_isGuard = false;
				m_isResultGuard = false;
			}
		}
	}
	else
	{
		m_isGuard = false;
		m_isCheckGuard = false;
		m_isResultGuard = false;

		m_isAttack = false;
		m_isCheckAttack = false;
		m_isAttackResult = false;
	}
}

void Enemy::InputTutorial()
{
	// 向きを指定
	Direction();

	if (!IsStun())
	{
		// 攻撃可能範囲に入っているかどうか
		if (IsAttackRange())
		{
			// 攻撃or防御していなかったら次の行動を決める
			if (!m_isAttack && !m_isGuard && !m_isStrongAttack)
			{
				// ターゲットの状態で動作を変更する
				BattleTypeT();
			}

			// 攻撃かどうか
			if (m_isAttackResult)
			{
				m_isAttackResult = false;
				m_isAttack = true;
				m_pFunc = &Enemy::Attack;
			}
			// 攻撃をしていない場合
			if (!m_isAttack)
			{
				m_isAttack = false;
			}

			// 強攻撃かどうか
			if (m_isStrongAttackResult && !m_isGuard)
			{
				m_isStrongAttackResult = false;
				m_isCheckStrongAttack = true;
				m_pFunc = &Enemy::StrongAttack;
			}
			// 強攻撃をしていない場合
			if (!m_isCheckStrongAttack)
			{
				m_isCheckStrongAttack = false;
			}

			// ガードをして攻撃をしていない場合
			if (m_isResultGuard && !m_isAttack)
			{
				m_isGuard = true;
				m_guardFrameCount++;
				m_pFunc = &Enemy::Guard;
			}
			// 指定のフレーム数はガードをし続ける
			if (m_guardFrameCount == kGuardFrameCountMax)
			{
				m_guardFrameCount = 0;
				m_isCheckGuard = false;
				m_isGuard = false;
				m_isResultGuard = false;
			}
		}
		else
		{
			TargetMove();

			m_isGuard = false;
			m_isCheckGuard = false;
			m_isResultGuard = false;

			m_isAttack = false;
			m_isCheckAttack = false;
			m_isAttackResult = false;
		}
	}
	else
	{
		m_isGuard = false;
		m_isCheckGuard = false;
		m_isResultGuard = false;

		m_isAttack = false;
		m_isCheckAttack = false;
		m_isAttackResult = false;
	}
}

void Enemy::Direction()
{
	// 自身の位置からターゲットの位置の角度を取得する
	const VECTOR direction = VSub(m_targetPos, m_pos);

	const float angle = atan2f(-direction.x, -direction.z);
	
	// 指定のフレーム数分だけ処理ずらす
	m_delayFrameAngle.push_front(angle);
	if (m_delayFrameAngle.size() > kDelayFrameAngle)
	{
		m_delayFrameAngle.pop_back();
	}
	SetAngle(m_delayFrameAngle.back());

	// プレイヤーの進行方向
	m_enemyRotMtx = MGetRotY(m_delayFrameAngle.back());
	SetRotMtx(m_enemyRotMtx);
}

// 行動範囲を制限する
void Enemy::FieldLimit()
{
	if (m_pos.x > 1000.0f + 100.0f)
	{
		m_pos.x = 1000.0f + 100.0f;
	}
	if (m_pos.x < -1000.0f - 100.0f)
	{
		m_pos.x = -1000.0f - 100.0f;
	}
	if (m_pos.z > 1000.0f + 100.0f)
	{
		m_pos.z = 1000.0f + 100.0f;
	}
	if (m_pos.z < -1000.0f - 100.0f)
	{
		m_pos.z = -1000.0f - 100.0f;
	}
}

void Enemy::MoveLeftAndRight(MATRIX mtxRot)
{
	if (GetRand(100) == 0)
	{
		m_isMoveRight = true;
		m_isMoveLeft = false;
	}
	if (GetRand(100) == 1)
	{
		m_isMoveLeft = true;
		m_isMoveRight = false;
	}
	if (GetRand(100) == 2)
	{
		m_isMoveLeft = false;
		m_isMoveRight = false;
	}
	int moveType = 0;
	if (m_isMoveLeft)
	{
		moveType = 1;
	}
	if (m_isMoveRight)
	{
		moveType = -1;
	}

	const VECTOR move = VTransform(VGet(static_cast<float>(moveType) * 10, 0, 0), mtxRot);
	m_pos = VAdd(m_pos, move);
}

void Enemy::BattleType()
{
	// ターゲットの状態で動作を変更する
	// ターゲットが攻撃している場合
	if (m_targetBattleState == BattleState::ATTACK)
	{
		if (GetRand(m_parameter.justGuardRate) == 0)
		{
			m_isCheckGuard = true;
			m_isResultGuard = true;	
		}
	}
	// ターゲットがスタンしている場合
	if (m_targetBattleState == BattleState::STUN)
	{
		// 強攻撃ができる場合はする
		// できない場合は通常攻撃をする
		if (GetStrongPower() == 100)
		{
			m_isCheckStrongAttack = true;
			m_isStrongAttackResult = true;
		}
		else
		{
			m_isCheckAttack = true;
			m_isAttackResult = true;
		}
	}

	// ターゲットが防御している場合
	if (m_targetBattleState == BattleState::GUARD)
	{
		if (GetRand(30) == 0)
		{
			m_isCheckAttack = true;
			m_isAttackResult = true;
		}
	}

	// アイドル状態の場合
	if (m_targetBattleState == BattleState::IDLE)
	{
		m_isCheckAttack = true;
		m_isAttackResult = true;
	}

}

void Enemy::BattleTypeGurad()
{
	if (GetRand(100) == 0)
	{
		m_isCheckGuard = true;
		m_isResultGuard = true;
	}
}

void Enemy::BattleTypeT()
{
	if (m_targetGuard)
	{
		// ターゲットが防御している場合
		if (m_targetBattleState == BattleState::GUARD)
		{
			m_isCheckAttack = true;
			m_isAttackResult = true;

			m_targetGuard = true;
		}
	}
}

