#include "furniture.h"
#include "../engine/globals.h"
#include "../engine/utils.h"
#include "../engine/camera.h"
#include "../world/sky.h"
#include <GL/freeglut.h>
#include <vector>

// Cấu trúc để lưu vị trí các cột điện để nối dây
static std::vector<Vector3> g_electricPolePositions;

void drawElectricPole() {
  // Thân cột bê tông (Concrete pole)
  Palette::CONCRETE.applyMaterial();
  glPushMatrix();
  glTranslatef(0, 4.0f, 0);
  drawCylinder(0.2f, 8.0f, 12);
  glPopMatrix();

  // Xà sứ (Cross-arms with insulators)
  Palette::METAL_DARK.applyMaterial();
  for (float y = 6.5f; y <= 7.5f; y += 0.5f) {
    glPushMatrix();
    glTranslatef(0, y, 0);
    drawCube(1.2f, 0.1f, 0.1f);
    
    // Sứ cách điện (Insulators)
    Palette::WALL_WHITE.applyMaterial();
    for (float x = -0.5f; x <= 0.5f; x += 0.5f) {
      glPushMatrix();
      glTranslatef(x, 0.15f, 0);
      drawCylinder(0.06f, 0.2f, 8);
      glPopMatrix();
    }
    glPopMatrix();
  }

  // Tủ điện/Công tơ (Electric meters/boxes)
  Palette::METAL_GRAY.applyMaterial();
  glPushMatrix();
  glTranslatef(0.15f, 3.0f, 0);
  drawCube(0.4f, 0.6f, 0.3f);
  glPopMatrix();
}

void drawStreetWires() {
    if (g_electricPolePositions.size() < 2) return;
    
    glDisable(GL_LIGHTING);
    glColor3f(0.1f, 0.1f, 0.1f);
    glLineWidth(1.5f);
    
    for (size_t i = 0; i < g_electricPolePositions.size() - 1; i++) {
        Vector3 p1 = g_electricPolePositions[i];
        Vector3 p2 = g_electricPolePositions[i+1];
        
        // Chỉ nối các cột cùng phía đường (X cùng dấu và gần nhau)
        if (p1.x * p2.x < 0 || fabsf(p1.z - p2.z) > 25.0f) continue;
        
        // Vẽ nhiều sợi dây với độ võng khác nhau
        for (int j = 0; j < 4; j++) {
            Vector3 start = p1 + Vector3(0, 6.5f + j*0.3f, 0);
            Vector3 end = p2 + Vector3(0, 6.5f + j*0.3f, 0);
            drawWire(start, end, 0.3f + j*0.15f, 16);
        }
    }
    
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
}

void streetLampDraw(bool isOn) {
  // Pole - Trụ đèn
  Palette::METAL_DARK.applyMaterial();
  glPushMatrix();
  glTranslatef(0, LAMP_HEIGHT / 2, 0);
  drawCylinder(0.12f, LAMP_HEIGHT, 12); // Tăng độ mịn
  glPopMatrix();

  // Base - Chân đế trụ
  Palette::METAL_GRAY.applyMaterial();
  glPushMatrix();
  glTranslatef(0, 0.4f, 0);
  drawCylinder(0.25f, 0.8f, 16); 
  glPopMatrix();

  // Arm - Tay đèn
  Palette::METAL_DARK.applyMaterial();
  glPushMatrix();
  glTranslatef(0.6f, LAMP_HEIGHT - 0.2f, 0);
  glRotatef(90, 0, 0, 1);
  drawCylinder(0.08f, 1.2f, 8);
  glPopMatrix();

  // Lamp Head - Đầu đèn
  glPushMatrix();
  glTranslatef(1.2f, LAMP_HEIGHT - 0.25f, 0);
  Palette::METAL_GRAY.applyMaterial();
  drawCube(0.5f, 0.2f, 0.35f); // Thân đầu đèn to hơn
  glPopMatrix();

  // Bulb / Light / Glow Effect
  bool lightActive = isOn && g_streetLightsOn;

  glPushMatrix();
  glTranslatef(1.2f, LAMP_HEIGHT - 0.35f, 0);
  if (lightActive) {
    Color warmLight(1.0f, 0.85f, 0.4f); // Warm sodium color
    setEmissiveMaterial(warmLight, warmLight);
    
    // HIỆU ỨNG QUẦNG SÁNG (GLOW HALO) - Nhiều lớp Billboard
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDepthMask(GL_FALSE);

    glPushMatrix();
    glRotatef(cameraGetYaw(), 0, -1, 0); 
    
    // Lớp 1: Lõi sáng (Core)
    warmLight.a = 0.8f; warmLight.apply();
    drawQuad(0.6f, 0.6f);
    
    // Lớp 2: Quầng trung tâm (Inner Halo)
    warmLight.a = 0.3f; warmLight.apply();
    drawQuad(1.8f, 1.8f);

    // Lớp 3: Quầng rộng (Outer Glow)
    warmLight.a = 0.1f; warmLight.apply();
    drawQuad(4.0f, 4.0f);
    
    glPopMatrix();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
  } else {
    Palette::GLASS_DARK.applyMaterial();
  }
  drawCube(0.4f, 0.1f, 0.25f); // Mặt kính đèn
  resetMaterial();
  glPopMatrix();
}

