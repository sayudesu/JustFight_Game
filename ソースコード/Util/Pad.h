#pragma once

// �p�b�h����
namespace Pad
{
	// �p�b�h�̓��͏�Ԏ擾
	void Update();

	// ������������
	bool IsPress(int button, int padNo = 0);
	// �g���K�[����
	bool IsTrigger(int button, int padNo = 0);

	// ���O�L�^�J�n�A�I��
	void StartRecordLog();
	void EndRecordLog();

	// ���O�Đ��J�n�A�I��
	void StartPlayLog();
	void EndPlayLog();
}
