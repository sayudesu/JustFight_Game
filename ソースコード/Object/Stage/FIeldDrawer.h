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

	// ハンドルデータを渡す
	int GetHandle();
	int GetHandle2();

private:
	// マップ用のハンドル
	GameObject* m_chessModel;
	GameObject* m_mapModel;
	// 位置
	VECTOR m_pos;	
};

