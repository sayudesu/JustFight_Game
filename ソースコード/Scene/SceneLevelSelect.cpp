#include "SceneLevelSelect.h"
#include "SceneMain.h"
#include "SceneTitle.h"

#include "../Util/SlideSelect.h"
#include "../Util/Game.h"
#include "../Util/GameObject.h"
#include "../Util/DifficultyData.h"
#include "../Util/ModelName.h"
#include "../Util/FontSize.h"
#include "../Util/SoundName.h"
#include "../Util/SubtitleData.h"

#include "../Object/Camera/Camera.h"
#include "../CSVData/ModelManager.h"
#include "../CSVData/FontManager.h"
#include "../CSVData/SoundManager.h"
#include "../CSVData/SubtitleManager.h"

namespace
{
	// 画像の基本情報
	constexpr float kImageAngle = 0.0f * DX_PI_F / 180.0f;
	constexpr float kImagePosX  = -450.0f;
	constexpr float kImagePosY  = 0.0f;
	constexpr float kImageSize  = 0.95f;

	// 選択可能数
	constexpr int kSelectMaxNum = 2;

	// 矢のサイズの変更速度
	constexpr float kImageArrowSizeChangeSpeed = 0.002f;

	// カメラ初期位置
	constexpr float kCameraStartX = 100.0f;
	constexpr float kCameraStartY = 300.0f;
	constexpr float kCameraStartZ = 100.0f;

	// カメラターゲット位置
	const VECTOR kCameraTargetPos = VGet(0.0f, -100.0f, -10.0f);

	// カメラの停止位置
	constexpr float kCameraStopX = 0.0f;
	constexpr float kCameraStopY = 32.0f;
	constexpr float kCameraStopZ = 10.0f;

	// マップモデルサイズ
	const VECTOR kMapPos        = VGet(0.0f, 0.0f, 0.0f);
	const VECTOR kMapModelAngle = VGet(0, 0, 180 * DX_PI_F / 180.0f);
	const VECTOR kMapModelSize  = VGet(1.0f, 1.0f, 1.0f);

	// モデル設定
	const VECTOR kChessModelAngle = VGet(-90 * DX_PI_F / 180.0f, 0, 0);
	const VECTOR kChessModelSize  = VGet(0.9f, 0.9f, 0.9f);

	// 画像読み込みファイル位置
	const char* const kDifficultySelectionBasePath = "Data/Image/UI/GameDifficultySelectionBase.png";
	const char* const kSelectDifficultyPath        = "Data/Image/UI/SelectDifficulty.png";
	const char* const kDecorationPath              = "Data/Image/UI/Decoration.png";
	const char* const kNovicePath                  = "Data/Image/UI/Novice.png";
	const char* const kIntermediatePath            = "Data/Image/UI/Intermediate.png";
	const char* const kExpertPath                  = "Data/Image/UI/Expert.png";
	const char* const kCharactorBgPath             = "Data/Image/UI/CharactorBg.png";
	const char* const kArrowPath                   = "Data/Image/UI/Arrow.png";
	const char* const kOptionBackPath              = "Data/Image/UI/OptionBack.png";
	const char* const kStartBottonPath             = "Data/Image/UI/StartBotton.png";

	// 画像位置
	VECTOR kSelectDifficultyPos  = VGet(kImagePosX, -300.0f, 0);
	VECTOR kDecorationPos        = VGet(kImagePosX, -250.0f, 0);
	VECTOR kNovicePos            = VGet(kImagePosX, 30.0f, 0);
	VECTOR kIntermediatePos      = VGet(kImagePosX, 30.0f, 0);
	VECTOR kExpertPos            = VGet(kImagePosX, 30.0f, 0);
	VECTOR kImageDifficultyBgPos = VGet(kImagePosX + 800.0f, 30.0f, 0);
	VECTOR kArrowPos             = VGet(kImagePosX, -120.0f, 0);
	VECTOR kArrowPos2            = VGet(kImagePosX, 180.0f, 0);
	VECTOR kOptionBackPos        = VGet(550.0f, 400.0f, 0);
	VECTOR kOptionBottonPos      = VGet(430.0f, 400.0f, 0);

	// 画像サイズ
	constexpr float kImageDifficultyBgSize = kImageSize + 2.0f;
	constexpr float kOptionBackSize        = 0;
	constexpr float kOptionBottonSize      = 0;

