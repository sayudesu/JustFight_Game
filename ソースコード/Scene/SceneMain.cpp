#include <DxLib.h>

#include "SceneMain.h"
#include "SceneResult.h"

#include "../Object/Camera/Camera.h"
#include "../Object/Player/Player.h"
#include "../Object/Enemy/Enemy.h"
#include "../Object/CharacterBase.h"
#include "../Object/Stage/FIeldDrawer.h"

#include "../Util/Collision3D.h"
#include "../Util/EffekseerDrawer.h"
#include "../Util/Game.h"
#include "../Util/Pad.h"
#include "../Util/BloodDrawer.h"
#include "../Util/CharacterName.h"
#include "../Util/BlurScreen.h"
#include "../Util/TutorialDrawer.h"
#include "../Util/Tips.h"
#include "../Util/SoundName.h"

#include "../UI/UIDrawer.h";

#include "../CSVData/SoundManager.h"

namespace
{
	// �v���C���[�̔ԍ����w�肷��
	constexpr int kPlayerNo = static_cast<int>(CharacterName::PLAYER);
	// �G�l�~�[�̔ԍ����w�肷��
	constexpr int kEnemyNo = static_cast<int>(CharacterName::ENEMY);
	// �v���C���[�A�G�l�~�[�̐�
	constexpr int kCharactorMaxNum = 2;

	// ���s���t�������ɕ`�悷��摜�ʒu
	static const char* const kCheckmateGraphPath = "Data/Image/UI/Checkmate.png";

	// ���s�����܂����ۂɃ`�F�b�N���C�g�摜���`�悳���܂ł̃t���[��
	constexpr int kCheckmateDrawFrame = 20;

	// �V�[�����ڍs����܂ł̃t���[��
	constexpr int kResultSceneChengeFrame = 60 * 3;

	// �`�F�b�N���C�g�摜�̃T�C�Y�̍ő�
	constexpr float kCheckmateGraphSizeMax = 1.0f;

	// �`�F�b�N���C�g�摜���ő�T�C�Y�܂ŏ���������l
	constexpr float kCheckmateGraphSizeMinRate = 1.0f;

	// �`�F�b�N���C�g�摜�̊p�x�̍ŏI�̒l
	constexpr float kCheckmateGraphEndRota = 0.0f;

	// �`�F�b�N���C�g�摜�̊p�x�̕ύX����l
	constexpr float kCheckMateGraphRate = 0.5f;

	// ���s�����̔w�i�̃A���t�@�l��ύX����ő�l
	constexpr int kResultAlphaRateMax = 128;

	// ���s�����̔w�i�̃A���t�@�l��ύX����l
	constexpr int kResultAlphaRate = 5;

	// ���s�����̔w�i�J���[
	constexpr int kResultColor = 0x000000;

	// ����`�悷��p
	constexpr int kbloodNum = 100;

	// �v���C���[�A�G�l�~�[�̏����ʒu
	static const VECTOR kInitPlayerPos = VGet(-300.0f, 300.0f, 0.0f);
	static const VECTOR kInitEnemyPos = VGet(300.0f, 300.0f, 0.0f);

	// �q�b�g�X�g�b�v��~�t���[��
	constexpr int kHitStopFrameMax = 4;

	// ���s���ʉ摜
	// �U���̎��g��
	constexpr float kFrequency = 0.07f;
	constexpr float kFrequencyTimes = 100.0f;
	// �U���̐U��
	constexpr float kAmplitude = static_cast<float>(Game::kScreenHeight) / 2.0f - 160.0f;

	// ���U���m�b�N�o�b�N
	constexpr float kStrongAttackKnockBack = -30.0f;
	// �ʏ�U���m�b�N�o�b�N
	constexpr float kNormalAttackKnockBack = -10.0f;

	// �K�[�h���Ă��ꍇ�̃m�b�N�o�b�N
	constexpr float kGuardKnockBack = -20.0f;
}

