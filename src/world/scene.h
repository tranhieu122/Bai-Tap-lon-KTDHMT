#pragma once
#include "../engine/globals.h"

// ============================================================
// GLOBAL SCENE MANAGER
// ============================================================

// Initialize all world systems
void sceneInit();

// Update scene logic (time, weather, AI)
void sceneUpdate(float dt);

// Render the entire world
void sceneDraw();
void drawTransparentObjects();
void drawShadowCasters();
int sceneGetDrawCalls();
int sceneGetPolygons();
