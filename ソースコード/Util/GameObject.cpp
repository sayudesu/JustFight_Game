#include "GameObject.h"

GameObject::GameObject(std::string name, const VECTOR& pos, VECTOR angle, VECTOR size, GameObject* parent) :
    m_pos(pos),
    m_angle(angle),
    m_pParent(parent)
{
    m_dataType = DataType::THREE;

    // ���f���̓ǂݍ���
    m_handle = MV1LoadModel(name.c_str());
    // �ʒu�̒���
    MV1SetPosition(m_handle, pos);
    // �T�C�Y�̒���
    MV1SetScale(m_handle, size);
}

GameObject::GameObject(std::string name, const VECTOR& pos, float angle, float size, GameObject* parent) :
    m_pos(pos), 
    m_angle2D(angle),
    m_pParent(parent)
{
    m_dataType = DataType::TWO;

    // �摜�̓ǂݍ���
    m_handle = LoadGraph(name.c_str());
    // �摜�T�C�Y���擾����
    GetGraphSizeF(m_handle, &m_sizeX, &m_sizeY);
    // �T�C�Y�̒���
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

    // �e�I�u�W�F�N�g�������ꍇ�A�e�I�u�W�F�N�g�𔲂���ꍇ
    if (m_pParent != nullptr && (!m_isParentEscape))
    {
        // �e�I�u�W�F�N�g�����݂���ꍇ�A�e�̍��W�Ɗp�x�Ɋ�Â��đ��ΓI�Ȉʒu���v�Z
        MATRIX rotationMatrixX = MGetRotX(m_angle.x);
        MATRIX rotationMatrixY = MGetRotY(m_angle.y);
        MATRIX rotationMatrixZ = MGetRotZ(m_angle.z);

        // ��]�s������ԂɊ|���đ��ΓI�Ȉʒu���v�Z
        MATRIX combinedRotationMatrix = MMult(MMult(rotationMatrixX, rotationMatrixY), rotationMatrixZ);
        relativePos = VTransform(relativePos, combinedRotationMatrix);

        relativePos = VAdd(relativePos, m_pParent->GetPos());

        // �e�I�u�W�F�N�g�����݂���ꍇ�A�e�̍��W�Ɋ�Â��đ��ΓI�Ȉʒu���v�Z
        relativePos = VSub(m_pos, m_pParent->GetPos());
        relativePos = VTransform(m_pos, MMult(MMult(MGetRotX(m_pParent->GetAngle().x), MGetRotY(m_pParent->GetAngle().y)), MGetRotZ(m_pParent->GetAngle().z)));
        m_childPos = VAdd(relativePos, m_pParent->GetPos());

    }

    // ���f���̈ʒu���w��
    MV1SetPosition(m_handle, m_pos);
    // ���f���̊p�x���w��
    MV1SetRotationXYZ(m_handle, m_angle);

    // �ʒu���ꎞ�I�ɋL�^
    m_tempPos = m_pos;

    // �e�I�u�W�F�N�g�������ꍇ�A�e�I�u�W�F�N�g�𔲂���ꍇ
    if (m_pParent != nullptr && (!m_isParentEscape))
    {
        // �q�I�u�W�F�N�g�̈ʒu
        MV1SetPosition(m_handle, m_childPos);
        // �ʒu���ꎞ�I�ɋL�^
        m_tempPos = m_childPos;
    }
}

void GameObject::Draw()
{
    // �`��
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
