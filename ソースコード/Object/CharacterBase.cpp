#include <cmath>

#include "CharacterBase.h"

#include "../Util/EffekseerDrawer.h"

#include "../CSVData/ModelManager.h"
#include "../CSVData/SoundManager.h"
#include "../CSVData/CharactorDataManager.h"

#include "../Util/GameObject.h"
#include "../Util/SoundName.h"
#include "../Util/BlurScreen.h"
#include "../Util/ParameterData.h"
#include "../Util/ModelName.h"
#include "../Util/Easing.h"
#include "../Util/Pad.h"

namespace
{
	// カプセル形状の半径
	float kCapsuleRadius = 64.0f;

	// 重力
	float kGravity = 32.0f;

	constexpr float kStrongAttackPowerMax = 100.0f;

	// 攻撃予兆フレーム時の武器の振動
	constexpr float kShakeRate = 15.0f;

	// モデルの角度
	constexpr float kAngleZ = 90 * DX_PI_F / 180.0f;

	// 追跡速度
	constexpr float kTargetMoveingSpeed = 5.0f;

	// オブジェクトサイズ
	const VECTOR kModelSize = VGet(40.0f, 40.0f, 40.0f);
	const VECTOR kWeaponSize = VGet(3.0f, 3.0f, 3.0f);
	const VECTOR kShieldSize = VGet(3.0f, 3.0f, 3.0f);
}

CharacterBase::CharacterBase(DifficultyData data ,VECTOR pos):
	m_pFunc(nullptr),
	m_effectHandle(-1),
	m_strongAttackPower(0),
	m_rotMtx({}),
	m_targetRotMtx({}),
	m_pos(pos),
	m_targetPos    (VGet(0,0,0)),
	m_vecWeapon    (VGet(0, 0, 0)),
	m_vecShield    (VGet(0, 0, 0)),
	m_tempWeaponPos(VGet(0, 0, 0)),
	m_tempFightingMeter(0),
	m_angle         (0.0f),
	m_isAttack      (false),
	m_isStrongAttack(false),
	m_isGuard       (false),
	m_isJustGuard   (false),
	m_isJustGuardCounter(false),
	m_isAway	    (false),
	m_isResultGuard (false),
	m_isResultDamage(false),
	m_isWeaponAttacksShield(false),
	m_isStun        (false),
	m_isAttackRange (false),
	m_targetHp      (1),
	m_attackFrame   (0),
	m_attackGapFrame(0),
	m_guardFrame    (0),
	m_justGuardFrame(0),
	m_justGuardCounterFrame(0),
	m_stunFrame     (0),
	m_recoilFrame   (0),
	m_guardOneFrame (0),
	m_attackAfterStopFrame(0),
	m_isSceneChange(false),
	m_comboAttack   (0),
	m_difficultyData(data),
	m_isHit(false)
{
	// メンバ関数の初期
	m_pFunc = &CharacterBase::Idle;

	// 自身の判別用IDを初期化
	m_myId = CharacterName::NONE;

	// 現在の行動を記録
	m_battleState       = BattleState::NONE;// 自身
	m_targetBattleState = BattleState::NONE;// ターゲット
	m_tempTargetBattleState = BattleState::NONE;// ターゲット
	
	// 自身のパラーメーター
	m_parameter = {};
}

CharacterBase::~CharacterBase()
{
}

void CharacterBase::Init()
{
	// モデルの相対位置
	m_vecWeapon = m_parameter.weaponRelativePos;
	m_vecShield = m_parameter.shieldRelativePos;

	// キャラごとのステータス情報を取得
	m_hp = m_parameter.hpMax;
	m_vec = m_parameter.knockBackPos;
	m_fightingMeter = m_parameter.fightingMeterMax;

	const VECTOR move = VTransform(m_vecWeapon, m_rotMtx);
	m_weaponPos = VAdd(m_pos, move);

	// オブジェクトの生成
	m_pCharactor = new GameObject(
		m_parameter.fileName.c_str(),
		m_pos,
		VGet(0.0f, m_angle, kAngleZ),
		kModelSize);

	// 武器オブジェクトの生成
	m_pWeapon = new GameObject(
		ModelManager::GetInstance().ModelType(ModelName::WEAPON),
		m_weaponPos,
		VGet(0.0f, m_angle, kAngleZ),
		kWeaponSize);

	// 盾オブジェクトの生成
	m_pShield = new GameObject(
		ModelManager::GetInstance().ModelType(ModelName::SHIELD),
		VGet(100, m_weaponPos.y, 100),
		VGet(0.0f, m_angle, kAngleZ),
		kShieldSize);

	// 位置情報の更新
	UpdatePos();
}

void CharacterBase::End()
{
	// 解放処理
	delete m_pCharactor;
	m_pCharactor = nullptr;
	delete m_pWeapon;
	m_pWeapon = nullptr;
	delete m_pShield;
	m_pShield = nullptr;
}

void CharacterBase::Update()
{
	// メンバ関数ポインタ
	(this->*m_pFunc)();
}

void CharacterBase::Draw()
{
	// キャラクターモデル
	m_pCharactor->Draw();
	// 武器モデル
	m_pWeapon->Draw();
	// 盾モデル
	m_pShield->Draw();
}