	// 画像角度
	constexpr float kArrowAngle        = kImageAngle + 180.0f * DX_PI_F / 180.0f;
	constexpr float kOptionBackAngle   = 1;
	constexpr float kOptionBottonAngle = 1;

	// モデルのスライド
	constexpr float kModelSlideRate = 9.0f;
	// 背景のスライド
	constexpr float kBgSlideRate = 60.0f;

	// 勝敗結果画像
	// 振動の周波数
	constexpr float kFrequency = 0.07f;
	// 振動の振幅
	constexpr float kAmplitude = 5.0f;

	// 選択時の揺れフレーム最大
	constexpr int kShakeSelectFrameMax = 5;

	// 揺れの幅
	constexpr float kShakePower = 5.0f;

	// 難易度テキスト座標
	const float kNoviceXOffset = -130.0f;
	const float kIntermediateXOffset = -42.0f;
	const float kExpertXOffset = -60.0f;
	const float kYOffset = 240.0f;
	const float kExtraYOffset = 32.0f;
	const float kAdditionalYOffset = 16.0f;

	// テキストカラー
	const int kTextColor = 0xffffff;

	// モデル移動位置
	constexpr float modelXOffset = Game::kScreenWidth / 2 + 3900.0f;
	constexpr float modelYOffset = Game::kScreenHeight / 2 + 1500.0f;

	// カメラ加速
	constexpr float KCameraSpeedBoost = 1.1f;

	// モデルのスライドする際の位置
	constexpr float kModelSlidePosYMax = -10.0f;

	// モデルの角度
	constexpr float kModelRotateX = -90 * DX_PI_F / 180.0f;

	// 矢印画像が上に移動する量
	const float kArrowMoveUpAmount = 10.0f;    
	// 矢印画像が下に移動する量
	const float kArrowMoveDownAmount = 10.0f;  
	// 矢印画像が矢印が最大で移動する距離
	const float kArrowMaxOffset = 20.0f;       
	// 矢印画像が元の位置に戻るときの移動量
	const float kArrowResetAmount = 15.0f;     
}

SceneLevelSelect::SceneLevelSelect():
	m_hTitle(-1),
	m_isInputController(false),
	m_bgPos(VGet(static_cast<float>(Game::kScreenWidth / 2), -static_cast<float>(Game::kScreenHeight / 2), 0.0f)),
	m_cameraSpeed(0.1f),
	m_modelSlidePosY(30.0f),
	m_arrowTimer(0)
{
	m_arrowPosX[0] = 0.0f;
	m_arrowPosX[1] = 0.0f;
	m_arrowPosY[0] = 0.0f;
	m_arrowPosY[1] = 0.0f;

	m_arrowShakeX[0] = 0.0f;
	m_arrowShakeX[1] = 0.0f;
	m_arrowShakeY[0] = 0.0f;
	m_arrowShakeY[1] = 0.0f;

	m_arrowSize[0] = 0.0f;
	m_arrowSize[1] = 0.0f;

	m_cameraPosX = 0.0f;
	m_cameraPosY = 0.0f;
	m_cameraPosZ = 0.0f;

	m_difficulty = 0.0f;

	m_enemyBgShakeX = 0.0f;
	m_enemyBgShakeY = 0.0f;
}

SceneLevelSelect::~SceneLevelSelect()
{
}

