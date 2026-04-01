#pragma once
#include "../engine/globals.h"

// ============================================================
// PEDESTRIAN SYSTEM
// ============================================================

// Initialize pedestrian pools
void peopleInit();

// Update pedestrian logic
void peopleUpdate(float dt);

// Render all pedestrians
void peopleDraw();
void personDraw(const PedestrianInfo& info);
void personStandingDraw(int colorVar);
void personWalkingDraw(int colorVar, float walkPhase);
void personSittingDraw(int colorVar);
void vendorDraw(int colorVar);
