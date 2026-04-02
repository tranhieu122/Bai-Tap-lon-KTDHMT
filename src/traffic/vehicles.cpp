#include <cstdio>
#include <string>
#include <vector>
#include <cmath>

#include "vehicles.h"
#include "../engine/texture.h"
#include "../engine/utils.h"
#include "../world/sky.h"
#include "../world/furniture.h"
#include "../world/vegetation.h"

void vehiclesInit() {
}

void vehiclesUpdate(float dt) {
    (void)dt;
}

// ============================================================
// WHEEL UTILITY
// ============================================================
static void drawWheel(float radius, float width) {
    Palette::IRON_BLACK.applyMaterial();
    glPushMatrix();
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f); // Flat
    drawCylinder(radius, width, 12);
    glPopMatrix();
    
    // Hubcap
    Palette::METAL_LIGHT.applyMaterial();
    glPushMatrix();
    glTranslatef(width / 2.0f + 0.01f, 0.0f, 0.0f);
    drawDisk(0.0f, radius * 0.6f, 8);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-width / 2.0f - 0.01f, 0.0f, 0.0f);
    glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
    drawDisk(0.0f, radius * 0.6f, 8);
    glPopMatrix();
}

static void drawHeadlight(float radius, bool isRear) {
    if (isRear) {
        Color c = Palette::NEON_RED;
        bool night = g_timeOfDay < 6.0f || g_timeOfDay > 18.0f;
        if (night) setEmissiveMaterial(c, c);
        else c.applyMaterial();
    } else {
        bool night = g_timeOfDay < 6.0f || g_timeOfDay > 18.0f;
        Color c = Color(1,1,1);
        if (night) setEmissiveMaterial(c, c);
        else c.applyMaterial();
    }
    drawHemisphere(radius, 6, 4);
    resetMaterial();
}

static void drawLicensePlate(const std::string& text) {
    if (text.empty()) return;
    
    glPushMatrix();
    // Slightly more compact plate
    glScalef(0.5f, 0.5f, 1.0f); 
    
    // Plate background (White) - using high-quality white
    Color(0.95f, 0.95f, 1.0f).applyMaterial();
    drawCube(0.4f, 0.2f, 0.01f);
    
    // Text (Black)
    glDisable(GL_LIGHTING);
    glColor3f(0.05f, 0.05f, 0.05f);
    // Position text precisely
    glPushMatrix();
    glTranslatef(0, -0.05f, 0.01f);
    glScalef(0.55f, 0.55f, 1.0f);
    float tw = (float)text.length() * 0.025f;
    drawText3D(-tw / 2.0f, 0, 0, text.c_str());
    glPopMatrix(); 
    glEnable(GL_LIGHTING);
    glPopMatrix();
}
static void drawRider(int colorVar, int style, bool isShadow) {
    if (isShadow) {
        // Draw simplified shadow version
        glPushMatrix();
        glTranslatef(-0.1f, 0.9f, 0);
        drawCube(0.3f, 0.6f, 0.3f); // Body
        glTranslatef(0, 0.45f, 0);
        drawSphere(0.18f, 6, 4); // Head/Helmet
        glPopMatrix();
        return;
    }

    Color shirtCol = getShirtColor(colorVar % 16);
    Color skinCol(0.85f, 0.7f, 0.6f);
    Color helmetCol = getVehicleColor((colorVar + 3) % 12);
    
    glPushMatrix();
    // Torso starts from origin (sitting point)
    
    // 1. Torso (Body)
    shirtCol.applyMaterial();
    glPushMatrix();
    glRotatef(-10.0f, 0, 0, 1); // Lean forward slightly
    glTranslatef(0, 0.3f, 0);
    drawCube(0.28f, 0.55f, 0.28f);
    
    // 2. Head & Helmet/Hat
    glPushMatrix();
    glTranslatef(0.1f, 0.45f, 0); // Position on top of torso
    skinCol.applyMaterial();
    drawSphere(0.12f, 10, 10); // Face/Head
    
    if (style == 0) { // MOTORBIKE: Modern Glossy Helmet
        // Use more vibrant, high-contrast helmet colors
        Color brightHelmet = helmetCol;
        if (colorVar % 2 == 0) brightHelmet = Palette::VEH_RED;
        else if (colorVar % 3 == 0) brightHelmet = Palette::VEH_YELLOW;
        brightHelmet.applyGlossyMaterial();
        
        glPushMatrix();
        glTranslatef(-0.02f, 0.08f, 0);
        drawSphere(0.19f, 12, 10); // Much larger helmet shell
        
        // Visor (Prominent Black glass)
        Palette::IRON_BLACK.applyMaterial();
        glPushMatrix();
        glTranslatef(0.12f, -0.02f, 0);
        drawCube(0.15f, 0.08f, 0.28f); // Larger, more visible visor
        glPopMatrix();
        glPopMatrix();
    } else { // BICYCLE/TRICYCLE: Traditonal Conical Hat (Non La)
        // Bright straw color for visibility
        Color(0.95f, 0.9f, 0.7f).applyMaterial(); 
        glPushMatrix();
        glTranslatef(0, 0.08f, 0);
        glRotatef(-5, 0, 0, 1); 
        drawCone(0.45f, 0.25f, 16); // Very wide and tall conical hat
        glPopMatrix();
    }
    glPopMatrix();
    glPopMatrix();
    
    // 3. Arms (reaching for handlebars)
    shirtCol.applyMaterial();
    // 3. Arms (reaching for handlebars)
    shirtCol.applyMaterial();
    // Left Arm
    glPushMatrix();
    glTranslatef(0.15f, 0.25f, 0.2f); // Shoulder
    glRotatef(-135.0f, 0, 0, 1);      // Point forward-down
    glTranslatef(0, 0.25f, 0);        // Center cylinder (length 0.5)
    drawCylinder(0.06f, 0.5f, 6);
    glPopMatrix();
    // Right Arm
    glPushMatrix();
    glTranslatef(0.15f, 0.25f, -0.2f);
    glRotatef(-135.0f, 0, 0, 1);
    glTranslatef(0, 0.25f, 0);
    drawCylinder(0.06f, 0.5f, 6);
    glPopMatrix();
    
    // 4. Legs (bent at knees)
    Palette::METAL_DARK.applyMaterial(); // Pants
    // Left Leg
    glPushMatrix();
    glTranslatef(-0.05f, -0.1f, 0.15f); // Hip
    glRotatef(-120.0f, 0, 0, 1);        // Thigh: forward-down
    glTranslatef(0, 0.2f, 0);           // Center cylinder (length 0.4)
    drawCylinder(0.08f, 0.4f, 6);
    glTranslatef(0, 0.2f, 0);           // knee joint
    glRotatef(80.0f, 0, 0, 1);          // Calf: bend back
    glTranslatef(0, 0.2f, 0);           // Center cylinder (length 0.4)
    drawCylinder(0.07f, 0.4f, 6);
    glPopMatrix();
    // Right Leg
    glPushMatrix();
    glTranslatef(-0.05f, -0.1f, -0.15f);
    glRotatef(-120.0f, 0, 0, 1);
    glTranslatef(0, 0.2f, 0);
    drawCylinder(0.08f, 0.4f, 6);
    glTranslatef(0, 0.2f, 0);
    glRotatef(80.0f, 0, 0, 1);
    glTranslatef(0, 0.2f, 0);
    drawCylinder(0.07f, 0.4f, 6);
    glPopMatrix();
    
    glPopMatrix();
    glPopMatrix();
    resetMaterial();
}

