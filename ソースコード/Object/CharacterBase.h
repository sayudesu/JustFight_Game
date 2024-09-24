#pragma once
#include <DxLib.h>

#include "../Util/CharacterName.h"
#include "../Util/BattleState.h"
#include "../Util/CharacterParameter.h"

#include "../Util/DifficultyData.h"

#include "../Util/Tips.h"

// ゲームオブジェクト
class GameObject;

// キャラクター専用基底クラス
class CharacterBase
{
public:
	CharacterBase(DifficultyData data,VECTOR pos);
	virtual ~CharacterBase();

	virtual void Init();
	virtual void End();
	virtual void Update();
	virtual void Draw();

	// 入力
	virtual void Input() = 0;
	// チュートリアル用
	virtual void InputTutorial() = 0;

protected:

	// アイドル状態
	virtual void Idle();
	// 攻撃
	virtual void Attack();
	virtual void AttackTwo();
	// 強攻撃
	virtual void StrongAttack();
	// 防御
	virtual void Guard();

private:
	// 位置情報の更新
	void UpdatePos(int shiftX = 0, int shiftY = 0, int shiftZ = 0);
	// フレームで向かいたい座標まで動かす
	float MoveByFrame(const float relativePos, const float EndPos, const int nowFrame, const int maxFrame);

	// ジャストガード
	virtual void JustGuard();
	// ジャストガードをされた場合
	virtual void Stun();

	// 体力の調整
	virtual void HitPoint();
	// ノックバック
	virtual void KnockBack();

	// 装備を元の位置に戻す処理
	virtual void WeaponDefaultPos();
	virtual void ShieldDefaultPos();

	// 重力
	virtual void Gravity();

protected:
	// パラメーターの調整
	virtual void InputParamType(int type);
	// 角度を取得
	virtual void SetAngle(float angle);
	virtual void SetRotMtx(MATRIX rotMtx);

	// ターゲット位置追跡移動
	virtual void TargetMove();

	// キャラクターの向きの調整
	virtual void Direction() = 0;

	// 強攻撃の為に溜めた力をリセットする
	void SetStrongPowerReset();

	// 壁に当たったかどうか
	bool GetCheckHitWall();
public:
	// 自身が誰をを返す
	CharacterName GetMyId();

	// 現在の行動の情報を渡す
	BattleState GetBattleState();

	// 位置
	VECTOR GetPos      ()const;// 自身
	VECTOR GetCollPos      ()const;// 自身
	VECTOR GetCollWeaponPos() const;
	VECTOR GetShieldPos ()const;// 盾

	// カプセル型判定用の位置を渡す
	VECTOR GetCapsulePosDown();
	VECTOR GetCapsulePosUp();
	float GetCapsuleRadius();

	// 角度
	MATRIX GetRot ()const;// 行列
	float GetAngle()const;// ラジアン

	// モデルの当たり判定用半径
	float GetModelRadius()const;
	
	// 装備の判定用半径
	float GetWeaponAttackRadius()const;
	float GetShieldRadius()const;

	// 装備の相対位置
	VECTOR GetWeaponAttackRelative()const;
	VECTOR GetShieldRelative()const;

	// フレーム関係
	int GetAttackFrame      ()const;// 現在の攻撃フレーム
	int GetGuardFrame       ()const;// 現在の防御フレーム
	int GetJustGuardFrame   ()const;// 現在のジャストガードフレーム
	int GetAttackFrameMax   ()const;// 最大攻撃フレーム
	int GetGuardFrameMax    ()const;// 最大防御フレーム
	int GetJustGuardFrameMax()const;// 最大ジャストガードフレーム
	int GetStunFrameMax     ()const;// 最大スタンのフレーム数

	// ゲージ関係
	int GetHp             ()const;// 現在のヒットポイント
	int GetMaxHp          ()const;// 現在のヒットポイント
	float GetFightingMeter()const;// 戦闘に必要なメーター

	bool IsAway()const;

	bool IsHitDamage()const;

	bool IsGuard()const;

	// 強攻撃をするための力を確認
	float GetStrongPower();
	// 強攻撃の最大値
	float GetkStrongAttackPowerMax();

	// 攻撃か可能かどうか
	bool IsAttack()const;

	// チュートリアル用
	// 現在できる行動を渡す
	bool GetTipsMove(Tips tips);

	// ジャストガードできたかどうか
	bool IsJustGuard()const;

	// スタン状態かどうか
	bool IsStun()const;

	// 攻撃範囲に入っているかどうか
	bool IsAttackRange()const;

	// 地面に当たっているかどうか、当たっている場合呼び出してください
	void SetFieldHit();

	// 攻撃が盾に当たったかどうか
	void SetWeaponAttacksShield(const bool isShieldHit);

	// 現在の行動の情報を受け取る
	void SetBattleState(BattleState state);