SceneMain::SceneMain(DifficultyData data):
	m_pUpdateFunc(nullptr),
	m_pCamera(nullptr),
	m_hCheckmate(-1),
	m_resultData(GameResultData::NONE),
	m_frameCount(0),
	m_difficultyData(data),
	m_checkmateSize(10.0f),
	m_checkmateRota(10.0f),
	m_checkmateBgBlendRate(0),
	m_checkmatePosY(0.0f),
	m_isBlur(false),
	m_isHit(false),
	m_hitFrameCount(0),
	m_hitStopFrame(0)
{
}

SceneMain::~SceneMain()
{
}

void SceneMain::Init()
{	
	// �V�[���J��
	m_pUpdateFunc = &SceneMain::UpdateGamePlay;

	// �L�����N�^�[�N���X
	m_pCharacter[kPlayerNo] = std::make_shared<Player>(m_difficultyData, kInitPlayerPos);
	m_pCharacter[kEnemyNo]  = std::make_shared<Enemy>(m_difficultyData, kInitEnemyPos);

	m_pCamera   = std::make_unique<Camera>();        // �J�����N���X
	m_pField    = std::make_unique<FieldDrawer>();   // �t�B�[���h�`��N���X
	m_pUi       = std::make_unique<UIDrawer>();      // UI�`��N���X
	m_pTutorial = std::make_unique<TutorialDrawer>();// �`���[�g���A���N���X

	// ������
	m_pCharacter[kPlayerNo]->Init();
	m_pCharacter[kEnemyNo]->Init();
	m_pCamera->Setting();
	m_pField->Init();
	m_pTutorial->Init();

	// �`�F�b�N���C�g�摜�̓ǂݍ���
	m_hCheckmate = LoadGraph(kCheckmateGraphPath);

	// �p�����[�^�̍X�V
	CheckParameter(false,false);

	// �X�N���[�����ʂ̏�����
	EffectScreen::GetInstance().BlurIReplayInit();	
}

void SceneMain::End()
{
	// �������
	m_pCharacter[kPlayerNo]->End();
	m_pCharacter[kEnemyNo]->End();

	m_pField->End();
	m_pTutorial->End();

	// �_���[�W�G�t�F�N�g�̃��������
	for (size_t i = 0; i < m_pBlood.size(); i++)
	{
		// �f���[�g����
		delete m_pBlood[i];
		m_pBlood[i] = nullptr;
	}

	// �`�F�b�N���C�g�摜�̃��������
	DeleteGraph(m_hCheckmate);
}

SceneBase* SceneMain::Update()
{
	// �V�[���J��
	return (this->*m_pUpdateFunc)();
}

