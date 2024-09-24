#pragma once

#include <vector>
#include <memory>
#include <string>

#include "SceneBase.h"

#include "../Util/GameResultData.h"
#include "../Util/DifficultyData.h"

class GameObject;
class Camera;
class SceneResult final : public SceneBase
{
public:
	SceneResult(GameResultData resultData, DifficultyData data);
	virtual ~SceneResult();

	virtual void Init() override;
	virtual void End() override;

	virtual SceneBase* Update() override;
	virtual void Draw() override;
private:
	// �Q�[���̃��U���g�f�[�^�[���擾����
	GameResultData m_resultData;

	// �摜�̓ǂݍ���
	int m_hImageResultBg;
	int m_hImageResult;

	// �摜�̊p�x
	float m_imageAngle;

	// 3D�I�u�W�F�N�g�����p
	std::unique_ptr<GameObject> m_pPlayer;
	std::unique_ptr<GameObject> m_pEnemy;

	// �J���������p
	std::unique_ptr<Camera> m_pCamera;

	// �G�p�X�̖��O
	std::string m_enemyPath;

	// ���s���ʉ摜�ʒu�����p
	float m_resultY;
};

