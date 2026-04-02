#include <cstdio>
#include <vector>
#include <string>

#include <GL/freeglut.h>

#include "traffic.h"
#include "../engine/globals.h"
#include "../engine/utils.h"
#include "../world/street.h"

// ============================================================
// LOGIC
// ============================================================
float g_phaseTimeLeft = 0.0f; // Global time left in current green/yellow phase

void trafficUpdate(float dt) {
    if (g_paused) return;

    g_trafficTimer += dt * g_animSpeed;

    // Vietnamese night mode: Yellow flash after 10PM or before 6AM
    if (g_timeOfDay > 22.0f || g_timeOfDay < 6.0f) {
        if (fmodf(g_elapsedTime, 1.0f) < 0.5f) {
            g_mainTrafficLight = TL_YELLOW;
            g_sideTrafficLight = TL_YELLOW;
        } else {
            g_mainTrafficLight = TL_OFF;
            g_sideTrafficLight = TL_OFF;
        }
        g_phaseTimeLeft = 0.0f;
        return;
    }

    // 4-Phase Cycle: North -> South -> East -> West
    // GREEN_DURATION (30s) + YELLOW_DURATION (3s) = 33s per phase
    float phaseLen = GREEN_DURATION + YELLOW_DURATION;
    float cycle = phaseLen * 4.0f;
    float t = fmodf(g_trafficTimer, cycle);
    
    int phase = (int)(t / phaseLen);
    g_phaseTimeLeft = phaseLen - fmodf(t, phaseLen);

    bool isYellow = (g_phaseTimeLeft <= YELLOW_DURATION);

    switch (phase) {
    case 0: // Phase 1: Main Green
        g_mainTrafficLight = isYellow ? TL_YELLOW : TL_GREEN;
        g_sideTrafficLight = TL_RED;
        break;
    case 1: // Phase 2: Side Green (for symmetry in this simple 1-road model, we alternate)
        g_mainTrafficLight = TL_RED;
        g_sideTrafficLight = isYellow ? TL_YELLOW : TL_GREEN;
        break;
    case 2: // Phase 3: Main Green again
        g_mainTrafficLight = isYellow ? TL_YELLOW : TL_GREEN;
        g_sideTrafficLight = TL_RED;
        break;
    case 3: // Phase 4: Side Green again
        g_mainTrafficLight = TL_RED;
        g_sideTrafficLight = isYellow ? TL_YELLOW : TL_GREEN;
        break;
    }
}

// ============================================================
// DRAWING
// ============================================================
static void drawLightPole(TrafficLightState state, bool overhang) {
    // Main pole
    Palette::METAL_DARK.applyMaterial();
    glPushMatrix();
    glTranslatef(0.0f, 3.0f, 0.0f);
    drawCylinder(0.12f, 6.0f, 12);
    glPopMatrix();

    // The light box itself
    glPushMatrix();
    
    // Depending on overhang, move the box
    if (overhang) {
        // Arm over road
        glPushMatrix();
        glTranslatef(2.5f, 5.8f, 0.0f);
        glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
        drawCylinder(0.06f, 5.0f, 6);
        glPopMatrix();

        // Box hanging on arm
    glTranslatef(4.5f, 5.8f, 0.0f);
    drawCube(0.4f, 1.2f, 0.4f);
  } else {
    // Box attached to pole
    glTranslatef(0.2f, 4.0f, 0.0f);
    drawCube(0.4f, 1.2f, 0.4f);
  }

    // Bulbs
    float startY = 0.4f;
    float spread = 0.4f;

    // Red
    glPushMatrix();
    glTranslatef(-0.25f, startY, 0.0f);
    if (state == TL_RED) {
        setEmissiveMaterial(Color(1.0f, 0.0f, 0.0f), Color(1.0f, 0.0f, 0.0f));
    } else {
        Color(0.2f, 0.0f, 0.0f).applyMaterial();
    }
    drawHemisphere(0.12f, 8, 4);
    resetMaterial();
    glPopMatrix();

    // Yellow
    glPushMatrix();
    glTranslatef(-0.25f, startY - spread, 0.0f);
    if (state == TL_YELLOW) {
        setEmissiveMaterial(Color(1.0f, 1.0f, 0.0f), Color(1.0f, 1.0f, 0.0f));
    } else {
        Color(0.2f, 0.2f, 0.0f).applyMaterial();
    }
    drawHemisphere(0.12f, 8, 4);
    resetMaterial();
    glPopMatrix();

    // Green
    glPushMatrix();
    glTranslatef(-0.25f, startY - spread * 2.0f, 0.0f);
    if (state == TL_GREEN) {
        setEmissiveMaterial(Color(0.0f, 1.0f, 0.0f), Color(0.0f, 1.0f, 0.0f));
    } else {
        Color(0.0f, 0.2f, 0.0f).applyMaterial();
    }
    drawHemisphere(0.12f, 8, 4);
    resetMaterial();
    glPopMatrix();
    
    // Countdown Timer (Vietnamese style)
    if (state != TL_OFF) {
        float ct = GREEN_DURATION + YELLOW_DURATION + RED_DURATION;
        float tNow = fmodf(g_trafficTimer, ct);
        int remaining = 0;
        if (state == TL_GREEN) remaining = (int)(GREEN_DURATION - tNow);
        else if (state == TL_YELLOW) remaining = (int)(GREEN_DURATION + YELLOW_DURATION - tNow);
        else remaining = (int)(ct - tNow);
        
        if (remaining < 0) remaining = 0;
        if (remaining > 99) remaining = 99;
        
        char buf[8];
        sprintf(buf, "%02d", remaining);
        
        glDisable(GL_LIGHTING);
        glColor3f(1.0f, 0.2f, 0.0f); // Orange-red digits
        drawText3D(-0.35f, 0.7f, 0.0f, buf);
        glEnable(GL_LIGHTING);
    }
    
    glPopMatrix(); // Light box pop
}

