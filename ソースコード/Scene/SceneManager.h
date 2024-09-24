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
	// フェード処理を開始する
	void StartFade();
	// フェード処理を初期化する
	void InitFade();
	// フェード処理を更新する
	void UpdateFadeIn();
	// フェードアウト処理をする
	void UpdateFadeOut();
	// フェード用描画
	void DrawFade();
private:
	// シーン切り替え用
	std::unique_ptr<SceneBase> m_pScene;
	// 移行したいシーンを記録する
	SceneBase* m_pTempScene;

	// フェード処理をしているかどうか
	bool m_isLoading;
	// フェードをスタートする
	bool m_isFade;
	// フェード用ブレンド率
	int m_blendRate;
	// フェード処理をするかどうか
	bool m_fadeIn;
	// フェードアウト処理をするかどうか
	bool m_fadeOut;
	// 初期化して始めの状態に戻すかどうか
	bool m_isInit;
};