void CharacterBase::TargetMove()
{
	// Y軸は変更したくないので始めの位置を記録
	const float tempPosY = m_pos.y;

	// 向きを算出
	VECTOR m_dir = VSub(m_targetPos, m_pos);

	// プレイヤーからエネミーまでの角度を求める
	const float angle = atan2(m_dir.y, m_dir.x);

	// 現在敵が向いている方向のベクトルを生成する
	const MATRIX enemyRotMtx = MGetRotY(angle);

	// 斜めになったとき((1, 1, 0)など)にいったん長さ１に戻す(正規化)
	if (VSquareSize(m_dir) > 0)
	{
		m_dir = VNorm(m_dir);
	}

	// 速度を求める
	const VECTOR velecity = VScale(m_dir, kTargetMoveingSpeed);

	// 位置を変える
	m_pos = VAdd(m_pos, velecity);

	m_pos.y = tempPosY;
}

void CharacterBase::Idle()
{
	// ステータスの更新
	if (m_battleState != BattleState::IDLE)
	{
		m_battleState = BattleState::IDLE;

		m_isHit = false;
	}	

	m_isSceneChange = false;

	m_isJustGuardCounter = false;
	m_isJustGuard = false;
	m_isGuard = false;
	// 
	SetFightingMeter(0.09f);

	// コンボ初期化
	m_comboAttack = 0;

	// 攻撃カウントの初期化
	m_attackFrame = 0;

	// 武器を元の位置に戻す
	WeaponDefaultPos();
	// 盾を元の位置に戻す
	ShieldDefaultPos();

	// 体力がなくなった場合
	if (m_hp <= 0)
	{
		m_vecWeapon.y = 100.0f;
	}

	// 自身がスタンした場合
	if (m_isStun)
	{
		m_pFunc = &CharacterBase::Stun;
	}

	// 武器の角度を変える
	if (m_weaponRotaY > 0.0f)
	{
		m_isAttack = true;
		m_weaponRotaY -= 0.5f;
	}
	else
	{
		m_isAttack = false;
		// 現在の行動を記録
		m_battleState = BattleState::IDLE;
		m_weaponRotaY = 0.0f;
	}

	// 武器のY軸角度を一時保管
	m_tempWeaponRotaY = m_weaponRotaY;

	// HP更新処理
	HitPoint();

	// 位置情報の更新
	UpdatePos();

	// シールドの角度を固定する
	m_shieldRotaY = (90) * DX_PI_F / 180.0f;
}

// 攻撃した場合
void CharacterBase::Attack()
{
	// ステータスの更新
	if (m_battleState != BattleState::ATTACK)
	{
		// 現在の行動を記録
		m_battleState = BattleState::ATTACK;
		// 攻撃サウンドの再生
		SoundManager::GetInstance().Play(SoundName::ATTACK);

		m_isHit = false;
	}

	// 次のコンボ攻撃に切り替える
	if (m_comboAttack == 2)
	{
		m_weaponRotaY = -(90.0f * 3.0f) * DX_PI_F / 180.0f;
		m_tempWeaponRotaY = m_weaponRotaY;
		// 攻撃を開始する
		m_isAttack = true;
		// 硬直状態用フレームをリセット
		m_attackAfterStopFrame = 0;
		// 攻撃予兆フレームをリセット
		m_attackGapFrame = 0;
		// 攻撃フレームをリセット
		m_attackFrame = 0;
		// シーン遷移用
		m_isSceneChange = false;
		// 次の攻撃に移行する
		m_pFunc = &CharacterBase::AttackTwo;

		// 武器の振動のリセット
		m_weaponShakeRate.x = 0.0f;
		m_weaponShakeRate.y = 0.0f;
		m_weaponShakeRate.z = 0.0f;

		return;
	}

	// 武器動かす
	if (!m_isSceneChange)
	{
		// 攻撃予兆フレーム
		if (m_attackGapFrame < m_parameter.attackFrameGapMax)
		{
			// 武器の振動用位置
			m_weaponShakeRate.x = static_cast<float>(GetRand(kShakeRate));
			m_weaponShakeRate.y = static_cast<float>(GetRand(kShakeRate));
			m_weaponShakeRate.z = static_cast<float>(GetRand(kShakeRate));
		}
		else
		{
			// イージングを使用し武器を振る速度を徐々に早くする
			m_weaponRotaY = Easing::InSine(m_attackFrame, m_parameter.attackFrameMax, (90 * 3) * DX_PI / 180.0f,0.0f);
			
			m_vecWeapon.z = MoveByFrame(m_tempWeaponPos.z, -30.0f, m_attackFrame, m_parameter.attackFrameMax);
			m_vecWeapon.x = MoveByFrame(m_parameter.weaponRelativePos.x, 0.0f, m_attackFrame, m_parameter.attackFrameMax);

			// 攻撃時のフレームを乗算
			m_attackFrame++;
		}

		// 攻撃予兆フレームを乗算
		m_attackGapFrame++;
	}

	// 最大フレームに到達したら
	if (m_attackFrame == m_parameter.attackFrameMax)
	{
		m_isSceneChange = true;
	}
	
	// シーンを切り替える事ができるなら
	if (m_isSceneChange)
	{	
		// 攻撃していない
		m_isAttack = false;

		// 攻撃後の硬直状態
		m_attackAfterStopFrame++;

		// 攻撃用フレームが攻撃最大フレームになった場合
		if (m_attackAfterStopFrame == m_parameter.attackAfterStopFrameMax)
		{
			// 硬直状態用フレームをリセット
			m_attackAfterStopFrame = 0;			
			//攻撃予兆フレームのリセット
			m_attackGapFrame = 0;
			// 攻撃フレームをリセット
			m_attackFrame = 0;
			// コンボ終了
			m_comboAttack = 0;
			// シーン遷移用
			m_isSceneChange = false;
			// シーン繊維
			m_pFunc = &CharacterBase::Idle;

			// 武器の振動のリセット
			m_weaponShakeRate.x = 0.0f;
			m_weaponShakeRate.y = 0.0f;
			m_weaponShakeRate.z = 0.0f;
		}
	}

	// スタン状態
	if (m_isStun)
	{
		m_pFunc = &CharacterBase::Stun;
	}

	// HP更新処理
	HitPoint();
	// 位置情報の更新
	UpdatePos(m_weaponShakeRate.x, m_weaponShakeRate.y, m_weaponShakeRate.z);

	m_pCharactor->SetPos(m_pos);
	m_pCharactor->SetRotate(VGet(0.0f, m_angle + ((90) * DX_PI_F / 180.0f), 0.0f));
	m_pCharactor->Update();
}