void SceneLevelSelect::Init()
{
	m_arrowSize[0] = kImageSize;
	m_arrowSize[1] = kImageSize;

	// カメラの位置が正しいかどうかを確認するための初期化
	m_isCameraStop[CameraStopData::X] = false;
	m_isCameraStop[CameraStopData::Y] = false;
	m_isCameraStop[CameraStopData::Z] = false;
	m_isCameraStop[CameraStopData::ALL] = false;

	m_hTitle = LoadGraph(kDifficultySelectionBasePath);

	m_pSelect = std::make_unique<SlideSelect>();
	m_pSelect->Init(kSelectMaxNum, true);

	// カメラの座標
	m_cameraPosX = kCameraStartX;
	m_cameraPosY = kCameraStartY;
	m_cameraPosZ = kCameraStartZ;

	// カメラインスタンス
	m_pCamera = std::make_unique<Camera>();
	// カメラターゲット位置初期化
	m_pCamera->SetTargetPos(kCameraTargetPos);

	// 3Dオブジェクト
	{
		// マップ
		m_pStage = std::make_unique<GameObject>(
			ModelManager::GetInstance().ModelType(ModelName::MAP3),
			kMapPos,
			kMapModelAngle,
			kMapModelSize);
	}

	// 2Dオブジェクト
	{
		// 背景
		m_hBg = std::make_shared<GameObject>(
			kDifficultySelectionBasePath,
			m_bgPos,
			kImageAngle,
			kImageSize
		);

		// 選択文字
		m_hSelect = std::make_shared<GameObject>(
			kSelectDifficultyPath,
			kSelectDifficultyPos,
			kImageAngle,
			kImageSize,
			m_hBg.get()
		);

		// 選択文字の下の飾り
		m_hDecoration = std::make_shared<GameObject>(
			kDecorationPath,
			kDecorationPos,
			kImageAngle,
			kImageSize,
			m_hBg.get()
		);

		// 難易度
		m_hNovice = std::make_shared<GameObject>(
			kNovicePath,
			kNovicePos,
			kImageAngle,
			kImageSize,
			m_hBg.get()
		);

		m_hIntermediate = std::make_shared<GameObject>(
			kIntermediatePath,
			kIntermediatePos,
			kImageAngle,
			kImageSize,
			m_hBg.get()
		);

		m_hExpert = std::make_shared<GameObject>(
			kExpertPath,
			kExpertPos,
			kImageAngle,
			kImageSize,
			m_hBg.get()
		);

		m_hImageDifficultyBg = std::make_shared<GameObject>(
			kCharactorBgPath,
			kImageDifficultyBgPos,
			kImageAngle,
			kImageDifficultyBgSize,
			m_hBg.get()
		);

		// 矢印上向き
		m_hArrow[0] = std::make_shared<GameObject>(
			kArrowPath,
			kArrowPos,
			kArrowAngle,
			m_arrowSize[0],
			m_hBg.get()
		);

		// 矢印下向き
		m_hArrow[1] = std::make_shared<GameObject>(
			kArrowPath,
			kArrowPos2,
			kImageAngle,
			m_arrowSize[1],
			m_hBg.get()
		);

		// オプションの背景
		m_hOptionBack = std::make_shared<GameObject>(
			kOptionBackPath,
			kOptionBackPos,
			kOptionBackSize,
			kOptionBackAngle,
			m_hBg.get()
		);

		// オプションボタン
		m_hOptionBotton = std::make_shared<GameObject>(
			kStartBottonPath,
			kOptionBottonPos,
			kOptionBottonSize,
			kOptionBottonAngle,
			m_hBg.get()
		);

		m_pStage->SetPos(VGet(0.0f, 0.0f, 0.0f));
		m_pStage->SetRotate(VGet(0.0f, 0.0f, 0.0f));
		m_pStage->Update();
	}

	// モデルの読み込み
	m_hModel[0] = std::make_unique<GameObject>(
		ModelManager::GetInstance().ModelType(ModelName::Pawn_B),
		ConvScreenPosToWorldPos({ Game::kScreenWidth / 2,Game::kScreenHeight / 2,0 }),
		kChessModelAngle,
		kChessModelSize);

	m_hModel[1] = std::make_unique<GameObject>(
		ModelManager::GetInstance().ModelType(ModelName::Knight_B),
		ConvScreenPosToWorldPos({ Game::kScreenWidth / 2,Game::kScreenHeight / 2,0 }),
		kChessModelAngle,
		kChessModelSize);

	m_hModel[2] = std::make_unique<GameObject>(
		ModelManager::GetInstance().ModelType(ModelName::Queen_B),
		ConvScreenPosToWorldPos({ Game::kScreenWidth / 2,Game::kScreenHeight / 2,0 }),
		kChessModelAngle,
		kChessModelSize);

	// モデルの回転角度を初期化
	m_modelRot[0] = 0.0f;
	m_modelRot[1] = 0.0f;
	m_modelRot[2] = 0.0f;

	// 矢印の初期位置を取得
	m_firstArrowPosX[0] = m_hArrow[0]->GetPos().x;
	m_firstArrowPosY[0] = m_hArrow[0]->GetPos().y;
	m_firstArrowPosX[1] = m_hArrow[1]->GetPos().x;
	m_firstArrowPosY[1] = m_hArrow[1]->GetPos().y;
	m_arrowPosX[0]      = m_hArrow[0]->GetPos().x;
	m_arrowPosY[0]      = m_hArrow[0]->GetPos().y;
	m_arrowPosX[1]      = m_hArrow[1]->GetPos().x;
	m_arrowPosY[1]      = m_hArrow[1]->GetPos().y;

	// 矢印の振動を初期化
	m_arrowShakeX[0] = 0.0f;
	m_arrowShakeY[0] = 0.0f;
	m_arrowShakeX[1] = 0.0f;
	m_arrowShakeY[1] = 0.0f;

	// 敵の座標をしる
	m_firstEnemyBgX = m_hImageDifficultyBg->GetPos().x;
	m_firstEnemyBgY = m_hImageDifficultyBg->GetPos().y;

	m_difficulty = m_hNovice->GetSize2D();
}

