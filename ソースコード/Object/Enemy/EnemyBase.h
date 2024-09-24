#pragma once

#include <DxLib.h>
#include "../../Util/DifficultyData.h"

#include "../CharacterBase.h"

class EnemyBase : public CharacterBase
{
public:
	EnemyBase(DifficultyData data, VECTOR pos) :
		CharacterBase(data,pos)
	{

	}
};

