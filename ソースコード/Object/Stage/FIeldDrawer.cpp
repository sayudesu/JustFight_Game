#include "FIeldDrawer.h"
#include "../../Util/GameObject.h"

#include "../../CSVData/ModelManager.h"
#include "../../Util/ModelName.h"

namespace
{
	// オブジェクトのサイズ
	constexpr float kChessSize = 0.6f;
	constexpr float kMapSize = 40.0f;
}

FieldDrawer::FieldDrawer():
	m_chessModel(nullptr),
	m_mapModel(nullptr),
	m_pos(VGet(0,0,0))
{
}

FieldDrawer::~FieldDrawer()
{
}

void FieldDrawer::Init()
{
	// オブジェクトの生成
	m_chessModel = new GameObject(
		ModelManager::GetInstance().ModelType(ModelName::MAP2),
		m_pos,
		VGet(0, 0, 0),
		VGet(kChessSize, kChessSize, kChessSize));

	m_mapModel = new GameObject(
		ModelManager::GetInstance().ModelType(ModelName::MAP3),
		VGet(m_pos.x, m_pos.y, m_pos.z),
		VGet(0, 0, 0),
		VGet(kMapSize, kMapSize, kMapSize));

	m_chessModel->Update();
	m_mapModel->Update();
}

void FieldDrawer::End()
{
	// 解放
	delete m_chessModel;
	m_chessModel = nullptr;

	delete m_mapModel;
	m_mapModel = nullptr;
}

void FieldDrawer::Draw()
{
	// 描画
	m_chessModel->Draw();
	m_mapModel->Draw();
}

int FieldDrawer::GetHandle()
{
	return m_chessModel->GetHandle();
}

int FieldDrawer::GetHandle2()
{
	return m_mapModel->GetHandle();
}
