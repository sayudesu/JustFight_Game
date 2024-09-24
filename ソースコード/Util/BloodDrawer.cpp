#include "BloodDrawer.h"
#include "game.h"

namespace
{
	// 重力
	constexpr float kGravity = 0.4f;
	// 大きさ
	constexpr float kSize = 2.0f;
	// 散らばる範囲の値x,z
	constexpr int kRndomXZRate = 10;
	// 散らばる範囲の値y
	constexpr int kRndomYRate = 12;
	// メモリ開放する条件での位置
	constexpr float kEndPosY = -100.0f;
	// 球体の分割数
	constexpr int kDivisionNum = 3;
}

BloodDrawer::BloodDrawer(VECTOR pos, int color)
{
	// パーティクルの初期位置
	m_pos = pos;
	m_vec = VGet(0, 0, 0);
	m_isErase = false;
	m_color = color;
	m_pFunc = &BloodDrawer::FirstMove;

}

BloodDrawer::~BloodDrawer()
{
}

void BloodDrawer::Init(int no)
{		
	// 移動量
	if (no % 2 == 0)
	{
		m_vec.x = (-GetRand(kRndomXZRate));
		m_vec.z = (-GetRand(kRndomXZRate));
	}
	else
	{
		m_vec.x = (GetRand(kRndomXZRate));
		m_vec.z = (GetRand(kRndomXZRate));
	}

	m_vec.y = static_cast<float>(GetRand(kRndomYRate) + 1);
}

void BloodDrawer::Update()
{
	(this->*m_pFunc)();
}

void BloodDrawer::Draw()
{
	// ３Ｄ空間上に球を描画する
	DrawSphere3D(m_pos, kSize, kDivisionNum, m_color, m_color, true);

}

bool BloodDrawer::IsGetErase()
{
	return m_isErase;
}

void BloodDrawer::FirstMove()
{
	// 重力を与える
	m_vec.y -= kGravity;
	// ベクトルの加算
	m_pos = VAdd(m_pos, m_vec);

	if (m_pos.y < kEndPosY)
	{
		m_isErase = true;
	}
}