void SceneLevelSelect::End()
{
	DeleteGraph(m_hTitle);
}

SceneBase* SceneLevelSelect::Update()
{
	// BGMの再生
	SoundManager::GetInstance().Play(SoundName::TITLE, true);

	// カメラが全て指定の位置に着くと処理を通す
	if (m_isCameraStop[CameraStopData::ALL])
	{
		m_pSelect->Update();
		m_modelSlidePosY -= kModelSlideRate;
	}

	// 入力待ち
	Input();

	// 選択後の背景処理
	SelectBg();

	// カメラ移動処理
	CameraMoveing();

	// モデル移動処理
	ModelMoveing();

	// 背景の更新処理
	m_hBg->Update();
	m_hSelect->Update();
	m_hDecoration->Update();
	m_hOptionBack->Update();
	m_hOptionBotton->Update();

	// 難易度
	m_hNovice->Update();
	m_hIntermediate->Update();
	m_hExpert->Update();

	m_hImageDifficultyBg->Update();

	// 矢印の更新処理
	m_hArrow[0]->Update();
	m_hArrow[1]->Update();

	// 全てのカメラの移動が停止したら
	if (m_isCameraStop[CameraStopData::ALL])
	{
		// 難易度調整
		// 弱い(チュートリアル)
		if (m_pSelect->GetResult() == static_cast<int>(DifficultyData::NOIVE))
		{
			SoundManager::GetInstance().Stop(SoundName::TITLE);
			return new SceneMain(DifficultyData::NOIVE);
		}
		// 普通
		if (m_pSelect->GetResult() == static_cast<int>(DifficultyData::INTERMEDIATE))
		{
			SoundManager::GetInstance().Stop(SoundName::TITLE);
			return new SceneMain(DifficultyData::INTERMEDIATE);
		}
		// 強い
		if (m_pSelect->GetResult() == static_cast<int>(DifficultyData::EXPERT))
		{
			SoundManager::GetInstance().Stop(SoundName::TITLE);
			return new SceneMain(DifficultyData::EXPERT);
		}
	}

	return this;
}