void parkBenchDraw() {
  // Bề mặt gỗ (Wood slats)
  Palette::WOOD_DARK.applyMaterial();

  const float benchW = 1.6f;
  // Ghế ngồi (Seat)
  for (int i = 0; i < 4; i++) {
    glPushMatrix();
    glTranslatef(0, 0.45f, i * 0.12f - 0.18f);
    drawCube(benchW, 0.04f, 0.1f);
    glPopMatrix();
  }
  // Tựa lưng (Backrest)
  for (int i = 0; i < 3; i++) {
    glPushMatrix();
    glTranslatef(0, 0.65f + i * 0.12f, -0.25f);
    glRotatef(12, 1, 0, 0);
    drawCube(benchW, 0.1f, 0.04f);
    glPopMatrix();
  }

  // Khung gang (Cast iron frame)
  Palette::IRON_BLACK.applyMaterial();
  float legX[] = { 0.7f, -0.7f };
  for (int i=0; i<2; i++) {
      glPushMatrix();
      glTranslatef(legX[i], 0.22f, -0.1f);
      drawCube(0.08f, 0.45f, 0.5f); // Chân ghế
      
      // Tay vịn (Armrest)
      glTranslatef(0.0f, 0.35f, 0.1f);
      drawCube(0.08f, 0.05f, 0.3f);
      glPopMatrix();
  }
}

void trashCanDraw() {
  // Thùng rác nhựa xanh phổ biến Việt Nam (Môi trường & Đô thị)
  Palette::MEDIAN_GREEN.applyMaterial();

  // Thân thùng (Cylinder)
  glPushMatrix();
  glTranslatef(0, 0.45f, 0);
  drawCylinder(0.3f, 0.9f, 16); 
  glPopMatrix();

  // Nắp thùng (Hood)
  glPushMatrix();
  glTranslatef(0, 0.95f, 0);
  drawHemisphere(0.32f, 16, 8);
  glPopMatrix();

  // Logo/Ký hiệu môi trường (White box proxy)
  Palette::WALL_WHITE.applyMaterial();
  glPushMatrix();
  glTranslatef(0, 0.6f, 0.25f);
  drawCube(0.15f, 0.15f, 0.12f);
  glPopMatrix();

  // Chân đế/Cột trụ (Metal)
  Palette::METAL_GRAY.applyMaterial();
  glPushMatrix();
  glTranslatef(0, 0.05f, 0);
  drawDisk(0.0f, 0.4f, 12); // Đế tròn sát đất
  glPopMatrix();
}

void powerPoleDraw() {
  // Concrete cylindrical pole typical in VN
  Palette::CONCRETE.applyMaterial();

  float poleH = 8.0f;
  glPushMatrix();
  glTranslatef(0, poleH / 2, 0);
  drawCylinder(0.15f, poleH, 8);
  glPopMatrix();

  // Cross arms
  Palette::METAL_GRAY.applyMaterial();
  glPushMatrix();
  glTranslatef(0, poleH - 0.5f, 0);
  drawCube(1.5f, 0.1f, 0.1f);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0, poleH - 1.2f, 0);
  drawCube(1.2f, 0.1f, 0.1f);
  glPopMatrix();

  // Insulators
  Palette::WALL_WHITE.applyMaterial();
  for (float x = -0.6f; x <= 0.6f; x += 0.4f) {
    if (fabs(x) < 0.1f)
      continue;
    glPushMatrix();
    glTranslatef(x, poleH - 0.4f, 0);
    drawCylinder(0.05f, 0.15f, 6);
    glPopMatrix();
  }

  // Mess of telecom wires (simplified as black box cluster on pole mid)
  Palette::IRON_BLACK.applyMaterial();
  glPushMatrix();
  glTranslatef(0, poleH / 2 + 1.0f, 0);
  drawCylinder(0.25f, 1.0f, 8); // Tangle of wires
  glPopMatrix();
}

