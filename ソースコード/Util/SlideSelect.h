#pragma once

// �^�C�g����ʂŃX���C�h���s���I�������܂�
class SlideSelect final
{
public:
	SlideSelect();
	~SlideSelect();

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="selectMaxNum">�I�����̍ő呃 0�` </param>
	/// <param name="isUpDown">�㉺�ɑI�����邩�ǂ���</param>
	void Init(int selectMaxNum, bool isUpDown);

	// �X�V����
	void Update();

public:
	// �I�������{�^���̌��ʂ̔ԍ���Ԃ��܂�
	int GetResult();
	// ���݂̑I�����Ă���ԍ���Ԃ��܂�
	int GetSelect();
	// ��̃{�^�������������ǂ�����Ԃ��܂�
	bool IsUpBotton();
	// ���̃{�^�������������ǂ�����Ԃ��܂�
	bool IsDownBotton();
	// ���݂̑I����Ԃ����Z�b�g����
	void Reset();
private:
	// �{�^���̎��
	enum SelectButton
	{
		UP,
		DOWN,

		BUTTONMAX,
	};
private:
	// �I�𒆂̔ԍ�
	int m_selectNo;
	// �I������
	int m_selectNoResult;

	// �I���ł���ő�̐�
	int m_selectMaxNum;

	// �I��p�t���[���J�E���g
	int m_selectFramesPressCount[SelectButton::BUTTONMAX];
	int m_selectUpSpeedFramePressCount[SelectButton::BUTTONMAX];

	// �I��ύX���\���ǂ���
	int m_isSelect;

	// �I����ύX����ׂ̃t���[��
	int m_selectFrameMax[SelectButton::BUTTONMAX];

	// ��{�^���������Ă��邩�ǂ���
	bool m_isUpBotton;
	// ���{�^���������Ă��邩�ǂ���
	bool m_isDownBotton;

	// �㉺�A���E�ǂ���̃{�^���g�p���邩�����߂�
	int m_selectButton[2];


};

