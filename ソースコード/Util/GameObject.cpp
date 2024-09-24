#include "GameObject.h"

GameObject::GameObject(std::string name, const VECTOR& pos, VECTOR angle, VECTOR size, GameObject* parent) :
    m_pos(pos),
    m_angle(angle),
    m_pParent(parent)
{
    m_dataType = DataType::THREE;

    // モデルの読み込み
    m_handle = MV1LoadModel(name.c_str());
    // 位置の調整
    MV1SetPosition(m_handle, pos);
    // サイズの調整
    MV1SetScale(m_handle, size);
}

GameObject::GameObject(std::string name, const VECTOR& pos, float angle, float size, GameObject* parent) :
    m_pos(pos), 
    m_angle2D(angle),
    m_pParent(parent)
{
    m_dataType = DataType::TWO;

    // 画像の読み込み
    m_handle = LoadGraph(name.c_str());
    // 画像サイズを取得する
    GetGraphSizeF(m_handle, &m_sizeX, &m_sizeY);
    // サイズの調整
    m_size2D = size;
}

GameObject::~GameObject()
{
    if (m_dataType == DataType::THREE)
    {
        MV1DeleteModel(m_handle);
    }
    else if (m_dataType == DataType::TWO)
    {
        DeleteGraph(m_handle);
    }
}

void GameObject::Update()
{
    VECTOR relativePos = m_pos;

    // 親オブジェクトが無い場合、親オブジェクトを抜ける場合
    if (m_pParent != nullptr && (!m_isParentEscape))
    {
        // 親オブジェクトが存在する場合、親の座標と角度に基づいて相対的な位置を計算
        MATRIX rotationMatrixX = MGetRotX(m_angle.x);
        MATRIX rotationMatrixY = MGetRotY(m_angle.y);
        MATRIX rotationMatrixZ = MGetRotZ(m_angle.z);

        // 回転行列を順番に掛けて相対的な位置を計算
        MATRIX combinedRotationMatrix = MMult(MMult(rotationMatrixX, rotationMatrixY), rotationMatrixZ);
        relativePos = VTransform(relativePos, combinedRotationMatrix);

        relativePos = VAdd(relativePos, m_pParent->GetPos());

        // 親オブジェクトが存在する場合、親の座標に基づいて相対的な位置を計算
        relativePos = VSub(m_pos, m_pParent->GetPos());
        relativePos = VTransform(m_pos, MMult(MMult(MGetRotX(m_pParent->GetAngle().x), MGetRotY(m_pParent->GetAngle().y)), MGetRotZ(m_pParent->GetAngle().z)));
        m_childPos = VAdd(relativePos, m_pParent->GetPos());

    }

    // モデルの位置を指定
    MV1SetPosition(m_handle, m_pos);
    // モデルの角度を指定
    MV1SetRotationXYZ(m_handle, m_angle);

    // 位置を一時的に記録
    m_tempPos = m_pos;

    // 親オブジェクトが無い場合、親オブジェクトを抜ける場合
    if (m_pParent != nullptr && (!m_isParentEscape))
    {
        // 子オブジェクトの位置
        MV1SetPosition(m_handle, m_childPos);
        // 位置を一時的に記録
        m_tempPos = m_childPos;
    }
}

void GameObject::Draw()
{
    // 描画
    if (m_dataType == DataType::THREE)
    {
        MV1DrawModel(m_handle);
    }
    else if (m_dataType == DataType::TWO)
    {
        DrawRotaGraphF(m_tempPos.x, m_tempPos.y, m_size2D, m_angle2D, m_handle, true);
    }
}

void GameObject::SetPos(VECTOR pos)
{
    m_pos = pos;
}

void GameObject::SetRotate(VECTOR angle)
{
    m_angle = angle;
}

void GameObject::SetSize(float size)
{
    m_size2D = size;
}

int GameObject::GetHandle()
{
    return m_handle;
}

VECTOR GameObject::GetPos()
{
    return m_pos;
}

VECTOR GameObject::GetAngle()
{
    return m_angle;
}

float GameObject::GetSize2D()
{
    return m_size2D;
}

float GameObject::GetGraphSizeX()
{
    if (m_dataType == DataType::TWO)
    {
        return m_sizeX;
    }

    return -1;
}

float GameObject::GetGraphSizeY()
{
    if (m_dataType == DataType::TWO)
    {
        return m_sizeY;
    }

    return -1;
}

VECTOR GameObject::GetCollPos(int modelFrameIndex)
{
    return MV1GetFramePosition(m_handle, modelFrameIndex);
}

void GameObject::SetParentEscape(bool isEscape)
{
    if (m_pParent != nullptr)
    {
        m_isParentEscape = isEscape;
        return;
    }
    return;
}
