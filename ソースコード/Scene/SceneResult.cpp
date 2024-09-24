#include <DxLib.h>

#include "SceneResult.h"
#include "SceneTitle.h"
#include "SceneLevelSelect.h"

#include "../Util/Game.h"
#include "../Util/Pad.h"
#include "../Util/GameObject.h"
#include "../Util/SoundName.h"
#include "../Util/FontSize.h"
#include "../\Util/ModelName.h"

#include "../CSVData/SoundManager.h"
#include "../CSVData/FontManager.h"
#include "../CSVData/ModelManager.h"

#include "../Object/Camera/Camera.h"

namespace
{
	// 3Dオブジェクトの角度
	const VECTOR kWinnerRota = VGet(0, 0 * DX_PI_F / 180.0f, 0);
	const VECTOR kLoserRota = VGet(0, 180 * DX_PI_F / 180.0f, -90 * DX_PI_F / 180.0f);
	// 3Dオブジェクトのサイズ
	constexpr float kChessModelSize = 12.0f;
	const VECTOR k3DModelSize = VGet(kChessModelSize, kChessModelSize, kChessModelSize);

	//画像パス
	const char* const kBgImagePath = "Data/Image/UI/GameDifficultySelectionBase.png";
	const char* const kWinPath     = "Data/Image/UI/Win.png";
	const char* const kLosePath    = "Data/Image/UI/Lose.png";

	// テキスト
	const char* const kNextButton = "A ボタンでタイトルに戻る";

	// テキスト位置
	constexpr int kNextTextPosUpX = Game::kScreenWidth / 2 - 470.0f;
	constexpr int kNextTextPosUpY = Game::kScreenHeight / 2 + 200;
	constexpr int kNextTextPosDownX = kNextTextPosUpX + 5;
	constexpr int kNextTextPosDownY = kNextTextPosUpY + 5;

	// テキストカラー
	constexpr int kNextTextUpColor = 0xffffff;
	constexpr int kNextTextDownColor = 0x000000;

	// 背景画像
	constexpr int kBgPosX = Game::kScreenWidth / 2;
	constexpr int kBgPosY = Game::kScreenHeight / 2;
	constexpr float kBgSize = 1.0f;

	// 勝敗結果
	constexpr int kResultGraphPosX = Game::kScreenWidth / 2;
	constexpr int kResultGraphPosY = Game::kScreenHeight / 2;

	// 負けた場合の画像の角度
	constexpr float kResultGraphRotaMax = 0.1f;
	constexpr float kResultGraphRate    = 0.001f;

	// 3Dモデルを描画する為に2Dから3Dへ座標を変換する
	const VECTOR kWinModel2DPos  = VGet(Game::kScreenWidth / 2 - 500.0f, Game::kScreenHeight / 2 + 150.0f, 0.5f);
	const VECTOR kLoseModel2DPos = VGet(Game::kScreenWidth / 2 + 400.0f, Game::kScreenHeight / 2, 0.5f);

	// カメラ位置
	const VECTOR kCameraPos = VGet(0.0f, 0.0f, -10000.0f);

	// 勝敗結果画像
	// 振動の周波数
	constexpr float kFrequency = 0.07f;
	// 振動の振幅
	constexpr float kAmplitude = 5.0f;
}

SceneResult::SceneResult(GameResultData resultData, DifficultyData data):
	m_imageAngle(0.0f),
	m_resultY(0.0f)
{
	// リザルトデータを取得する
	m_resultData = resultData;

	// 勝負した敵のモデルのパスを取得する	
	if (data == DifficultyData::NOIVE)
	{
		m_enemyPath = ModelManager::GetInstance().ModelType(ModelName::Pawn_B);
	}
	if (data == DifficultyData::INTERMEDIATE)
	{
		m_enemyPath = ModelManager::GetInstance().ModelType(ModelName::Knight_B);
	}
	if (data == DifficultyData::EXPERT)
	{
		m_enemyPath = ModelManager::GetInstance().ModelType(ModelName::Queen_B);
	}

	// カメラのインスタンス生成
	m_pCamera = std::make_unique<Camera>();
	// カメラの設定
	m_pCamera->SetPos(kCameraPos);
	m_pCamera->Setting();
}

SceneResult::~SceneResult()
{
}

