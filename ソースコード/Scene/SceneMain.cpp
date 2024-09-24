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
	// プレイヤーの番号を指定する
	constexpr int kPlayerNo = static_cast<int>(CharacterName::PLAYER);
	// エネミーの番号を指定する
	constexpr int kEnemyNo = static_cast<int>(CharacterName::ENEMY);
	// プレイヤー、エネミーの数
	constexpr int kCharactorMaxNum = 2;

	// 勝敗が付いた時に描画する画像位置
	static const char* const kCheckmateGraphPath = "Data/Image/UI/Checkmate.png";

	// 勝敗が決まった際にチェックメイト画像が描画されるまでのフレーム
	constexpr int kCheckmateDrawFrame = 20;

	// シーンが移行するまでのフレーム
	constexpr int kResultSceneChengeFrame = 60 * 3;

	// チェックメイト画像のサイズの最大
	constexpr float kCheckmateGraphSizeMax = 1.0f;

	// チェックメイト画像を最大サイズまで小さくする値
	constexpr float kCheckmateGraphSizeMinRate = 1.0f;

	// チェックメイト画像の角度の最終の値
	constexpr float kCheckmateGraphEndRota = 0.0f;

	// チェックメイト画像の角度の変更する値
	constexpr float kCheckMateGraphRate = 0.5f;

	// 勝敗決定後の背景のアルファ値を変更する最大値
	constexpr int kResultAlphaRateMax = 128;

	// 勝敗決定後の背景のアルファ値を変更する値
	constexpr int kResultAlphaRate = 5;

	// 勝敗決定後の背景カラー
	constexpr int kResultColor = 0x000000;

	// 血を描画する用
	constexpr int kbloodNum = 100;

	// プレイヤー、エネミーの初期位置
	static const VECTOR kInitPlayerPos = VGet(-300.0f, 300.0f, 0.0f);
	static const VECTOR kInitEnemyPos = VGet(300.0f, 300.0f, 0.0f);

	// ヒットストップ停止フレーム
	constexpr int kHitStopFrameMax = 4;

	// 勝敗結果画像
	// 振動の周波数
	constexpr float kFrequency = 0.07f;
	constexpr float kFrequencyTimes = 100.0f;
	// 振動の振幅
	constexpr float kAmplitude = static_cast<float>(Game::kScreenHeight) / 2.0f - 160.0f;

	// 強攻撃ノックバック
	constexpr float kStrongAttackKnockBack = -30.0f;
	// 通常攻撃ノックバック
	constexpr float kNormalAttackKnockBack = -10.0f;

	// ガードしてた場合のノックバック
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
	// シーン遷移
	m_pUpdateFunc = &SceneMain::UpdateGamePlay;

	// キャラクタークラス
	m_pCharacter[kPlayerNo] = std::make_shared<Player>(m_difficultyData, kInitPlayerPos);
	m_pCharacter[kEnemyNo]  = std::make_shared<Enemy>(m_difficultyData, kInitEnemyPos);

	m_pCamera   = std::make_unique<Camera>();        // カメラクラス
	m_pField    = std::make_unique<FieldDrawer>();   // フィールド描画クラス
	m_pUi       = std::make_unique<UIDrawer>();      // UI描画クラス
	m_pTutorial = std::make_unique<TutorialDrawer>();// チュートリアルクラス

	// 初期化
	m_pCharacter[kPlayerNo]->Init();
	m_pCharacter[kEnemyNo]->Init();
	m_pCamera->Setting();
	m_pField->Init();
	m_pTutorial->Init();

	// チェックメイト画像の読み込み
	m_hCheckmate = LoadGraph(kCheckmateGraphPath);

	// パラメータの更新
	CheckParameter(false,false);

	// スクリーン効果の初期化
	EffectScreen::GetInstance().BlurIReplayInit();	
}

void SceneMain::End()
{
	// 解放処理
	m_pCharacter[kPlayerNo]->End();
	m_pCharacter[kEnemyNo]->End();

	m_pField->End();
	m_pTutorial->End();

	// ダメージエフェクトのメモリ解放
	for (size_t i = 0; i < m_pBlood.size(); i++)
	{
		// デリート処理
		delete m_pBlood[i];
		m_pBlood[i] = nullptr;
	}

	// チェックメイト画像のメモリ解放
	DeleteGraph(m_hCheckmate);
}

SceneBase* SceneMain::Update()
{
	// シーン遷移
	return (this->*m_pUpdateFunc)();
}

