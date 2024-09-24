#pragma once
#include <DxLib.h>
#include <array>

class BloodDrawer
{
public:
	BloodDrawer(VECTOR pos, int color);
	virtual ~BloodDrawer();
	// 初期化
	virtual void Init(int no);
	// 更新処理
	virtual void Update();
	// 描画
	virtual void Draw();
	// 消すかどうか
	virtual bool IsGetErase();
private:
	// 始めの動き
	void FirstMove();
private:
	// 位置
	VECTOR m_pos;
	// 移動量
	VECTOR m_vec;
	// 消すかどうか
	bool m_isErase;
	// 色
	int m_color;
	// メンバ関数ポインタ
	void(BloodDrawer::* m_pFunc)();
};

