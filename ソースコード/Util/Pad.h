#pragma once

// パッド処理
namespace Pad
{
	// パッドの入力状態取得
	void Update();

	// 押し下げ判定
	bool IsPress(int button, int padNo = 0);
	// トリガー判定
	bool IsTrigger(int button, int padNo = 0);

	// ログ記録開始、終了
	void StartRecordLog();
	void EndRecordLog();

	// ログ再生開始、終了
	void StartPlayLog();
	void EndPlayLog();
}
