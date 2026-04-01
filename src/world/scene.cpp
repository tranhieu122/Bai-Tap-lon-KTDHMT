#include "../engine/globals.h"
#include "../engine/utils.h"
#include "../engine/lighting.h"
#include "../engine/texture.h"
#include "street.h"
#include "buildings.h"
#include "furniture.h"
#include "vegetation.h"
#include "../traffic/traffic.h"
#include "../traffic/people.h"
#include <vector>
#include <cmath>
#include <algorithm>

std::vector<FurnitureInfo> g_sceneFurniture;
std::vector<TreeInfo> g_sceneTrees;
std::vector<TrafficSignInfo> g_sceneSigns;

namespace {
int drawCalls = 0;

void spawnBuildings() {
  g_buildings.clear();

  // Block layout logic: Place buildings along main street Z axis
  float startZ = -STREET_LENGTH / 2.0f;
  float endZ = STREET_LENGTH / 2.0f;
  float bldgX = STREET_WIDTH / 2 + SIDEWALK_WIDTH + BLOCK_DEPTH / 2;

  // Draw 2 rows (Left side and Right side)
  for (int roadSide = 0; roadSide < 2; roadSide++) {
    float curZ = startZ + 1.0f;
    bool isRight = (roadSide == 1);
    float signX = isRight ? 1.0f : -1.0f;
    float signZ = 1.0f;

    while (curZ < endZ - 5.0f) {
      BuildingInfo info;
      int typeRoll = randomInt(0, 100);
      
      // Select Type
      if (typeRoll < 15)      info.type = BLDG_COLONIAL;
      else if (typeRoll < 30) info.type = BLDG_SHOPHOUSE;
      else if (typeRoll < 45) info.type = BLDG_APARTMENT_BLOCK;
      else if (typeRoll < 55) info.type = BLDG_MODERN_SHOPFRONT;
      else if (typeRoll < 65) info.type = BLDG_MARKET;
      else if (typeRoll < 75) info.type = BLDG_GLASS_TOWER;
      else if (typeRoll < 80) info.type = BLDG_SKYSCRAPER;
      else if (typeRoll < 85) info.type = BLDG_MODERN_VILLA;
      else                    info.type = (BuildingType)randomInt(BLDG_TUBE_HOUSE, BLDG_APARTMENT);

      // Width
      if (info.type == BLDG_GLASS_TOWER)        info.width = randomFloat(10.0f, 14.0f);
      else if (info.type == BLDG_SKYSCRAPER)     info.width = randomFloat(12.0f, 18.0f);
      else if (info.type == BLDG_MARKET)         info.width = randomFloat(12.0f, 15.0f);
      else if (info.type == BLDG_MODERN_VILLA)   info.width = randomFloat(8.0f, 12.0f);
      else                                      info.width = randomFloat(MIN_BUILDING_WIDTH, MAX_BUILDING_WIDTH);

      info.depth = BLOCK_DEPTH * randomFloat(0.8f, 1.0f);

      // Floors
      if (info.type == BLDG_GLASS_TOWER)        info.floors = randomInt(15, 25);
      else if (info.type == BLDG_SKYSCRAPER)     info.floors = randomInt(30, 50);
      else if (info.type == BLDG_COLONIAL)       info.floors = randomInt(2, 3);
      else if (info.type == BLDG_SHOPHOUSE)      info.floors = randomInt(3, 5);
      else if (info.type == BLDG_MARKET)         info.floors = 1;
      else if (info.type == BLDG_MODERN_VILLA)   info.floors = 2;
      else                                      info.floors = randomInt(2, 6);

      info.height = info.floors * FLOOR_HEIGHT + ((info.type == BLDG_SHOP || info.type == BLDG_MODERN_SHOPFRONT) ? SHOP_FRONT_HEIGHT : 0);
      info.colorVariant = randomInt(0, 15);
      info.hasShopFront = (randomInt(0, 2) != 0) || (info.type == BLDG_SHOPHOUSE);
      info.hasBalcony = (randomInt(0, 1) == 0);
      info.shopName = "SHOP " + std::to_string(randomInt(1, 999));

      // Position
      float zPos = signZ * (curZ + info.width / 2.0f);
      info.transform = Transform(Vector3(signX * bldgX, 0, zPos), Vector3(0, (isRight ? -90.0f : 90.0f), 0));

      g_buildings.push_back(info);
      curZ += info.width + 1.0f; // Gap
    }
  }

  // Add 2 FEATURE BUILDINGS at the center (Z=0)
  for (int roadSide = 0; roadSide < 2; roadSide++) {
    bool isRight = (roadSide == 1);
    float signX = isRight ? 1.0f : -1.0f;
    float bldgX = STREET_WIDTH / 2 + SIDEWALK_WIDTH + BLOCK_DEPTH / 2;

    BuildingInfo feature;
    feature.type = BLDG_GLASS_TOWER;
    feature.width = 15.0f;
    feature.floors = 35;
    feature.height = feature.floors * FLOOR_HEIGHT;
    feature.depth = BLOCK_DEPTH;
    feature.colorVariant = 5; // Blue glass
    feature.hasShopFront = true;
    feature.shopName = isRight ? "CENTRAL PLAZA R" : "CENTRAL PLAZA L";
    feature.transform = Transform(Vector3(signX * bldgX, 0, 0), Vector3(0, (isRight ? -90.0f : 90.0f), 0));
    g_buildings.push_back(feature);
  }
}

void spawnProps() {
  g_sceneFurniture.clear();
  g_sceneTrees.clear();
  g_sceneSigns.clear();

  float swX = STREET_WIDTH / 2 + 1.0f;
  float startZ = -STREET_LENGTH / 2;

  for (float z = startZ; z < STREET_LENGTH / 2; z += LAMP_SPACING) {
    // Lamps
    FurnitureInfo lampL; lampL.type = FURN_STREET_LAMP;
    lampL.transform = Transform(Vector3(-swX, 0, z), Vector3(0, 90, 0));
    g_sceneFurniture.push_back(lampL);

    FurnitureInfo lampR; lampR.type = FURN_STREET_LAMP;
    lampR.transform = Transform(Vector3(swX, 0, z), Vector3(0, -90, 0));
    g_sceneFurniture.push_back(lampR);

    // Sidewalk details
    if (randomInt(0, 100) < 20) {
        FurnitureInfo tea; tea.type = FURN_TEA_GROUP;
        tea.transform = Transform(Vector3(-swX - 1.5f, 0, z + 2.0f), Vector3(0, randomFloat(0, 360), 0));
        g_sceneFurniture.push_back(tea);
    }
    
    if (randomInt(0, 100) < 15) {
        FurnitureInfo stall; stall.type = FURN_VENDOR_STALL;
        stall.color = Color(randomFloat(0.5f, 1.0f), randomFloat(0.5f, 1.0f), 0.2f);
        stall.transform = Transform(Vector3(swX + 1.3f, 0, z + 4.0f), Vector3(0, -90, 0));
        g_sceneFurniture.push_back(stall);
    }
    
    // Electric Poles - placed every LAMP_SPACING
    FurnitureInfo poleL; poleL.type = FURN_ELECTRIC_POLE;
    poleL.transform = Transform(Vector3(-swX - 1.0f, 0, z), Vector3(0, 0, 0));
    g_sceneFurniture.push_back(poleL);

    FurnitureInfo poleR; poleR.type = FURN_ELECTRIC_POLE;
    poleR.transform = Transform(Vector3(swX + 1.0f, 0, z), Vector3(0, 180, 0));
    g_sceneFurniture.push_back(poleR);
  }

  // Trees loop
  for (float z = startZ; z < STREET_LENGTH / 2; z += TREE_SPACING) {
    TreeInfo tL; tL.type = (TreeType)randomInt(0, 2); tL.age = 1.2f;
    tL.transform = Transform(Vector3(-swX - 1.5f, 0, z));
    g_sceneTrees.push_back(tL);

    TreeInfo tR; tR.type = (TreeType)randomInt(0, 2); tR.age = 1.2f;
    tR.transform = Transform(Vector3(swX + 1.5f, 0, z));
    g_sceneTrees.push_back(tR);
  }

  // Bus stops
  FurnitureInfo bus; bus.type = FURN_BUS_STOP;
  bus.transform = Transform(Vector3(-swX - 1.5f, 0, -25.0f), Vector3(0, 90, 0));
  g_sceneFurniture.push_back(bus);
  bus.transform = Transform(Vector3(swX + 1.5f, 0, 25.0f), Vector3(0, -90, 0));
  g_sceneFurniture.push_back(bus);
}
} // namespace

void sceneInit() {
  seedRandom(42);
  spawnBuildings();
  spawnProps();
}

void sceneDraw() {
  drawCalls = 0;
  furnitureClearState(); 
  
  lightingApplyGlobal();
  lightingApplyLocal();
  streetDraw(); drawCalls++;
  for (const auto &b : g_buildings) { drawBuilding(b); drawCalls++; }
  for (const auto &f : g_sceneFurniture) { furnitureDraw(f); drawCalls++; }
  
  drawStreetWires();
  
  trafficLightsDraw(); drawCalls++;
  for (const auto &t : g_sceneTrees) { vegetationDraw(t); drawCalls++; }
}

void drawTransparentObjects() {}
int sceneGetDrawCalls() { return drawCalls; }
int sceneGetPolygons() { return drawCalls * 250; }
void drawShadowCasters() {
  for (const auto &b : g_buildings) drawBuilding(b);
  for (const auto &f : g_sceneFurniture) furnitureDraw(f);
  trafficLightsDraw();
}
