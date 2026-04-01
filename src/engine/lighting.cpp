#include "lighting.h"
#include "sky.h"
#include "globals.h"
#include <GL/freeglut.h>

void lightingInit() {
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    
    // Smooth shading for better rounded objects
    glShadeModel(GL_SMOOTH);
    
    // Global ambient - hoi am de khong bi toi den
    float globalAmb[] = { 0.12f, 0.11f, 0.14f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmb);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    
    // Bat normalize de scale khong lam hong lighting
    glEnable(GL_NORMALIZE);
}

void lightingApplyGlobal() {
    // GL_LIGHT0 is our Sun / Moon directional light
    glEnable(GL_LIGHT0);

    Color amb = skyGetAmbientLight();
    Color diff = skyGetDirectionalLight();
    Vector3 dir = skyGetLightDirection();
    
    // Specular highlight from sun - tinh toan theo gio
    float specR = 0.4f, specG = 0.4f, specB = 0.4f;
    if (g_timeOfDay >= 10.0f && g_timeOfDay <= 15.0f) {
        // Trua nang gat - specular manh, hoi vang
        specR = 1.0f; specG = 0.98f; specB = 0.85f;
    }
    else if (g_timeOfDay >= 16.0f && g_timeOfDay <= 18.0f) {
        // Gio vang - specular am cam
        float t = (g_timeOfDay - 16.0f) / 2.0f;
        specR = 1.0f; specG = 0.7f - t*0.3f; specB = 0.3f - t*0.1f;
    }
    else if (g_timeOfDay < 5.0f || g_timeOfDay > 19.5f) {
        // Dem - specular yeu, mau xanh trang
        specR = 0.15f; specG = 0.15f; specB = 0.22f;
    }
    
    float l0_amb[] = { amb.r, amb.g, amb.b, 1.0f };
    float l0_dif[] = { diff.r, diff.g, diff.b, 1.0f };
    float l0_spc[] = { specR, specG, specB, 1.0f };
    
    // Directional light format: w=0.0 means infinite distance
    float l0_pos[] = { dir.x, dir.y, dir.z, 0.0f };
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, l0_amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, l0_dif);
    glLightfv(GL_LIGHT0, GL_SPECULAR, l0_spc);
    glLightfv(GL_LIGHT0, GL_POSITION, l0_pos);
}

void lightingApplyLocal() {
    // Turn off used lights initially
    for(int i=1; i<MAX_LIGHTS; i++) {
        glDisable(GL_LIGHT0 + i);
    }
    
    // Example: Use GL_LIGHT1 to GL_LIGHT7 for local streetlamps close to camera
    if (!g_streetLightsOn || (g_timeOfDay >= 6.0f && g_timeOfDay <= 18.0f)) return;
    
    // We would normally find the nearest N streetlamps and bind them to GL_LIGHTi.
    // For fixed pipeline limitations, let's just create 4 static lights at intersections
    
    float lightY = LAMP_HEIGHT - 0.2f;
    float spread = CROSSWALK_WIDTH + 4.0f;
    
    // Using explicit Vector3 array to avoid initializer list issues in older MSVC
    Vector3 lightPositions[6];
    lightPositions[0] = Vector3(-spread, lightY, -spread);
    lightPositions[1] = Vector3( spread, lightY, -spread);
    lightPositions[2] = Vector3(-spread, lightY,  spread);
    lightPositions[3] = Vector3( spread, lightY,  spread);
    lightPositions[4] = Vector3(0, lightY, -STREET_WIDTH * 1.5f);
    lightPositions[5] = Vector3(0, lightY,  STREET_WIDTH * 1.5f);
    
    float l_dif[] = { 1.0f, 0.8f, 0.4f, 1.0f }; // Softer warm light
    float l_spc[] = { 0.6f, 0.4f, 0.2f, 1.0f };
    
    int l_idx = GL_LIGHT1;
    for(int i=0; i<6; i++) {
        glEnable(l_idx);
        float l_pos[] = { lightPositions[i].x, lightPositions[i].y, lightPositions[i].z, 1.0f };
        
        glLightfv(l_idx, GL_DIFFUSE, l_dif);
        glLightfv(l_idx, GL_SPECULAR, l_spc);
        glLightfv(l_idx, GL_POSITION, l_pos);
        
        // Attenuation tinh chỉnh cho ánh sáng mượt hơn
        glLightf(l_idx, GL_CONSTANT_ATTENUATION, 1.0f);
        glLightf(l_idx, GL_LINEAR_ATTENUATION, 0.08f);
        glLightf(l_idx, GL_QUADRATIC_ATTENUATION, 0.012f);
        
        l_idx++;
        if (l_idx >= GL_LIGHT0 + MAX_LIGHTS) break;
    }
}
