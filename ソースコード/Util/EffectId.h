#pragma once
enum Id
{
	// 通常ガードした場合
	Guard,
	// ジャストガードした場合
	JustGuard,
	// スタン状態になった場合
	Stun,

	Max
};

enum EffectPlayType
{
	NORMAL,
	LOOP,
};

struct EffectData
{
	// 再生用ハンドル
	int *playingHandle;
	//
	Id id;
	// 再生タイプ
	EffectPlayType type;
	// 位置
	VECTOR pos;
	// サイズ
	VECTOR size;
	// 角度
	VECTOR angle;
	// 再生フレーム
	int playFrameMax;
	int playFrameCount;
};