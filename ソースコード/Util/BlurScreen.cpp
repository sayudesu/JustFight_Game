#include <DxLib.h>

#include "BlurScreen.h"

#include "Game.h"

EffectScreen* EffectScreen::m_pInstance = nullptr;

namespace
{
	// 揺れ具合
	constexpr float kShakeMaxPower = 20.0f;
	constexpr float kShakeRate = 0.95f;

	// 揺れフレーム
	constexpr int kShakeFrame = 30;

	// ブレンドモード無しの場合の値
	constexpr int kNoBlendModeRate = 128;

	// ブラーをかける際のアルファ値
	constexpr int kBlurAlphaRate = 150;
}

void EffectScreen::Load()
{
	// 攻撃演出用
	m_shake = 0.0f;
	m_shakeFrame = 0;

	// 加工用の画面ハンドルを保存
	int sw = 0, sh = 0, bit = 0;      // 画面幅＆画面高＆ビット数
	GetScreenState(&sw, &sh, &bit);   // 幅と高さを取得しておく

	for (int i = 0; i < static_cast<int>(DamageEffectNo::MAX); i++)
	{
		m_damageScreen[i] = MakeScreen(sw, sh);// 加工用画面を用意する
	}
	// モーションブラー用
	m_notBlendDraw = 0;
	m_current = 0;
	m_alpha = kBlurAlphaRate;

	// 画像ハンドルを作成する
	for (int i = 0; i < static_cast<int>(ScreenEffectNo::MAX); ++i)
	{
		m_blurScreen[i] = MakeScreen(Game::kScreenWidth, Game::kScreenHeight);
	}
}

void EffectScreen::Unload()
{
	// 画像ハンドルを削除する
	for (int i = 0; i < static_cast<int>(DamageEffectNo::MAX); ++i)
	{
		if (m_damageScreen[i] != -1)
		{
			DeleteGraph(m_damageScreen[i]);
		}
	}
}

void EffectScreen::ClearScreen()
{
	// 画面をクリアにする
	ClearDrawScreen();
}

void EffectScreen::ScreenBack()
{
	SetDrawScreen(DX_SCREEN_BACK);
}

void EffectScreen::BlurIReplayInit()
{
	// 画像ハンドルを初期化し、新しい画像ハンドルを作成する	
	for (int i = 0; i < static_cast<int>(ScreenEffectNo::MAX); ++i)
	{
		if (m_blurScreen[i] != -1)
		{
			DeleteGraph(m_blurScreen[i]);
		}

		m_blurScreen[i] = MakeScreen(Game::kScreenWidth, Game::kScreenHeight);
	}
	m_current      = 0;
	m_notBlendDraw = 0;
}

void EffectScreen::BlurRelease()
{
	// 画像ハンドルを削除する
	for (int i = 0; i < static_cast<int>(ScreenEffectNo::MAX); ++i)
	{
		if (m_blurScreen[i] != -1)
		{
			DeleteGraph(m_blurScreen[i]);
		}
	}
}

void EffectScreen::BlurPreRenderBlurScreen()
{
	// 画面の作成
	SetDrawScreen(m_blurScreen[m_current]);
}

void EffectScreen::BlurPostRenderBlurScreen(bool isBlurDraw)
{
	// アルファ値変更用
	int alphaRate = 0;
	// 描画する画面のフレーム
	int backFrameRate = 0;

	// ブラーを視覚化するかどうか
	if (isBlurDraw)
	{
		alphaRate     = m_alpha;
		backFrameRate = 1;		
	}
	else
	{
		alphaRate     = 0;
		backFrameRate = 0;
	}

	// アルファの値を変更する
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alphaRate);

	// 画面の保存数をカウントが上回ったら処理を実行する
	m_notBlendDraw++;
	if (m_notBlendDraw > static_cast<int>(ScreenEffectNo::MAX))
	{
		// backFrameRateフレーム前の画面
		DrawExtendGraph(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_blurScreen[backFrameRate - m_current], false);		
	}

	// 以下の処理にブレンドモードを適用しない
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	// バイリニア法で描画
	SetDrawMode(DX_DRAWMODE_BILINEAR);

	// 画面の切り替え
	SetDrawScreen(DX_SCREEN_BACK);

	// 画面揺れ効果があるかどうか
	if (static_cast<int>(m_shake) != 0)
	{
		// 加算合成する
		SetDrawBlendMode(DX_BLENDMODE_ADD, 128);
	
		// グラフィックハンドル化した画面を描画させる
		DrawGraphF(m_shake, m_shake, m_blurScreen[m_current], false);

		// ガウスでぼかしを入れる
		GraphFilter(m_damageScreen[static_cast<int>(DamageEffectNo::QUAKE)], DX_GRAPH_FILTER_GAUSS, 32, 1400);
	}

	// ブラー効果を得た画像を描画する
	DrawGraphF(m_shake, m_shake, m_blurScreen[m_current], false);

	// ブラー効果用に使用していなかった配列の番号を計算する
	m_current = 1 - m_current;

	// 画面揺れ効果があるかどうか
	if (static_cast<int>(m_shake) != 0)
	{
		// ブレンドモードを初期状態に戻す
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, kNoBlendModeRate);
	}
}

void EffectScreen::QuakeReplayInit()
{	
	// 揺れ具合
	m_shake      = kShakeMaxPower;
	// 揺れるフレーム数
	m_shakeFrame = kShakeFrame;
}

void EffectScreen::QuakeUpdate()
{	
	// 画面の揺れ具合の調整
	if (m_shakeFrame > 0) 
	{
		m_shake = -m_shake;
		m_shake *= kShakeRate;
		m_shakeFrame--;
	}
	else
	{
		m_shake = 0.0f;
	}
}