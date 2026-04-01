#pragma once
#include "../engine/globals.h"

// ============================================================
// STREET FURNITURE SYSTEM
// ============================================================

// Draw general street furniture items
void furnitureDraw(const FurnitureInfo& info);
void streetLampDraw(bool isOn);
void parkBenchDraw();
void trashCanDraw();
void powerPoleDraw();
void streetSignDraw(const char* label);
void busStopDraw();
void planterBoxDraw();

// Authentic Vietnamese Polish
void drawElectricPole();
void drawStreetWires();
void furnitureClearState();


