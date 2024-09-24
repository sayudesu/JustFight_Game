#pragma once
#include <DxLib.h>

class GameObject;
class FieldDrawer
{
public:
	FieldDrawer();
	~FieldDrawer();

	void Init();
	void End();
	void Draw();

	// �n���h���f�[�^��n��
	int GetHandle();
	int GetHandle2();

private:
	// �}�b�v�p�̃n���h��
	GameObject* m_chessModel;
	GameObject* m_mapModel;
	// �ʒu
	VECTOR m_pos;	
};

