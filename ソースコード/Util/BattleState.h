#pragma once

// 戦闘状態
enum class BattleState
{
	NONE,         // 空
	IDLE,         // 待機状態
	ATTACK,       // 攻撃
	ATTACKTWO,    // コンボ攻撃
	STRONGATTACK, // 強い攻撃
	GUARD,        // 防御
	JUSTGUARD,    // 防御
	STUN,         // スタン
};