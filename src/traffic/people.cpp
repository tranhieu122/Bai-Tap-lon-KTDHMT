#include <cmath>
#include <cstdio>
#include <vector>
#include <string>

#include <GL/freeglut.h>

#include "people.h"
#include "../engine/texture.h"
#include "../engine/utils.h"
#include "../world/sky.h"
#include "../world/furniture.h"

void peopleInit() {
}

void peopleUpdate(float dt) {
    (void)dt;
}

static void drawHandbag(bool isShadow) {
    if (isShadow) return;
    glPushMatrix();
    // Position the bag next to the hand
    glTranslatef(0.15f, 0.7f, 0.05f);
    
    // Bag body
    Palette::LEATHER_BROWN.applyMaterial();
    drawCube(0.12f, 0.25f, 0.25f);
    
    // Handle
    Palette::METAL_DARK.applyMaterial();
    glPushMatrix();
    glTranslatef(0, 0.15f, 0);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    for(int i=0; i<=10; i++) {
        float a = i * 3.14159265f / 10.0f;
        glVertex3f(0, 0.1f * sinf(a), 0.1f * cosf(a));
    }
    glEnd();
    glPopMatrix();
    
    glPopMatrix();
}

// ============================================================
// HIGH-FIDELITY PROCEDURAL HUMAN (HFPH)
// ============================================================
static void drawNonLa() {
    glPushMatrix();
    glTranslatef(0.0f, 0.02f, 0.01f); 
    Palette::WOOD_LIGHT.applyMaterial(); 
    drawDisk(0.0f, 0.35f, 18); 
    drawCone(0.35f, 0.22f, 18); 
    glPopMatrix();
}

static void drawHuman(int colorVar, int shirtColorIdx, float yOffset, 
                      float walkPhase = 0, bool isShadow = false, bool hasBag = false) {
    (void)shirtColorIdx; // Suppress warning - model 3D uses general skin material for now                       
    Color skin = Palette::SKIN_MEDIUM;
    if (colorVar % 3 == 0) skin = Palette::SKIN_LIGHT;
    
    // Animation nuances
    float sway = sinf(walkPhase) * 1.8f;   // Slight lateral tilt
    
    glPushMatrix();
    glTranslatef(0.0f, yOffset, 0.0f);
    glRotatef(sway, 0, 0, 1); 

    // 1. NHÂN VẬT 3D CHÍNH (Thay thế toàn bộ các khối ghép trước đây)
    if (!g_humanMesh.vertices.empty()) {
        if (!isShadow) {
            skin.applyMaterial();
        }
        // Tỉ lệ 0.92f phù hợp với chiều cao trung bình trong phố
        drawModel(g_humanMesh, 0.92f, false, 0.0f, 0.0f);
    } else {
        // Fallback dự phòng nếu model không nạp được
        glPushMatrix();
        glTranslatef(0, 0.85f, 0);
        drawCube(0.3f, 1.7f, 0.3f);
        glPopMatrix();
    }

    // 2. PHỤ KIỆN VIỆT NAM (Nón Lá & Túi xách)
    if (colorVar % 2 == 0) {
        glPushMatrix();
        glTranslatef(0.0f, 1.74f, 0.01f); // Đặt lên đỉnh đầu model 3D
        drawNonLa();
        glPopMatrix();
    }

    if (hasBag) {
        drawHandbag(isShadow);
    }

    glPopMatrix(); // Kết thúc Root của drawHuman
}

void personStandingDraw(int colorVar, int shirtColor, bool isShadow, bool hasBag) {
    drawHuman(colorVar, shirtColor, 0, 0, isShadow, hasBag);
}

void personWalkingDraw(int colorVar, int shirtColor, float walkPhase, bool isWaving, bool isShadow, bool hasBag) {
    (void)isWaving;
    float swing = sinf(walkPhase);
    float yOffset = fabsf(swing) * 0.04f; 
    
    glPushMatrix();
    glRotatef(3.0f, 1, 0, 0); // Natural lean
    drawHuman(colorVar, shirtColor, yOffset, walkPhase, isShadow, hasBag);
    glPopMatrix();
}

void personSittingDraw(int colorVar, int shirtColor, bool isShadow) {
    drawHuman(colorVar, shirtColor, -0.45f, 0, isShadow, false);
}

void vendorDraw(int colorVar, int shirtColor, bool isShadow) {
    // Basic human body for vendor
    drawHuman(colorVar, shirtColor, 0, 0, isShadow, false); 
    
    // Pole (Đòn gánh) - Precise shoulder rest
    if (!isShadow) Palette::WOOD_LIGHT.applyMaterial();
    glPushMatrix();
    glTranslatef(0.23f, 1.47f, 0.0f); 
    glRotatef(90, 1, 0, 0); 
    drawCylinder(0.025f, 2.3f, 8);
    glPopMatrix();
    
    if (!isShadow) {
        Color basketCol(0.72f, 0.62f, 0.38f);
        // Front
        glPushMatrix();
        glTranslatef(0.23f, 0.85f, 1.1f);
        basketCol.applyMaterial();
        drawHemisphere(0.35f, 12, 6);
        Palette::WOOD_DARK.applyMaterial();
        glBegin(GL_LINES); glVertex3f(0, 0, 0); glVertex3f(0, 0.62f, 0); glEnd();
        glPopMatrix();
        // Back
        glPushMatrix();
        glTranslatef(0.23f, 0.85f, -1.1f);
        basketCol.applyMaterial();
        drawHemisphere(0.35f, 12, 6);
        Palette::WOOD_DARK.applyMaterial();
        glBegin(GL_LINES); glVertex3f(0, 0, 0); glVertex3f(0, 0.62f, 0); glEnd();
        glPopMatrix();
    }
}

void personDraw(const PedestrianInfo& info, bool isShadow) {
    if (info.isInside) return; // Hide if inside building

    glPushMatrix();
    glTranslatef(info.position.x, info.position.y, info.position.z);
    glRotatef(info.rotation, 0, 1, 0); 
    
    switch(info.state) {
        case PED_STANDING: 
            personStandingDraw(info.colorVariant, info.shirtColor, isShadow, info.hasBag); 
            break;
        case PED_WALKING:  
        case PED_ENTERING_BUILDING:
        case PED_EXITING_BUILDING:
            personWalkingDraw(info.colorVariant, info.shirtColor, info.walkPhase, info.isWaving, isShadow, info.hasBag); 
            break;
        case PED_SITTING:  
            personSittingDraw(info.colorVariant, info.shirtColor, isShadow); 
            break;
        case PED_VENDOR:   
            vendorDraw(info.colorVariant, info.shirtColor, isShadow); 
            break;
    }
    glPopMatrix();
}
