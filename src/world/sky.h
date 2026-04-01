#pragma once
#include "../engine/globals.h"

// ============================================================
// SKY AND ENVIRONMENT SYSTEM
// ============================================================

// Initialize sky parameters
void skyInit();

// Update sky state (time of day, light colors)
void skyUpdate(float dt);

// Draw skybox and sun/moon
void skyDraw();

// Getters for environment lighting
Color skyGetAmbientLight();
Color skyGetDirectionalLight();
Vector3 skyGetLightDirection();
TimeOfDay skyGetTimeState();
void skySetTime(float hours);
float skyGetTime();
void skySetWeather(WeatherType type);
WeatherType skyGetWeather();
const char* skyGetWeatherName();
void weatherEffectDraw();