// 攻撃コンボ2
void CharacterBase::AttackTwo()
{	
	// ステータスの更新
	if (m_battleState != BattleState::ATTACKTWO)
	{
		// 現在の行動を記録
		m_battleState = BattleState::ATTACKTWO;
		// 攻撃サウンドの再生
		SoundManager::GetInstance().Play(SoundName::ATTACK);

		m_isHit = false;
	}

	// 武器動かす
	if (!m_isSceneChange)
	{
		// イージングを使用し武器を振る速度を徐々に早くする
		m_weaponRotaY = Easing::InSine(m_attackFrame, m_parameter.attackFrameMax, -((90 * 5) * DX_PI / 180.0f), m_tempWeaponRotaY);

		m_vecWeapon.z = MoveByFrame(m_tempWeaponPos.z, -30.0f, m_attackFrame, m_parameter.attackFrameMax);
		m_vecWeapon.x = MoveByFrame(m_parameter.weaponRelativePos.x, 0.0f, m_attackFrame, m_parameter.attackFrameMax);		

		// 攻撃時のフレームを乗算
		m_attackFrame++;
	}

	// 最大フレームに到達したら	
	if (m_attackFrame == m_parameter.attackFrameMax)
	{
		m_isSceneChange = true;
	}

	// シーンを切り替える事ができるなら
	if (m_isSceneChange)
	{
		// 攻撃後の硬直状態
		m_attackAfterStopFrame++;
		if (m_attackAfterStopFrame == m_parameter.attackAfterStopFrameMax)
		{
			m_tempWeaponRotaY = 0;
			// 攻撃終了
			m_isAttack = false;
			// 硬直状態用フレームをリセット
			m_attackAfterStopFrame = 0;
			// 攻撃フレームをリセット
			m_attackFrame = 0;
			// コンボ終了
			m_comboAttack = 0;
			// シーン遷移用
			m_isSceneChange = false;
			// シーン繊維
			m_pFunc = &CharacterBase::Idle;
		}
	}

	// スタン状態
	if (m_isStun)
	{
		m_pFunc = &CharacterBase::Stun;
	}

	// 重力後で処理の位置を変えます
	if (m_isGravity)
	{
		m_pos.y -= kGravity;
	}
	m_isGravity = true;

	// 武器
	{
		VECTOR move = VTransform(m_vecWeapon, m_rotMtx);
		m_weaponPos = VAdd(m_pos, move);
		m_pWeapon->SetPos(m_weaponPos);
		m_pWeapon->SetRotate(VGet((90 * 3) * DX_PI / 180.0f, m_angle - m_weaponRotaY, 0));
		m_pWeapon->Update();
	}
	// 盾
	{
		VECTOR move = VTransform(m_vecShield, m_rotMtx);
		move = VAdd(m_pos, move);
		m_pShield->SetPos(move);
		m_pShield->SetRotate(VGet(0, m_angle - m_shieldRotaY, 0));
		m_pShield->Update();
	}

	m_pCharactor->SetPos(m_pos);
	m_pCharactor->SetRotate(VGet(0.0f, m_angle + ((90) * DX_PI_F / 180.0f), 0.0f));
	m_pCharactor->Update();

	m_capsuleUpDown = m_pos;
	m_capsuleUpDown.y = m_pos.y + 50.0f;

	m_capsuleUpPos = m_capsuleUpDown;
	m_capsuleUpPos.y = m_capsuleUpDown.y + 150.0f;

	// HP更新処理
	HitPoint();
	// ノックバックされた場合
	KnockBack();

	m_shieldRotaY = (90) * DX_PI_F / 180.0f;
}