SceneBase* SceneMain::UpdateGamePlay()
{
	// BGMの再生
	SoundManager::GetInstance().Play(SoundName::PLAY, true);

	if (m_hitStopFrame == 0)
	{
		// 入力処理
		InputCharacter();
		// 更新処理
		UpdateCharacter();
		// ヒットストップフレームのリセット
		m_hitStopFrame = 0;
	}
	else
	{
		// ヒットストップフレームを数を減らす
		m_hitStopFrame--;
	}

	// 敵の攻撃可能範囲にいるかどうか
	if (CheckModelAboutHIt(m_pCharacter[kPlayerNo], m_pCharacter[kEnemyNo]))
	{
		m_pCharacter[kEnemyNo]->SetAttackRange(true);
	}
	else
	{
		m_pCharacter[kEnemyNo]->SetAttackRange(false);
	}

	// カメラにプレイヤーとエネミーの位置を渡す
	m_pCamera->SetTargetPos(m_pCharacter[kPlayerNo]->GetPos());
	// カメラにプレイヤーの角度と位置を渡す
	m_pCamera->SetPlayerAngle(m_pCharacter[kPlayerNo]->GetAngle());
		// カメラの更新処理
	m_pCamera->Update();

	// 勝敗の確認
	CheckResult();

	// プレイヤーに攻撃がヒットしたかどうか
	const bool isDamageBlur = m_pCharacter[kPlayerNo]->IsHitDamage() && !m_pCharacter[kPlayerNo]->IsGuard();
	// エネミーに攻撃がヒットしたかどうか
	const bool isDamageBlurEnemy = m_pCharacter[kEnemyNo]->IsHitDamage() && !m_pCharacter[kEnemyNo]->IsGuard();

	// プレイヤーが回避行動しているかどうか
	const bool isAwayBlur = m_pCharacter[kPlayerNo]->IsAway();

	// 攻撃がヒットしている、プレイヤーが回避している場合にブラー効果をオンにする
	m_isBlur = isDamageBlur || isAwayBlur;

	// 攻撃をされた場合
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

	// ヒット判定がある場合
	if (m_hitFrameCount == 1)
	{
		m_isHit = true;
	}

	// パラメータの更新
	CheckParameter(isDamageBlur, m_isHit);

	// 攻撃が当たった場合停止フレームをしていする
	if (m_isHit)
	{
		m_hitStopFrame = kHitStopFrameMax;
		m_isHit = false;
	}

	// ダメージエフェクトを更新
	for (auto& blood : m_pBlood)
	{
		blood->Update();
	}
	// 削除できる要素を探す
	for (int i = 0; i < m_pBlood.size(); i++)
	{
		if (m_pBlood[i]->IsGetErase())
		{
			// デリート処理
			delete m_pBlood[i];
			m_pBlood[i] = nullptr;
			// 要素の削除
			m_pBlood.erase(m_pBlood.begin() + i);
		}
	}

	// 画面振動更新処理
	EffectScreen::GetInstance().QuakeUpdate();

	return this;
}

SceneBase* SceneMain::UpdateGameResult()
{
	// 指定フレームの後にリザルト画面に移動する
	m_frameCount++;

	// ボタンを押した場合
	if (Pad::IsTrigger(PAD_INPUT_1))
	{
		// BGMの停止
		SoundManager::GetInstance().Stop(SoundName::PLAY);
		return new SceneResult(m_resultData, m_difficultyData);
	}

	// 指定したフレームまでカウントが進むと
	if (m_frameCount >= kResultSceneChengeFrame)
	{
		// BGMの停止
		SoundManager::GetInstance().Stop(SoundName::PLAY);
		return new SceneResult(m_resultData, m_difficultyData);
	}

	// 画像の位置を動かす計算
	m_checkmatePosY = cosf(static_cast<float>(m_frameCount) * kFrequency) * kFrequencyTimes + kAmplitude;
	// 予めでかくしたサイズを1にする
	if (m_checkmateSize > kCheckmateGraphSizeMax)
	{
		m_checkmateSize -= kCheckmateGraphSizeMinRate;
	}
	// 予め変更した角度を0にする
	if (m_checkmateRota > kCheckmateGraphEndRota)
	{
		m_checkmateRota -= kCheckMateGraphRate;
	}

	// ブレンド率を変更
	if (m_checkmateBgBlendRate < kResultAlphaRateMax)
	{
		m_checkmateBgBlendRate += kResultAlphaRate;
	}

	return this;
}

