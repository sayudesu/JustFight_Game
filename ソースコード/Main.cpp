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

// プログラムはWinMainから始まります
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// windowモード設定
	ChangeWindowMode(Game::kWindowMode);
	// ウインドウ名設定
	SetMainWindowText(Game::kTitleText);
	// 画面サイズの設定
	SetGraphMode(Game::kScreenWidth, Game::kScreenHeight, Game::kColorDepth);
	// windowモード時にwindowサイズを変更するかどうか
	SetWindowSizeChangeEnableFlag(Game::kWindowSizeChange);
	// ログテキストを作成するかどうか
	SetOutApplicationLogValidFlag(Game::kCreateLogText);

	// ダブルバッファモード
	SetDrawScreen(DX_SCREEN_BACK);

	// DirectX11を使用するようにする。(DirectX9も可、一部機能不可)
	// Effekseerを使用するには必ず設定する。
	SetUseDirect3DVersion(DX_DIRECT3D_11);

	// ＤＸライブラリ初期化処理
	if (DxLib_Init() == -1)		
	{
		// エラーが起きたら直ちに終了
		return -1;			
	}

	// Effekseerを初期化する。
	// 引数には画面に表示する最大パーティクル数を設定する。
	if (Effekseer_Init(8000) == -1)
	{
		DxLib_End();
		return -1;
	}

	//ゲーム中にウィンドウモードを切り替えてもグラフィックスハンドルを
	//リセットしない
	// フルスクリーンウインドウの切り替えでリソースが消えるのを防ぐ。
	// Effekseerを使用する場合は必ず設定する。
	SetChangeScreenModeGraphicsSystemResetFlag(false);

	// 3D関連の設定
	// Zバッファを有効にする。
	SetUseZBuffer3D(true);
	// Zバッファへの書き込みを有効にする。
	// Effekseerを使用する場合、2DゲームでもZバッファを使用する。
	SetWriteZBuffer3D(true);
	// ポリゴンの裏面を描画しない
	SetUseBackCulling(true);


	SetUseLighting(false);
	
	// 読み込み
	// サウンド
	SoundManager::GetInstance().Load(
		CSVSoundData::fileNamePath,
		CSVSoundData::firstData);

	// フォント
	FontManager::GetInstance().Load(
		CSVFontData::filePath,
		CSVFontData::data
	);

	// キャラクタのパラメータ
	CharactorDataManager::GetInstance().Load(
		CSVCharactorData::fileSmallPath, CSVCharactorData::firstSmallData,
		CSVCharactorData::fileMediumPath, CSVCharactorData::firstMediumData,
		CSVCharactorData::fileLargePath, CSVCharactorData::firstLargeData);

	// モデル
	ModelManager::GetInstance().Load(
		CSVModelData::filePath,
		CSVModelData::firstData);

	// 描画する文字の読み込み
	SubtitleManager::GetInstance().Load(
		CSVSubtitleData::filePath,
		CSVSubtitleData::data);

	// エフェクシア
	EffekseerDrawer::GetInstance().Load();

	// スクリーン加工用
	EffectScreen::GetInstance().Load();

	// シーンマネージャー初期化
	SceneManager* pScene = new SceneManager();
	pScene->Init();

	while (ProcessMessage() == 0)
	{
		LONGLONG  time = GetNowHiPerformanceCount();
		// 画面のクリア
		ClearDrawScreen();

		// 更新処理
		pScene->Update();
		EffekseerDrawer::GetInstance().Update();

		// 描画処理
		pScene->Draw();
		EffekseerDrawer::GetInstance().Draw();

		// 裏画面を表画面を入れ替える
		ScreenFlip();

		// escキーを押したら終了する
		if (CheckHitKey(KEY_INPUT_ESCAPE))	break;

		// fpsを60に固定
		while (GetNowHiPerformanceCount() - time < 16667)
		{
		}
	}

	// 内部のメモリ解放
	SoundManager::GetInstance().Unload();
	ModelManager::GetInstance().Unload();
	CharactorDataManager::GetInstance().Unload();
	EffekseerDrawer::GetInstance().Unload();
	EffectScreen::GetInstance().Unload();
	FontManager::GetInstance().Unload();

	// メモリ解放
	SoundManager::GetInstance().Destroy();
	ModelManager::GetInstance().Destroy();
	CharactorDataManager::GetInstance().Destroy();
	EffekseerDrawer::GetInstance().Destroy();
	EffectScreen::GetInstance().Destroy();
	FontManager::GetInstance().Destroy();

	// シーンマネジャーの解放処理
	pScene->End();
	delete pScene;

	// Effekseerを終了する。
	Effkseer_End();

	// ＤＸライブラリ使用の終了処理
	DxLib_End();				

	// ソフトの終了 
	return 0;				
}