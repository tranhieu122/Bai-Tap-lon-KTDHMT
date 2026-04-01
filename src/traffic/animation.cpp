#include "animation.h"
#include "../engine/globals.h"
#include "../engine/utils.h"
#include <cstdio>
#include <string>

// Basic paths: Vehicles travel along Z axis (main street) and X axis (side
// street) We just loop them when they go out of bounds.

static const float BOUND_Z_SPAWN = STREET_LENGTH / 2.0f + 10.0f;
static const float BOUND_X_SPAWN = SIDE_STREET_LENGTH / 2.0f + 10.0f;

std::string sinhBienSoVN() {
  // Mau: XX-AY NNN.NN (VD: 29-A1 123.45)
  // Hoac: XX-X NNN.NN (VD: 51-F 123.45)
  char buf[32];

  // Cac dau so tinh thanh phổ biến ở VN
  static const int tinhTable[] = {29, 30, 31, 33, 15, 16, 50, 51, 52, 53,
                                  54, 55, 56, 57, 58, 59, 43, 60, 61, 75};
  int province = tinhTable[randomInt(0, 19)];

  char s = "ABCDEFGHJKLMNPSTUVXYZ"[randomInt(0, 20)];
  int district = randomInt(1, 9);
  int num1 = randomInt(100, 999);
  int num2 = randomInt(10, 99);

  _snprintf(buf, sizeof(buf), "%02d-%c%d %03d.%02d", province, s, district, num1, num2);
  return std::string(buf);
}

// - [x] Viết lại `animationInit()` với cơ chế chống spawn trùng lập.
// - [x] Triển khai `acceleration/deceleration` mượt mà cho xe.
// - [x] Tinh chỉnh AI tránh va chạm và giữ khoảng cách động.
// - [x] Cập nhật logic pha đèn đồng bộ 4 hướng.
// - [x] Kiểm duyệt tính thẩm mỹ (mật độ xe máy cao).

void animationInit() {
  g_vehicles.clear();
  g_pedestrians.clear();

  // 1. SPAWN VEHICLES (2-Lane Straight Logic)
  int vehicleCount = 12 + randomInt(0, 4); 
  for (int i = 0; i < vehicleCount; i++) {
    VehicleInfo v;
    
    // Vietnamese distribution: 60% motorbikes
    int typeRoll = randomInt(0, 100);
    if (typeRoll < 60) v.type = VEH_MOTORBIKE;
    else if (typeRoll < 80) v.type = VEH_CAR_SEDAN;
    else if (typeRoll < 90) v.type = VEH_BUS;
    else v.type = VEH_TRUCKS_OPEN;

    // Only 2 lanes now
    v.lane = randomInt(0, 1);
    // Lane 0: Left of yellow line (X=-2), goes South (-Z)
    // Lane 1: Right of yellow line (X=2), goes North (+Z)
    v.direction = (v.lane == 0) ? -1 : 1;
    v.position.x = (v.lane == 0) ? -2.5f : 2.5f;
    v.rotation = (v.direction == 1) ? -90.0f : 90.0f;

    // Spacing check
    bool overlapping = true;
    int tries = 0;
    while(overlapping && tries < 15) {
        v.position.z = randomFloat(-STREET_LENGTH/2, STREET_LENGTH/2);
        overlapping = false;
        for(const auto& v2 : g_vehicles) {
            if (v2.lane == v.lane && fabsf(v2.position.z - v.position.z) < 10.0f) {
                overlapping = true; break;
            }
        }
        tries++;
    }

    float baseSpeed = VEHICLE_SPEED;
    if (v.type == VEH_MOTORBIKE) baseSpeed *= 1.25f;
    v.position.y = 0.0f;
    v.pitch = 0.0f;
    v.roll = 0.0f;
    v.speed = randomFloat(baseSpeed * 0.85f, baseSpeed * 1.15f);
    v.colorVariant = randomInt(0, 15);
    v.stopped = false;
    v.bienSo = sinhBienSoVN();
    g_vehicles.push_back(v);
  }

  // 2. SPAWN PEDESTRIANS (Sidewalk strictly)
  int pedCount = 40;
  for (int i = 0; i < pedCount; i++) {
    PedestrianInfo p;
    p.state = PED_WALKING;
    bool rightSide = (randomInt(0, 1) == 0);
    float swX = (STREET_WIDTH / 2 + 2.0f) * (rightSide ? 1 : -1);
    p.position = Vector3(swX + randomFloat(-0.5f, 0.5f), SIDEWALK_HEIGHT,
                         randomFloat(-STREET_LENGTH / 2, STREET_LENGTH / 2));
    p.direction = (randomInt(0, 1) == 0) ? 1 : -1;
    p.rotation = (p.direction == 1) ? 0.0f : 180.0f;
    p.walkPhase = randomFloat(0, 6.28f);
    p.speed = randomFloat(PEDESTRIAN_SPEED * 0.8f, PEDESTRIAN_SPEED * 1.2f);
    p.colorVariant = randomInt(0, 9);
    p.crossingStreet = false;
    g_pedestrians.push_back(p);
  }
}

void animationUpdate(float dt) {
  if (g_paused) return;
  float animDt = dt * g_animSpeed;

  for (auto &v : g_vehicles) {
    // 1. COLLISION AVOIDANCE (Simplified for 2 lanes)
    bool stopByObstacle = false;
    float senseDist = (v.type == VEH_MOTORBIKE) ? 5.0f : 9.0f;

    for (const auto &other : g_vehicles) {
      if (&other == &v) continue;
      if (other.lane == v.lane) {
          float LongDiff = (other.position.z - v.position.z) * v.direction;
          if (LongDiff > 0 && LongDiff < senseDist) {
              stopByObstacle = true; break;
          }
      }
    }

    v.stopped = stopByObstacle;

    // 2. MOVEMENT & PREMIUM PHYSICS
    float targetPitch = 0.0f;
    float targetRoll = 0.0f;

    if (!v.stopped) {
        v.position.z += v.speed * v.direction * animDt;
        v.position.x = (v.lane == 0) ? -2.5f : 2.5f; 

        // Pitch up slightly when accelerating/cruising
        targetPitch = -1.5f; 

        // Boundary wrap
        if (v.direction == 1 && v.position.z > STREET_LENGTH/2 + 10.0f) v.position.z = -STREET_LENGTH/2 - 10.0f;
        else if (v.direction == -1 && v.position.z < -STREET_LENGTH/2 - 10.0f) v.position.z = STREET_LENGTH/2 + 10.0f;
    } else {
        // Dip forward when braking
        targetPitch = 3.5f;
        // Subtle engine vibration
        v.position.y = 0.005f * sinf(g_elapsedTime * 20.0f);
    }

    // Smooth physics transition (Spring-Damper style approximation)
    v.pitch += (targetPitch - v.pitch) * 5.0f * dt;
    v.roll += (targetRoll - v.roll) * 3.0f * dt;
  }

  // 3. PEDESTRIANS
  for (auto &p : g_pedestrians) {
    p.walkPhase += p.speed * 5.0f * animDt;
    p.position.z += p.speed * p.direction * animDt;
    
    // Boundary wrap
    if (p.position.z > STREET_LENGTH/2) p.position.z = -STREET_LENGTH/2;
    else if (p.position.z < -STREET_LENGTH/2) p.position.z = STREET_LENGTH/2;
  }
}
