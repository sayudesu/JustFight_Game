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
	// 3D�I�u�W�F�N�g�̊p�x
	const VECTOR kWinnerRota  = VGet(0, 0   * DX_PI_F / 180.0f, 0);
	const VECTOR kWinnerRota2 = VGet(0, 180 * DX_PI_F / 180.0f, 0);

	//�I���{�^���p����
	const char* const kPresskey = "Press any key";

	// �e�L�X�g�ʒu
	constexpr int kTextPosUpX = Game::kScreenWidth / 2 - 272 + 12;
	constexpr int kTextPosUpY = Game::kScreenHeight / 2 + 256;

	constexpr int kTextPosDownX = kTextPosUpX + 5;
	constexpr int kTextPosDownY = kTextPosUpY + 5;

	// �e�L�X�g�J���[
	constexpr int kTextUpColor   = 0xffffff;
	constexpr int kTextDownColor = 0x111111;

	// �摜�p�X
	const char* const kTitleImagePath = "Data/Image/Title.png";
	const char* const kBgImagePath = "Data/Image/UI/GameDifficultySelectionBase.png";

	// 3D���f����`�悷��ׂ�2D����3D�֍��W��ϊ�����
	const VECTOR kWhiteModelPos = VGet(Game::kScreenWidth / 2 - 700.0f, Game::kScreenHeight / 2 + 150.0f, 0.5f);
	const VECTOR kBlackModelPos = VGet(Game::kScreenWidth / 2 + 700.0f, Game::kScreenHeight / 2 + 150.0f, 0.5f);

	// �I�u�W�F�N�g�T�C�Y
	const VECTOR kModelSize = VGet(12.0f, 12.0f, 12.0f);

	// �^�C�g���摜
	constexpr int kTitleGprahPosX = Game::kScreenWidth / 2;
	constexpr int kTitleGprahPosY = Game::kScreenHeight / 2;	

	// �e�L�X�g�̃A���t�@�l�����p
	// �U���̎��g��
	constexpr float kTextFrequency = 0.06f;
	// �U���̐U��
	constexpr float kTextkAmplitude = 255.0f;

	// ���f���̉�]�p
	// �U���̎��g��
	constexpr float kModelFrequency = 0.007f;
	// �U���̐U��
	constexpr float kModelkAmplitude = 15.0f;
}

SceneTitle::SceneTitle():
	m_hTitle(-1),
	m_hBg(-1),
	m_blendAlpha(0)
{
	// �摜�̓ǂݍ���
	m_hTitle = LoadGraph(kTitleImagePath);
	m_hBg    = LoadGraph(kBgImagePath);
}

SceneTitle::~SceneTitle()
{
}

void SceneTitle::Init()
{
	// �R���X�g���N�^�ŃJ�����̏���ݒ��X�N���[�����W����3D��ԍ��W�ւ̕ϊ����s��
	const VECTOR whiteModelPos = ConvScreenPosToWorldPos(kWhiteModelPos);
	const VECTOR blackModelPos = ConvScreenPosToWorldPos(kBlackModelPos);
	
	// �v���C���[�I�u�W�F�N�g�C���X�^���X����
	m_pPlayer = std::make_unique<GameObject>(
		ModelManager::GetInstance().ModelType(ModelName::Knight_W),
		whiteModelPos,
		kWinnerRota,
		kModelSize
		);
	// �G�l�~�[�I�u�W�F�N�g�C���X�^���X����
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
	// ���������
	DeleteGraph(m_hTitle);
	DeleteGraph(m_hBg);
}

SceneBase* SceneTitle::Update()
{	
	// �^�C�g���摜�̈ړ��̌v�Z
	static float timer = 0.0f;
	m_blendAlpha = cos(timer * kTextFrequency) * kTextkAmplitude;
	timer++;

	// ���f���̉�]�̌v�Z
	static float rota = 0.0f;
	const float rotaModel = cos(rota * kModelFrequency) * kModelkAmplitude;
	rota++;

	// �{�^�����������ꍇ
	if (Pad::IsTrigger(PAD_INPUT_1) || 
		Pad::IsTrigger(PAD_INPUT_2) || 
		Pad::IsTrigger(PAD_INPUT_3) || 
		Pad::IsTrigger(PAD_INPUT_4))
	{
		timer = 0;
		rota  = 0;
		return new SceneLevelSelect();
	}

	// 3D���f���̊p�x�ύX
	m_pPlayer->SetRotate(VGet(kWinnerRota.x, kWinnerRota.y + rotaModel, kWinnerRota.z));
	m_pEnemy->SetRotate(VGet(kWinnerRota2.x, kWinnerRota2.y + rotaModel, kWinnerRota2.z));

	// 3D���f���̍X�V����
	m_pPlayer->Update();
	m_pEnemy->Update();

	return this;
}

void SceneTitle::Draw()
{
	// �w�i
	DrawExtendGraph(0, 0, Game::kScreenWidth, Game::kScreenHeight, m_hBg, true);

	// �^�C�g���`��
	DrawRotaGraph(kTitleGprahPosX, kTitleGprahPosY, 1, 0.0f, m_hTitle, true);

	// 3D���f���`��
	m_pPlayer->Draw();
	m_pEnemy->Draw();

	// �{�^���U���`��
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(m_blendAlpha));
	FontManager::GetInstance().DrawString(kTextPosDownX, kTextPosDownY, kPresskey, kTextDownColor, FontSize::GENEITERAMIN_MEDIUM);
	FontManager::GetInstance().DrawString(kTextPosUpX, kTextPosUpY, kPresskey, kTextUpColor, FontSize::GENEITERAMIN_MEDIUM);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}