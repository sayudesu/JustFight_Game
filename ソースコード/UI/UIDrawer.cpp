#include <DxLib.h>

#include "../Util/Game.h"

#include "../Util/GameObject.h"

#include "UIDrawer.h"

namespace
{
	// ダメージを受けた場合に描画する演出の最大フレーム数
	constexpr int kDamageEffectFrameMax = 30;

	// 攻撃を受けた場合の最大カウント数
	constexpr int kDamageNoCountMax = 4;

	// 画像の位置調整
	constexpr int kDamageEffectY = 300;

	// アルファ値変更用
	constexpr int kAlphaRate = 5;
}

UIDrawer::UIDrawer()
{
	// ダメージエフェクト画像の読み込み
	m_hDamageEffect[0] = LoadGraph("Data/Image/UI/DamageEffect.png");
	m_hDamageEffect[1] = LoadGraph("Data/Image/UI/DamageEffect2.png");
	m_hDamageEffect[2] = LoadGraph("Data/Image/UI/DamageEffect3.png");
	m_hDamageEffect[3] = LoadGraph("Data/Image/UI/DamageEffect4.png");
	m_hDamageEffect[4] = LoadGraph("Data/Image/UI/DamageEffect5.png");

	m_handle[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::BG)]					= LoadGraph("Data/Image/UI/StatusBaseWhite.png");
	m_handle[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::CHARACTOR)]		    = LoadGraph("Data/Image/UI/HoseWhite.png");
	m_handle[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::HP)]					= LoadGraph("Data/Image/UI/HPGauge.png");
	m_handle[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::HP_BG)]				= LoadGraph("Data/Image/UI/HPGaugeBase.png");
	m_handle[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::SP)]					= LoadGraph("Data/Image/UI/SpecialMovesGauge.png");
	m_handle[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::SP_BG)]              = LoadGraph("Data/Image/UI/SpecialMovesGaugeBase.png");
	m_handle[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::FIGHT_POWER)]        = LoadGraph("Data/Image/UI/BodyGauge.png");
	m_handle[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::FIGHT_POWER_CENTER)] = LoadGraph("Data/Image/UI/BodyGaugeMid.png");
	m_handle[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::FIGHT_POWER_BG)]     = LoadGraph("Data/Image/UI/BodyGaugeBase.png");
	m_handle[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::FIGHT_OUTSIDE)]      = LoadGraph("Data/Image/UI/BodyGaugeOutSide.png");
	m_handle[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::BAR_OUTSIDE)]        = LoadGraph("Data/Image/UI/GaugeOutSide.png");

	m_handle[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::BG)] = LoadGraph("Data/Image/UI/StatusBaseBlack.png");
	m_handle[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::CHARACTOR)] = LoadGraph("Data/Image/UI/HoseBlack.png");
	m_handle[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::HP)] = LoadGraph("Data/Image/UI/HPGauge.png");
	m_handle[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::HP_BG)] = LoadGraph("Data/Image/UI/HPGaugeBase.png");
	m_handle[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::SP)] = LoadGraph("Data/Image/UI/SpecialMovesGauge.png");
	m_handle[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::SP_BG)] = LoadGraph("Data/Image/UI/SpecialMovesGaugeBase.png");
	m_handle[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::FIGHT_POWER)] = LoadGraph("Data/Image/UI/BodyGauge.png");
	m_handle[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::FIGHT_POWER_CENTER)] = LoadGraph("Data/Image/UI/BodyGaugeMid.png");
	m_handle[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::FIGHT_POWER_BG)] = LoadGraph("Data/Image/UI/BodyGaugeBase.png");
	m_handle[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::FIGHT_OUTSIDE)] = LoadGraph("Data/Image/UI/BodyGaugeOutSide.png");
	m_handle[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::BAR_OUTSIDE)] = LoadGraph("Data/Image/UI/GaugeOutSide.png");


	m_damageCount = -1;

	// 描画位置テスト
	int x = 0;
	int y = 0;
	GetGraphSize(
		m_handle[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::BG)],
		&x, &y);

	// プレイヤー
	{
		m_playerPos = VGet(315.0f, static_cast<float>(Game::kScreenHeight) - static_cast<float>(y) + 5.0f, 0.0f);

		m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::BG)] = std::make_unique<GameObject>(
			"Data/Image/UI/StatusBaseWhite.png",
			m_playerPos,
			0.0f,
			1.0f
			);

		m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::CHARACTOR)] = std::make_unique<GameObject>(
			"Data/Image/UI/HoseWhite.png",
			VGet(270, -30, 0),
			0.0f,
			1.0f,
			m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::BG)].get()
			);

		m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::HP)] = std::make_unique<GameObject>(
			"Data/Image/UI/HPGauge.png",
			VGet(-11, -25, 0),
			0.0f,
			1.0f,
			m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::BG)].get()
		);

		m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::HP_BG)] = std::make_unique<GameObject>(
			"Data/Image/UI/HPGaugeBase.png",
			VGet(-11, -25, 0),
			0.0f,
			1.0f,
			m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::BG)].get()
		);

		m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::BAR_OUTSIDE)] = std::make_unique<GameObject>(
			"Data/Image/UI/GaugeOutSide.png",
			VGet(-11, -25, 0),
			0.0f,
			1.0f,
			m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::BG)].get()
		);

		m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::SP)] = std::make_unique<GameObject>(
			"Data/Image/UI/SpecialMovesGauge.png",
			VGet(-12, 18, 0),
			0.0f,
			1.0f,
			m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::BG)].get()
		);

		m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::SP_BG)] = std::make_unique<GameObject>(
			"Data/Image/UI/SpecialMovesGaugeBase.png",
			VGet(-12, 18, 0),
			0.0f,
			1.0f,
			m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::BG)].get()
		);

		m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::BAR_OUTSIDE2)] = std::make_unique<GameObject>(
			"Data/Image/UI/GaugeOutSide.png",
			VGet(-12, 18, 0),
			0.0f,
			1.0f,
			m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::BG)].get()
		);
	}


	// 描画位置テスト
	int x2 = 0;
	int y2 = 0;
	GetGraphSize(
		m_handle[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::BG)],
		&x2, &y2);

	// エネミー
	{
		m_enemyPos = VGet(static_cast<float>(Game::kScreenWidth) - 315.0f, static_cast<float>(y) + 5.0f + 50.0f, 0.0f);

		m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::BG)] = std::make_unique<GameObject>(
			"Data/Image/UI/StatusBaseBlack.png",
			m_enemyPos,
			0.0f,
			1.0f
		);

		m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::CHARACTOR)] = std::make_unique<GameObject>(
			"Data/Image/UI/HoseBlack.png",
			VGet(-270, -30, 0),
			0.0f,
			1.0f,
			m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::BG)].get()
		);

		m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::HP)] = std::make_unique<GameObject>(
			"Data/Image/UI/HPGauge.png",
			VGet(11, -25, 0),
			0.0f,
			1.0f,
			m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::BG)].get()
		);

		m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::HP_BG)] = std::make_unique<GameObject>(
			"Data/Image/UI/HPGaugeBase.png",
			VGet(11, -25, 0),
			0.0f,
			1.0f,
			m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::BG)].get()
		);

		m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::BAR_OUTSIDE)] = std::make_unique<GameObject>(
			"Data/Image/UI/GaugeOutSide.png",
			VGet(11, -25, 0),
			0.0f,
			1.0f,
			m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::BG)].get()
		);

		m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::SP)] = std::make_unique<GameObject>(
			"Data/Image/UI/SpecialMovesGauge.png",
			VGet(12, 18, 0),
			0.0f,
			1.0f,
			m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::BG)].get()
		);

		m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::SP_BG)] = std::make_unique<GameObject>(
			"Data/Image/UI/SpecialMovesGaugeBase.png",
			VGet(12, 18, 0),
			0.0f,
			1.0f,
			m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::BG)].get()
		);

		m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::BAR_OUTSIDE2)] = std::make_unique<GameObject>(
			"Data/Image/UI/GaugeOutSide.png",
			VGet(12, 18, 0),
			0.0f,
			1.0f,
			m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::BG)].get()
		);
	}


	m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::BG)]->Update();
	m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::CHARACTOR)]->Update();
	m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::HP)]->Update();
	m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::HP_BG)]->Update();
	m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::SP)]->Update();
	m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::SP_BG)]->Update();
	m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::BAR_OUTSIDE)]->Update();
	m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::BAR_OUTSIDE2)]->Update();

	m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::BG)]->Update();
	m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::CHARACTOR)]->Update();
	m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::HP)]->Update();
	m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::HP_BG)]->Update();
	m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::SP)]->Update();
	m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::SP_BG)]->Update();
	m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::BAR_OUTSIDE)]->Update();
	m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::BAR_OUTSIDE2)]->Update();

	m_damageEffect = false;

	m_damageEffectFrame = 0;
}