void SceneLevelSelect::Draw()
{
	// 描画処理
	m_pStage->Draw();

	m_hBg->Draw();
	m_hSelect->Draw();
	m_hDecoration->Draw();

	m_hImageDifficultyBg->Draw();

	m_hArrow[0]->Draw();
	m_hArrow[1]->Draw();

	// 難易度
	if (m_pSelect->GetSelect() == 0)
	{
		m_hNovice->Draw();
		if (m_isCameraStop[CameraStopData::ALL])
		{
			m_hModel[0]->Draw();
		}

		FontManager::GetInstance().DrawString(
			static_cast<int>(m_hBg->GetPos().x + m_hNovice->GetPos().x + kNoviceXOffset),
			static_cast<int>(m_hBg->GetPos().y + m_hNovice->GetPos().y + kYOffset),
			SubtitleManager::GetInstance().SubtitleStringData(Subtitle::NOVICE),
			kTextColor,
			FontSize::GENEITERAMIN_SMALL
		);

		FontManager::GetInstance().DrawString(
			static_cast<int>(m_hBg->GetPos().x + m_hNovice->GetPos().x + kNoviceXOffset),
			static_cast<int>(m_hBg->GetPos().y + m_hNovice->GetPos().y + kYOffset + kExtraYOffset + kAdditionalYOffset),
			"実装中...",
			kTextColor,
			FontSize::GENEITERAMIN_SMALL
		);
	}
	else if (m_pSelect->GetSelect() == 1)
	{
		m_hIntermediate->Draw();
		if (m_isCameraStop[CameraStopData::ALL])
		{
			m_hModel[1]->Draw();
		}

		FontManager::GetInstance().DrawString(
			static_cast<int>(m_hBg->GetPos().x + m_hNovice->GetPos().x + kIntermediateXOffset),
			static_cast<int>(m_hBg->GetPos().y + m_hNovice->GetPos().y + kYOffset),
			SubtitleManager::GetInstance().SubtitleStringData(Subtitle::INTERMEDIATE),
			kTextColor,
			FontSize::GENEITERAMIN_SMALL
		);
	}
	else if (m_pSelect->GetSelect() == 2)
	{
		m_hExpert->Draw();
		if (m_isCameraStop[CameraStopData::ALL])
		{
			m_hModel[2]->Draw();
		}

		FontManager::GetInstance().DrawString(
			static_cast<int>(m_hBg->GetPos().x + m_hNovice->GetPos().x + kExpertXOffset),
			static_cast<int>(m_hBg->GetPos().y + m_hNovice->GetPos().y + kYOffset),
			SubtitleManager::GetInstance().SubtitleStringData(Subtitle::EXPERT),
			kTextColor,
			FontSize::GENEITERAMIN_SMALL
		);
	}

#if false
	// オプションボタンの描画
	m_hOptionBack->Draw();
	m_hOptionBotton->Draw();

	FontManager::GetInstance().DrawString(
		m_hBg->GetPos().x + m_hOptionBack->GetPos().x - 30.0f,
		m_hBg->GetPos().y + m_hOptionBack->GetPos().y - 20.0f,
		SubtitleManager::GetInstance().SubtitleStringData(Subtitle::OPSION), 0xffffff, FontSize::GENEITERAMIN_SMALL);

	// ポーズ画面の描画
	PoseScreen::GetInstance().Draw();
#endif
}

void SceneLevelSelect::CameraMoveing()
{
	// カメラ処理
	m_pCamera->SetPos(VGet(m_cameraPosX, m_cameraPosY, m_cameraPosZ));

	// カメラがx,y,zそれぞれ停止したかをチェックする
	if (m_isCameraStop[CameraStopData::X] &&
		m_isCameraStop[CameraStopData::Y] && 
		m_isCameraStop[CameraStopData::Z])
	{
		// 画面の中心に移動
		if (m_bgPos.y < static_cast<float>(Game::kScreenHeight) / 2)
		{
			m_bgPos.y += kBgSlideRate;
			m_hBg->SetPos(m_bgPos);
		}
		else
		{
			// 画面の中心に固定
			m_bgPos.y = static_cast<float>(Game::kScreenHeight) / 2;

			m_isCameraStop[CameraStopData::ALL] = true;
		}
	}

	m_cameraSpeed = (m_cameraSpeed * KCameraSpeedBoost);

	if (m_cameraPosX > kCameraStopX)
	{
		m_cameraPosX -= m_cameraSpeed;
	}
	else
	{
		m_cameraPosX = kCameraStopX;
		m_isCameraStop[CameraStopData::X] = true;
	}

	if (m_cameraPosY > kCameraStopY)
	{
		m_cameraPosY -= m_cameraSpeed;
	}
	else
	{
		m_cameraPosY = kCameraStopY;
		m_isCameraStop[CameraStopData::Y] = true;
	}

	if (m_cameraPosZ > kCameraStopZ)
	{
		m_cameraPosZ -= m_cameraSpeed;
	}
	else
	{
		m_cameraPosZ = kCameraStopZ;
		m_isCameraStop[CameraStopData::Z] = true;
	}

	// カメラの位置を変更している為設定を変更
	m_pCamera->Setting();
}

void SceneLevelSelect::ModelMoveing()
{
	// 3Dモデルの描画
	// 指定の座標を超えると固定
	if (m_modelSlidePosY < kModelSlidePosYMax)
	{
		m_modelSlidePosY = kModelSlidePosYMax;
	}

	// モデルの位置をスクリーン座標からワールド座標に変換
	const VECTOR  modelPos = ConvScreenPosToWorldPos({modelXOffset,modelYOffset,0});

	for (int i = 0; i < 3; i++)
	{
		// 位置の更新
		m_hModel[i]->SetPos
		(
			{
				modelPos.x,
				modelPos.y + m_modelSlidePosY,
				modelPos.z
			}
		);

		// 回転
		m_modelRot[i]++;

		// 回転の更新
		m_hModel[i]->SetRotate
		(
			{
				kModelRotateX,
				0,
				m_modelRot[i] * DX_PI_F / 180.0f
			}
		);

		// 3Dモデルの更新
		m_hModel[i]->Update();
	}
}