void SceneMain::InputCharacter()
{
	// if 難易度が中、大だったら
	// else if 難易度が小だったら チュートリアル
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
	// キャラクターの更新処理
	UpdateCharacter(m_pCharacter[kPlayerNo], m_pCharacter[kEnemyNo], true);
	UpdateCharacter(m_pCharacter[kEnemyNo], m_pCharacter[kPlayerNo], false);
}

void SceneMain::CheckParameter(bool isDamage,bool isHit)
{
	// UIにパラメーターの状態を渡す
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
	// 新しい画面の作成
	EffectScreen::GetInstance().BlurPreRenderBlurScreen();	
	// 画面をクリアにする
	EffectScreen::GetInstance().ClearScreen();		
	
	// DxLibの仕様上SetDrawScreenでカメラの位置などの設定が
	// 初期化されるのでここで再指定
	m_pCamera->Setting();

	// 上と同様初期化させるのでもう一度再設定する
	EffekseerDrawer::GetInstance().EffekseerSync();

	// マップの描画
	m_pField->Draw();

	// 血しぶきの描画
	for (auto& blood : m_pBlood)
	{
		blood->Draw();
	}
	// キャラクターの描画
	for (auto& character : m_pCharacter)
	{
		character->Draw();
	}

	// ブラー効果、画面の振動効果を描画する
	EffectScreen::GetInstance().BlurPostRenderBlurScreen(m_isBlur);

	// UIの描画
	m_pUi->Draw();

	if (m_difficultyData == DifficultyData::NOIVE)
	{
		// チュートリアル用描画
		m_pTutorial->Draw();
	}

	// 勝敗がついた場合描画する
	if (m_pUpdateFunc == &SceneMain::UpdateGameResult)
	{
		// アルファ値を変更してゲーム画面を暗くする
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_checkmateBgBlendRate);
		DrawBox(0, 0, Game::kScreenWidth, Game::kScreenHeight, kResultColor, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		// 勝敗判別画像
		DrawRotaGraph(Game::kScreenWidth / 2, static_cast<int>(m_checkmatePosY), m_checkmateSize, m_checkmateRota, m_hCheckmate, true);
	}
}

void SceneMain::CheckResult()
{
	// 勝敗条件処理
	// HPが0になった場合
	if (m_pCharacter[kPlayerNo]->GetHp() <= 0) // プレイヤー
	{
		m_frameCount++;
		if (m_frameCount > kCheckmateDrawFrame)
		{
			m_frameCount = 0;
			m_resultData = GameResultData::OVER;
		}
	}
	else if (m_pCharacter[kEnemyNo]->GetHp() <= 0) // エネミー
	{
		m_frameCount++;
		if (m_frameCount > kCheckmateDrawFrame)
		{
			m_frameCount = 0;
			m_resultData = GameResultData::CREAR;
		}
	}

	// 場外に出た場合
	if (CheckCollMap(m_pCharacter[kPlayerNo])) // プレイヤー
	{
		m_resultData = GameResultData::OVER;
	}
	else if (CheckCollMap(m_pCharacter[kEnemyNo])) // エネミー
	{
		m_resultData = GameResultData::CREAR;
	}

	// ゲームのクリア,オーバー条件の確認
	if (m_resultData != GameResultData::NONE)
	{
		m_pUpdateFunc = &SceneMain::UpdateGameResult;
		m_frameCount = 0;
	}
}

// 当たり判定
// 武器と体の判定
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

// 武器と盾の判定
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

// 武器と体範囲の判定
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

// 体範囲と体範囲の判定
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

// 地面のモデルとプレイヤーの判定
bool SceneMain::CheckCollMap(std::shared_ptr<CharacterBase> character)
{
	MV1_COLL_RESULT_POLY_DIM HitPolyDim;

	// モデルとカプセルとの当たり判定
	HitPolyDim = MV1CollCheck_Capsule(
		m_pField->GetHandle(),
		-1,
		character->GetCapsulePosDown(),
		character->GetCapsulePosUp(),
		character->GetCapsuleRadius());

	// 当たったかどうかで処理を分岐
	if (HitPolyDim.HitNum >= 1)
	{		
		// 当たった情報キャラクターにを渡す
		character->SetFieldHit();
	}

	// モデルとカプセルとの当たり判定
	HitPolyDim = MV1CollCheck_Capsule(
		m_pField->GetHandle2(),
		-1,
		character->GetCapsulePosDown(),
		character->GetCapsulePosUp(),
		character->GetCapsuleRadius());

	// 当たったかどうかで処理を分岐
	if (HitPolyDim.HitNum >= 1)
	{
		// ゲームオーバー
		return true;
		// 当たった情報キャラクターにを渡す
		character->SetFieldHit();
	}

	// 当たり判定情報の後始末
	MV1CollResultPolyDimTerminate(HitPolyDim);

	return false;
}

