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
void vehicleDraw(const VehicleInfo& info);

// Helper drawing functions
void drawCar(int type, int colorIdx);
void drawMotorbike(int type, int colorIdx);
void drawBus(int type, int colorIdx);
void carDraw(int colorVar, const std::string& bienSo, bool isTaxi = false, Color taxiCol = Color(1,1,0));
void suvDraw(int colorVar, const std::string& bienSo);
void busDraw(int colorVar, const std::string& bienSo);
void truckDraw(VehicleType type, const std::string& bienSo);
void emergencyVehicleDraw(int colorVar, const std::string& bienSo, bool isAmbulance, bool isPolice);
void drawBicycle(int colorVar);

 
