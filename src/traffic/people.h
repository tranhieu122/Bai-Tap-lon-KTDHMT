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
void personDraw(const PedestrianInfo& info, bool isShadow = false);
void personStandingDraw(int colorVar, bool isShadow = false);
void personWalkingDraw(int colorVar, float walkPhase, bool isWaving = false, bool isShadow = false);
void personSittingDraw(int colorVar, bool isShadow = false);
void vendorDraw(int colorVar, bool isShadow = false);
