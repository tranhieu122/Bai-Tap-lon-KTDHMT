#include "people.h"
#include "../engine/texture.h"
#include "../engine/utils.h"
#include <GL/freeglut.h>
#include <cmath>
#include <cstdio>

void peopleInit() {
}

void peopleUpdate(float dt) {
}

// ============================================================
// HUMAN RIGGING HELPER
// ============================================================
// A simple generic human model that we can pose
static void drawHuman(int colorVar, float rLegA, float rKneeA, float lLegA, float lKneeA, 
                      float rArmA, float rElbowA, float lArmA, float lElbowA, float yOffset) {
                          
    Color shirt = getShirtColor(colorVar);
    Color skin = Palette::SKIN_MEDIUM;
    if (colorVar % 3 == 0) skin = Palette::SKIN_LIGHT;
    
    // Joint positions/dimensions
    float armH = 0.6f;
    float legH = 0.8f;
    float bodyH = 0.6f;
    float shoulderW = 0.4f;

    glPushMatrix();
    glTranslatef(0.0f, yOffset, 0.0f);

    // HEAD & TORSO (Mô hình Mesh chân thực)
    glPushMatrix();
    if (!g_humanMesh.vertices.empty()) {
        skin.applyMaterial(); // Sắc thái da người chân thực
        // Human: scale 1.1, isZUp=false, intrinsicRotation=180 (face forward)
        drawModel(g_humanMesh, 1.1f, false, 0.0f, 180.0f);
    } else {
        shirt.applyMaterial();
        drawMesh(getCharacterMesh()); 
        
        // CHỈ VẼ TAY CHÂN NẾU KHÔNG CÓ MÔ HÌNH 3D (để tránh lỗi "người ngoài hành tinh")
        // Tay phải
        glPushMatrix();
        glTranslatef(-shoulderW/2 - 0.05f, 1.45f, 0.05f);
        glRotatef(rArmA, 1, 0, 0);
        shirt.applyMaterial();
        drawCylinder(0.06f, armH/2, 8);
        glTranslatef(0, -armH/4, 0);
        glRotatef(rElbowA, 1, 0, 0);
        skin.applyMaterial();
        drawCylinder(0.05f, armH/2, 8);
        glPopMatrix();

        // Tay trái
        glPushMatrix();
        glTranslatef(shoulderW/2 + 0.05f, 1.45f, 0.05f);
        glRotatef(lArmA, 1, 0, 0);
        shirt.applyMaterial();
        drawCylinder(0.06f, armH/2, 8);
        glTranslatef(0, -armH/4, 0);
        glRotatef(lElbowA, 1, 0, 0);
        skin.applyMaterial();
        drawCylinder(0.05f, armH/2, 8);
        glPopMatrix();
    }
    glPopMatrix();

    glPopMatrix(); // End Root
}

// ============================================================
// SPECIFIC STATES
// ============================================================
void personStandingDraw(int colorVar) {
    // A-pose or relaxed pose
    // Arms slightly down to the side, legs straight
    drawHuman(colorVar, 0, 0, 0, 0, 5, 0, 5, 0, 0);
}