void streetSignDraw(const char *text) {
  // Pole
  Palette::METAL_GRAY.applyMaterial();
  glPushMatrix();
  glTranslatef(0, 1.2f, 0);
  drawCylinder(0.04f, 2.4f, 6);
  glPopMatrix();

  // Board (Blue rect in VN for street names)
  Color(0.1f, 0.3f, 0.8f).applyMaterial();
  glPushMatrix();
  glTranslatef(0, 2.3f, 0.05f);
  drawCube(0.8f, 0.3f, 0.02f);
  glPopMatrix();

  // Text
  if (text) {
    glColor3f(1, 1, 1);
    glDisable(GL_LIGHTING);
    drawText3D(-0.35f, 2.25f, 0.07f, text);
    glEnable(GL_LIGHTING);
  }
}

void busStopDraw() {
  // Main structure - shelter
  float shelterW = 3.0f;
  float shelterH = 2.8f;
  float shelterD = 1.5f;

  // Pillars (4 corners)
  Palette::METAL_GRAY.applyMaterial();
  float px = shelterW / 2.0f - 0.1f;
  float pz = shelterD / 2.0f - 0.1f;
  glPushMatrix(); glTranslatef(-px, shelterH / 2, pz); drawCube(0.1f, shelterH, 0.1f); glPopMatrix();
  glPushMatrix(); glTranslatef( px, shelterH / 2, pz); drawCube(0.1f, shelterH, 0.1f); glPopMatrix();
  glPushMatrix(); glTranslatef(-px, shelterH / 2, -pz); drawCube(0.1f, shelterH, 0.1f); glPopMatrix();
  glPushMatrix(); glTranslatef( px, shelterH / 2, -pz); drawCube(0.1f, shelterH, 0.1f); glPopMatrix();

  // Roof
  Palette::METAL_DARK.applyMaterial();
  glPushMatrix();
  glTranslatef(0, shelterH + 0.05f, 0);
  drawCube(shelterW + 0.3f, 0.1f, shelterD + 0.3f);
  glPopMatrix();

  // Back panel
  Palette::GLASS_CLEAR.applyGlossyMaterial();
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glPushMatrix();
  glTranslatef(0, shelterH / 2, -pz - 0.05f);
  drawQuad(shelterW - 0.2f, shelterH - 0.2f);
  glPopMatrix();
  glDisable(GL_BLEND);

  // Bench inside
  Palette::WOOD_DARK.applyMaterial();
  glPushMatrix();
  glTranslatef(0, 0.45f, -pz + 0.3f);
  drawCube(shelterW - 0.6f, 0.06f, 0.35f);
  // Bench legs
  Palette::METAL_GRAY.applyMaterial();
  glPushMatrix(); glTranslatef(-shelterW / 2 + 0.5f, -0.2f, 0); drawCube(0.05f, 0.4f, 0.05f); glPopMatrix();
  glPushMatrix(); glTranslatef( shelterW / 2 - 0.5f, -0.2f, 0); drawCube(0.05f, 0.4f, 0.05f); glPopMatrix();
  glPopMatrix();

  // Sign board
  Color(0.1f, 0.4f, 0.8f).applyMaterial();
  glPushMatrix();
  glTranslatef(0, shelterH + 0.4f, 0);
  drawCube(1.5f, 0.4f, 0.05f);
  // Text
  glDisable(GL_LIGHTING);
  glColor3f(1, 1, 1);
  drawText3D(-0.4f, -0.05f, 0.03f, "BUS STOP");
  glEnable(GL_LIGHTING);
  glPopMatrix();
}

void stoolDraw(Color col) {
  // Ghế nhựa "huyền thoại" Việt Nam - chi tiết hơn
  col.applyMaterial();
  float w = 0.35f, h = 0.45f;
  
  // Mặt ghế (Seat)
  glPushMatrix();
  glTranslatef(0, h - 0.03f, 0);
  drawRoundedBox(w, 0.06f, w, 0.05f, 4); // Dùng rounded box cho mặt ghế
  glPopMatrix();

  // 4 chân ghế có độ vát (Tapered legs)
  for (int i = 0; i < 4; i++) {
    glPushMatrix();
    float tx = (i < 2 ? 1 : -1) * (w / 2 - 0.05f);
    float tz = (i % 2 == 0 ? 1 : -1) * (w / 2 - 0.05f);
    glTranslatef(tx, h / 2 - 0.03f, tz);
    glRotatef((tx < 0 ? 1.0f : -1.0f) * 8.0f, 0, 0, 1);
    glRotatef((tz < 0 ? 1.0f : -1.0f) * 8.0f, 1, 0, 0);
    drawCube(0.04f, h - 0.06f, 0.04f);
    glPopMatrix();
  }
}