SceneBase* SceneMain::UpdateGamePlay()
{
	// BGM�̍Đ�
	SoundManager::GetInstance().Play(SoundName::PLAY, true);

	if (m_hitStopFrame == 0)
	{
		// ���͏���
		InputCharacter();
		// �X�V����
		UpdateCharacter();
		// �q�b�g�X�g�b�v�t���[���̃��Z�b�g
		m_hitStopFrame = 0;
	}
	else
	{
		// �q�b�g�X�g�b�v�t���[���𐔂����炷
		m_hitStopFrame--;
	}

	// �G�̍U���\�͈͂ɂ��邩�ǂ���
	if (CheckModelAboutHIt(m_pCharacter[kPlayerNo], m_pCharacter[kEnemyNo]))
	{
		m_pCharacter[kEnemyNo]->SetAttackRange(true);
	}
	else
	{
		m_pCharacter[kEnemyNo]->SetAttackRange(false);
	}

	// �J�����Ƀv���C���[�ƃG�l�~�[�̈ʒu��n��
	m_pCamera->SetTargetPos(m_pCharacter[kPlayerNo]->GetPos());
	// �J�����Ƀv���C���[�̊p�x�ƈʒu��n��
	m_pCamera->SetPlayerAngle(m_pCharacter[kPlayerNo]->GetAngle());
		// �J�����̍X�V����
	m_pCamera->Update();

	// ���s�̊m�F
	CheckResult();

	// �v���C���[�ɍU�����q�b�g�������ǂ���
	const bool isDamageBlur = m_pCharacter[kPlayerNo]->IsHitDamage() && !m_pCharacter[kPlayerNo]->IsGuard();
	// �G�l�~�[�ɍU�����q�b�g�������ǂ���
	const bool isDamageBlurEnemy = m_pCharacter[kEnemyNo]->IsHitDamage() && !m_pCharacter[kEnemyNo]->IsGuard();

	// �v���C���[������s�����Ă��邩�ǂ���
	const bool isAwayBlur = m_pCharacter[kPlayerNo]->IsAway();

	// �U�����q�b�g���Ă���A�v���C���[��������Ă���ꍇ�Ƀu���[���ʂ��I���ɂ���
	m_isBlur = isDamageBlur || isAwayBlur;

	// �U�������ꂽ�ꍇ
	if (isDamageBlur)
	{
		m_hitFrameCount++;
	}
	else if (isDamageBlurEnemy)
	{
		m_hitFrameCount++;
	}
	else
	{
		m_hitFrameCount = 0;
	}

	// �q�b�g���肪����ꍇ
	if (m_hitFrameCount == 1)
	{
		m_isHit = true;
	}

	// �p�����[�^�̍X�V
	CheckParameter(isDamageBlur, m_isHit);

	// �U�������������ꍇ��~�t���[�������Ă�����
	if (m_isHit)
	{
		m_hitStopFrame = kHitStopFrameMax;
		m_isHit = false;
	}

	// �_���[�W�G�t�F�N�g���X�V
	for (auto& blood : m_pBlood)
	{
		blood->Update();
	}
	// �폜�ł���v�f��T��
	for (int i = 0; i < m_pBlood.size(); i++)
	{
		if (m_pBlood[i]->IsGetErase())
		{
			// �f���[�g����
			delete m_pBlood[i];
			m_pBlood[i] = nullptr;
			// �v�f�̍폜
			m_pBlood.erase(m_pBlood.begin() + i);
		}
	}

	// ��ʐU���X�V����
	EffectScreen::GetInstance().QuakeUpdate();

	return this;
}

SceneBase* SceneMain::UpdateGameResult()
{
	// �w��t���[���̌�Ƀ��U���g��ʂɈړ�����
	m_frameCount++;

	// �{�^�����������ꍇ
	if (Pad::IsTrigger(PAD_INPUT_1))
	{
		// BGM�̒�~
		SoundManager::GetInstance().Stop(SoundName::PLAY);
		return new SceneResult(m_resultData, m_difficultyData);
	}

	// �w�肵���t���[���܂ŃJ�E���g���i�ނ�
	if (m_frameCount >= kResultSceneChengeFrame)
	{
		// BGM�̒�~
		SoundManager::GetInstance().Stop(SoundName::PLAY);
		return new SceneResult(m_resultData, m_difficultyData);
	}

	// �摜�̈ʒu�𓮂����v�Z
	m_checkmatePosY = cosf(static_cast<float>(m_frameCount) * kFrequency) * kFrequencyTimes + kAmplitude;
	// �\�߂ł��������T�C�Y��1�ɂ���
	if (m_checkmateSize > kCheckmateGraphSizeMax)
	{
		m_checkmateSize -= kCheckmateGraphSizeMinRate;
	}
	// �\�ߕύX�����p�x��0�ɂ���
	if (m_checkmateRota > kCheckmateGraphEndRota)
	{
		m_checkmateRota -= kCheckMateGraphRate;
	}

	// �u�����h����ύX
	if (m_checkmateBgBlendRate < kResultAlphaRateMax)
	{
		m_checkmateBgBlendRate += kResultAlphaRate;
	}

	return this;
}

void SceneMain::InputCharacter()
{
	// if ��Փx�����A�傾������
	// else if ��Փx������������ �`���[�g���A��
	if (m_difficultyData == DifficultyData::INTERMEDIATE ||
		m_difficultyData == DifficultyData::EXPERT)
	{
		m_pCharacter[kPlayerNo]->Input();
		m_pCharacter[kEnemyNo]->Input();
	}
	else if (m_difficultyData == DifficultyData::NOIVE)
	{
		m_pCharacter[kPlayerNo]->InputTutorial();
		m_pCharacter[kEnemyNo]->InputTutorial();
		m_pTutorial->SetTips(Tips::MOVE);

		if (m_pCharacter[kPlayerNo]->GetTipsMove(Tips::ATTACK))
		{
			m_pTutorial->SetTips(Tips::ATTACK);
		}
		if (m_pCharacter[kPlayerNo]->GetTipsMove(Tips::GUARD))
		{
			m_pTutorial->SetTips(Tips::GUARD);
		}
	}
}

