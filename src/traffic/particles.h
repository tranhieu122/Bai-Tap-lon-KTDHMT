#pragma once
#include "../engine/globals.h"

// Initialize particle pools
void particlesInit();
void particlesUpdate(float dt);
void particlesDraw();

// Spawners
void emitRainDrop(const Vector3& pos);
void emitSmoke(const Vector3& pos, const Vector3& vel);
