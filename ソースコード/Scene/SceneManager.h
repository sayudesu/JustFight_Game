#pragma once

#include <memory>

class SceneBase;
class SceneManager final
{
public:
	SceneManager();
	virtual ~SceneManager();

	void Init();
	void End();

	void Update();
	void Draw();
private:
	// �t�F�[�h�������J�n����
	void StartFade();
	// �t�F�[�h����������������
	void InitFade();
	// �t�F�[�h�������X�V����
	void UpdateFadeIn();
	// �t�F�[�h�A�E�g����������
	void UpdateFadeOut();
	// �t�F�[�h�p�`��
	void DrawFade();
private:
	// �V�[���؂�ւ��p
	std::unique_ptr<SceneBase> m_pScene;
	// �ڍs�������V�[�����L�^����
	SceneBase* m_pTempScene;

	// �t�F�[�h���������Ă��邩�ǂ���
	bool m_isLoading;
	// �t�F�[�h���X�^�[�g����
	bool m_isFade;
	// �t�F�[�h�p�u�����h��
	int m_blendRate;
	// �t�F�[�h���������邩�ǂ���
	bool m_fadeIn;
	// �t�F�[�h�A�E�g���������邩�ǂ���
	bool m_fadeOut;
	// ���������Ďn�߂̏�Ԃɖ߂����ǂ���
	bool m_isInit;
};