void SceneMain::UpdateCharacter()
{
	// �L�����N�^�[�̍X�V����
	UpdateCharacter(m_pCharacter[kPlayerNo], m_pCharacter[kEnemyNo], true);
	UpdateCharacter(m_pCharacter[kEnemyNo], m_pCharacter[kPlayerNo], false);
}

void SceneMain::CheckParameter(bool isDamage,bool isHit)
{
	// UI�Ƀp�����[�^�[�̏�Ԃ�n��
	for (int i = 0; i < kCharactorMaxNum; i++)
	{
		m_pUi->SetParam(
			m_pCharacter[i]->GetMyId(),
			m_pCharacter[i]->GetHp(),
			m_pCharacter[i]->GetMaxHp(),
			m_pCharacter[i]->GetStrongPower(),
			m_pCharacter[i]->GetkStrongAttackPowerMax(),
			static_cast<int>(m_pCharacter[i]->GetFightingMeter()),
			isDamage,
			isHit);
	}
}

void SceneMain::Draw()
{	
	// �V������ʂ̍쐬
	EffectScreen::GetInstance().BlurPreRenderBlurScreen();	
	// ��ʂ��N���A�ɂ���
	EffectScreen::GetInstance().ClearScreen();		
	
	// DxLib�̎d�l��SetDrawScreen�ŃJ�����̈ʒu�Ȃǂ̐ݒ肪
	// �����������̂ł����ōĎw��
	m_pCamera->Setting();

	// ��Ɠ��l������������̂ł�����x�Đݒ肷��
	EffekseerDrawer::GetInstance().EffekseerSync();

	// �}�b�v�̕`��
	m_pField->Draw();

	// �����Ԃ��̕`��
	for (auto& blood : m_pBlood)
	{
		blood->Draw();
	}
	// �L�����N�^�[�̕`��
	for (auto& character : m_pCharacter)
	{
		character->Draw();
	}

	// �u���[���ʁA��ʂ̐U�����ʂ�`�悷��
	EffectScreen::GetInstance().BlurPostRenderBlurScreen(m_isBlur);

	// UI�̕`��
	m_pUi->Draw();

	if (m_difficultyData == DifficultyData::NOIVE)
	{
		// �`���[�g���A���p�`��
		m_pTutorial->Draw();
	}

	// ���s�������ꍇ�`�悷��
	if (m_pUpdateFunc == &SceneMain::UpdateGameResult)
	{
		// �A���t�@�l��ύX���ăQ�[����ʂ��Â�����
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_checkmateBgBlendRate);
		DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, kResultColor, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		// ���s���ʉ摜
		DrawRotaGraph(Game::kScreenWidth / 2, static_cast<int>(m_checkmatePosY), m_checkmateSize, m_checkmateRota, m_hCheckmate, true);
	}
}

void SceneMain::CheckResult()
{
	// ���s��������
	// HP��0�ɂȂ����ꍇ
	if (m_pCharacter[kPlayerNo]->GetHp() <= 0) // �v���C���[
	{
		m_frameCount++;
		if (m_frameCount > kCheckmateDrawFrame)
		{
			m_frameCount = 0;
			m_resultData = GameResultData::OVER;
		}
	}
	else if (m_pCharacter[kEnemyNo]->GetHp() <= 0) // �G�l�~�[
	{
		m_frameCount++;
		if (m_frameCount > kCheckmateDrawFrame)
		{
			m_frameCount = 0;
			m_resultData = GameResultData::CREAR;
		}
	}

	// ��O�ɏo���ꍇ
	if (CheckCollMap(m_pCharacter[kPlayerNo])) // �v���C���[
	{
		m_resultData = GameResultData::OVER;
	}
	else if (CheckCollMap(m_pCharacter[kEnemyNo])) // �G�l�~�[
	{
		m_resultData = GameResultData::CREAR;
	}

	// �Q�[���̃N���A,�I�[�o�[�����̊m�F
	if (m_resultData != GameResultData::NONE)
	{
		m_pUpdateFunc = &SceneMain::UpdateGameResult;
		m_frameCount = 0;
	}
}