// 1が攻撃をする側
// 2が攻撃を受ける側
void SceneMain::UpdateCharacter(std::shared_ptr<CharacterBase> character1, std::shared_ptr<CharacterBase> character2,bool isPlayer)
{
#if true
	// プレイヤー更新処理
	character1->Update();

	// ターゲットのHPを取得
	character1->SetTargetHp(character2->GetHp());
	// ターゲットの位置を受け取る
	character1->SetTargetPos(character2->GetPos());

	// ターゲットの戦闘の状態を受け取る
	character1->SetBattleState(character2->GetBattleState());

	// 回転角度を取得
	character2->SetTargetMtxRota(character1->GetRot());

	// ガード成功いない状態
	character1->SetWeaponAttacksShield(false);
	
	// 攻撃が当たるのが可能な場合
	// ガードしている場合は無条件で攻撃可能
	if (!character1->IsAttack() || character1->GetBattleState() == BattleState::GUARD)
	{
		// ジャストガード処理
		// 攻撃が当たっていた場合
		// 相手がスタン状態ではない場合
		// 自身がガード状態の場合
		if (CheckWeaponAndShieldHIt(character2, character1) && character2->GetBattleState() != BattleState::STUN && character1->GetBattleState() == BattleState::GUARD)
		{
			// ジャストガードフレーム
			if(character1->GetGuardFrame() < character1->GetJustGuardFrameMax())
			{
				// ジャストガードが成功したかどうか
				character1->SetJustGuard(true);

				// エフェクトを再生
				character1->SetCollJustGuardEffect();

				// 戦いに必要な特殊なメーターを減らす
				character2->SetFightingMeter(-110.0f);

				// 振動開始
				StartJoypadVibration(DX_INPUT_PAD1, 1000, 1000, -1);

				// 攻撃された場合
				character1->IsHit();

				return;
			}
		}
		// 通常ガード処理
		// 通常ガードが出来るかどうか
		if (character1->GetGuardFrame() > character1->GetGuardFrameMax() && character2->GetBattleState() != BattleState::STUN)
		{
			// 攻撃状態だったら
			// 攻撃が盾に当たったかどうか
			if (CheckWeaponAndShieldHIt(character2, character1))
			{
				// ノックバック
				character1->SetGuardKnockBack(true, kGuardKnockBack);

				// 強攻撃するための力を溜める
				character1->SetStrongPower(20);

				// エフェクトの再生
				character1->SetCollGuardEffect();

				// 攻撃を盾で防いだ
				character1->SetWeaponAttacksShield(true);

				return;
			}		
		}
		else
		{
			// 攻撃をしたかどうか
			const bool isAttack       = character1->GetBattleState() == BattleState::ATTACK;
			const bool isAttackTow    = character1->GetBattleState() == BattleState::ATTACKTWO;
			const bool isStrongAttack = character1->GetBattleState() == BattleState::STRONGATTACK;

			// 攻撃を与える処理
			if (isAttack || isAttackTow || isStrongAttack)
			{
				// 攻撃が当たったかどうか
				if (CheckWeaponAndBodyHit(character1, character2))
				{
					// ダメージを与える
					character2->SetDamage(true);

					// ノックバック
					// 強攻撃だった場合			
					if (character1->GetBattleState() == BattleState::STRONGATTACK)
					{
						character2->SetGuardKnockBack(true, kStrongAttackKnockBack);
					}
					else
					{
						character2->SetGuardKnockBack(true, kNormalAttackKnockBack);
					}

					// ダメージエフェクトのカラー調整
					int color = 0xffffff;
					// プレイヤーだった場合
					if (isPlayer)
					{
						color = 0x000000;
					}
					else
					{
						color = 0xffffff;
					}

					// ガードしていなかったら
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

					// 振動開始
					StartJoypadVibration(DX_INPUT_PAD1, 1000 / 3, 1000 / 2, -1);

					// 攻撃された場合
					character1->IsHit();

					return;
				}			
			}
		}
	}
#endif
}