void SceneResult::Init()
{
	// コンストラクタでカメラの情報を設定後スクリーン座標から3D空間座標への変換を行う
	const VECTOR kWinnerPos = ConvScreenPosToWorldPos(kWinModel2DPos);
	const VECTOR kLoserPos  = ConvScreenPosToWorldPos(kLoseModel2DPos);

	// 勝敗によってプレイヤー、エネミーのモデル位置を変更する為の変数
	VECTOR playerScreenToWorldPos{};
	VECTOR playerRota{};	
	VECTOR enemyScreenToWorldPos{};
	VECTOR enemyRota{};

	// 背景画像読み込み
	m_hImageResultBg = LoadGraph(kBgImagePath);

	// プレイヤーが勝利した場合
	if (m_resultData == GameResultData::CREAR)
	{
		// 画像読み込み
		m_hImageResult = LoadGraph(kWinPath);

		// モデルのトランスフォーム
		playerScreenToWorldPos = kWinnerPos;
		enemyScreenToWorldPos = kLoserPos;
		playerRota = kWinnerRota;
		enemyRota = kLoserRota;
	}
	// エネミーが勝利した場合
	else if (m_resultData == GameResultData::OVER)
	{
		// 画像読み込み
		m_hImageResult = LoadGraph(kLosePath);

		// モデルのトランスフォーム
		playerScreenToWorldPos = kLoserPos;
		enemyScreenToWorldPos = kWinnerPos;
		playerRota = kLoserRota;
		enemyRota = kWinnerRota;
	}

	// プレイヤーオブジェクトインスタンス生成
	m_pPlayer = std::make_unique<GameObject>(
		ModelManager::GetInstance().ModelType(ModelName::Knight_W),
		playerScreenToWorldPos,
		playerRota,
		k3DModelSize
		);
	// エネミーオブジェクトインスタンス生成
	m_pEnemy = std::make_unique<GameObject>(
		m_enemyPath,
		enemyScreenToWorldPos,
		enemyRota,
		k3DModelSize
		);
}

void SceneResult::End()
{
	// 解放処理
	DeleteGraph(m_hImageResult);
	DeleteGraph(m_hImageResultBg);
}

SceneBase* SceneResult::Update()
{
	// プレイヤーの更新処理
	m_pPlayer->Update();
	// エネミーの更新処理
	m_pEnemy->Update();

	static int timer = 0;
	m_resultY = cos(timer * kFrequency) * kAmplitude;
	timer++;

	// 敗北した場合は画像を傾ける
	if (m_resultData == GameResultData::OVER)
	{
		// 負けた場合のBGM
		SoundManager::GetInstance().Play(SoundName::LOSE,true);

		// ResultGraphの角度を変更する
		if (m_imageAngle < kResultGraphRotaMax)
		{
			m_imageAngle += kResultGraphRate;
		}
	}

	if (m_resultData == GameResultData::CREAR)
	{
		// 勝った場合のBGM
		SoundManager::GetInstance().Play(SoundName::WIN, true);
	}

	// 選択
	if (Pad::IsTrigger(PAD_INPUT_1))
	{
		// サウンド管理
		SoundManager::GetInstance().Stop(SoundName::WIN);
		SoundManager::GetInstance().Stop(SoundName::LOSE);
		SoundManager::GetInstance().Play(SoundName::SELECT);
		timer = 0;

		// シーンの切り替え
		return new SceneLevelSelect();
	}
	
	return this;
}

void SceneResult::Draw()
{
	// 背景を描画
	DrawRotaGraph(kBgPosX, kBgPosY, kBgSize, 0.0f, m_hImageResultBg, true);
	DrawRotaGraph(kResultGraphPosX, kResultGraphPosY + m_resultY, 1, m_imageAngle, m_hImageResult, true);

	// キャラクターの描画
	m_pPlayer->Draw();
	m_pEnemy->Draw();

	// ボタン説明
	FontManager::GetInstance().DrawString(kNextTextPosDownX, kNextTextPosDownY, kNextButton, kNextTextDownColor, FontSize::GENEITERAMIN_MEDIUM);
	FontManager::GetInstance().DrawString(kNextTextPosUpX, kNextTextPosUpY, kNextButton, kNextTextUpColor, FontSize::GENEITERAMIN_MEDIUM);
}