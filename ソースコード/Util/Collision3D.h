#pragma once
#include <DxLib.h>

namespace Coll
{
	/// <summary>
	/// ‹…‚Æ‹…‚Ì“–‚½‚è”»’è
	/// </summary>
	/// <param name="pos1">ˆÊ’u</param>
	/// <param name="pos2">ˆÊ’u</param>
	/// <param name="radius1">”¼Œa</param>
	/// <param name="radius2">”¼Œa</param>
	/// <returns>“–‚½‚Á‚½‚©‚Ç‚¤‚©</returns>
	bool IsCheckHit(VECTOR pos1, VECTOR pos2,float radius1, float radius2);
};

