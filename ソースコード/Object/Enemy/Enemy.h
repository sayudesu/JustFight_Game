#pragma once
#include <DxLib.h>
#include <deque>

#include "EnemyBase.h"

#include "../../Util/DifficultyData.h"

class Enemy : public EnemyBase
{
public:
	Enemy(DifficultyData data,VECTOR pos);
	virtual ~Enemy();

	void Input()override;
	void InputTutorial()override;

private:

	// 場外に落ちない用にする
	void FieldLimit();

	// ターゲットに近づいた場合左右移動をする
	void MoveLeftAndRight(MATRIX mtxRot);

	// 戦闘態勢を決める
	void BattleType();
	void BattleTypeGurad();
	void BattleTypeT();

	// キャラクターの向き
	void Direction()override;
private:
	// プレイヤーの方向を見る為のアングルに遅延を入れる
	std::deque<float> m_delayFrameAngle;

	// 防御
	bool m_isCheckGuard;
	bool m_isResultGuard;
	// 通常攻撃
	bool m_isCheckAttack;
	bool m_isAttackResult;

	// 強攻撃
	bool m_isCheckStrongAttack;
	bool m_isStrongAttackResult;
	
	// 左右移動用
	bool m_isMoveLeft;
	bool m_isMoveRight;

	// 回避用のベクトル
	VECTOR m_awayVec;
	// 回避の移動用相対位置
	VECTOR m_awayRelativePos;

	int m_guardFrameCount = 0;

	// 敵専用の回転行列
	MATRIX m_enemyRotMtx;

	// チュートリアル用
	bool m_targetGuard;
};