// 強攻撃した場合
void CharacterBase::StrongAttack()
{
	// ステータスの更新
	if (m_battleState != BattleState::STRONGATTACK)
	{
		// 現在の行動を記録
		m_battleState = BattleState::STRONGATTACK;

		m_isHit = false;
	}

	// 武器動かす
	if (!m_isSceneChange)
	{
		if (m_attackGapFrame < m_parameter.strongAttackFrameGapMax)
		{
			m_vecWeapon.x = MoveByFrame(0.0f, 0.0f, m_attackGapFrame, m_parameter.strongAttackFrameGapMax);
			m_vecWeapon.y = MoveByFrame(0.0f, 150.0f, m_attackGapFrame, m_parameter.strongAttackFrameGapMax);
			m_vecWeapon.z = MoveByFrame(0.0f, -100.0f, m_attackGapFrame, m_parameter.strongAttackFrameGapMax);

			// 攻撃時のフレームを乗算
			m_attackGapFrame++;
		}
		if (m_attackGapFrame == m_parameter.strongAttackFrameGapMax)
		{
			m_shieldRotaY += 30.0f * DX_PI_F / 180.0f;
			//m_vecWeapon.y = MoveByFrame(m_parameter.weaponRelativePos.y, 500.0f, m_attackFrame, m_parameter.strongAttackFrameMax);
			m_vecWeapon.z = MoveByFrame(m_parameter.weaponRelativePos.z, -500.0f, m_attackFrame, m_parameter.strongAttackFrameMax);

			// 攻撃時のフレームを乗算
			m_attackFrame++;
		}
	}

	// 最大フレームに到達したら
	if (m_attackFrame == m_parameter.strongAttackFrameMax)
	{
		m_isSceneChange = true;
	}

	// シーンを切り替える事ができるなら
	if (m_isSceneChange)
	{
		// 強攻撃の力をなくす
		SetStrongPowerReset();
		m_shieldRotaY = 0.0f;
		// 攻撃フレームをリセット
		m_attackFrame = 0;
		m_attackGapFrame = 0;
		// シーン遷移用
		m_isSceneChange = false;
		// シーン繊維
		m_pFunc = &CharacterBase::Idle;
	}

	// 重力後で処理の位置を変えます
	if (m_isGravity)
	{
		m_pos.y -= kGravity;
	}
	m_isGravity = true;

	// 武器
	{
		VECTOR move = VTransform(m_vecWeapon, m_rotMtx);
		m_weaponPos = VAdd(m_pos, move);
		m_pWeapon->SetPos(m_weaponPos);
		m_pWeapon->SetRotate(VGet(m_shieldRotaY, m_angle - m_weaponRotaY + (90 * 3) * DX_PI_F / 180.0f, 0.0f));
		m_pWeapon->Update();
	}
	// 盾
	{
		VECTOR move = VTransform(m_vecShield, m_rotMtx);
		move = VAdd(m_pos, move);
		m_pShield->SetPos(move);
		m_pShield->SetRotate(VGet(0, m_angle, 0));
		m_pShield->Update();
	}

	// キャラクターの位置
	m_pCharactor->SetPos(m_pos);
	m_pCharactor->SetRotate(VGet(0.0f, m_angle + ((90) * DX_PI_F / 180.0f), 0.0f));
	m_pCharactor->Update();

	m_capsuleUpDown = m_pos;
	m_capsuleUpDown.y = m_pos.y + 50.0f;

	m_capsuleUpPos = m_capsuleUpDown;
	m_capsuleUpPos.y = m_capsuleUpDown.y + 150.0f;

	// HP更新処理
	HitPoint();
	// ノックバックされた場合
	KnockBack();
}

// ガードした場合
void CharacterBase::Guard()
{	
	m_attackFrame = 0;
	m_vecWeapon.x = -80.0f;
	m_weaponRotaY = 0;

	// ステータスの更新
	if (m_battleState != BattleState::GUARD)
	{
		// 現在の行動を記録
		m_battleState = BattleState::GUARD;
		// ガードサウンドの再生
		SoundManager::GetInstance().Play(SoundName::GUARD);
	}


	// 最大フレーム内に目標地点まで移動する
	if (m_guardFrame < m_parameter.guardFrameMax)
	{
		// イージングを使用し武器を振る速度を徐々に早くする
		m_shieldRotaY = Easing::InQuad(m_guardFrame, m_parameter.guardFrameMax, (90) * DX_PI_F / 180.0f, 0.0f);

		m_vecShield.x = MoveByFrame(m_parameter.shieldRelativePos.x, 0.0f, m_guardFrame, m_parameter.guardFrameMax);
		m_vecShield.z = MoveByFrame(m_parameter.shieldRelativePos.z, -50.0f, m_guardFrame, m_parameter.guardFrameMax);

		// フレームカウント
		m_guardFrame++;

		// ジャストガードのフレーム
		// + 1 はジャストガードの範囲外を示す
		if (m_justGuardFrame < m_parameter.justGuardFrameMax + 1)
		{
			m_justGuardFrame++;
		}
		
	}
	else
	{
		// フレームカウント
		m_guardFrame++;
		m_vecShield.x = 0.0f;

		m_shieldRotaY = (0) * DX_PI_F / 180.0f;
	}

	// ガードをやめた場合
	if (!m_isGuard)
	{		
		m_guardFrame = 0;
		m_justGuardFrame = 0;
		m_pFunc = &CharacterBase::Idle;
	}

	if (m_isJustGuard)
	{
		m_isSceneChange = false;
		m_guardFrame = 0;
		m_pFunc = &CharacterBase::JustGuard;
	}

	if (m_isStun)
	{
		m_pFunc = &CharacterBase::Stun;
	}

	if (m_isWeaponAttacksShield)
	{
		m_guardOneFrame++;		
	}
	else
	{
		m_guardOneFrame = 0;
	}

	// ガード出来た場合のサウンドの再生
	if (m_guardOneFrame == 1)
	{
		SoundManager::GetInstance().Play(SoundName::GUARDRESULT);
	}

	// 現在のHPを調整
	HitPoint();
	// 位置情報の更新
	UpdatePos();
}

