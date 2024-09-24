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
	// 3D�I�u�W�F�N�g�̊p�x
	const VECTOR kWinnerRota = VGet(0, 0 * DX_PI_F / 180.0f, 0);
	const VECTOR kLoserRota = VGet(0, 180 * DX_PI_F / 180.0f, -90 * DX_PI_F / 180.0f);
	// 3D�I�u�W�F�N�g�̃T�C�Y
	constexpr float kChessModelSize = 12.0f;
	const VECTOR k3DModelSize = VGet(kChessModelSize, kChessModelSize, kChessModelSize);

	//�摜�p�X
	const char* const kBgImagePath = "Data/Image/UI/GameDifficultySelectionBase.png";
	const char* const kWinPath     = "Data/Image/UI/Win.png";
	const char* const kLosePath    = "Data/Image/UI/Lose.png";

	// �e�L�X�g
	const char* const kNextButton = "A �{�^���Ń^�C�g���ɖ߂�";

	// �e�L�X�g�ʒu
	constexpr int kNextTextPosUpX = Game::kScreenWidth / 2 - 470.0f;
	constexpr int kNextTextPosUpY = Game::kScreenHeight / 2 + 200;
	constexpr int kNextTextPosDownX = kNextTextPosUpX + 5;
	constexpr int kNextTextPosDownY = kNextTextPosUpY + 5;

	// �e�L�X�g�J���[
	constexpr int kNextTextUpColor = 0xffffff;
	constexpr int kNextTextDownColor = 0x000000;

	// �w�i�摜
	constexpr int kBgPosX = Game::kScreenWidth / 2;
	constexpr int kBgPosY = Game::kScreenHeight / 2;
	constexpr float kBgSize = 1.0f;

	// ���s����
	constexpr int kResultGraphPosX = Game::kScreenWidth / 2;
	constexpr int kResultGraphPosY = Game::kScreenHeight / 2;

	// �������ꍇ�̉摜�̊p�x
	constexpr float kResultGraphRotaMax = 0.1f;
	constexpr float kResultGraphRate    = 0.001f;

	// 3D���f����`�悷��ׂ�2D����3D�֍��W��ϊ�����
	const VECTOR kWinModel2DPos  = VGet(Game::kScreenWidth / 2 - 500.0f, Game::kScreenHeight / 2 + 150.0f, 0.5f);
	const VECTOR kLoseModel2DPos = VGet(Game::kScreenWidth / 2 + 400.0f, Game::kScreenHeight / 2, 0.5f);

	// �J�����ʒu
	const VECTOR kCameraPos = VGet(0.0f, 0.0f, -10000.0f);

	// ���s���ʉ摜
	// �U���̎��g��
	constexpr float kFrequency = 0.07f;
	// �U���̐U��
	constexpr float kAmplitude = 5.0f;
}

SceneResult::SceneResult(GameResultData resultData, DifficultyData data):
	m_imageAngle(0.0f),
	m_resultY(0.0f)
{
	// ���U���g�f�[�^���擾����
	m_resultData = resultData;

	// ���������G�̃��f���̃p�X���擾����	
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

	// �J�����̃C���X�^���X����
	m_pCamera = std::make_unique<Camera>();
	// �J�����̐ݒ�
	m_pCamera->SetPos(kCameraPos);
	m_pCamera->Setting();
}

SceneResult::~SceneResult()
{
}

