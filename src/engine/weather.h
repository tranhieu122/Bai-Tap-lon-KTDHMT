#pragma once
#include "globals.h"

// ============================================================
// WEATHER SYSTEM CONTROLLER
// Note: Actual rendering happens in sky.cpp and particles.cpp
// This is mainly for logic coordination and sound cues if any.
// ============================================================

void weatherInit();
void weatherUpdate(float dt);
