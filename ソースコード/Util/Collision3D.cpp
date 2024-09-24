#include <cmath>
#include "Collision3D.h"

namespace Coll
{
	bool Coll::IsCheckHit(VECTOR pos1, VECTOR pos2, float radius1, float radius2)
	{
		// �x�N�g���̌��Z
		const VECTOR vec = VSub(pos1, pos2);
		// �x�N�g���̃T�C�Y
	    const float del = sqrt(VSquareSize(vec));
	
		if (del < radius1 + radius2)
		{
			return true;
		}

		return false;
	}
}