// �����蔻��
// ����Ƒ̂̔���
bool SceneMain::CheckWeaponAndBodyHit(std::shared_ptr<CharacterBase> character1,std::shared_ptr<CharacterBase> character2)
{
	if (Coll::IsCheckHit(
		character1->GetCollWeaponPos(), character2->GetCollPos(),
		character1->GetWeaponAttackRadius(), character2->GetWeaponAttackRadius()))
	{
		return true;
	}
	return false;
}

// ����Ə��̔���
bool SceneMain::CheckWeaponAndShieldHIt(std::shared_ptr<CharacterBase> character1,std::shared_ptr<CharacterBase> character2)
{
	if (Coll::IsCheckHit(
		character1->GetCollWeaponPos(), character2->GetShieldPos(),
		character1->GetWeaponAttackRadius(), character2->GetShieldRadius()))
	{
		return true;
	}
	return false;
}

// ����Ƒ͈̔͂̔���
bool SceneMain::CheckWeaponAndModelAboutHIt(std::shared_ptr<CharacterBase> character1,std::shared_ptr<CharacterBase> character2)
{
	if (Coll::IsCheckHit(
		character1->GetCollWeaponPos(), character2->GetCollPos(),
		character1->GetWeaponAttackRadius(), character2->GetModelRadius()))
	{
		return true;
	}
	return false;
}

// �͈̔͂Ƒ͈̔͂̔���
bool SceneMain::CheckModelAboutHIt(std::shared_ptr<CharacterBase> character1,std::shared_ptr<CharacterBase> character2)
{
	if (Coll::IsCheckHit(
		character1->GetPos(), character2->GetCollPos(),
		character1->GetModelRadius(), character2->GetModelRadius()))
	{
		return true;
	}
	return false;
}

// �n�ʂ̃��f���ƃv���C���[�̔���
bool SceneMain::CheckCollMap(std::shared_ptr<CharacterBase> character)
{
	MV1_COLL_RESULT_POLY_DIM HitPolyDim;

	// ���f���ƃJ�v�Z���Ƃ̓����蔻��
	HitPolyDim = MV1CollCheck_Capsule(
		m_pField->GetHandle(),
		-1,
		character->GetCapsulePosDown(),
		character->GetCapsulePosUp(),
		character->GetCapsuleRadius());

	// �����������ǂ����ŏ����𕪊�
	if (HitPolyDim.HitNum >= 1)
	{		
		// �����������L�����N�^�[�ɂ�n��
		character->SetFieldHit();
	}

	// ���f���ƃJ�v�Z���Ƃ̓����蔻��
	HitPolyDim = MV1CollCheck_Capsule(
		m_pField->GetHandle2(),
		-1,
		character->GetCapsulePosDown(),
		character->GetCapsulePosUp(),
		character->GetCapsuleRadius());

	// �����������ǂ����ŏ����𕪊�
	if (HitPolyDim.HitNum >= 1)
	{
		// �Q�[���I�[�o�[
		return true;
		// �����������L�����N�^�[�ɂ�n��
		character->SetFieldHit();
	}

	// �����蔻����̌�n��
	MV1CollResultPolyDimTerminate(HitPolyDim);

	return false;
}

