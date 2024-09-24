#pragma once
#include <memory>
#include <vector>

#include "SceneBase.h"

#include "../Util/CharacterName.h"

#include "../Util/DifficultyData.h"
#include "../Util/GameResultData.h"

namespace
{
	constexpr int kCharacterNum = 2;

	constexpr int kEffectNum = 3;
}

class Camera;
class CharacterBase;
class BloodDrawer;
class FieldDrawer;
class UIDrawer;
class EffectScreen;
class TutorialDrawer;

class SceneMain final : public SceneBase
{
public:
	SceneMain(DifficultyData data);
	virtual ~SceneMain();

	virtual void Init() override;
	virtual void End() override;

	virtual SceneBase* Update() override;
	virtual void Draw() override;
private:
	// �V�[���J��
	// �Q�[�����C��
	SceneBase* UpdateGamePlay();
	// �Q�[���I�[�o�[
	SceneBase* UpdateGameResult();
private:
	// ����n
	void InputCharacter();
	void UpdateCharacter();
	// �p�����[�^�[�����p
	void CheckParameter(bool isDamage, bool isHit);
	// ���s�̊m�F
	void CheckResult();

	// �����蔻��
	// ����Ƒ̂̔���
	bool CheckWeaponAndBodyHit(std::shared_ptr<CharacterBase> chara1, std::shared_ptr<CharacterBase> chara2);
	// ����Ə��̔���
	bool CheckWeaponAndShieldHIt(std::shared_ptr<CharacterBase> chara1, std::shared_ptr<CharacterBase> chara2);
	// ����Ƒ͈̔͂̔���
	bool CheckWeaponAndModelAboutHIt(std::shared_ptr<CharacterBase> chara1, std::shared_ptr<CharacterBase> chara2);
	// �͈̔͂Ƒ͈̔͂̔���
	bool CheckModelAboutHIt(std::shared_ptr<CharacterBase> chara1, std::shared_ptr<CharacterBase> chara2);
	// �n�ʂ̃��f���ƃv���C���[�̔���
	bool CheckCollMap(std::shared_ptr<CharacterBase> character);

	// �L�����N�^�[�̍X�V����
	void UpdateCharacter(std::shared_ptr<CharacterBase> chara1, std::shared_ptr<CharacterBase> chara2,bool isPlayer);
private:
	// �����o�[�֐��|�C���^
	SceneBase* (SceneMain::* m_pUpdateFunc)() = &SceneMain::UpdateGamePlay;
	// �J�����N���X
	std::unique_ptr<Camera> m_pCamera;
	// �L�����N�^�[�N���X
	std::shared_ptr<CharacterBase> m_pCharacter[kCharacterNum];

	// ���`��N���X
	std::vector<BloodDrawer*> m_pBlood;
	// �t�B�[���h�N���X
	std::unique_ptr<FieldDrawer>m_pField; 
	// UI�N���X
	std::unique_ptr<UIDrawer> m_pUi;
	// �`���[�g���A���p�N���X
	std::unique_ptr<TutorialDrawer> m_pTutorial;

	// ���s�͂����ꍇ�̉摜
	int m_hCheckmate;

	// ���s�̌���
	GameResultData m_resultData;

	// �N���A��J�E���g
	int m_frameCount;

	// ��Փx�f�[�^
	DifficultyData m_difficultyData;

	// �`�F�b�N���C�g���ɕ\������摜�p
	float m_checkmateSize;
	float m_checkmateRota;
	// �`�F�b�N���C�g���w�i�̃u�����h�A���t�@�p
	int m_checkmateBgBlendRate;
	float m_checkmatePosY;

	// �u���[�������邩�ǂ���
	bool m_isBlur;

	// ���ݍU������Ă���
	bool m_isHit;
	// �U�������������ꍇ�̃J�E���g
	int m_hitFrameCount;
	// �q�b�g�X�g�b�v����t���[��
	int m_hitStopFrame;
};