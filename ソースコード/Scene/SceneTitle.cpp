#include <DxLib.h>

#include "SceneTitle.h"
#include "SceneLevelSelect.h"

#include "../Util/Game.h"
#include "../Util/Pad.h"
#include "../CSVData/FontManager.h"
#include "../Util/FontSize.h"

#include "../Object/Camera/Camera.h"
#include "../Object/Player/Player.h"
#include "../Object/Enemy/Enemy.h"

#include "../Util/GameObject.h"

#include "../Util/ModelName.h"
#include "../CSVData/ModelManager.h"

namespace
{
	// 3Dオブジェクトの角度
	const VECTOR kWinnerRota  = VGet(0, 0   * DX_PI_F / 180.0f, 0);
	const VECTOR kWinnerRota2 = VGet(0, 180 * DX_PI_F / 180.0f, 0);

	//選択ボタン用文字
	const char* const kPresskey = "Press any key";

	// テキスト位置
	constexpr int kTextPosUpX = Game::kScreenWidth / 2 - 272 + 12;
	constexpr int kTextPosUpY = Game::kScreenHeight / 2 + 256;

	constexpr int kTextPosDownX = kTextPosUpX + 5;
	constexpr int kTextPosDownY = kTextPosUpY + 5;

	// テキストカラー
	constexpr int kTextUpColor   = 0xffffff;
	constexpr int kTextDownColor = 0x111111;

	// 画像パス
	const char* const kTitleImagePath = "Data/Image/Title.png";
	const char* const kBgImagePath = "Data/Image/UI/GameDifficultySelectionBase.png";

	// 3Dモデルを描画する為に2Dから3Dへ座標を変換する
	const VECTOR kWhiteModelPos = VGet(Game::kScreenWidth / 2 - 700.0f, Game::kScreenHeight / 2 + 150.0f, 0.5f);
	const VECTOR kBlackModelPos = VGet(Game::kScreenWidth / 2 + 700.0f, Game::kScreenHeight / 2 + 150.0f, 0.5f);

	// オブジェクトサイズ
	const VECTOR kModelSize = VGet(12.0f, 12.0f, 12.0f);

	// タイトル画像
	constexpr int kTitleGprahPosX = Game::kScreenWidth / 2;
	constexpr int kTitleGprahPosY = Game::kScreenHeight / 2;	

	// テキストのアルファ値調整用
	// 振動の周波数
	constexpr float kTextFrequency = 0.06f;
	// 振動の振幅
	constexpr float kTextkAmplitude = 255.0f;

	// モデルの回転用
	// 振動の周波数
	constexpr float kModelFrequency = 0.007f;
	// 振動の振幅
	constexpr float kModelkAmplitude = 15.0f;
}

SceneTitle::SceneTitle():
	m_hTitle(-1),
	m_hBg(-1),
	m_blendAlpha(0)
{
	// 画像の読み込み
	m_hTitle = LoadGraph(kTitleImagePath);
	m_hBg    = LoadGraph(kBgImagePath);
}

SceneTitle::~SceneTitle()
{
}

void SceneTitle::Init()
{
	// コンストラクタでカメラの情報を設定後スクリーン座標から3D空間座標への変換を行う
	const VECTOR whiteModelPos = ConvScreenPosToWorldPos(kWhiteModelPos);
	const VECTOR blackModelPos = ConvScreenPosToWorldPos(kBlackModelPos);
	
	// プレイヤーオブジェクトインスタンス生成
	m_pPlayer = std::make_unique<GameObject>(
		ModelManager::GetInstance().ModelType(ModelName::Knight_W),
		whiteModelPos,
		kWinnerRota,
		kModelSize
		);
	// エネミーオブジェクトインスタンス生成
	m_pEnemy = std::make_unique<GameObject>(
		ModelManager::GetInstance().ModelType(ModelName::Knight_B),
		blackModelPos,
		kWinnerRota2,
		kModelSize
		);

	m_pPlayer->Update();
	m_pEnemy->Update();
}

void SceneTitle::End()
{
	// メモリ解放
	DeleteGraph(m_hTitle);
	DeleteGraph(m_hBg);
}

SceneBase* SceneTitle::Update()
{	
	// タイトル画像の移動の計算
	static float timer = 0.0f;
	m_blendAlpha = cos(timer * kTextFrequency) * kTextkAmplitude;
	timer++;

	// モデルの回転の計算
	static float rota = 0.0f;
	const float rotaModel = cos(rota * kModelFrequency) * kModelkAmplitude;
	rota++;

	// ボタンをおした場合
	if (Pad::IsTrigger(PAD_INPUT_1) || 
		Pad::IsTrigger(PAD_INPUT_2) || 
		Pad::IsTrigger(PAD_INPUT_3) || 
		Pad::IsTrigger(PAD_INPUT_4))
	{
		timer = 0;
		rota  = 0;
		return new SceneLevelSelect();
	}

	// 3Dモデルの角度変更
	m_pPlayer->SetRotate(VGet(kWinnerRota.x, kWinnerRota.y + rotaModel, kWinnerRota.z));
	m_pEnemy->SetRotate(VGet(kWinnerRota2.x, kWinnerRota2.y + rotaModel, kWinnerRota2.z));

	// 3Dモデルの更新処理
	m_pPlayer->Update();
	m_pEnemy->Update();

	return this;
}

void SceneTitle::Draw()
{
	// 背景
	DrawExtendGraph(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_hBg, true);

	// タイトル描画
	DrawRotaGraph(kTitleGprahPosX, kTitleGprahPosY, 1, 0.0f, m_hTitle, true);

	// 3Dモデル描画
	m_pPlayer->Draw();
	m_pEnemy->Draw();

	// ボタン誘導描画
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(m_blendAlpha));
	FontManager::GetInstance().DrawString(kTextPosDownX, kTextPosDownY, kPresskey, kTextDownColor, FontSize::GENEITERAMIN_MEDIUM);
	FontManager::GetInstance().DrawString(kTextPosUpX, kTextPosUpY, kPresskey, kTextUpColor, FontSize::GENEITERAMIN_MEDIUM);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}