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

    // 3D���f���̏ꍇ�̃R���X�g���N�^ �I�[�o�[���[�h
    GameObject(std::string name, const VECTOR& pos, VECTOR angle, VECTOR size, GameObject* parent = nullptr);

    // 2D���f���̏ꍇ�̃R���X�g���N�^ �I�[�o�[���[�h
    GameObject(std::string name, const VECTOR& pos, float angle, float size, GameObject* parent = nullptr);

    ~GameObject();

    void Update();

    void Draw();

    // �ʒu���󂯎��
    void SetPos(VECTOR pos);

    // �p�x���󂯎��
    void SetRotate(VECTOR angle);

    // �T�C�Y���󂯎��
    void SetSize(float size);

    // �n���h���f�[�^��n��
    int GetHandle();

    // �ʒu��n��
    VECTOR GetPos();

    // �p�x��n��
    VECTOR GetAngle();

    float GetSize2D();

    // 2D�摜��X�̃T�C�Y��n��
    float GetGraphSizeX();

    // 2D�摜��Y�̃T�C�Y��n��
    float GetGraphSizeY();

    // ���f���̃t���[���C���f�b�N�X���󂯎�肻�̈ʒu��Ԃ�
    VECTOR GetCollPos(int modelFrameIndex);

    // �e�I�u�W�F�N�g���甲���o��
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

    // 2D�p
    float m_angle2D = 0;
    float m_size2D = 0;
    float m_sizeX = 0.0f;
    float m_sizeY = 0.0f;

    DataType m_dataType{};
};

