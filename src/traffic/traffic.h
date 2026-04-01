#pragma once
#include "../engine/globals.h"

// ============================================================
// TRAFFIC LOGIC & LIGHTS SYSTEM
// ============================================================

// Initialize traffic systems
void trafficInit();

// Update traffic logic
void trafficUpdate(float dt);

// Render traffic lights and props
void trafficLightsDraw();
void trafficSignDraw(const TrafficSignInfo& info);
void drawPowerLines(float x1, float y1, float z1, float x2, float y2, float z2);
