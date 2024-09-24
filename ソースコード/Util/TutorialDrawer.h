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

	// �`�悵�������������߂�
	void SetTips(Tips tips);

private:
	// �����p����
	std::string m_text;
	std::string m_step;

	// �`�悵�����`�b�v�X�p
	bool m_tips[static_cast<int>(Tips::MAX)];

	// �`���[�g���A���I���p�J�E���g
	int m_endFrameCount;
};

