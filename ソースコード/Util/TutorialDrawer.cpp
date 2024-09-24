#include <DxLib.h>

#include "TutorialDrawer.h"

#include "../CSVData/FontManager.h"
#include "../CSVData/SubtitleManager.h"

#include "FontSize.h"
#include "SubtitleData.h"

TutorialDrawer::TutorialDrawer()
{

}

TutorialDrawer::~TutorialDrawer()
{

}

void TutorialDrawer::Init()
{

	for (int i = 0; i < static_cast<int>(Tips::MAX); i++)
	{
		m_tips[i] = false;
	}

	// énÇﬂÇÃê‡ñæÇçXêV
	m_step = SubtitleManager::GetInstance().SubtitleStringData(Subtitle::TIPS_ONE);
	m_text = SubtitleManager::GetInstance().SubtitleStringData(Subtitle::TIPS_LEFTSTICK);

	m_endFrameCount = 0;
}

void TutorialDrawer::End()
{

}

void TutorialDrawer::Update()
{

}

void TutorialDrawer::Draw()
{
	// ògëgÇ›
	DrawBox(50, 50, 1260, 300, 0xffffff, true);
	DrawBox(50 + 1, 50 + 1, 1260 + 1, 300 + 1, 0xff0000, false);

	// ê‡ñæÇï`âÊ
	FontManager::GetInstance().DrawString(70, 60, m_step, 0x000000, FontSize::GENEITERAMIN_SMALL);
	FontManager::GetInstance().DrawString(50, 100, m_text, 0x000000, FontSize::GENEITERAMIN_MEDIUM);
}

void TutorialDrawer::SetTips(Tips tips)
{

	for (int i = 0; i < static_cast<int>(Tips::MAX); i++)
	{
		m_tips[i] = false;
	}

	m_tips[static_cast<int>(tips)] = true;

	// åªç›ÇÃçsìÆÇÃï∂éöÇï`âÊ
	if (m_tips[static_cast<int>(Tips::MOVE)])
	{
		m_step = SubtitleManager::GetInstance().SubtitleStringData(Subtitle::TIPS_ONE);
		m_text = SubtitleManager::GetInstance().SubtitleStringData(Subtitle::TIPS_LEFTSTICK);
	}
	else if (m_tips[static_cast<int>(Tips::ATTACK)])
	{
		m_step = SubtitleManager::GetInstance().SubtitleStringData(Subtitle::TIPS_ONE);
		m_text = SubtitleManager::GetInstance().SubtitleStringData(Subtitle::TIPS_ATTACK);
	}
	else if (m_tips[static_cast<int>(Tips::GUARD)])
	{
		m_step = SubtitleManager::GetInstance().SubtitleStringData(Subtitle::TIPS_THREE);
		m_text = SubtitleManager::GetInstance().SubtitleStringData(Subtitle::TIPS_GUARD);

		m_endFrameCount++;
	}

	if (m_endFrameCount > 60 * 3)
	{
		m_step = SubtitleManager::GetInstance().SubtitleStringData(Subtitle::TIPS_END);
		m_text = SubtitleManager::GetInstance().SubtitleStringData(Subtitle::TIPS_END_TOW);
	}
}
