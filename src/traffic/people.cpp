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

static void drawHuman(int colorVar, int shirtColorIdx, float rLegA, float rKneeA, float lLegA, float lKneeA, 
                      float rArmA, float lArmA, bool isWaving, float yOffset, 
                      float walkPhase = 0, bool isShadow = false, bool hasBag = false) {
                           
    Color shirt = getShirtColor(shirtColorIdx);
    Color pants = getPantsColor(colorVar);
    Color skin = Palette::SKIN_MEDIUM;
    if (colorVar % 3 == 0) skin = Palette::SKIN_LIGHT;
    
    // Animation nuances
    float sway = sinf(walkPhase) * 1.8f;   // Slight lateral tilt
    float hips = cosf(walkPhase) * 4.5f;   // Hip tilt
    float torsoRot = sinf(walkPhase) * 6.0f; // Body rotation
    
    glPushMatrix();
    glTranslatef(0.0f, yOffset, 0.0f);
    glRotatef(sway, 0, 0, 1); 

    // 1. HEAD & NECK
    glPushMatrix();
    glTranslatef(0.0f, 1.54f, 0.0f); 
    if (!isShadow) skin.applyMaterial();
    drawCylinder(0.045f, 0.12f, 8); // Neck
    
    glTranslatef(0.0f, 0.14f, 0.0f); 
    drawSphere(0.12f, 12, 10); // Head
    
    // Eyes Detail
    if (!isShadow) {
        Palette::METAL_DARK.applyMaterial();
        glPushMatrix();
        glTranslatef(-0.04f, 0.03f, 0.11f);
        drawCube(0.02f, 0.02f, 0.01f); // L-Eye
        glTranslatef(0.08f, 0, 0);
        drawCube(0.02f, 0.02f, 0.01f); // R-Eye
        glPopMatrix();
        
        // Hair (if no hat)
        if (colorVar % 2 != 0) {
            Palette::WOOD_DARK.applyMaterial();
            glPushMatrix();
            glTranslatef(0, 0.06f, -0.02f);
            drawSphere(0.125f, 10, 8);
            glPopMatrix();
        }
    }
    
    if (colorVar % 2 == 0) drawNonLa();
    glPopMatrix();

    // 2. TORSO (Seamless Shoulders)
    glPushMatrix();
    glTranslatef(0.0f, 1.25f, 0.0f);
    glRotatef(torsoRot, 0, 1, 0); 
    if (!isShadow) shirt.applyMaterial();
    
    // Chest/Shoulder part (Wide)
    drawCube(0.44f, 0.42f, 0.22f); 
    // Waist/Belly (Narrower)
    glTranslatef(0, -0.32f, 0);
    drawCube(0.34f, 0.4f, 0.18f);
    glPopMatrix();

    // 3. L-LEG & FOOT
    glPushMatrix();
    glTranslatef(-0.11f, 0.95f, 0.0f);
    glRotatef(lLegA, 1, 0, 0);
    glRotatef(-hips, 0, 0, 1); 
    if (!isShadow) pants.applyMaterial();
    
    glPushMatrix(); glTranslatef(0, -0.22f, 0); drawCylinder(0.08f, 0.45f, 8); glPopMatrix();
    glTranslatef(0.0f, -0.45f, 0.0f);
    glRotatef(lKneeA, 1, 0, 0);
    glPushMatrix(); glTranslatef(0, -0.22f, 0); drawCylinder(0.065f, 0.45f, 8); glPopMatrix();
    
    if (!isShadow) Palette::METAL_DARK.applyMaterial();
    glTranslatef(0.0f, -0.45f, 0.08f);
    drawCube(0.09f, 0.06f, 0.22f);
    glPopMatrix();

    // 4. R-LEG & FOOT
    glPushMatrix();
    glTranslatef(0.11f, 0.95f, 0.0f);
    glRotatef(rLegA, 1, 0, 0);
    glRotatef(hips, 0, 0, 1);
    if (!isShadow) pants.applyMaterial();
    
    glPushMatrix(); glTranslatef(0, -0.22f, 0); drawCylinder(0.08f, 0.45f, 8); glPopMatrix();
    glTranslatef(0.0f, -0.45f, 0.0f);
    glRotatef(rKneeA, 1, 0, 0);
    glPushMatrix(); glTranslatef(0, -0.22f, 0); drawCylinder(0.065f, 0.45f, 8); glPopMatrix();
    
    if (!isShadow) Palette::METAL_DARK.applyMaterial();
    glTranslatef(0.0f, -0.45f, 0.08f);
    drawCube(0.09f, 0.06f, 0.22f);
    glPopMatrix();

    // 5. L-ARM (Connects directly to torso edge)
    glPushMatrix();
    glTranslatef(-0.22f, 1.44f, 0.0f); // Pivot at the side edge of torso top
    if (isWaving) glRotatef(-150.0f, 1, 0, 0); 
    else glRotatef(lArmA, 1, 0, 0);
    
    if (!isShadow) shirt.applyMaterial();
    glPushMatrix(); glTranslatef(0, -0.28f, 0); drawCylinder(0.055f, 0.55f, 8); glPopMatrix();
    
    if (!isShadow) skin.applyMaterial();
    glTranslatef(0, -0.55f, 0);
    drawSphere(0.05f, 6, 6);
    glPopMatrix();

    // 6. R-ARM
    glPushMatrix();
    glTranslatef(0.22f, 1.44f, 0.0f);
    glRotatef(rArmA, 1, 0, 0);
    
    if (!isShadow) shirt.applyMaterial();
    glPushMatrix(); glTranslatef(0, -0.28f, 0); drawCylinder(0.055f, 0.55f, 8); glPopMatrix();
    
    if (!isShadow) skin.applyMaterial();
    glTranslatef(0, -0.55f, 0);
    drawSphere(0.05f, 6, 6);
    glPopMatrix();

    // Handbag logic
    if (hasBag) {
        drawHandbag(isShadow);
    }

    glPopMatrix(); // End Root
}

void personStandingDraw(int colorVar, int shirtColor, bool isShadow, bool hasBag) {
    drawHuman(colorVar, shirtColor, 0, 0, 0, 0, 4, 4, false, 0, 0, isShadow, hasBag);
}

void personWalkingDraw(int colorVar, int shirtColor, float walkPhase, bool isWaving, bool isShadow, bool hasBag) {
    float swing = sinf(walkPhase);
    float yOffset = fabsf(swing) * 0.04f; 
    
    float legA = swing * 28.0f;
    float lKnee = (swing > 0) ? swing * 35.0f : 5.0f;
    float rKnee = (swing < 0) ? -swing * 35.0f : 5.0f;
    
    glPushMatrix();
    glRotatef(3.0f, 1, 0, 0); // Natural lean
    drawHuman(colorVar, shirtColor, -legA, rKnee, legA, lKnee, legA * 0.9f, -legA * 0.9f, isWaving, yOffset, walkPhase, isShadow, hasBag);
    glPopMatrix();
}

void personSittingDraw(int colorVar, int shirtColor, bool isShadow) {
    drawHuman(colorVar, shirtColor, -85, 85, -85, 85, 20, 20, false, -0.45f, 0, isShadow, false);
}

void vendorDraw(int colorVar, int shirtColor, bool isShadow) {
    // Arms matching shoulder height
    drawHuman(colorVar, shirtColor, 0, 0, 0, 0, -85, -85, false, 0, 0, isShadow, false); 
    
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
