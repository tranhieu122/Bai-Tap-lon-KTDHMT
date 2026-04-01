#pragma once
#include "../engine/globals.h"

// ============================================================
// STREET AND ROAD SYSTEM
// ============================================================

// Initialize road layouts and meshes
void streetInit();

// Update any road-related logic (if any)
void streetUpdate(float dt);

// Draw the main street, side street, intersection, sidewalks, and crosswalks
void streetDraw();
