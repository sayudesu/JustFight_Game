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
	// �摜�̊�{���
	constexpr float kImageAngle = 0.0f * DX_PI_F / 180.0f;
	constexpr float kImagePosX  = -450.0f;
	constexpr float kImagePosY  = 0.0f;
	constexpr float kImageSize  = 0.95f;

	// �I���\��
	constexpr int kSelectMaxNum = 2;

	// ��̃T�C�Y�̕ύX���x
	constexpr float kImageArrowSizeChangeSpeed = 0.002f;

	// �J���������ʒu
	constexpr float kCameraStartX = 100.0f;
	constexpr float kCameraStartY = 300.0f;
	constexpr float kCameraStartZ = 100.0f;

	// �J�����^�[�Q�b�g�ʒu
	const VECTOR kCameraTargetPos = VGet(0.0f, -100.0f, -10.0f);

	// �J�����̒�~�ʒu
	constexpr float kCameraStopX = 0.0f;
	constexpr float kCameraStopY = 32.0f;
	constexpr float kCameraStopZ = 10.0f;

	// �}�b�v���f���T�C�Y
	const VECTOR kMapPos        = VGet(0.0f, 0.0f, 0.0f);
	const VECTOR kMapModelAngle = VGet(0, 0, 180 * DX_PI_F / 180.0f);
	const VECTOR kMapModelSize  = VGet(1.0f, 1.0f, 1.0f);

	// ���f���ݒ�
	const VECTOR kChessModelAngle = VGet(-90 * DX_PI_F / 180.0f, 0, 0);
	const VECTOR kChessModelSize  = VGet(0.9f, 0.9f, 0.9f);

	// �摜�ǂݍ��݃t�@�C���ʒu
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

	// �摜�ʒu
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

	// �摜�T�C�Y
	constexpr float kImageDifficultyBgSize = kImageSize + 2.0f;
	constexpr float kOptionBackSize        = 0;
	constexpr float kOptionBottonSize      = 0;

	// �摜�p�x
	constexpr float kArrowAngle        = kImageAngle + 180.0f * DX_PI_F / 180.0f;
	constexpr float kOptionBackAngle   = 1;
	constexpr float kOptionBottonAngle = 1;

	// ���f���̃X���C�h
	constexpr float kModelSlideRate = 9.0f;
	// �w�i�̃X���C�h
	constexpr float kBgSlideRate = 60.0f;

	// ���s���ʉ摜
	// �U���̎��g��
	constexpr float kFrequency = 0.07f;
	// �U���̐U��
	constexpr float kAmplitude = 5.0f;

	// �I�����̗h��t���[���ő�
	constexpr int kShakeSelectFrameMax = 5;

	// �h��̕�
	constexpr float kShakePower = 5.0f;

	// ��Փx�e�L�X�g���W
	const float kNoviceXOffset = -130.0f;
	const float kIntermediateXOffset = -42.0f;
	const float kExpertXOffset = -60.0f;
	const float kYOffset = 240.0f;
	const float kExtraYOffset = 32.0f;
	const float kAdditionalYOffset = 16.0f;

	// �e�L�X�g�J���[
	const int kTextColor = 0xffffff;

	// ���f���ړ��ʒu
	constexpr float modelXOffset = Game::kScreenWidth / 2 + 3900.0f;
	constexpr float modelYOffset = Game::kScreenHeight / 2 + 1500.0f;

	// �J��������
	constexpr float KCameraSpeedBoost = 1.1f;

	// ���f���̃X���C�h����ۂ̈ʒu
	constexpr float kModelSlidePosYMax = -10.0f;

	// ���f���̊p�x
	constexpr float kModelRotateX = -90 * DX_PI_F / 180.0f;

	// ���摜����Ɉړ������
	const float kArrowMoveUpAmount = 10.0f;    
	// ���摜�����Ɉړ������
	const float kArrowMoveDownAmount = 10.0f;  
	// ���摜����󂪍ő�ňړ����鋗��
	const float kArrowMaxOffset = 20.0f;       
	// ���摜�����̈ʒu�ɖ߂�Ƃ��̈ړ���
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

	// �J�����̈ʒu�����������ǂ������m�F���邽�߂̏�����
	m_isCameraStop[CameraStopData::X] = false;
	m_isCameraStop[CameraStopData::Y] = false;
	m_isCameraStop[CameraStopData::Z] = false;
	m_isCameraStop[CameraStopData::ALL] = false;

	m_hTitle = LoadGraph(kDifficultySelectionBasePath);

	m_pSelect = std::make_unique<SlideSelect>();
	m_pSelect->Init(kSelectMaxNum, true);

	// �J�����̍��W
	m_cameraPosX = kCameraStartX;
	m_cameraPosY = kCameraStartY;
	m_cameraPosZ = kCameraStartZ;

	// �J�����C���X�^���X
	m_pCamera = std::make_unique<Camera>();
	// �J�����^�[�Q�b�g�ʒu������
	m_pCamera->SetTargetPos(kCameraTargetPos);

	// 3D�I�u�W�F�N�g
	{
		// �}�b�v
		m_pStage = std::make_unique<GameObject>(
			ModelManager::GetInstance().ModelType(ModelName::MAP3),
			kMapPos,
			kMapModelAngle,
			kMapModelSize);
	}

	// 2D�I�u�W�F�N�g
	{
		// �w�i
		m_hBg = std::make_shared<GameObject>(
			kDifficultySelectionBasePath,
			m_bgPos,
			kImageAngle,
			kImageSize
		);

		// �I�𕶎�
		m_hSelect = std::make_shared<GameObject>(
			kSelectDifficultyPath,
			kSelectDifficultyPos,
			kImageAngle,
			kImageSize,
			m_hBg.get()
		);

		// �I�𕶎��̉��̏���
		m_hDecoration = std::make_shared<GameObject>(
			kDecorationPath,
			kDecorationPos,
			kImageAngle,
			kImageSize,
			m_hBg.get()
		);

		// ��Փx
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

		// �������
		m_hArrow[0] = std::make_shared<GameObject>(
			kArrowPath,
			kArrowPos,
			kArrowAngle,
			m_arrowSize[0],
			m_hBg.get()
		);

		// ��󉺌���
		m_hArrow[1] = std::make_shared<GameObject>(
			kArrowPath,
			kArrowPos2,
			kImageAngle,
			m_arrowSize[1],
			m_hBg.get()
		);

		// �I�v�V�����̔w�i
		m_hOptionBack = std::make_shared<GameObject>(
			kOptionBackPath,
			kOptionBackPos,
			kOptionBackSize,
			kOptionBackAngle,
			m_hBg.get()
		);

		// �I�v�V�����{�^��
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

	// ���f���̓ǂݍ���
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

	// ���f���̉�]�p�x��������
	m_modelRot[0] = 0.0f;
	m_modelRot[1] = 0.0f;
	m_modelRot[2] = 0.0f;

	// ���̏����ʒu���擾
	m_firstArrowPosX[0] = m_hArrow[0]->GetPos().x;
	m_firstArrowPosY[0] = m_hArrow[0]->GetPos().y;
	m_firstArrowPosX[1] = m_hArrow[1]->GetPos().x;
	m_firstArrowPosY[1] = m_hArrow[1]->GetPos().y;
	m_arrowPosX[0]      = m_hArrow[0]->GetPos().x;
	m_arrowPosY[0]      = m_hArrow[0]->GetPos().y;
	m_arrowPosX[1]      = m_hArrow[1]->GetPos().x;
	m_arrowPosY[1]      = m_hArrow[1]->GetPos().y;

	// ���̐U����������
	m_arrowShakeX[0] = 0.0f;
	m_arrowShakeY[0] = 0.0f;
	m_arrowShakeX[1] = 0.0f;
	m_arrowShakeY[1] = 0.0f;

	// �G�̍��W������
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
	// BGM�̍Đ�
	SoundManager::GetInstance().Play(SoundName::TITLE, true);

	// �J�������S�Ďw��̈ʒu�ɒ����Ə�����ʂ�
	if (m_isCameraStop[CameraStopData::ALL])
	{
		m_pSelect->Update();
		m_modelSlidePosY -= kModelSlideRate;
	}

	// ���͑҂�
	Input();

	// �I����̔w�i����
	SelectBg();

	// �J�����ړ�����
	CameraMoveing();

	// ���f���ړ�����
	ModelMoveing();

	// �w�i�̍X�V����
	m_hBg->Update();
	m_hSelect->Update();
	m_hDecoration->Update();
	m_hOptionBack->Update();
	m_hOptionBotton->Update();

	// ��Փx
	m_hNovice->Update();
	m_hIntermediate->Update();
	m_hExpert->Update();

	m_hImageDifficultyBg->Update();

	// ���̍X�V����
	m_hArrow[0]->Update();
	m_hArrow[1]->Update();

	// �S�ẴJ�����̈ړ�����~������
	if (m_isCameraStop[CameraStopData::ALL])
	{
		// ��Փx����
		// �ア(�`���[�g���A��)
		if (m_pSelect->GetResult() == static_cast<int>(DifficultyData::NOIVE))
		{
			SoundManager::GetInstance().Stop(SoundName::TITLE);
			return new SceneMain(DifficultyData::NOIVE);
		}
		// ����
		if (m_pSelect->GetResult() == static_cast<int>(DifficultyData::INTERMEDIATE))
		{
			SoundManager::GetInstance().Stop(SoundName::TITLE);
			return new SceneMain(DifficultyData::INTERMEDIATE);
		}
		// ����
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
	// �`�揈��
	m_pStage->Draw();

	m_hBg->Draw();
	m_hSelect->Draw();
	m_hDecoration->Draw();

	m_hImageDifficultyBg->Draw();

	m_hArrow[0]->Draw();
	m_hArrow[1]->Draw();

	// ��Փx
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
			"������...",
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
	// �I�v�V�����{�^���̕`��
	m_hOptionBack->Draw();
	m_hOptionBotton->Draw();

	FontManager::GetInstance().DrawString(
		m_hBg->GetPos().x + m_hOptionBack->GetPos().x - 30.0f,
		m_hBg->GetPos().y + m_hOptionBack->GetPos().y - 20.0f,
		SubtitleManager::GetInstance().SubtitleStringData(Subtitle::OPSION), 0xffffff, FontSize::GENEITERAMIN_SMALL);

	// �|�[�Y��ʂ̕`��
	PoseScreen::GetInstance().Draw();
#endif
}

void SceneLevelSelect::CameraMoveing()
{
	// �J��������
	m_pCamera->SetPos(VGet(m_cameraPosX, m_cameraPosY, m_cameraPosZ));

	// �J������x,y,z���ꂼ���~���������`�F�b�N����
	if (m_isCameraStop[CameraStopData::X] &&
		m_isCameraStop[CameraStopData::Y] && 
		m_isCameraStop[CameraStopData::Z])
	{
		// ��ʂ̒��S�Ɉړ�
		if (m_bgPos.y < static_cast<float>(Game::kScreenHeight) / 2)
		{
			m_bgPos.y += kBgSlideRate;
			m_hBg->SetPos(m_bgPos);
		}
		else
		{
			// ��ʂ̒��S�ɌŒ�
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

	// �J�����̈ʒu��ύX���Ă���אݒ��ύX
	m_pCamera->Setting();
}

void SceneLevelSelect::ModelMoveing()
{
	// 3D���f���̕`��
	// �w��̍��W�𒴂���ƌŒ�
	if (m_modelSlidePosY < kModelSlidePosYMax)
	{
		m_modelSlidePosY = kModelSlidePosYMax;
	}

	// ���f���̈ʒu���X�N���[�����W���烏�[���h���W�ɕϊ�
	const VECTOR  modelPos = ConvScreenPosToWorldPos({modelXOffset,modelYOffset,0});

	for (int i = 0; i < 3; i++)
	{
		// �ʒu�̍X�V
		m_hModel[i]->SetPos
		(
			{
				modelPos.x,
				modelPos.y + m_modelSlidePosY,
				modelPos.z
			}
		);

		// ��]
		m_modelRot[i]++;

		// ��]�̍X�V
		m_hModel[i]->SetRotate
		(
			{
				kModelRotateX,
				0,
				m_modelRot[i] * DX_PI_F / 180.0f
			}
		);

		// 3D���f���̍X�V
		m_hModel[i]->Update();
	}
}

void SceneLevelSelect::Input()
{
	// �{�^���U���p
	float y = cos(m_arrowTimer * kFrequency) * kAmplitude;
	m_arrowTimer++;

	// �ʒu�̍X�V
	m_hArrow[0]->SetPos({ m_arrowPosX[0] + m_arrowShakeX[0],m_arrowPosY[0] + m_arrowShakeY[0] + y,0 }

	);
	m_hArrow[1]->SetPos({ m_arrowPosX[1] + m_arrowShakeX[1],m_arrowPosY[1] + m_arrowShakeY[1] + (-y),0 });

	// ����������ꍇ
	if (m_pSelect->IsUpBotton())
	{
		// �����w��̍ő吔�܂ŏ�ɏ㏸������
		// �ŏI�n�_�ɓ��B����Ɨ����ŗh�炷
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
		// ��������Ă��Ȃ��ꍇ�͌��̈ʒu�ɖ߂�h��͂��Ȃ�
		m_arrowPosY[0] += kArrowResetAmount;
		if (m_arrowPosY[0] > m_firstArrowPosY[0])
		{
			m_arrowPosY[0] = m_firstArrowPosY[0];
			m_arrowShakeX[0] = 0;
			m_arrowShakeY[0] = 0;
		}
	}
	// �����������ꍇ
	if (m_pSelect->IsDownBotton())
	{
		// �����w��̍ő吔�܂ŉ��ɉ��~������
		// �ŏI�n�_�ɓ��B����Ɨ����ŗh�炷
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
		// ���������Ă��Ȃ��ꍇ�͌��̈ʒu�ɖ߂�h��͂��Ȃ�
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
	// �ʒu�̍X�V
	m_hImageDifficultyBg->SetPos
	(
		{
			m_firstEnemyBgX + m_enemyBgShakeX,
			m_firstEnemyBgY + m_enemyBgShakeY,
			m_hImageDifficultyBg->GetPos().x,
		}
	);

	// �I����ύX�����ꍇ
	if (m_tempSelect != m_pSelect->GetSelect())
	{
		m_isEnemyBgShake = true;
	}

	// �w��̃t���[�����̊ԗh�炷
	if (m_isEnemyBgShake)
	{
		// �h�炷�^�C�����J�E���g
		m_enemyBgshakeCount++;

		// ���W�̕ύX
		m_enemyBgShakeX = static_cast<float>(GetRand(kShakePower)) - kShakePower;
		m_enemyBgShakeY = static_cast<float>(GetRand(kShakePower)) - kShakePower;

		// �w��̃t���[���ŗh����I��������
		if (m_enemyBgshakeCount == kShakeSelectFrameMax)
		{
			// ���Z�b�g
			m_enemyBgshakeCount = 0;
			m_isEnemyBgShake = false;
		}
	}
	else
	{
		// �h��p���W�̃��Z�b�g
		m_enemyBgShakeX = 0;
		m_enemyBgShakeY = 0;
	}

	// ���݂̑I�����ꎞ�I�ɋL�^
	m_tempSelect = m_pSelect->GetSelect();
}