// ジャストガードした場合
void CharacterBase::JustGuard()
{	
	// ステータスの更新
	if (m_battleState != BattleState::JUSTGUARD)
	{
		// 現在の行動を記録
		m_battleState = BattleState::JUSTGUARD;
		// ジャストガードサウンドの再生
		SoundManager::GetInstance().Play(SoundName::JUSTGUARD);

		m_isHit = false;
	}

	m_isJustGuardCounter = true;

	// 最大フレーム内に目標地点まで移動する
	if (!m_isSceneChange)
	{
		if (m_justGuardCounterFrame > 10 && m_justGuardCounterFrame < 20)
		{
			m_shieldRotaY = MoveByFrame(0.0f, (static_cast<float>(90) / 2.0f) * DX_PI_F / 180.0f, m_justGuardCounterFrame, 10);
		}
		if (m_justGuardCounterFrame < 15)
		{
			m_vecShield.x = MoveByFrame(m_parameter.shieldRelativePos.x, 100.0f, m_justGuardCounterFrame, 15);
		}
		if (m_justGuardCounterFrame < 15)
		{
			m_vecShield.y = MoveByFrame(m_parameter.shieldRelativePos.y, 150.0f, m_justGuardCounterFrame, 15);
		}
		if (m_justGuardCounterFrame < 15)
		{
			m_vecShield.z = MoveByFrame(m_parameter.shieldRelativePos.z, -50.0f, m_justGuardCounterFrame, 15);
		}
		m_justGuardCounterFrame++;
	}

	// 最大フレームに到達したら
	if (m_justGuardCounterFrame == 30)
	{
		m_isSceneChange = true;
	}

	// シーンを切り替える事ができるなら
	if (m_isSceneChange)
	{
		m_isJustGuard = false;
		m_isJustGuardCounter = false;
		m_justGuardCounterFrame = 0;
		m_vecShield = m_parameter.shieldRelativePos;
		m_shieldRotaY = 0.0f;
		m_isSceneChange = false;	
		m_pFunc = &CharacterBase::Idle;
	}

	// HP更新処理
	HitPoint();
	// 位置情報の更新
	UpdatePos();
}

// スタンした場合
void CharacterBase::Stun()
{
	// スタン状態のフレームをカウント
	m_stunFrame++;

	// ステータスの更新
	if (m_battleState != BattleState::STUN)
	{
		// 現在の行動を記録
		m_battleState = BattleState::STUN;

		// 他フレームのリセット
		m_attackFrame = 0;
		m_attackGapFrame = 0;
		m_guardFrame = 0;
		m_justGuardFrame = 0;
	}

	// スタン状態のサウンド再生
	SoundManager::GetInstance().Play(SoundName::STUN,true);

	// 回転角度を変更する
	m_stunRota += 0.3f;

	if (m_vecWeapon.y < m_parameter.weaponRelativePos.y + 60.0f)
	{
		m_vecWeapon.y += 10.0f;
	}

	if (m_parameter.stunFrameMax < m_stunFrame)
	{
		m_fightingMeter = 30.0f;
		m_stunRota = 0.0f;
		m_stunFrame = 0;
		m_isStun = false;
		m_pFunc = &CharacterBase::Idle;
	}

	// HP更新処理
	HitPoint();
	// 位置情報の更新
	UpdatePos();
}

void CharacterBase::HitPoint()
{
	// 攻撃を受けたら
	if (m_isResultDamage)
	{
		// 攻撃を受けている場合
		m_isHitNow = true;

		// 現在の攻撃と前回の攻撃が違う場合かつ
		// 自身がガードしていない場合
		// ジャストガードが成功していない場合
		// 相手がジャストガードでもない場合
		if (!m_isHitResult &&
			m_battleState != BattleState::GUARD && 
			m_battleState != BattleState::JUSTGUARD&&
			m_targetBattleState != BattleState::STUN)
		{
			// ダメージサウンドの再生
			SoundManager::GetInstance().Play(SoundName::DAMAGE);

			// プレイヤー特有の処理
			if(m_myId == CharacterName::PLAYER)
			{
				// 画面に対してのエフェクト
				EffectScreen::GetInstance().QuakeReplayInit();
			}

			// 体力を減らす
			m_hp--;

			if (m_hp <= 0)
			{
				m_hp = 0;
			}

			// 一時的に最後の攻撃の種類を取得
			m_tempTargetBattleState = m_targetBattleState;
		}

		// 攻撃を無効化
		m_isResultDamage = false;
	}
	else
	{
		m_isHitNow = false;
	}

	if (m_isHitNow)
	{
		m_isHitResult = true;
	}
	else
	{
		m_isHitResult = false;
	}

	// スタン状態にする
	if (m_fightingMeter <= 0.0f)
	{
		m_isStun = true;
	}
}

