#pragma once
#include "../engine/globals.h"

// ============================================================
// ANIMATION & PATHING SYSTEM
// ============================================================

// Initialize all dynamic moving objects (traffic, pedestrians)
void animationInit();

// Update animation states
void animationUpdate(float dt);