// 1���U�������鑤
// 2���U�����󂯂鑤
void SceneMain::UpdateCharacter(std::shared_ptr<CharacterBase> character1, std::shared_ptr<CharacterBase> character2,bool isPlayer)
{
#if true
	// �v���C���[�X�V����
	character1->Update();

	// �^�[�Q�b�g��HP���擾
	character1->SetTargetHp(character2->GetHp());
	// �^�[�Q�b�g�̈ʒu���󂯎��
	character1->SetTargetPos(character2->GetPos());

	// �^�[�Q�b�g�̐퓬�̏�Ԃ��󂯎��
	character1->SetBattleState(character2->GetBattleState());

	// ��]�p�x���擾
	character2->SetTargetMtxRota(character1->GetRot());

	// �K�[�h�������Ȃ����
	character1->SetWeaponAttacksShield(false);
	
	// �U����������̂��\�ȏꍇ
	// �K�[�h���Ă���ꍇ�͖������ōU���\
	if (!character1->IsAttack() || character1->GetBattleState() == BattleState::GUARD)
	{
		// �W���X�g�K�[�h����
		// �U�����������Ă����ꍇ
		// ���肪�X�^����Ԃł͂Ȃ��ꍇ
		// ���g���K�[�h��Ԃ̏ꍇ
		if (CheckWeaponAndShieldHIt(character2, character1) && character2->GetBattleState() != BattleState::STUN && character1->GetBattleState() == BattleState::GUARD)
		{
			// �W���X�g�K�[�h�t���[��
			if(character1->GetGuardFrame() < character1->GetJustGuardFrameMax())
			{
				// �W���X�g�K�[�h�������������ǂ���
				character1->SetJustGuard(true);

				// �G�t�F�N�g���Đ�
				character1->SetCollJustGuardEffect();

				// �킢�ɕK�v�ȓ���ȃ��[�^�[�����炷
				character2->SetFightingMeter(-110.0f);

				// �U���J�n
				StartJoypadVibration(DX_INPUT_PAD1, 1000, 1000, -1);

				// �U�����ꂽ�ꍇ
				character1->IsHit();

				return;
			}
		}
		// �ʏ�K�[�h����
		// �ʏ�K�[�h���o���邩�ǂ���
		if (character1->GetGuardFrame() > character1->GetGuardFrameMax() && character2->GetBattleState() != BattleState::STUN)
		{
			// �U����Ԃ�������
			// �U�������ɓ����������ǂ���
			if (CheckWeaponAndShieldHIt(character2, character1))
			{
				// �m�b�N�o�b�N
				character1->SetGuardKnockBack(true, kGuardKnockBack);

				// ���U�����邽�߂̗͂𗭂߂�
				character1->SetStrongPower(20);

				// �G�t�F�N�g�̍Đ�
				character1->SetCollGuardEffect();

				// �U�������Ŗh����
				character1->SetWeaponAttacksShield(true);

				return;
			}		
		}
		else
		{
			// �U�����������ǂ���
			const bool isAttack       = character1->GetBattleState() == BattleState::ATTACK;
			const bool isAttackTow    = character1->GetBattleState() == BattleState::ATTACKTWO;
			const bool isStrongAttack = character1->GetBattleState() == BattleState::STRONGATTACK;

			// �U����^���鏈��
			if (isAttack || isAttackTow || isStrongAttack)
			{
				// �U���������������ǂ���
				if (CheckWeaponAndBodyHit(character1, character2))
				{
					// �_���[�W��^����
					character2->SetDamage(true);

					// �m�b�N�o�b�N
					// ���U���������ꍇ			
					if (character1->GetBattleState() == BattleState::STRONGATTACK)
					{
						character2->SetGuardKnockBack(true, kStrongAttackKnockBack);
					}
					else
					{
						character2->SetGuardKnockBack(true, kNormalAttackKnockBack);
					}

					// �_���[�W�G�t�F�N�g�̃J���[����
					int color = 0xffffff;
					// �v���C���[�������ꍇ
					if (isPlayer)
					{
						color = 0x000000;
					}
					else
					{
						color = 0xffffff;
					}

					// �K�[�h���Ă��Ȃ�������
					const bool isGruad = character2->GetBattleState() != BattleState::GUARD;
					if (isGruad)
					{
						for (int i = 0; i < kbloodNum; i++)
						{
							m_pBlood.push_back(new BloodDrawer(
								VGet(character2->GetPos().x,
									 character2->GetPos().y + 100.0f,
									 character2->GetPos().z), color));
							m_pBlood.back()->Init(i);
						}
					}

					// �U���J�n
					StartJoypadVibration(DX_INPUT_PAD1, 1000 / 3, 1000 / 2, -1);

					// �U�����ꂽ�ꍇ
					character1->IsHit();

					return;
				}			
			}
		}
	}
#endif
}