void teaTableDraw() {
  // Bàn trà gỗ thấp
  Palette::WOOD_DARK.applyMaterial();
  float w = 0.65f, h = 0.42f;
  
  // Mặt bàn
  glPushMatrix();
  glTranslatef(0, h, 0);
  drawCube(w, 0.05f, w);
  glPopMatrix();

  // Khung và chân bàn
  Palette::IRON_BLACK.applyMaterial();
  for (int i=0; i<4; i++) {
      glPushMatrix();
      float tx = (i < 2 ? 0.25f : -0.25f);
      float tz = (i % 2 == 0 ? 0.25f : -0.25f);
      glTranslatef(tx, h / 2, tz);
      drawCylinder(0.02f, h, 8);
      glPopMatrix();
  }
}

void planterBoxDraw() {
  Palette::CONCRETE.applyMaterial();

  // Box
  glPushMatrix();
  glTranslatef(0, 0.3f, 0);
  drawCube(1.2f, 0.6f, 1.2f);
  glPopMatrix();

  // Soil
  Palette::TREE_BARK.applyMaterial(); // Brown dirt
  glPushMatrix();
  glTranslatef(0, 0.61f, 0);
  drawQuadXZ(1.1f, 1.1f);
  glPopMatrix();
}

// ============================================================
// VIETNAMESE SIDEWALK STALL (Ganh hang rong)
// ============================================================
void sidewalkStallDraw(Color col) {
    // Gánh hàng rong truyền thống
    Palette::WOOD_DARK.applyMaterial();
    
    // Đòn gánh (Bamboo pole)
    glPushMatrix();
    glTranslatef(0, 1.2f, 0);
    drawCylinder(0.03f, 2.2f, 8); // Xoay lại thành cylinder nằm ngang
    glPopMatrix();

    // Thúng/Mẹt (Baskets)
    for (int i = 0; i < 2; i++) {
        float bx = (i == 0 ? 0.9f : -0.9f);
        glPushMatrix();
        glTranslatef(bx, 0.35f, 0);
        col.applyMaterial();
        drawCylinder(0.45f, 0.5f, 16); // Thân thúng rộng hơn
        
        // Vành thúng (Rim)
        Palette::WOOD_LIGHT.applyMaterial();
        glPushMatrix(); 
        glTranslatef(0, 0.22f, 0); 
        drawDisk(0.42f, 0.48f, 16); 
        glPopMatrix();

        // Dây treo (Ropes)
        Palette::METAL_GRAY.applyMaterial();
        glBegin(GL_LINES);
        glVertex3f(0, 0.85f, 0); glVertex3f(0, 0.25f, 0);
        glEnd();
        glPopMatrix();
    }
}

// ============================================================
// TEA GROUP (Tra da via he)
// ============================================================
void teaGroupDraw() {
    // One table and 4 stools around it
    glPushMatrix();
    teaTableDraw();
    
    // Area around table
    for (int i = 0; i < 4; i++) {
        float ang = i * 90.0f + 45.0f;
        float rad = 0.7f;
        glPushMatrix();
        glTranslatef(rad * cosf(ang * (float)M_PI / 180.0f), 0, rad * sinf(ang * (float)M_PI / 180.0f));
        // Random colored plastic stools
        Color stoolCols[] = { Palette::VEH_RED, Palette::VEH_BLUE, Color(0.8f, 0.8f, 0.1f) };
        stoolDraw(stoolCols[i % 3]);
        glPopMatrix();
    }
    glPopMatrix();
}


void furnitureDraw(const FurnitureInfo &info) {
  glPushMatrix();
  info.transform.applyGL();

  switch (info.type) {
  case FURN_STREET_LAMP:
    streetLampDraw((g_timeOfDay < 6.0f || g_timeOfDay > 18.0f));
    break;
  case FURN_BENCH:
    parkBenchDraw();
    break;
  case FURN_TRASH_CAN:
    trashCanDraw();
    break;
  case FURN_POWER_POLE:
    powerPoleDraw();
    break;
  case FURN_STREET_SIGN:
    streetSignDraw("Xuan Thuy");
    break;
  case FURN_BUS_STOP:
    busStopDraw();
    break;
  case FURN_PLANTER_BOX:
    planterBoxDraw();
    break;
  case FURN_STOOL:
    stoolDraw(info.color);
    break;
  case FURN_TEA_TABLE:
    teaTableDraw();
    break;
  case FURN_VENDOR_STALL:
    sidewalkStallDraw(info.color);
    break;
  case FURN_TEA_GROUP:
    teaGroupDraw();
    break;
  case FURN_ELECTRIC_POLE:
    drawElectricPole();
    // Lưu lại vị trí để nối dây điện
    g_electricPolePositions.push_back(info.transform.position);
    break;
  }

  glPopMatrix();
}

void furnitureClearState() {
    g_electricPolePositions.clear();
}
