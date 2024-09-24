#include <DxLib.h>
#include "SlideSelect.h"
#include "Pad.h"
#include "Game.h"

#include "../CSVData/SoundManager.h"
#include "SoundName.h"

namespace 
{
	// スライドするまでのボタンを押しているフレーム
	constexpr int kSelectFrameMax = 20;
	// スライドするまでのボタンを押している最小フレーム
	constexpr int kSelectFrameMin = 1;
}

SlideSelect::SlideSelect():
	m_selectNo(1),
	m_selectNoResult(-1),
	m_selectMaxNum(0),
	m_isSelect(false),
	m_isUpBotton(false),
	m_isDownBotton(false)
{
	for (int i = 0; i < SelectButton::BUTTONMAX; i++)
	{
		m_selectFramesPressCount[i] = 0;
		m_selectUpSpeedFramePressCount[i] = 0;
		m_selectFrameMax[i] = kSelectFrameMax;
	}
	m_selectButton[0] = 0;
	m_selectButton[1] = 0;
}

SlideSelect::~SlideSelect()
{
}

void SlideSelect::Init(int selectMaxNum,bool isUpDown)
{
	// スライドの最大数
	m_selectMaxNum = selectMaxNum;

	// 上下スライドかどうか
	if (isUpDown)
	{
		// スライド上下の場合
		m_selectButton[0] = PAD_INPUT_UP;
		m_selectButton[1] = PAD_INPUT_DOWN;
	}
	else
	{
		// スライド左右の場合
		m_selectButton[0] = PAD_INPUT_LEFT;
		m_selectButton[1] = PAD_INPUT_RIGHT;
	}
}

void SlideSelect::Update()
{
	m_isUpBotton   = false;
	m_isDownBotton = false;
	// 上にスライド
	if (Pad::IsPress(m_selectButton[0]))
	{
		m_isUpBotton = true;
		// ボタンを押している間フレーム単位でカウントする
		m_selectFramesPressCount[SelectButton::UP]++;
		m_selectUpSpeedFramePressCount[SelectButton::UP]++;
		
		// 選択ボタンを押した瞬間か
		// 選択ボタンを押し続けている際に選択を変更する
		if (m_selectFramesPressCount[SelectButton::UP] == 1 ||
			m_selectUpSpeedFramePressCount[SelectButton::UP] > m_selectFrameMax[SelectButton::UP])
		{
			// 選択を変更する
			m_selectNo--;
			SoundManager::GetInstance().Play(SoundName::SLIDE);

			// 選択した場合フレームを初期化する
			m_selectUpSpeedFramePressCount[SelectButton::UP] = 0;

			if (m_selectFrameMax[SelectButton::UP] > kSelectFrameMin)
			{
				m_selectFrameMax[SelectButton::UP]--;
			}

			// 最小数になると最大数に変更する
			if (m_selectNo <= -1)
			{
				m_selectNo = m_selectMaxNum;
			}
		}
	}
	else
	{
		// フレームカウントを初期化する
		m_selectFramesPressCount[SelectButton::UP] = 0;
		m_selectUpSpeedFramePressCount[SelectButton::UP] = 0;

		m_selectFrameMax[SelectButton::UP] = kSelectFrameMax;
	}

	// 下にスライド
	if (Pad::IsPress(m_selectButton[1]))
	{
		m_isDownBotton = true;
		// ボタンを押している間フレーム単位でカウントする
		m_selectFramesPressCount[SelectButton::DOWN]++;
		m_selectUpSpeedFramePressCount[SelectButton::DOWN]++;

		// 選択ボタンを押した瞬間か
		// 選択ボタンを押し続けている際に選択を変更する
		if (m_selectFramesPressCount[SelectButton::DOWN] == 1 ||
			m_selectUpSpeedFramePressCount[SelectButton::DOWN] > m_selectFrameMax[SelectButton::DOWN])
		{
			// 選択を変更する
			m_selectNo++;
			SoundManager::GetInstance().Play(SoundName::SLIDE);

			// 選択した場合フレームを初期化する
			m_selectUpSpeedFramePressCount[SelectButton::DOWN] = 0;

			if (m_selectFrameMax[SelectButton::DOWN] > kSelectFrameMin)
			{
				m_selectFrameMax[SelectButton::DOWN]--;
			}

			// 最大数になると最小数にする
			if (m_selectNo > m_selectMaxNum)
			{
				m_selectNo = 0;
			}
		}
	}
	else
	{
		// フレームカウントを初期化する
		m_selectFramesPressCount[SelectButton::DOWN] = 0;
		m_selectUpSpeedFramePressCount[SelectButton::DOWN] = 0;

		m_selectFrameMax[SelectButton::DOWN] = kSelectFrameMax;
	}

	// 選択
	if (Pad::IsTrigger(PAD_INPUT_1))
	{
		SoundManager::GetInstance().Play(SoundName::SELECT);
		m_selectNoResult = m_selectNo;
	}
}

int SlideSelect::GetResult()
{
	return m_selectNoResult;
}

int SlideSelect::GetSelect()
{
	return m_selectNo;
}

bool SlideSelect::IsUpBotton()
{
	return m_isUpBotton;
}

bool SlideSelect::IsDownBotton()
{
	return m_isDownBotton;
}

void SlideSelect::Reset()
{
	m_selectNo = 0;
}
