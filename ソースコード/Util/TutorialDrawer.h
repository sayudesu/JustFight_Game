#pragma once

#include <string>
#include <map>
#include "Tips.h"

class TutorialDrawer
{
public:
	TutorialDrawer();
	~TutorialDrawer();

	void Init();
	void End();

	void Update();
	void Draw();

public:

	// 描画したい説明を決める
	void SetTips(Tips tips);

private:
	// 説明用文字
	std::string m_text;
	std::string m_step;

	// 描画したいチップス用
	bool m_tips[static_cast<int>(Tips::MAX)];

	// チュートリアル終了用カウント
	int m_endFrameCount;
};