void SceneResult::Init()
{
	// �R���X�g���N�^�ŃJ�����̏���ݒ��X�N���[�����W����3D��ԍ��W�ւ̕ϊ����s��
	const VECTOR kWinnerPos = ConvScreenPosToWorldPos(kWinModel2DPos);
	const VECTOR kLoserPos  = ConvScreenPosToWorldPos(kLoseModel2DPos);

	// ���s�ɂ���ăv���C���[�A�G�l�~�[�̃��f���ʒu��ύX����ׂ̕ϐ�
	VECTOR playerScreenToWorldPos{};
	VECTOR playerRota{};	
	VECTOR enemyScreenToWorldPos{};
	VECTOR enemyRota{};

	// �w�i�摜�ǂݍ���
	m_hImageResultBg = LoadGraph(kBgImagePath);

	// �v���C���[�����������ꍇ
	if (m_resultData == GameResultData::CREAR)
	{
		// �摜�ǂݍ���
		m_hImageResult = LoadGraph(kWinPath);

		// ���f���̃g�����X�t�H�[��
		playerScreenToWorldPos = kWinnerPos;
		enemyScreenToWorldPos = kLoserPos;
		playerRota = kWinnerRota;
		enemyRota = kLoserRota;
	}
	// �G�l�~�[�����������ꍇ
	else if (m_resultData == GameResultData::OVER)
	{
		// �摜�ǂݍ���
		m_hImageResult = LoadGraph(kLosePath);

		// ���f���̃g�����X�t�H�[��
		playerScreenToWorldPos = kLoserPos;
		enemyScreenToWorldPos = kWinnerPos;
		playerRota = kLoserRota;
		enemyRota = kWinnerRota;
	}

	// �v���C���[�I�u�W�F�N�g�C���X�^���X����
	m_pPlayer = std::make_unique<GameObject>(
		ModelManager::GetInstance().ModelType(ModelName::Knight_W),
		playerScreenToWorldPos,
		playerRota,
		k3DModelSize
		);
	// �G�l�~�[�I�u�W�F�N�g�C���X�^���X����
	m_pEnemy = std::make_unique<GameObject>(
		m_enemyPath,
		enemyScreenToWorldPos,
		enemyRota,
		k3DModelSize
		);
}

void SceneResult::End()
{
	// �������
	DeleteGraph(m_hImageResult);
	DeleteGraph(m_hImageResultBg);
}

SceneBase* SceneResult::Update()
{
	// �v���C���[�̍X�V����
	m_pPlayer->Update();
	// �G�l�~�[�̍X�V����
	m_pEnemy->Update();

	static int timer = 0;
	m_resultY = cos(timer * kFrequency) * kAmplitude;
	timer++;

	// �s�k�����ꍇ�͉摜���X����
	if (m_resultData == GameResultData::OVER)
	{
		// �������ꍇ��BGM
		SoundManager::GetInstance().Play(SoundName::LOSE,true);

		// ResultGraph�̊p�x��ύX����
		if (m_imageAngle < kResultGraphRotaMax)
		{
			m_imageAngle += kResultGraphRate;
		}
	}

	if (m_resultData == GameResultData::CREAR)
	{
		// �������ꍇ��BGM
		SoundManager::GetInstance().Play(SoundName::WIN, true);
	}

	// �I��
	if (Pad::IsTrigger(PAD_INPUT_1))
	{
		// �T�E���h�Ǘ�
		SoundManager::GetInstance().Stop(SoundName::WIN);
		SoundManager::GetInstance().Stop(SoundName::LOSE);
		SoundManager::GetInstance().Play(SoundName::SELECT);
		timer = 0;

		// �V�[���̐؂�ւ�
		return new SceneLevelSelect();
	}
	
	return this;
}

void SceneResult::Draw()
{
	// �w�i��`��
	DrawRotaGraph(kBgPosX, kBgPosY, kBgSize, 0.0f, m_hImageResultBg, true);
	DrawRotaGraph(kResultGraphPosX, kResultGraphPosY + m_resultY, 1, m_imageAngle, m_hImageResult, true);

	// �L�����N�^�[�̕`��
	m_pPlayer->Draw();
	m_pEnemy->Draw();

	// �{�^������
	FontManager::GetInstance().DrawString(kNextTextPosDownX, kNextTextPosDownY, kNextButton, kNextTextDownColor, FontSize::GENEITERAMIN_MEDIUM);
	FontManager::GetInstance().DrawString(kNextTextPosUpX, kNextTextPosUpY, kNextButton, kNextTextUpColor, FontSize::GENEITERAMIN_MEDIUM);
}