UIDrawer::~UIDrawer()
{
	for (int i = 0; i < 5; i++)
	{
		DeleteGraph(m_hDamageEffect[i]);
	}
	for (int i = 0; i < static_cast<int>(CharacterName::MAX); i++)
	{
		for (int j = 0; j < static_cast<int>(HandleType::MAX); j++)
		{
			DeleteGraph(m_handle[i][j]);
		}
	}
}

void UIDrawer::Update()
{
	for (int i = 0; i < static_cast<int>(CharacterName::MAX); i++)
	{
		for (int j = 0; j < static_cast<int>(HandleType::MAX); j++)
		{
			m_image[i][j]->Update();
		}
	}	
}

void UIDrawer::Draw()
{
	// プレイヤー
	{
		m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::BG)          ]->Draw();
		m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::CHARACTOR)   ]->Draw();
		m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::HP)          ]->Draw();

		// 左上
		int dXL = (static_cast<int>(m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::HP)]->GetPos().x )+
			static_cast<int>(m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::BG)]->GetPos().x))-
			(static_cast<int>(m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::HP)]->GetGraphSizeX()) / 2);

		// 右上
		int dX = (static_cast<int>(m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::HP)]->GetPos().x) +
			static_cast<int>(m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::BG)]->GetPos().x)) +
			(static_cast<int>(m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::HP)]->GetGraphSizeX()) /2);

		// 上
		int dY = static_cast<int>(m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::HP)]->GetPos().y) +
			static_cast<int>(m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::BG)]->GetPos().y) -
			(static_cast<int>(m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::HP)]->GetGraphSizeY()) / 2);

		int dX2 = dX * (m_hpNum[static_cast<int>(CharacterName::PLAYER)] + 1) / (m_hpMax[static_cast<int>(CharacterName::PLAYER)] + 1);

		// 下
		int dY2 = (static_cast<int>(m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::HP)]->GetPos().y) +
			static_cast<int>(m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::BG)]->GetPos().y)) +
			(static_cast<int>(m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::HP)]->GetGraphSizeY()) /2);

		m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::HP_BG)]->Draw();

		int hpSliderX = static_cast<int>(m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::HP)]->GetGraphSizeX());
		DrawBox(dXL, dY, dXL + hpSliderX * m_hpNum[static_cast<int>(CharacterName::PLAYER)] / m_hpMax[static_cast<int>(CharacterName::PLAYER)], dY2, 0x007700, true);

		m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::SP)]->Draw();

		dX = (m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::SP)]->GetPos().x +
			m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::BG)]->GetPos().x) +
			(m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::SP)]->GetGraphSizeX() / 2);

		dY = m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::SP)]->GetPos().y +
			m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::BG)]->GetPos().y -
			(m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::SP)]->GetGraphSizeY() / 2);

		dX2 = dX * (m_skillNum[static_cast<int>(CharacterName::PLAYER)]) / (m_skillMax[static_cast<int>(CharacterName::PLAYER)]);

		dY2 = (m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::SP)]->GetPos().y +
			m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::BG)]->GetPos().y) +
			(m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::SP)]->GetGraphSizeY() / 2);

		m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::SP_BG)       ]->Draw();

		m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::BAR_OUTSIDE) ]->Draw();
		m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::BAR_OUTSIDE2)]->Draw();

		hpSliderX = m_image[static_cast<int>(CharacterName::PLAYER)][static_cast<int>(HandleType::SP)]->GetGraphSizeX();
		DrawBox(dXL, dY, dXL + hpSliderX * m_skillNum[static_cast<int>(CharacterName::PLAYER)] / m_skillMax[static_cast<int>(CharacterName::PLAYER)], dY2, 0xaaaa00, true);
	}

	// エネミー
	{
		m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::BG)]->Draw();
		m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::CHARACTOR)]->Draw();
		m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::HP)]->Draw();


		// 左上
		int dXL = (m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::HP)]->GetPos().x +
			m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::BG)]->GetPos().x) -
			(m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::HP)]->GetGraphSizeX() / 2);

		// 右上
		int dX = (m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::HP)]->GetPos().x +
			m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::BG)]->GetPos().x) +
			(m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::HP)]->GetGraphSizeX() / 2);

		// 上
		int dY = m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::HP)]->GetPos().y +
			m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::BG)]->GetPos().y -
			(m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::HP)]->GetGraphSizeY() / 2);

		int dX2 = dX * (m_hpNum[static_cast<int>(CharacterName::ENEMY)] + 1) / (m_hpMax[static_cast<int>(CharacterName::ENEMY)] + 1);

		// 下
		int dY2 = (m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::HP)]->GetPos().y +
			m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::BG)]->GetPos().y) +
			(m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::HP)]->GetGraphSizeY() / 2);

		m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::HP_BG)]->Draw();

		int hpOffset = m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::HP)]->GetGraphSizeX();
		DrawBox(dXL, dY, dXL + hpOffset * m_hpNum[static_cast<int>(CharacterName::ENEMY)] / m_hpMax[static_cast<int>(CharacterName::ENEMY)], dY2, 0x007700, true);

		m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::SP)]->Draw();

		dX = (m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::SP)]->GetPos().x +
			m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::BG)]->GetPos().x) +
			(m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::SP)]->GetGraphSizeX() / 2);

		dY = m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::SP)]->GetPos().y +
			m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::BG)]->GetPos().y -
			(m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::SP)]->GetGraphSizeY() / 2);

		dX2 = dX * (m_skillNum[static_cast<int>(CharacterName::ENEMY)]) / (m_skillMax[static_cast<int>(CharacterName::ENEMY)]);

		dY2 = (m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::SP)]->GetPos().y +
			m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::BG)]->GetPos().y) +
			(m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::SP)]->GetGraphSizeY() / 2);

		m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::SP_BG)]->Draw();

		m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::BAR_OUTSIDE)]->Draw();
		m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::BAR_OUTSIDE2)]->Draw();

		hpOffset = m_image[static_cast<int>(CharacterName::ENEMY)][static_cast<int>(HandleType::SP)]->GetGraphSizeX();
		DrawBox(dXL, dY, dXL + hpOffset * m_skillNum[static_cast<int>(CharacterName::ENEMY)] / m_skillMax[static_cast<int>(CharacterName::ENEMY)], dY2, 0xaaaa00, true);
	}

	// エフェクトを描画するフレームを指定する
	if (m_damageEffect)
	{
		m_damageEffectFrame = kDamageEffectFrameMax;
	}

	// ダメージを受けると
	if (m_isHit)
	{
		// ダメージを受けた数をカウントする
		m_damageCount++;
		if (m_damageCount > kDamageNoCountMax)
		{
			m_damageCount = kDamageNoCountMax;
		}
	}

	// ダメージエフェクト描画中だった場合
	if (m_damageEffectFrame != 0)
	{
		m_damageEffectFrame--;
		// 描画ブレンドモードでアルファブレンドを変更する
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_damageEffectFrame * kAlphaRate);
		int posY = 0;
		if (m_damageCount == 0)
		{
			posY = kDamageEffectY;
		}
		// 血のエフェクト
		DrawRotaGraph(Game::kScreenWidth/2,Game::kScreenHeight/2 + posY, 1, 0.0f, m_hDamageEffect[m_damageCount], true);	
		// 描画ブレンドモードをノーブレンドにする
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

}

void UIDrawer::SetParam(CharacterName name, int hpNum, int hpMax, float skillNum, float skillMax, int fightMeterNum, bool damageEffect,bool isHit)
{
	m_hpNum[static_cast<int>(name)] = hpNum;
	m_hpMax[static_cast<int>(name)] = hpMax;
	m_skillNum[static_cast<int>(name)] = static_cast<int>(skillNum);
	m_skillMax[static_cast<int>(name)] = static_cast<int>(skillMax);
	m_fightMeterNum[static_cast<int>(name)] = fightMeterNum;

	m_damageEffect = damageEffect;
	m_isHit = isHit;
}