#pragma once
#include "Globals.hpp"
#include "Macros.hpp"
#include "LagSwitch.hpp"
#include "Speedglitch.hpp"

inline void initMacros() {
    initSpeedglitch();
}

inline void UpdateMacros() {
    if (enabled[0]) freezeMacro();
    if (enabled[1]) laughClip();
    if (enabled[2]) extendedDanceClip();
    if (enabled[4]) LagSwitch();
    if (enabled[5]) BuckeyClip();
    if (enabled[6]) speedglitchMacro();
    if (enabled[7]) SpamKeyMacro();
}