#pragma once
#include <DxLib.h>
#include <memory>

#include "SceneBase.h"

class GameObject;
class SlideSelect;
class GameObject;
class Camera;
class SceneLevelSelect : public SceneBase
{
public:
	SceneLevelSelect();
	virtual ~SceneLevelSelect();

	virtual void Init();
	virtual void End();
	virtual SceneBase* Update();
	virtual void Draw();
private:
	// �J�����ړ�����
	void CameraMoveing();
	// ���f���ړ�����
	void ModelMoveing();
	// ���͏���
	void Input();
	// �I�������w�i�֘A�̏���
	void SelectBg();
private:
	int m_hTitle;
	// �w�i�摜
	std::shared_ptr<GameObject> m_hBg;
	std::shared_ptr<GameObject> m_hSelect;
	std::shared_ptr<GameObject> m_hDecoration;
	// ��Փx
	std::shared_ptr<GameObject> m_hNovice;       // �ȒP
	std::shared_ptr<GameObject> m_hIntermediate; // ����
	std::shared_ptr<GameObject> m_hExpert;       // ���
	// �G�̃��f��
	std::shared_ptr<GameObject> m_hModel[3];
	// �G�̔w�i
	std::shared_ptr<GameObject> m_hImageDifficultyBg;
	// �I��p���
	std::shared_ptr<GameObject> m_hArrow[2];
	// �I�v�V�����{�^���̔w�i
	std::shared_ptr<GameObject>m_hOptionBack;
	// �I�v�V�����{�^��
	std::shared_ptr<GameObject>m_hOptionBotton;

	std::unique_ptr<Camera> m_pCamera;

	std::unique_ptr<SlideSelect> m_pSelect;

	std::unique_ptr<GameObject>m_pStage;

	// �R���g���[���[��ڑ����Ă邩�ǂ���
	bool m_isInputController;

	// �J�������w��̈ʒu�ɒ�~�������ǂ���x.y.z���ꂼ�ꂪ
	bool m_isCameraStop[4];

	// �w�i�摜�ʒu
	VECTOR m_bgPos;

	// �G�̔w�i�摜
	int m_tempSelect;
	bool m_isEnemyBgShake;
	int m_enemyBgshakeCount;
	float m_firstEnemyBgX;
	float m_firstEnemyBgY;
	float m_enemyBgShakeX;
	float m_enemyBgShakeY;

	// ���
	float m_arrowSize[2];

	float m_arrowPosX[2];
	float m_arrowPosY[2];

	float m_firstArrowPosX[2];
	float m_firstArrowPosY[2];

	float m_arrowShakeX[2];
	float m_arrowShakeY[2];

	// �G���f��
	float m_modelRot[3];
	float m_modelSlidePosY;

	// �J�����̍��W
	float m_cameraPosX;
	float m_cameraPosY;
	float m_cameraPosZ;

	// �J�����̈ړ��X�s�[�h
	float m_cameraSpeed;

	// ��Փx�����̑傫��
	float m_difficulty;

	int m_arrowTimer;

	// �J������~�p
	enum CameraStopData
	{
		X,
		Y,
		Z,
		ALL,
	};
};