void CharacterBase::KnockBack()
{
	// ガードが成功したら
	// 後ろにノックバック
	if (m_isResultGuard)
	{
		// ノックバック用のベクトルは相対位置から見てZマイナス方向なので0.0fになるまで足していく
		if (m_vecKnockBack < 0.0f)
		{
			m_vecKnockBack += 1.0f;
			VECTOR move = VTransform(VGet(0, 0, m_vecKnockBack), m_targetRotMtx);
			m_pos = VAdd(m_pos, move);
		}
		else
		{			
			m_isResultGuard = false;
		}
	}
}

void CharacterBase::WeaponDefaultPos()
{
	// 武器を元の位置に戻す
	{
		bool isEndX = false;
		bool isEndZ = false;
		if (m_vecWeapon.x > m_parameter.weaponRelativePos.x)
		{
			m_vecWeapon.x -= 10.0f;
		}
		else
		{
			isEndX = true;
			m_vecWeapon.x = m_parameter.weaponRelativePos.x;
		}

		if (m_vecWeapon.z < m_parameter.weaponRelativePos.z)
		{
			m_vecWeapon.z += 30.0f;
		}
		else
		{
			isEndZ = true;
			m_vecWeapon.z = m_parameter.weaponRelativePos.z;
		}

		if (isEndX && isEndZ)
		{
			m_isAttack = false;
			m_isStrongAttack = false;
		}

		if (m_vecWeapon.y < m_parameter.weaponRelativePos.y)
		{
			m_vecWeapon.y -= 10.0f;
		}
		else
		{
			m_vecWeapon.y = m_parameter.weaponRelativePos.y;
		}
	}
}

void CharacterBase::ShieldDefaultPos()
{
	// 盾を元の位置に戻す
	{
		bool isEndX = false;
		bool isEndY = false;
		if (m_vecShield.x < m_parameter.shieldRelativePos.x)
		{
			m_vecShield.x += 30.0f;
		}
		else
		{
			m_vecShield.x = m_parameter.shieldRelativePos.x;
			isEndX = true;
		}

		if (m_vecShield.y < m_parameter.shieldRelativePos.y)
		{
			m_vecShield.y -= 30.0f;
		}
		else
		{
			m_vecShield.y = m_parameter.shieldRelativePos.y;
			isEndY = true;
		}
		if (isEndX && isEndY)
		{
			m_isGuard = false;
		}
	}
}

void CharacterBase::Gravity()
{
	// 重力後で処理の位置を変えます
	if (m_isGravity)
	{
		m_pos.y -= kGravity;
	}
	m_isGravity = true;

}

void CharacterBase::UpdatePos(int shiftX, int shiftY, int shiftZ)
{
	// 重力
	Gravity();

	// 武器
	{
		VECTOR move = VTransform(m_vecWeapon, m_rotMtx);
		m_weaponPos = VAdd(m_pos, move);
		VECTOR shakePos{ m_weaponPos.x + shiftX ,m_weaponPos.y + shiftY ,m_weaponPos.z + shiftZ };
		m_pWeapon->SetPos(shakePos);
		m_pWeapon->SetRotate(VGet(90 * DX_PI / 180.0f, m_angle - m_weaponRotaY, 0.0f));
		m_pWeapon->Update();
	}
	// 盾
	{
		VECTOR move = VTransform(m_vecShield, m_rotMtx);
		move = VAdd(m_pos, move);
		m_pShield->SetPos(move);
		m_pShield->SetRotate(VGet(0, m_angle - m_shieldRotaY, 0));
		m_pShield->Update();
	}	

	// キャラクターの位置
	m_pCharactor->SetPos(m_pos);
	m_pCharactor->SetRotate(VGet(0.0f, m_stunRota + m_angle + ((90) * DX_PI_F / 180.0f), 0.0f));
	m_pCharactor->Update();

	m_capsuleUpDown = m_pos;
	m_capsuleUpDown.y = m_pos.y + 50.0f;

	m_capsuleUpPos = m_capsuleUpDown;
	m_capsuleUpPos.y = m_capsuleUpDown.y + 150.0f;

	// ノックバックされた場合
	KnockBack();
}

float CharacterBase::MoveByFrame(const float relativePos, const float EndPos, const int nowFrame, const int maxFrame)
{
	// 位置 = 相対位置 + (向かいたい座標 - 相対位置) * (現在のフレーム / 最大フレーム)　　
	const float resultPos = relativePos + (EndPos - relativePos) * (static_cast<float>(nowFrame) / static_cast<float>(maxFrame));
	return resultPos;
}

