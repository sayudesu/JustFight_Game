#pragma once

// �퓬���
enum class BattleState
{
	NONE,         // ��
	IDLE,         // �ҋ@���
	ATTACK,       // �U��
	ATTACKTWO,    // �R���{�U��
	STRONGATTACK, // �����U��
	GUARD,        // �h��
	JUSTGUARD,    // �h��
	STUN,         // �X�^��
};