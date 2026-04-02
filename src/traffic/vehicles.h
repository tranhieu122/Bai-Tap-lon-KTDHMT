#pragma once
#include "../engine/globals.h"

// ============================================================
// VEHICLE SYSTEM
// ============================================================

// Initialize vehicle pools
void vehiclesInit();

// Update vehicle logic
void vehiclesUpdate(float dt);

// Render all vehicles
void vehiclesDraw();
void vehicleDraw(const VehicleInfo& info, bool isShadow = false);

// Helper drawing functions
void carDraw(int colorVar, const std::string& bienSo, bool isTaxi = false, Color taxiCol = Color(1,1,0), bool isShadow = false);
void suvDraw(int colorVar, const std::string& bienSo, bool isShadow = false);
void busDraw(int colorVar, const std::string& bienSo, bool isShadow = false);
void truckDraw(VehicleType type, const std::string& bienSo, bool isShadow = false);
void emergencyVehicleDraw(int colorVar, const std::string& bienSo, bool isAmbulance, bool isPolice, bool isShadow = false);
void motorbikeDraw(int colorVar, const std::string& bienSo, bool isShadow = false);
void drawTricycle(int colorVar, const std::string& bienSo, bool isShadow = false);
void drawBicycle(int colorVar, bool isShadow = false);

 
