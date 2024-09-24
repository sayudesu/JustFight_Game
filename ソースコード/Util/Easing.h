#pragma once
#include <DxLib.h>
#include <cmath>

// �C�[�W���O����
namespace Easing
{
    inline float InSine(int time, int totaltime, float max, float min)
    {
        max -= min;

        return -max * std::cos(time * (90.0f * DX_PI_F / 180.0f) / totaltime) + max + min;
    }

    inline float InQuad(int time, int totaltime, float max, float min)
    {
        max -= min;
        time /= totaltime;

        return max * time * time + min;
    }
}