	// 相手の角度
	void SetTargetMtxRota(const MATRIX rot);

	// 攻撃範囲に入っているかどうか
	void SetAttackRange(const bool isRange);

	// 攻撃を受けたかどうか
	void SetDamage(const bool isDamage);

	// ガードが成功しかたどうか
	void SetGuardKnockBack(bool isGuard, float vec);

	// ジャストガードが成功したかどうか
	void SetJustGuard(const bool isJustGuard);

	// 戦いに必要な特殊なメーターを変更する
	void SetFightingMeter(const float fightingMeter);

	// 戦いに必要な特殊なメーターを管理する
	void FightingMeter();

	// ターゲットの位置を取得する
	void SetTargetPos(const VECTOR pos);

	// ターゲットの体力を取得
	void SetTargetHp(const int hp);

	// エフェクトを呼び出す
	void SetCollGuardEffect    ();// 通常防御
	void SetCollJustGuardEffect();// ジャストガード

	// 強攻撃を出すための力を溜める
	void SetStrongPower(int power);

	// 攻撃をされたかどうか
	void IsHit();
private:
	// エフェクトハンドル
	int m_effectHandle;

	// 強攻撃を出すための力を溜める
	int m_strongAttackPower;

	// ノックバック用ベクトル
	float m_vecKnockBack;

	// 角度
	MATRIX m_rotMtx;
	MATRIX m_targetRotMtx;
	// 装備
	VECTOR m_vecWeapon;
	VECTOR m_vecShield;
	VECTOR m_weaponPos;
	// カプセルの上の位置
	VECTOR m_capsuleUpPos;
	VECTOR m_capsuleUpDown;

	// 一時的な武器の位置
	VECTOR m_tempWeaponPos;

	// 体力
	// 戦いに必要な特殊なメーター
	int m_hp;
	float m_tempFightingMeter;
	float m_fightingMeter;

	// フレーム関連
	int m_attackGapFrame;// 攻撃時の遊びのフレーム
	int m_attackFrame;   // 遊びから攻撃のフレーム
	int m_guardFrame;    // 防御するまでのフレーム
	int m_justGuardFrame;// ジャストガードのフレーム
	int m_stunFrame;     // スタン状態のフレーム
	int m_recoilFrame;   // 攻撃を弾かれば場合のフレーム
	int m_attackAfterStopFrame; // 攻撃後の硬直フレーム
	int m_justGuardCounterFrame;// ジャストガードを成功させた後のカウント

	// 防御した場合の1フレームだけ判定をとる用フレームカウント
	int m_guardOneFrame;

	// 攻撃用シーン繊維の移動が可能かどうか
	int m_isSceneChange;

	// 敵からの攻撃をガードできたかどうか
	bool m_isResultGuard;
	bool m_isResultDamage;

	// 盾を攻撃したかどうか
	bool m_isWeaponAttacksShield;

	// スタン状態かどうか
	bool m_isStun;

	// 攻撃範囲にいるかどうか
	bool m_isAttackRange;

	// ターゲットの体力
	int m_targetHp;

	// 壁に当たったかどうか
	bool m_isHItWall;

	// 攻撃の予兆フレーム時の振動
	VECTOR m_weaponShakeRate;

	// 現在の行動を記録
	BattleState m_battleState;

	GameObject* m_pCharactor;
	GameObject* m_pWeapon;
	GameObject* m_pShield;

	bool m_isHitNow = false;
	bool m_isHitResult = false;

	float m_shieldRotaY = 0.0f;
	float m_weaponRotaY = 0.0f;
	float m_tempWeaponRotaY = 0.0f;

	// スタンした場合の回転(ラジアン)
	float m_stunRota = 0.0f;

	// 攻撃が当たったかどうか
	bool m_isHit;

protected:
	
	// メンバ関数ポインタ
	void(CharacterBase::*m_pFunc)();

	// 重力
	bool m_isGravity;

	// 位置
	VECTOR m_pos;
	VECTOR m_vec;
	VECTOR m_targetPos;

	// 角度
	float m_angle;

	// 状態
	bool m_isAttack;
	bool m_isStrongAttack;
	bool m_isGuard;
	bool m_isJustGuard;
	bool m_isJustGuardCounter;

	// 回避
	bool m_isAway;

	// ターゲットの距離
	VECTOR m_targetRange;

	// 自身のIDを取得する
	CharacterName m_myId;

	// ターゲットの行動を記録
	BattleState m_targetBattleState;
	BattleState m_tempTargetBattleState;

	// キャラクター全てのパラメーター
	CharacterParameter m_parameter;

	// コンボ技用
	int m_comboAttack;

	// 難易度
	DifficultyData m_difficultyData;

	// チュートリアル用
	// どの行動ができるか決める
	bool m_isTipsMove[static_cast<int>(Tips::MAX)];
};