void drawPowerLines(float x1, float y1, float z1, float x2, float y2, float z2) {
    glDisable(GL_LIGHTING);
    Palette::IRON_BLACK.apply();
    glBegin(GL_LINE_STRIP);
    for(int i=0; i<=10; i++) {
        float t = (float)i/10.0f;
        float x = x1 + (x2-x1)*t;
        float z = z1 + (z2-z1)*t;
        float y = y1 + (y2-y1)*t - 0.4f * sinf(t * (float)M_PI);
        glVertex3f(x, y, z);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

void trafficLightsDraw() {
    // Traffic lights removed for straight boulevard layout
}

void trafficSignDraw(const TrafficSignInfo& info) {
    glPushMatrix();
    info.transform.applyGL();

    // Pole
    Palette::METAL_GRAY.applyMaterial();
    glPushMatrix();
    glTranslatef(0.0f, 1.2f, 0.0f);
    drawCylinder(0.04f, 2.4f, 6);
    glPopMatrix();

    // Sign plate
    glPushMatrix();
    glTranslatef(0, 2.2f, 0.05f);

    switch(info.type) {
        case SIGN_STOP:
            // Red octagon
            Color(0.8f, 0.1f, 0.1f).applyMaterial();
            glBegin(GL_POLYGON);
            glNormal3f(0.0f, 0.0f, 1.0f);
            for(int i=0; i<8; i++) {
                float a = (float)i * (float)M_PI / 4.0f + (float)M_PI / 8.0f;
                glVertex3f(0.5f * cosf(a), 0.5f * sinf(a), 0.0f);
            }
            glEnd();
            
            // White STOP Text
            glColor3f(1.0f, 1.0f, 1.0f);
            glDisable(GL_LIGHTING);
            drawText3D(-0.2f, -0.05f, 0.01f, "DUNG");
            glEnable(GL_LIGHTING);
            break;

        case SIGN_SPEED_LIMIT:
            // White circle, red border
            Palette::WALL_WHITE.applyMaterial();
            drawDisk(0.0f, 0.5f, 16);
            
            Color(0.8f, 0.1f, 0.1f).applyMaterial();
            glPushMatrix(); glTranslatef(0.0f, 0.0f, 0.01f);
            drawDisk(0.4f, 0.5f, 16);
            glPopMatrix();

            // Black number (50)
            glColor3f(0.0f, 0.0f, 0.0f);
            glDisable(GL_LIGHTING);
            drawText3D(-0.15f, -0.05f, 0.02f, "40");
            glEnable(GL_LIGHTING);
            break;
            
        case SIGN_CROSSWALK:
            // Blue square
            Color(0.1f, 0.4f, 0.8f).applyMaterial();
            drawCube(0.8f, 0.8f, 0.02f);
            // Stickman (P triangle representation)
            Palette::WALL_WHITE.applyMaterial();
            glBegin(GL_TRIANGLES);
            glNormal3f(0.0f, 0.0f, 1.0f);
            glVertex3f(0.0f, 0.2f, 0.02f);
            glVertex3f(-0.15f, -0.2f, 0.02f);
            glVertex3f(0.15f, -0.2f, 0.02f);
            glEnd();
            break;

        case SIGN_ONE_WAY:
            // Blue rectangle with arrow
            Color(0.1f, 0.4f, 0.8f).applyMaterial();
            drawCube(0.8f, 0.3f, 0.02f);
            // Arrow
            Palette::WALL_WHITE.applyMaterial();
            glBegin(GL_TRIANGLES);
            glNormal3f(0.0f, 0.0f, 1.0f);
            glVertex3f(0.2f, 0.0f, 0.02f);
            glVertex3f(-0.2f, 0.1f, 0.02f);
            glVertex3f(-0.2f, -0.1f, 0.02f);
            glEnd();
            break;

        default:
            break;
    }

    glPopMatrix();
    glPopMatrix();
}
