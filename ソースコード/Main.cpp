#include <DxLib.h>
#include <EffekseerForDXLib.h> 
#include <crtdbg.h>

#include "Scene/SceneManager.h"

#include "Util/Game.h"
#include "Util/EffekseerDrawer.h"
#include "Util/BlurScreen.h"

#include "CSVData/CSVSoundData.h"
#include "CSVData/CSVCharactorData.h"
#include "CSVData/CSVModelData.h"
#include "CSVData/CSVFontData.h"
#include "CSVData/CSVSubtitleData.h"

#include "CSVData/CharactorDataManager.h"
#include "CSVData/SoundManager.h"
#include "CSVData/ModelManager.h"
#include "CSVData/FontManager.h"
#include "CSVData/SubtitleManager.h"

// �v���O������WinMain����n�܂�܂�
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// window���[�h�ݒ�
	ChangeWindowMode(Game::kWindowMode);
	// �E�C���h�E���ݒ�
	SetMainWindowText(Game::kTitleText);
	// ��ʃT�C�Y�̐ݒ�
	SetGraphMode(Game::kScreenWidth, Game::kScreenHeight, Game::kColorDepth);
	// window���[�h����window�T�C�Y��ύX���邩�ǂ���
	SetWindowSizeChangeEnableFlag(Game::kWindowSizeChange);
	// ���O�e�L�X�g���쐬���邩�ǂ���
	SetOutApplicationLogValidFlag(Game::kCreateLogText);

	// �_�u���o�b�t�@���[�h
	SetDrawScreen(DX_SCREEN_BACK);

	// DirectX11���g�p����悤�ɂ���B(DirectX9���A�ꕔ�@�\�s��)
	// Effekseer���g�p����ɂ͕K���ݒ肷��B
	SetUseDirect3DVersion(DX_DIRECT3D_11);

	// �c�w���C�u��������������
	if (DxLib_Init() == -1)		
	{
		// �G���[���N�����璼���ɏI��
		return -1;			
	}

	// Effekseer������������B
	// �����ɂ͉�ʂɕ\������ő�p�[�e�B�N������ݒ肷��B
	if (Effekseer_Init(8000) == -1)
	{
		DxLib_End();
		return -1;
	}

	//�Q�[�����ɃE�B���h�E���[�h��؂�ւ��Ă��O���t�B�b�N�X�n���h����
	//���Z�b�g���Ȃ�
	// �t���X�N���[���E�C���h�E�̐؂�ւ��Ń��\�[�X��������̂�h���B
	// Effekseer���g�p����ꍇ�͕K���ݒ肷��B
	SetChangeScreenModeGraphicsSystemResetFlag(false);

	// 3D�֘A�̐ݒ�
	// Z�o�b�t�@��L���ɂ���B
	SetUseZBuffer3D(true);
	// Z�o�b�t�@�ւ̏������݂�L���ɂ���B
	// Effekseer���g�p����ꍇ�A2D�Q�[���ł�Z�o�b�t�@���g�p����B
	SetWriteZBuffer3D(true);
	// �|���S���̗��ʂ�`�悵�Ȃ�
	SetUseBackCulling(true);


	SetUseLighting(false);
	
	// �ǂݍ���
	// �T�E���h
	SoundManager::GetInstance().Load(
		CSVSoundData::fileNamePath,
		CSVSoundData::firstData);

	// �t�H���g
	FontManager::GetInstance().Load(
		CSVFontData::filePath,
		CSVFontData::data
	);

	// �L�����N�^�̃p�����[�^
	CharactorDataManager::GetInstance().Load(
		CSVCharactorData::fileSmallPath, CSVCharactorData::firstSmallData,
		CSVCharactorData::fileMediumPath, CSVCharactorData::firstMediumData,
		CSVCharactorData::fileLargePath, CSVCharactorData::firstLargeData);

	// ���f��
	ModelManager::GetInstance().Load(
		CSVModelData::filePath,
		CSVModelData::firstData);

	// �`�悷�镶���̓ǂݍ���
	SubtitleManager::GetInstance().Load(
		CSVSubtitleData::filePath,
		CSVSubtitleData::data);

	// �G�t�F�N�V�A
	EffekseerDrawer::GetInstance().Load();

	// �X�N���[�����H�p
	EffectScreen::GetInstance().Load();

	// �V�[���}�l�[�W���[������
	SceneManager* pScene = new SceneManager();
	pScene->Init();

	while (ProcessMessage() == 0)
	{
		LONGLONG  time = GetNowHiPerformanceCount();
		// ��ʂ̃N���A
		ClearDrawScreen();

		// �X�V����
		pScene->Update();
		EffekseerDrawer::GetInstance().Update();

		// �`�揈��
		pScene->Draw();
		EffekseerDrawer::GetInstance().Draw();

		// ����ʂ�\��ʂ����ւ���
		ScreenFlip();

		// esc�L�[����������I������
		if (CheckHitKey(KEY_INPUT_ESCAPE))	break;

		// fps��60�ɌŒ�
		while (GetNowHiPerformanceCount() - time < 16667)
		{
		}
	}

	// �����̃��������
	SoundManager::GetInstance().Unload();
	ModelManager::GetInstance().Unload();
	CharactorDataManager::GetInstance().Unload();
	EffekseerDrawer::GetInstance().Unload();
	EffectScreen::GetInstance().Unload();
	FontManager::GetInstance().Unload();

	// ���������
	SoundManager::GetInstance().Destroy();
	ModelManager::GetInstance().Destroy();
	CharactorDataManager::GetInstance().Destroy();
	EffekseerDrawer::GetInstance().Destroy();
	EffectScreen::GetInstance().Destroy();
	FontManager::GetInstance().Destroy();

	// �V�[���}�l�W���[�̉������
	pScene->End();
	delete pScene;

	// Effekseer���I������B
	Effkseer_End();

	// �c�w���C�u�����g�p�̏I������
	DxLib_End();				

	// �\�t�g�̏I�� 
	return 0;				
}