void CharacterBase::InputParamType(int type)
{
	// パラメーター調整
	m_parameter.attackFrameMax = CharactorDataManager::GetInstance().GetParamData(ParamData::ATTACK_FRAME_MAX, type).element[0];
	m_parameter.attackFrameGapMax = CharactorDataManager::GetInstance().GetParamData(ParamData::ATTACK_FRAME_GAP_MAX, type).element[0];
	m_parameter.attackTotalFrame = CharactorDataManager::GetInstance().GetParamData(ParamData::ATTACK_TOTAL_FRAME, type).element[0];

	m_parameter.attackAfterStopFrameMax = CharactorDataManager::GetInstance().GetParamData(ParamData::ATTACK_AFTER_STOP_FRAME_MAX, type).element[0];

	m_parameter.strongAttackFrameMax = CharactorDataManager::GetInstance().GetParamData(ParamData::STRONG_ATTACK_FRAME_MAX, type).element[0];
	m_parameter.strongAttackFrameGapMax = CharactorDataManager::GetInstance().GetParamData(ParamData::STRONG_ATTACK_FRAME_GAP_MAX, type).element[0];
	m_parameter.strongAttackTotalFrame = CharactorDataManager::GetInstance().GetParamData(ParamData::STRONG_ATTACK_TOTAL_FRAME, type).element[0];

	m_parameter.guardFrameMax = CharactorDataManager::GetInstance().GetParamData(ParamData::GUARD_FRAME_MAX, type).element[0];
	m_parameter.justGuardFrameMax = CharactorDataManager::GetInstance().GetParamData(ParamData::JUST_GUARD_FRAME_MAX, type).element[0];

	m_parameter.stunFrameMax = CharactorDataManager::GetInstance().GetParamData(ParamData::STUN_FRAME_MAX, type).element[0];

	m_parameter.hpMax = CharactorDataManager::GetInstance().GetParamData(ParamData::HP_MAX, type).element[0];
	m_parameter.fightingMeterMax = CharactorDataManager::GetInstance().GetParamData(ParamData::FIGHTING_METER_MAX, type).element[0];

	m_parameter.weaponRelativePos =
	{
		CharactorDataManager::GetInstance().GetParamData(ParamData::WEAPON_RELATIVE_POS, type).element[0],
		CharactorDataManager::GetInstance().GetParamData(ParamData::WEAPON_RELATIVE_POS, type).element[1],
		CharactorDataManager::GetInstance().GetParamData(ParamData::WEAPON_RELATIVE_POS, type).element[2]
	};
	m_parameter.shieldRelativePos =
	{
		CharactorDataManager::GetInstance().GetParamData(ParamData::SHIELD_RELATIVE_POS, type).element[0],
		CharactorDataManager::GetInstance().GetParamData(ParamData::SHIELD_RELATIVE_POS, type).element[1],
		CharactorDataManager::GetInstance().GetParamData(ParamData::SHIELD_RELATIVE_POS, type).element[2]
	};

	m_parameter.weaponAttackRadius = CharactorDataManager::GetInstance().GetParamData(ParamData::WEAPON_ATTACK_RADIUS, type).element[0];
	m_parameter.shieldRadius = CharactorDataManager::GetInstance().GetParamData(ParamData::SHIELD_RADIUS, type).element[0];
	m_parameter.modelRadius = CharactorDataManager::GetInstance().GetParamData(ParamData::MODEL_RADIUS, type).element[0];

	m_parameter.weaponAttackPos =
	{
		CharactorDataManager::GetInstance().GetParamData(ParamData::WEAPON_ATTACK_POS, type).element[0],
		CharactorDataManager::GetInstance().GetParamData(ParamData::WEAPON_ATTACK_POS, type).element[1],
		CharactorDataManager::GetInstance().GetParamData(ParamData::WEAPON_ATTACK_POS, type).element[2]
	};
	m_parameter.knockBackPos =
	{
		CharactorDataManager::GetInstance().GetParamData(ParamData::KNOCKBACK_POS, type).element[0],
		CharactorDataManager::GetInstance().GetParamData(ParamData::KNOCKBACK_POS, type).element[1],
		CharactorDataManager::GetInstance().GetParamData(ParamData::KNOCKBACK_POS, type).element[2]
	};

	m_parameter.weaponBackSpeed = CharactorDataManager::GetInstance().GetParamData(ParamData::WEAPON_BACK_SPEED, type).element[0];
	m_parameter.shieldBackSpeed = CharactorDataManager::GetInstance().GetParamData(ParamData::SHIELD_BACK_SPEED, type).element[0];

	m_parameter.justGuardRate = CharactorDataManager::GetInstance().GetParamData(ParamData::JUSTGUARDRATE, type).element[0];
}

void CharacterBase::SetAngle(float angle)
{
	m_angle = angle;
}

void CharacterBase::SetRotMtx(MATRIX rotMtx)
{
	m_rotMtx = rotMtx;
}


CharacterName CharacterBase::GetMyId()
{
	return m_myId;
}

BattleState CharacterBase::GetBattleState()
{
	return m_battleState;
}

VECTOR CharacterBase::GetPos()const
{
	return m_pos;
}

VECTOR CharacterBase::GetCollPos() const
{
	return VGet(m_pos.x, m_pos.y + 100.0f, m_pos.z);
}

VECTOR CharacterBase::GetCollWeaponPos() const
{
	return m_pWeapon->GetCollPos(3);
}

VECTOR CharacterBase::GetShieldPos() const
{
	VECTOR move = VTransform(m_vecShield, m_rotMtx);
	move = VAdd(m_pos, move);
	return move;
}

VECTOR CharacterBase::GetCapsulePosDown()
{
	return m_capsuleUpDown;
}

VECTOR CharacterBase::GetCapsulePosUp()
{	
	return m_capsuleUpPos;
}

float CharacterBase::GetCapsuleRadius()
{
	return kCapsuleRadius;
}

MATRIX CharacterBase::GetRot()const
{
	return m_rotMtx;
}

float CharacterBase::GetAngle() const
{
	return m_angle;
}

float CharacterBase::GetModelRadius() const
{
	return m_parameter.modelRadius ;
}

float CharacterBase::GetWeaponAttackRadius() const
{
	return m_parameter.weaponAttackRadius;
}

VECTOR CharacterBase::GetWeaponAttackRelative() const
{
	return m_parameter.weaponAttackPos;
}

