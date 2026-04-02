#pragma once
#include "../engine/globals.h"

// ============================================================
// BUILDING SYSTEM
// ============================================================

// Build geometry for a specific building at runtime
void drawBuilding(const BuildingInfo& info, bool isShadow = false);
void drawTubeHouse(float w, float h, float d, int floors, int colorIdx, bool hasShop, bool hasBalcony, const std::string& shopName);
void drawOfficeBuilding(float w, float h, float d, int floors, int colorIdx);
void drawShop(float w, float h, float d, int colorIdx, const std::string& shopName);
void drawCafe(float w, float h, float d, int colorIdx, const std::string& shopName);
void drawOldApartment(float w, float h, float d, int floors, int colorIdx);
void drawColonialBuilding(float w, float h, float d, int colorIdx);
void drawShophouse(float w, float h, float d, int floors, int colorIdx, const std::string& shopName);
void drawApartmentBlock(float w, float h, float d, int floors, int colorIdx);
void drawModernShopfront(float w, float h, float d, int colorIdx, const std::string& shopName);
void drawMarketBuilding(float w, float h, float d, int colorIdx);
void drawTempleBuilding(float w, float h, float d, int colorIdx);
void drawGarageBuilding(float w, float h, float d, int colorIdx);



static void drawArchedWindow(float w, float h, int variant);
