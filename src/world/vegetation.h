#pragma once
#include "../engine/globals.h"

// ============================================================
// VEGETATION SYSTEM (TREES, BUSHES, PLANTS)
// ============================================================

// Initialize vegetation data
void vegetationInit();

// Render all vegetation in the scene
void vegetationDraw(const TreeInfo &info);

// Helper functions for specific plants
void drawTree(float x, float z, int type);
void drawFlowerBed(float x, float z, float w, float d);
void drawPalmTree(float h);
void drawOakTree(float h);
