#pragma once
#include <DxLib.h>

namespace Coll
{
	/// <summary>
	/// ���Ƌ��̓����蔻��
	/// </summary>
	/// <param name="pos1">�ʒu</param>
	/// <param name="pos2">�ʒu</param>
	/// <param name="radius1">���a</param>
	/// <param name="radius2">���a</param>
	/// <returns>�����������ǂ���</returns>
	bool IsCheckHit(VECTOR pos1, VECTOR pos2,float radius1, float radius2);
};