void personWalkingDraw(int colorVar, float walkPhase) {
    // walkPhase is 0 to 2*PI
    float swing = sinf(walkPhase);           // Primary swing (-1 to 1)
    float swingDouble = sinf(walkPhase * 2.0f); // Double frequency for bobbing
    
    // Angles for a "Premium" gait
    float legSwingArc = 32.0f;
    float armSwingArc = 28.0f;
    float hipSwayArc = 8.0f;
    float shoulderSwayArc = -6.0f; // Shoulders sway opposite to hips
    
    // 1. Core Body Motion
    float yOffset = fabsf(swing) * 0.08f; // Bounce at the peak of each step
    float hipSway = swing * hipSwayArc;
    float shoulderSway = swing * shoulderSwayArc;
    
    // 2. Leg Pose (with Knee Flex)
    float rLegA = -swing * legSwingArc; 
    float lLegA = swing * legSwingArc;
    
    // Knee flex: bend when the leg is swinging forward (not weight bearing)
    float rKneeA = (swing < 0) ? -swing * 45.0f : 5.0f; 
    float lKneeA = (swing > 0) ? swing * 45.0f : 5.0f;
    
    // 3. Arm Pose (with Elbow Lag)
    float rArmA = swing * armSwingArc;
    float lArmA = -swing * armSwingArc;
    
    // Elbow lag (secondary motion)
    float rElbowA = -20.0f + (swing > 0 ? -swing * 30.0f : swing * 10.0f);
    float lElbowA = -20.0f + (swing < 0 ? swing * 30.0f : -swing * 10.0f);

    // DRAW HIERARCHY
    glPushMatrix();
    glTranslatef(0.0f, yOffset, 0.0f);
    glRotatef(hipSway, 0, 1, 0); // Whole body sways a bit with hips
    
    // Note: In a real hierarchical rig, we'd draw torso then legs. 
    // Here we pass everything to drawHuman which handles the pieces.
    drawHuman(colorVar, rLegA, rKneeA, lLegA, lKneeA, rArmA, rElbowA, lArmA, lElbowA, 0.0f);
    
    glPopMatrix();
}

void personSittingDraw(int colorVar) {
    // Hips down, legs bent 90 degrees at hip, 90 at knee
    // Root Y offset must drop by thigh length
    float yOffset = -0.4f; // approx -legH/2
    drawHuman(colorVar, -90, 90, -90, 90, 10, -45, 10, -45, yOffset);
}

void vendorDraw(int colorVar) {
    // "Gánh hàng rong" - Carrying a bamboo pole with two baskets
    
    // Standing pose but arms holding the pole
    drawHuman(colorVar, 0, 0, 0, 0, -30, -80, -30, -80, 0); // Arms raised to hold pole
    
    // The Pole (Đòn gánh)
    Palette::WOOD_LIGHT.applyMaterial();
    glPushMatrix(); // Relative to ground, at shoulder approx height
    glTranslatef(0, PERSON_HEIGHT * 0.8f, -0.1f); 
    glRotatef(10, 0, 0, 1); // slight tilt
    drawCylinder(0.02f, 2.0f, 6);
    
    // Baskets (Thúng)
    float poleX = 0.9f;
    Color basketCol(0.8f, 0.7f, 0.4f); // Wicker
    
    // Front basket
    glPushMatrix();
    glTranslatef(0, -0.6f, poleX);
    basketCol.applyMaterial();
    drawHemisphere(0.3f, 12, 6);
    // Strings
    Palette::WOOD_DARK.applyMaterial();
    drawWire(Vector3(0, 0, 0), Vector3(0, 0.6f, 0), 0.0f, 1);
    glPopMatrix();
    
    // Rear basket
    glPushMatrix();
    glTranslatef(0, -0.6f, -poleX);
    basketCol.applyMaterial();
    drawHemisphere(0.3f, 12, 6);
    // Strings
    Palette::WOOD_DARK.applyMaterial();
    drawWire(Vector3(0, 0, 0), Vector3(0, 0.6f, 0), 0.0f, 1);
    glPopMatrix();
    
    glPopMatrix(); // End pole
}


// ============================================================
// MAIN DISPATCHER
// ============================================================
void personDraw(const PedestrianInfo& info) {
    glPushMatrix();
    glTranslatef(info.position.x, info.position.y, info.position.z);
    glRotatef(info.rotation, 0, 1, 0); // Face direction
    
    switch(info.state) {
        case PED_STANDING: personStandingDraw(info.colorVariant); break;
        case PED_WALKING:  personWalkingDraw(info.colorVariant, info.walkPhase); break;
        case PED_SITTING:  personSittingDraw(info.colorVariant); break;
        case PED_VENDOR:   vendorDraw(info.colorVariant); break;
    }
    
    glPopMatrix();
}