float CharacterBase::GetShieldRadius() const
{
	return m_parameter.shieldRadius;
}

VECTOR CharacterBase::GetShieldRelative() const
{
	return m_parameter.shieldRelativePos;
}

void CharacterBase::SetTargetPos(VECTOR pos)
{
	m_targetPos = pos;
}

void CharacterBase::SetTargetHp(const int hp)
{
	m_targetHp = hp;
}

void CharacterBase::SetCollGuardEffect()
{
	// ガード際のエフェクト再生
	EffekseerDrawer::GetInstance().Play(
		m_effectHandle, Id::Guard,
		EffectPlayType::NORMAL,
		GetShieldPos(),
		VGet(1, 1, 1),
		VGet(0, m_angle, 0));
}

void CharacterBase::SetCollJustGuardEffect()
{
	// ジャストガードした際のエフェクト再生
	EffekseerDrawer::GetInstance().Play(
		m_effectHandle, Id::JustGuard,
		EffectPlayType::NORMAL,
		VGet(GetShieldPos().x, GetShieldPos().y + 100.0f, GetShieldPos().z),
		VGet(1, 1, 1),
		VGet(0, 0, 0));
}

void CharacterBase::SetStrongPower(int power)
{
	m_strongAttackPower += power;
	if (m_strongAttackPower >= 100)
	{
		m_strongAttackPower = 100;
	}
}

void CharacterBase::IsHit()
{
	m_isHit = true;
}

void CharacterBase::SetStrongPowerReset()
{
	m_strongAttackPower = 0;
}

int CharacterBase::GetAttackFrame()const
{
	const int totalFrame = m_attackFrame + m_attackGapFrame;
	return totalFrame;
}

int CharacterBase::GetGuardFrame()const
{
	return m_guardFrame;
}

int CharacterBase::GetJustGuardFrame()const
{
	return m_justGuardFrame;
}

int CharacterBase::GetAttackFrameMax()const
{
	if (m_isAttack)
	{
		return m_parameter.attackTotalFrame;
	}
	else if (m_isStrongAttack)
	{
		return m_parameter.strongAttackTotalFrame;
	}

	return -1;
}

int CharacterBase::GetGuardFrameMax()const
{
	return m_parameter.guardFrameMax ;
}

int CharacterBase::GetJustGuardFrameMax()const
{
	return m_parameter.justGuardFrameMax;
}

int CharacterBase::GetStunFrameMax() const
{
	return m_parameter.stunFrameMax;
}

int CharacterBase::GetHp()const
{
	return m_hp;
}

int CharacterBase::GetMaxHp() const
{
	return m_parameter.hpMax;
}

float CharacterBase::GetFightingMeter()const
{
	return m_fightingMeter;
}

bool CharacterBase::IsAway() const
{
	return m_isAway;
}

bool CharacterBase::IsHitDamage() const
{
	return m_isHitNow;
}

bool CharacterBase::IsGuard() const
{
	return m_isGuard;
}

float CharacterBase::GetStrongPower()
{
	return m_strongAttackPower;
}

float CharacterBase::GetkStrongAttackPowerMax()
{
	return kStrongAttackPowerMax;
}

bool CharacterBase::IsAttack() const
{
	return m_isHit;
}

bool CharacterBase::GetTipsMove(Tips tips)
{
	return m_isTipsMove[static_cast<int>(tips)];
}

bool CharacterBase::GetCheckHitWall()
{
	return m_isHItWall;
}

bool CharacterBase::IsJustGuard() const
{
	return m_isJustGuard;
}

bool CharacterBase::IsStun() const
{
	return m_isStun;
}

bool CharacterBase::IsAttackRange() const
{
	return m_isAttackRange;
}

void CharacterBase::SetFieldHit()
{
	m_isGravity = false;	
}

void CharacterBase::SetWeaponAttacksShield(const bool isShieldHit)
{
	m_isWeaponAttacksShield = isShieldHit;
}

void CharacterBase::SetBattleState(BattleState state)
{	
	m_targetBattleState = state;
}

void CharacterBase::SetTargetMtxRota(MATRIX rot)
{
	m_targetRotMtx = rot;
}

void CharacterBase::SetAttackRange(const bool isRange)
{
	m_isAttackRange = isRange;
}

void CharacterBase::SetDamage(bool isDamage)
{
	m_isResultDamage = isDamage;
}

void CharacterBase::SetGuardKnockBack(bool isGuard,float vec)
{
	m_isResultGuard = isGuard;
	m_vecKnockBack = vec;
}

void CharacterBase::SetJustGuard(bool isJustGuard)
{
	m_isJustGuard = isJustGuard;
}

void CharacterBase::SetFightingMeter(const float fightingMeter)
{
	m_tempFightingMeter = fightingMeter;
#if true
	FightingMeter();
#endif
}

void CharacterBase::FightingMeter()
{
	m_fightingMeter += m_tempFightingMeter;

	// 戦いに必要なメーターが最大値になると最大値で固定
	if (m_fightingMeter > m_parameter.fightingMeterMax)
	{
		m_fightingMeter = m_parameter.fightingMeterMax;
	}
	// 戦いに必要なメーターが最小値になると最小値で固定
	if (m_fightingMeter < 0.0f)
	{
		m_isStun = true;
		m_fightingMeter = 0.0f;
	}
}