// ============================================================
// HIGH-DETAIL MOTORBIKE (Honda Wave/SH style)
// ============================================================
void motorbikeDraw(int colorVar, const std::string& bienSo, bool isShadow) {
    float wheelRadius = 0.3f;
    float wheelW = 0.08f;
    Color bodyCol = getVehicleColor(colorVar);
    
    // 1. Wheels with Spoke detail
    for (int i = 0; i < 2; i++) {
        float x = (i == 0) ? 0.7f : -0.6f;
        glPushMatrix();
        glTranslatef(x, wheelRadius, 0.0f);
        drawWheel(wheelRadius, wheelW);
        // Spoke logic
        Palette::METAL_GRAY.applyMaterial();
        for (int s = 0; s < 8; s++) {
            glPushMatrix();
            glRotatef(s * 45.0f, 0, 0, 1);
            drawCube(wheelRadius * 1.8f, 0.01f, 0.01f);
            glPopMatrix();
        }
        // Fenders (Chan bun)
        Palette::IRON_BLACK.applyMaterial();
        glPushMatrix();
        glTranslatef(0, wheelRadius * 0.5f, 0);
        drawCube(wheelRadius * 1.2f, wheelRadius * 0.2f, wheelW * 1.2f);
        glPopMatrix();
        glPopMatrix();
    }
    
    // 2. Fork & Handlebars
    Palette::METAL_CHROME.applyGlossyMaterial();
    glPushMatrix();
    glTranslatef(0.6f, wheelRadius + 0.3f, 0.0f);
    glRotatef(-20.0f, 0.0f, 0.0f, 1.0f);
    drawCube(0.04f, 0.8f, 0.15f); // Front fork stems
    glPopMatrix();
    
    // Handlebars
    glPushMatrix();
    glTranslatef(0.45f, wheelRadius + 0.85f, 0.0f);
    Palette::METAL_DARK.applyMaterial();
    drawCube(0.05f, 0.05f, 0.7f); // main bar
    // Grips
    Palette::IRON_BLACK.applyMaterial();
    glPushMatrix(); glTranslatef(0, 0, 0.35f); drawCylinder(0.03f, 0.15f, 6); glPopMatrix();
    glPushMatrix(); glTranslatef(0, 0, -0.35f); drawCylinder(0.03f, 0.15f, 6); glPopMatrix();
    // Mirrors
    glPushMatrix();
    glTranslatef(-0.05f, 0.15f, 0.25f);
    drawCylinder(0.01f, 0.2f, 4); // Stem
    glTranslatef(0, 0.1f, 0); drawSphere(0.06f, 6, 6); // Mirror head
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-0.05f, 0.15f, -0.25f);
    drawCylinder(0.01f, 0.2f, 4); // Stem
    glTranslatef(0, 0.1f, 0); drawSphere(0.06f, 6, 6); // Mirror head
    glPopMatrix();
    glPopMatrix();
    
    // 3. Main Body & Fairing (Yem)
    bodyCol.applyMaterial();
    glPushMatrix();
    glTranslatef(0, wheelRadius + 0.35f, 0);
    drawCube(1.1f, 0.45f, 0.32f); // Main chassis
    
    // Front Fairing (Yem xe)
    glPushMatrix();
    glTranslatef(0.45f, 0.1f, 0);
    glRotatef(-25.0f, 0.0f, 0.0f, 1.0f);
    drawCube(0.15f, 0.75f, 0.45f);
    glPopMatrix();
    glPopMatrix();
    
    // 4. Engine & Exhaust
    Palette::METAL_GRAY.applyMaterial();
    glPushMatrix();
    glTranslatef(0.0f, wheelRadius + 0.2f, 0.0f);
    drawCube(0.35f, 0.35f, 0.35f); // Engine block
    // Cooling fins (Detail)
    for (float ey = -0.1f; ey < 0.2f; ey += 0.05f) {
        glPushMatrix(); glTranslatef(0, ey, 0); drawCube(0.38f, 0.01f, 0.38f); glPopMatrix();
    }
    // Exhaust (Pipe)
    Palette::METAL_CHROME.applyGlossyMaterial();
    glPushMatrix();
    glTranslatef(-0.2f, -0.05f, 0.18f);
    glRotatef(175.0f, 0.0f, 1.0f, 0.0f);
    drawCylinder(0.05f, 0.9f, 8);
    glPopMatrix();
    glPopMatrix();
    
    // 5. Seat (Two-tone)
    Palette::IRON_BLACK.applyMaterial();
    glPushMatrix();
    glTranslatef(-0.15f, wheelRadius + 0.6f, 0.0f);
    drawCube(0.9f, 0.12f, 0.38f);
    // Passenger section (Grey)
    Palette::METAL_GRAY.applyMaterial();
    glPushMatrix(); glTranslatef(-0.35f, 0.01f, 0); drawCube(0.2f, 0.12f, 0.38f); glPopMatrix();
    glPopMatrix();
    
    // 6. Lights & Plates
    glPushMatrix();
    glTranslatef(0.55f, wheelRadius + 0.85f, 0.0f);
    bodyCol.applyMaterial();
    drawCube(0.22f, 0.22f, 0.35f); 
    glTranslatef(0.12f, 0.0f, 0.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    drawHeadlight(0.1f, false); // Front light
    glPopMatrix();
    
    if (!isShadow) {
        // Taillight & Plate
        glPushMatrix();
        glTranslatef(-0.7f, wheelRadius + 0.4f, 0.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        drawHeadlight(0.07f, true);
        glTranslatef(0, -0.15f, 0.02f);
        glScalef(0.4f, 0.4f, 1.0f);
        drawLicensePlate(bienSo);
        glPopMatrix();
    }

    // 7. RIDER (Driver)
    glPushMatrix();
    glTranslatef(-0.15f, wheelRadius + 0.6f, 0); // Position exactly on motorbike seat
    drawRider(colorVar, 0, isShadow); // Style 0: Helmet
    glPopMatrix();
}

// ============================================================
// HIGH-DETAIL SUV (Sports Utility Vehicle)
// ============================================================
void suvDraw(int colorVar, const std::string& bienSo, bool isShadow) {
    Color bodyCol = getVehicleColor(colorVar);
    float wheelR = 0.42f, wheelW = 0.28f;
    float L = 5.2f, W = 2.2f;
    
    // Wheels
    float wbx = L * 0.32f, wbz = W * 0.42f;
    glPushMatrix(); glTranslatef(wbx, wheelR, wbz); drawWheel(wheelR, wheelW); glPopMatrix();
    glPushMatrix(); glTranslatef(wbx, wheelR, -wbz); drawWheel(wheelR, wheelW); glPopMatrix();
    glPushMatrix(); glTranslatef(-wbx, wheelR, wbz); drawWheel(wheelR, wheelW); glPopMatrix();
    glPushMatrix(); glTranslatef(-wbx, wheelR, -wbz); drawWheel(wheelR, wheelW); glPopMatrix();
    
    // Body
    bodyCol.applyMaterial();
    glPushMatrix();
    glTranslatef(0, wheelR + 0.5f, 0);
    drawCube(L, 0.9f, W); // Lower body
    glPushMatrix();
    glTranslatef(-0.2f, 0.9f, 0);
    drawTrapezoid(L * 0.7f, L * 0.55f, 0.8f, W * 0.95f); // Upper cabin
    glPopMatrix();
    
    // Roof Rack (SUV detail) - Skip in shadow
    if (!isShadow) {
        Palette::METAL_DARK.applyMaterial();
        glPushMatrix(); glTranslatef(0, 1.75f, W*0.35f); drawCube(L*0.5f, 0.05f, 0.05f); glPopMatrix();
        glPushMatrix(); glTranslatef(0, 1.75f, -W*0.35f); drawCube(L*0.5f, 0.05f, 0.05f); glPopMatrix();
    }
    glPopMatrix();
    
    // Plate (Skip in shadow)
    if (!isShadow) {
        glPushMatrix(); glTranslatef(L/2 + 0.01f, wheelR + 0.4f, 0); glRotatef(90,0,1,0); drawLicensePlate(bienSo); glPopMatrix();
    }
}

// ============================================================
// CITY BUS (Xe Buyt)
// ============================================================
void busDraw(int colorVar, const std::string& bienSo, bool isShadow) {
    (void)colorVar;
    Color bodyCol = Color(0.2f, 0.5f, 0.2f); // Characteristic Hanoi/Saigon green
    float wheelR = 0.55f, wheelW = 0.4f;
    float L = 10.0f, W = 2.6f;

    // Wheels (6-wheeler)
    glPushMatrix(); glTranslatef(L*0.35f, wheelR, W*0.42f); drawWheel(wheelR, wheelW); glPopMatrix();
    glPushMatrix(); glTranslatef(L*0.35f, wheelR, -W*0.42f); drawWheel(wheelR, wheelW); glPopMatrix();
    glPushMatrix(); glTranslatef(-L*0.3f, wheelR, W*0.42f); drawWheel(wheelR, wheelW); glPopMatrix();
    glPushMatrix(); glTranslatef(-L*0.3f, wheelR, -W*0.42f); drawWheel(wheelR, wheelW); glPopMatrix();

    // Body
    bodyCol.applyMaterial();
    glPushMatrix();
    glTranslatef(0, wheelR + 1.2f, 0);
    drawCube(L, 2.5f, W);
    
    // Windows (Large bus windows) - Skip in shadow
    if (!isShadow) {
        Palette::GLASS_DARK.applyMaterial();
        glEnable(GL_BLEND);
        for (float x = -L/2 + 1.2f; x < L/2 - 1.0f; x += 1.8f) {
            glPushMatrix(); glTranslatef(x, 0.5f, W/2 + 0.02f); drawQuad(1.5f, 1.2f); glPopMatrix();
            glPushMatrix(); glTranslatef(x, 0.5f, -W/2 - 0.02f); drawQuad(1.5f, 1.2f); glPopMatrix();
        }
        glDisable(GL_BLEND);
    }
    glPopMatrix();
    
    // Plate (Skip in shadow)
    if (!isShadow) {
        glPushMatrix(); glTranslatef(L/2 + 0.01f, 0.8f, 0); glRotatef(90, 0, 1, 0); drawLicensePlate(bienSo); glPopMatrix();
    }
}

// ============================================================
// HIGH-DETAIL SEDAN (Includes Taxi variants)
// ============================================================
void carDraw(int colorVar, const std::string& bienSo, bool isTaxi, Color taxiCol, bool isShadow) {
    Color bodyCol = isTaxi ? taxiCol : getVehicleColor(colorVar);
    float wheelR = 0.35f, wheelW = 0.22f;
    float L = CAR_LENGTH, W = CAR_WIDTH, H = CAR_HEIGHT;
    
    // 1. Wheels
    float wbx = L * 0.3f, wbz = W * 0.4f;
    glPushMatrix(); glTranslatef(wbx, wheelR, wbz); drawWheel(wheelR, wheelW); glPopMatrix();
    glPushMatrix(); glTranslatef(wbx, wheelR, -wbz); drawWheel(wheelR, wheelW); glPopMatrix();
    glPushMatrix(); glTranslatef(-wbx, wheelR, wbz); drawWheel(wheelR, wheelW); glPopMatrix();
    glPushMatrix(); glTranslatef(-wbx, wheelR, -wbz); drawWheel(wheelR, wheelW); glPopMatrix();
    
    // 2. Body Shell
    bodyCol.applyMaterial();
    glPushMatrix();
    glTranslatef(0, wheelR + 0.3f, 0);
    drawCube(L, 0.6f, W); // Basic base
    
    // Cabin (Passenger area)
    glPushMatrix();
    glTranslatef(-0.1f, 0.6f, 0);
    drawTrapezoid(L * 0.7f, L * 0.45f, H * 0.6f, W * 0.95f);
    glPopMatrix();
    
    // 3. Windows & Interior (Skip in shadow)
    if (!isShadow) {
        Palette::GLASS_DARK.applyMaterial();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // Windshield
        glPushMatrix(); glTranslatef(0.6f, 1.0f, 0); glRotatef(55, 0, 0, 1); drawQuadXZ(0.9f, W * 0.9f); glPopMatrix();
        // Side Windows
        glPushMatrix(); glTranslatef(0, 0.85f, W * 0.48f); drawQuad(L * 0.6f, 0.45f); glPopMatrix();
        glPushMatrix(); glTranslatef(0, 0.85f, -W * 0.48f); drawQuad(L * 0.6f, 0.45f); glPopMatrix();
        
        // Interior Proxy (Seats/Steering)
        glDisable(GL_BLEND);
        Palette::METAL_DARK.applyMaterial();
        glPushMatrix(); glTranslatef(0.4f, 0.7f, 0.3f); drawCube(0.1f, 0.2f, 0.4f); glPopMatrix(); // steering
        glPushMatrix(); glTranslatef(0.1f, 0.65f, 0.4f); drawCube(0.4f, 0.5f, 0.4f); glPopMatrix(); // front seat
        glPushMatrix(); glTranslatef(-0.3f, 0.65f, 0); drawCube(0.4f, 0.5f, W * 0.8f); glPopMatrix(); // back seat
    }
    
    // 4. Details: Mirrors, Handles (Skip in shadow)
    if (!isShadow) {
        bodyCol.applyMaterial();
        glPushMatrix(); glTranslatef(0.5f, 0.75f, W * 0.52f); drawSphere(0.08f, 6, 6); glPopMatrix(); // R mirror
        glPushMatrix(); glTranslatef(0.5f, 0.75f, -W * 0.52f); drawSphere(0.08f, 6, 6); glPopMatrix(); // L mirror
        Palette::METAL_CHROME.applyGlossyMaterial();
        glPushMatrix(); glTranslatef(0, 0.6f, W * 0.51f); drawCube(0.15f, 0.03f, 0.02f); glPopMatrix(); // Handle
    }
    
    // 5. Taxi Sign (Skip in shadow)
    if (isTaxi) {
        Palette::WALL_WHITE.applyMaterial();
        glPushMatrix();
        glTranslatef(0, 1.25f, 0);
        drawCube(0.3f, 0.18f, 0.6f);
        // "TAXI" text (glRasterPos causes crash in shadow)
        if (!isShadow) {
            glDisable(GL_LIGHTING);
            glColor3f(0, 0, 0);
            drawText3D(-0.1f, -0.05f, 0.31f, "TAXI");
            glEnable(GL_LIGHTING);
        }
        glPopMatrix();
    }
    glPopMatrix();
    
    // 6. Lights & Plates (Skip plates in shadow)
    float lx = L/2 + 0.02f, ly = wheelR + 0.45f, lz = W/2 - 0.25f;
    glPushMatrix(); glTranslatef(lx, ly, lz); glRotatef(90, 0,1,0); drawHeadlight(0.16f, false); glPopMatrix();
    glPushMatrix(); glTranslatef(lx, ly, -lz); glRotatef(90,0,1,0); drawHeadlight(0.16f, false); glPopMatrix();
    glPushMatrix(); glTranslatef(-lx, ly, lz); glRotatef(-90,0,1,0); drawHeadlight(0.14f, true); glPopMatrix();
    glPushMatrix(); glTranslatef(-lx, ly, -lz); glRotatef(-90,0,1,0); drawHeadlight(0.14f, true); glPopMatrix();
    
    if (!isShadow) {
        glPushMatrix(); glTranslatef(lx + 0.01f, ly - 0.15f, 0); glRotatef(90, 0, 1, 0); drawLicensePlate(bienSo); glPopMatrix();
        glPushMatrix(); glTranslatef(-lx - 0.01f, ly - 0.15f, 0); glRotatef(-90, 0, 1, 0); drawLicensePlate(bienSo); glPopMatrix();
    }
}

// ============================================================
// PREMIUM MODEL (Camaro OBJ)
// ============================================================
void drawCamaro(int colorVar) {
    if (g_camaroMesh.vertices.empty()) {
        // Fallback to basic car if not loaded
        carDraw(colorVar, "", false);
        return;
    }
    
    getVehicleColor(colorVar).applyMaterial();
    // Camaro: scale 0.8, Y-up, slight height offset 0.01f
    drawModel(g_camaroMesh, 0.8f, false, 0.01f, 0.0f);
}

void drawRacingCar(int colorVar) {
    if (g_racingCarMesh.vertices.empty()) {
        carDraw(colorVar, "", false);
        return;
    }
    
    getVehicleColor(colorVar).applyMaterial();
    // Racing Car: scale 1.0, Y-up, height offset 0.05f
    // Need higher glossy finish for racing car
    Palette::METAL_CHROME.applyGlossyMaterial(128.0f);
    getVehicleColor(colorVar).apply(); 
    drawModel(g_racingCarMesh, 1.0f, false, 0.05f, 0.0f);
}

// ============================================================
// EMERGENCY VEHICLES (Police/Ambulance)
// ============================================================
void emergencyVehicleDraw(int colorVar, const std::string& bienSo, bool isAmbulance, bool isPolice, bool isShadow) {
    (void)colorVar; (void)bienSo; (void)isAmbulance;
    Color bodyCol = isPolice ? Color(0.1f, 0.1f, 0.4f) : Palette::VEH_WHITE;
    
    // Use SUV base for emergency vehicles
    float wheelR = 0.45f, wheelW = 0.25f;
    float L = 5.0f, W = 2.1f;
    
    // Body & Shell
    bodyCol.applyMaterial();
    glPushMatrix();
    glTranslatef(0, wheelR + 0.5f, 0);
    drawCube(L, 0.8f, W);
    glPushMatrix();
    glTranslatef(-0.2f, 0.9f, 0);
    drawCube(L * 0.7f, 1.0f, W * 0.95f); // Cabin
    glPopMatrix();
    
    // Flashing Lights (Red/Blue or Amber)
    float blink = sinf(g_elapsedTime * 10.0f);
    glPushMatrix();
    glTranslatef(0, 1.95f, 0);
    Palette::METAL_GRAY.applyMaterial();
    drawCube(0.4f, 0.15f, W * 0.8f); // Bar base
    
    // Blue strobe
    if (blink > 0) setEmissiveMaterial(Palette::NEON_BLUE, Palette::NEON_BLUE);
    else Palette::METAL_DARK.applyMaterial();
    glPushMatrix(); glTranslatef(0, 0.05f, 0.4f); drawCube(0.35f, 0.12f, 0.35f); glPopMatrix();
    
    // Red strobe
    if (blink < 0) setEmissiveMaterial(Palette::NEON_RED, Palette::NEON_RED);
    else Palette::METAL_DARK.applyMaterial();
    glPushMatrix(); glTranslatef(0, 0.05f, -0.4f); drawCube(0.35f, 0.12f, 0.35f); glPopMatrix();
    resetMaterial();
    glPopMatrix();

    // Markings (Skip in shadow to prevent crash)
    if (!isShadow) {
        if (isPolice) {
            glDisable(GL_LIGHTING); glColor3f(1, 1, 1);
            glPushMatrix(); glTranslatef(0, 1.25f, W/2 + 0.02f); glScalef(2,2,2); drawText3D(-0.3f, 0, 0, "POLICE"); glPopMatrix();
            glEnable(GL_LIGHTING);
        } else {
            setEmissiveMaterial(Palette::NEON_RED, Palette::NEON_RED);
            glPushMatrix(); glTranslatef(L/2 - 0.5f, 1.3f, W/2 + 0.02f); drawCube(0.4f, 0.4f, 0.02f); drawCube(0.1f, 0.8f, 0.02f); glPopMatrix();
            resetMaterial();
        }
    }
    glPopMatrix();
    
    // Common wheels/lights
    float wbx = L*0.3f, wbz = W*0.4f;
    glPushMatrix(); glTranslatef(wbx, wheelR, wbz); drawWheel(wheelR, wheelW); glPopMatrix();
    glPushMatrix(); glTranslatef(wbx, wheelR, -wbz); drawWheel(wheelR, wheelW); glPopMatrix();
    glPushMatrix(); glTranslatef(-wbx, wheelR, wbz); drawWheel(wheelR, wheelW); glPopMatrix();
    glPushMatrix(); glTranslatef(-wbx, wheelR, -wbz); drawWheel(wheelR, wheelW); glPopMatrix();
}

// ============================================================
// HEAVY TRUCKS (With Cargo variants)
// ============================================================
void truckDraw(VehicleType type, const std::string& bienSo, bool isShadow) {
    float wheelR = 0.55f, wheelW = 0.35f;
    float L = 7.5f, W = 2.4f;
    Color cabCol = Palette::VEH_BLUE;
    
    // 1. Chassis & Wheels (10-wheeler style)
    Palette::METAL_DARK.applyMaterial();
    glPushMatrix();
    glTranslatef(0, wheelR + 0.2f, 0);
    drawCube(L, 0.4f, W * 0.8f);
    glPopMatrix();
    
    float wx[] = { L*0.35f, -L*0.2f, -L*0.38f };
    for (float x : wx) {
        glPushMatrix(); glTranslatef(x, wheelR, W*0.42f); drawWheel(wheelR, wheelW); glPopMatrix();
        glPushMatrix(); glTranslatef(x, wheelR, -W*0.42f); drawWheel(wheelR, wheelW); glPopMatrix();
    }
    
    // 2. Cab
    cabCol.applyMaterial();
    glPushMatrix();
    glTranslatef(L*0.38f, wheelR + 1.2f, 0);
    drawCube(2.2f, 1.8f, W);
    // Windshield & Grill (Skip glass in shadow)
    if (!isShadow) {
        Palette::GLASS_DARK.applyMaterial();
        glPushMatrix(); glTranslatef(1.11f, 0.4f, 0); drawQuad(W - 0.2f, 1.0f); glPopMatrix();
    }
    Palette::METAL_GRAY.applyMaterial();
    glPushMatrix(); glTranslatef(1.11f, -0.4f, 0); drawCube(0.02f, 0.4f, W * 0.7f); glPopMatrix();
    glPopMatrix();
    
    // 3. Cargo Section (Lowered to sit on chassis)
    glPushMatrix();
    glTranslatef(-L*0.12f, wheelR + 1.25f, 0); // Reduced from 1.6f to sit on bed
    if (type == VEH_TRUCKS_CONTAINER) {
        Color contCol = (bienSo.length() > 0) ? getVehicleColor(bienSo[0] % 8) : Palette::VEH_RED;
        contCol.applyMaterial();
        drawCube(5.0f, 2.5f, W + 0.1f);
        // Ribbed detail (Slightly narrower to prevent z-fighting)
        Palette::METAL_DARK.applyMaterial();
        for (float rx = -2.4f; rx < 2.5f; rx += 0.4f) {
            glPushMatrix(); glTranslatef(rx, 0, 0); drawCube(0.08f, 2.52f, W + 0.08f); glPopMatrix();
        }
    } else { // VEH_TRUCKS_OPEN / CANVAS
        Palette::IRON_BLACK.applyMaterial();
        drawCube(5.0f, 0.8f, W + 0.1f); // Lower bed
        // Canvas/Tarp top - lowered to sit on bed
        Color tarpCol = Color(0.1f, 0.25f, 0.15f); // Military green tarp
        tarpCol.applyMaterial();
        glPushMatrix();
        glTranslatef(0, 0.6f, 0); // Lowered from 1.0f
        drawRoundedBox(5.0f, 1.2f, W + 0.1f, 0.3f); // Reduced height from 2.0f
        glPopMatrix();
    }
    glPopMatrix();

    // 4. Plate (Skip in shadow)
    if (!isShadow) {
        glPushMatrix(); glTranslatef(-L*0.5f - 0.01f, wheelR + 0.4f, 0); glRotatef(-90, 0, 1, 0); drawLicensePlate(bienSo); glPopMatrix();
    }
}

// ============================================================
// TRICYCLE (Xich Lo / Xe Loi)
// ============================================================
void drawTricycle(int colorVar, const std::string& bienSo, bool isShadow) {
    (void)isShadow;
    (void)colorVar; (void)bienSo;
    float wheelR = 0.45f;
    float wheelW = 0.05f;
    Color bodyCol = getVehicleColor(colorVar);

    // 1. Rear Wheels (Two)
    for (int i = 0; i < 2; i++) {
        glPushMatrix();
        glTranslatef(-0.8f, wheelR, (i == 0 ? 1 : -1) * 0.6f);
        drawWheel(wheelR, wheelW);
        // Spokes
        Palette::METAL_GRAY.applyMaterial();
        for (int s = 0; s < 12; s++) {
            glPushMatrix(); glRotatef(s * 30.0f, 0, 0, 1); drawCube(wheelR * 1.9f, 0.01f, 0.01f); glPopMatrix();
        }
        glPopMatrix();
    }

    // 2. Front Wheel (One)
    glPushMatrix();
    glTranslatef(1.0f, wheelR, 0);
    drawWheel(wheelR, wheelW);
    // Spokes
    Palette::METAL_GRAY.applyMaterial();
    for (int s = 0; s < 12; s++) {
        glPushMatrix(); glRotatef(s * 30.0f, 0, 0, 1); drawCube(wheelR * 1.9f, 0.01f, 0.01f); glPopMatrix();
    }
    glPopMatrix();

    // 3. Frame
    Palette::METAL_DARK.applyMaterial();
    glPushMatrix();
    glTranslatef(0, wheelR + 0.1f, 0);
    drawCube(1.8f, 0.1f, 0.1f); // main bar
    glPopMatrix();

    // 4. Passenger Seat (Front-facing bucket)
    bodyCol.applyMaterial();
    glPushMatrix();
    glTranslatef(0.4f, wheelR + 0.5f, 0);
    drawCube(1.0f, 0.8f, 0.8f);
    // Seat cushion
    Palette::IRON_BLACK.applyMaterial();
    glPushMatrix(); glTranslatef(0, 0, 0); drawCube(0.8f, 0.2f, 0.7f); glPopMatrix();
    glPopMatrix();

    // 5. Driver Seat & Pedals
    Palette::IRON_BLACK.applyMaterial();
    glPushMatrix();
    glTranslatef(-0.6f, wheelR + 0.6f, 0);
    drawCube(0.3f, 0.1f, 0.25f);
    glPopMatrix();

    // Rider (Driver)
    glPushMatrix();
    glTranslatef(-0.6f, wheelR + 0.65f, 0); // Position exactly on tricycle seat
    drawRider(colorVar + 2, 2, isShadow); // Style 2: Non La
    glPopMatrix();
}

// ============================================================
// BICYCLE (Xe Dap)
// ============================================================
void drawBicycle(int colorVar, bool isShadow) {
    (void)isShadow;
    float wheelR = 0.35f;
    float wheelW = 0.03f;
    Color frameCol = getVehicleColor(colorVar);

    // 1. Wheels
    for (int i = 0; i < 2; i++) {
        glPushMatrix();
        glTranslatef((i == 0 ? 0.7f : -0.7f), wheelR, 0);
        drawWheel(wheelR, wheelW);
        // Spokes
        Palette::METAL_GRAY.applyMaterial();
        for (int s = 0; s < 16; s++) {
            glPushMatrix(); glRotatef(s * 22.5f, 0, 0, 1); drawCube(wheelR * 1.95f, 0.005f, 0.005f); glPopMatrix();
        }
        glPopMatrix();
    }

    // 2. Frame (Diamond geometry)
    frameCol.applyMaterial();
    glPushMatrix();
    glTranslatef(0, wheelR + 0.3f, 0);
    // Top tube
    glPushMatrix(); glTranslatef(0, 0.3f, 0); drawCube(1.2f, 0.04f, 0.04f); glPopMatrix();
    // Down tube
    glPushMatrix(); glTranslatef(0.2f, 0, 0); glRotatef(45, 0, 0, 1); drawCube(0.8f, 0.04f, 0.04f); glPopMatrix();
    // Seat tube
    glPushMatrix(); glTranslatef(-0.3f, 0.1f, 0); drawCube(0.04f, 0.6f, 0.04f); glPopMatrix();
    glPopMatrix();

    // 3. Handlebars & Seat
    Palette::IRON_BLACK.applyMaterial();
    // Handlebars
    glPushMatrix(); glTranslatef(0.6f, wheelR + 0.7f, 0); drawCube(0.05f, 0.05f, 0.5f); glPopMatrix();
    // Seat
    glPushMatrix(); glTranslatef(-0.35f, wheelR + 0.8f, 0); drawCube(0.25f, 0.05f, 0.15f); glPopMatrix();

    // 4. RIDER (Bicyclist)
    glPushMatrix();
    glTranslatef(-0.35f, wheelR + 0.85f, 0); // Position exactly on bicycle seat
    drawRider(colorVar + 1, 1, isShadow); // Style 1: Non La
    glPopMatrix();
}

// ============================================================
// MAIN DISPATCHER
// ============================================================
void vehicleDraw(const VehicleInfo& info, bool isShadow) {
    glPushMatrix();
    glTranslatef(info.position.x, info.position.y, info.position.z);
    glRotatef(info.rotation, 0.0f, 1.0f, 0.0f);
    
    // Apply Premium Physics Rotations
    glRotatef(info.pitch, 0.0f, 0.0f, 1.0f); // Pitch about local side axis
    glRotatef(info.roll, 1.0f, 0.0f, 0.0f);  // Roll about local forward axis
    
    switch(info.type) {
        case VEH_MOTORBIKE: motorbikeDraw(info.colorVariant, info.bienSo, isShadow); break;
        case VEH_CAR_SEDAN: 
            if (info.colorVariant % 2 == 0) {
                drawCamaro(info.colorVariant);
            } else {
                getVehicleColor(info.colorVariant).applyMaterial();
                // Racing car model
                drawModel(g_racingCarMesh, 0.82f, false, 0.02f, 0.0f);
            }
            break;
        case VEH_CAR_SUV:   suvDraw(info.colorVariant, info.bienSo, isShadow); break;
        case VEH_BUS:       busDraw(info.colorVariant, info.bienSo, isShadow); break;
        
        case VEH_TRUCKS_OPEN:      truckDraw(VEH_TRUCKS_OPEN, info.bienSo, isShadow); break;
        case VEH_TRUCKS_CONTAINER: truckDraw(VEH_TRUCKS_CONTAINER, info.bienSo, isShadow); break;
        case VEH_AMBULANCE:        emergencyVehicleDraw(info.colorVariant, info.bienSo, true, false, isShadow); break;
        case VEH_POLICE:           emergencyVehicleDraw(info.colorVariant, info.bienSo, false, true, isShadow); break;
        case VEH_TAXI_YELLOW:      carDraw(info.colorVariant, info.bienSo, true, Palette::VEH_YELLOW, isShadow); break;
        case VEH_TAXI_GREEN:       carDraw(info.colorVariant, info.bienSo, true, Palette::VEH_GREEN, isShadow); break;
        
        case VEH_BICYCLE:   drawBicycle(info.colorVariant, isShadow); break;
        case VEH_TRICYCLE:  drawTricycle(info.colorVariant, info.bienSo, isShadow); break;
        default: carDraw(info.colorVariant, info.bienSo, false, Color(1,1,1), isShadow); break; 
    }
    glPopMatrix();
}

