#pragma once
#include <DxLib.h>
#include <string>
#include <cmath>

class GameObject
{
private:
    enum class DataType
    {
        THREE,
        TWO,
    };
public:

    // 3Dモデルの場合のコンストラクタ オーバーロード
    GameObject(std::string name, const VECTOR& pos, VECTOR angle, VECTOR size, GameObject* parent = nullptr);

    // 2Dモデルの場合のコンストラクタ オーバーロード
    GameObject(std::string name, const VECTOR& pos, float angle, float size, GameObject* parent = nullptr);

    ~GameObject();

    void Update();

    void Draw();

    // 位置を受け取る
    void SetPos(VECTOR pos);

    // 角度を受け取る
    void SetRotate(VECTOR angle);

    // サイズを受け取る
    void SetSize(float size);

    // ハンドルデータを渡す
    int GetHandle();

    // 位置を渡す
    VECTOR GetPos();

    // 角度を渡す
    VECTOR GetAngle();

    float GetSize2D();

    // 2D画像のXのサイズを渡す
    float GetGraphSizeX();

    // 2D画像のYのサイズを渡す
    float GetGraphSizeY();

    // モデルのフレームインデックスを受け取りその位置を返す
    VECTOR GetCollPos(int modelFrameIndex);

    // 親オブジェクトから抜け出す
    void SetParentEscape(bool isEscape);

private:
    int m_handle = -1;
    VECTOR m_pos = { 0,0,0 };
    VECTOR m_childPos = { 0,0,0 };
    VECTOR m_tempPos = { 0,0,0 };
    VECTOR m_angle = { 0,0,0 };
    VECTOR m_size = {0,0,0};
    GameObject* m_pParent = nullptr;
    bool m_isParentEscape = false;

    // 2D用
    float m_angle2D = 0;
    float m_size2D = 0;
    float m_sizeX = 0.0f;
    float m_sizeY = 0.0f;

    DataType m_dataType{};
};