void SceneLevelSelect::Input()
{
	// ボタン誘導用
	float y = cos(m_arrowTimer * kFrequency) * kAmplitude;
	m_arrowTimer++;

	// 位置の更新
	m_hArrow[0]->SetPos({ m_arrowPosX[0] + m_arrowShakeX[0],m_arrowPosY[0] + m_arrowShakeY[0] + y,0 }

	);
	m_hArrow[1]->SetPos({ m_arrowPosX[1] + m_arrowShakeX[1],m_arrowPosY[1] + m_arrowShakeY[1] + (-y),0 });

	// 上を押した場合
	if (m_pSelect->IsUpBotton())
	{
		// 矢印を指定の最大数まで上に上昇させる
		// 最終地点に到達すると乱数で揺らす
		m_arrowPosY[0] -= kArrowMoveUpAmount;
		if (m_arrowPosY[0] < m_firstArrowPosY[0] - kArrowMaxOffset)
		{
			m_arrowPosY[0] = m_firstArrowPosY[0] - kArrowMaxOffset;
			m_arrowShakeX[0] = static_cast<float>(GetRand(kShakePower)) - kShakePower;
			m_arrowShakeY[0] = static_cast<float>(GetRand(kShakePower)) - kShakePower;
		}
	}
	else
	{
		// 上を押していない場合は元の位置に戻り揺れはしない
		m_arrowPosY[0] += kArrowResetAmount;
		if (m_arrowPosY[0] > m_firstArrowPosY[0])
		{
			m_arrowPosY[0] = m_firstArrowPosY[0];
			m_arrowShakeX[0] = 0;
			m_arrowShakeY[0] = 0;
		}
	}
	// 下を押した場合
	if (m_pSelect->IsDownBotton())
	{
		// 矢印を指定の最大数まで下に下降させる
		// 最終地点に到達すると乱数で揺らす
		m_arrowPosY[1] += kArrowMoveDownAmount;
		if (m_arrowPosY[1] > m_firstArrowPosY[1] + kArrowMaxOffset)
		{
			m_arrowPosY[1] = m_firstArrowPosY[1] + kArrowMaxOffset;
			m_arrowShakeX[1] = static_cast<float>(GetRand(kShakePower)) - kShakePower;
			m_arrowShakeY[1] = static_cast<float>(GetRand(kShakePower)) - kShakePower;
		}
	}
	else
	{
		// 下を押していない場合は元の位置に戻り揺れはしない
		m_arrowPosY[1] -= kArrowResetAmount;
		if (m_arrowPosY[1] < m_firstArrowPosY[1])
		{
			m_arrowPosY[1] = m_firstArrowPosY[1];
			m_arrowShakeX[1] = 0.0f;
			m_arrowShakeY[1] = 0.0f;
		}
	}
}

void SceneLevelSelect::SelectBg()
{
	// 位置の更新
	m_hImageDifficultyBg->SetPos
	(
		{
			m_firstEnemyBgX + m_enemyBgShakeX,
			m_firstEnemyBgY + m_enemyBgShakeY,
			m_hImageDifficultyBg->GetPos().x,
		}
	);

	// 選択を変更した場合
	if (m_tempSelect != m_pSelect->GetSelect())
	{
		m_isEnemyBgShake = true;
	}

	// 指定のフレーム数の間揺らす
	if (m_isEnemyBgShake)
	{
		// 揺らすタイムをカウント
		m_enemyBgshakeCount++;

		// 座標の変更
		m_enemyBgShakeX = static_cast<float>(GetRand(kShakePower)) - kShakePower;
		m_enemyBgShakeY = static_cast<float>(GetRand(kShakePower)) - kShakePower;

		// 指定のフレームで揺れを終了させる
		if (m_enemyBgshakeCount == kShakeSelectFrameMax)
		{
			// リセット
			m_enemyBgshakeCount = 0;
			m_isEnemyBgShake = false;
		}
	}
	else
	{
		// 揺れ用座標のリセット
		m_enemyBgShakeX = 0;
		m_enemyBgShakeY = 0;
	}

	// 現在の選択を一時的に記録
	m_tempSelect = m_pSelect->GetSelect();
}