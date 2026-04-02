#include "particles.h"
#include "../engine/globals.h"
#include "../engine/utils.h"
#include "../engine/camera.h"
#include <GL/freeglut.h>
#include <cstdio>

namespace {
    Particle rain[MAX_RAIN_PARTICLES];
    Particle smoke[MAX_SMOKE_PARTICLES];
}

void particlesInit() {
    for(int i=0; i<MAX_RAIN_PARTICLES; i++) rain[i].active = false;
    for(int i=0; i<MAX_SMOKE_PARTICLES; i++) smoke[i].active = false;
}

void particlesUpdate(float dt) {
    if (g_paused) return;
    
    float animDt = dt * g_animSpeed;
    
    // Rain
    static float rainAccumulator = 0.0f;
    if (g_weather == WEATHER_RAIN && g_rainEnabled) {
        rainAccumulator += 1500.0f * dt; // Increased density for boulevard
        // Spawn rain relative to camera to save particles
        Vector3 camPos = cameraGetPosition();
        
        // Spawn new drops
        int spawnRequest = (int)rainAccumulator;
        rainAccumulator -= (float)spawnRequest;
        
        for(int i=0; i<MAX_RAIN_PARTICLES && spawnRequest > 0; i++) {
            if (!rain[i].active) {
                rain[i].active = true;
                // Spawn randomly around camera
                float rx = randomFloat(-40.0f, 40.0f); // Slightly wider range
                float rz = randomFloat(-40.0f, 40.0f);
                float ry = randomFloat(25.0f, 45.0f); // Fall from higher
                rain[i].position = camPos + Vector3(rx, ry, rz);
                
                // Wind slightly pushes rain
                rain[i].velocity = Vector3(randomFloat(-1.5f, 4.5f), randomFloat(-30.0f, -22.0f), randomFloat(-1.0f, 1.0f));
                rain[i].life = 0.0f;
                rain[i].maxLife = 2.5f; 
                rain[i].size = randomFloat(0.04f, 0.08f); 
                rain[i].color = Color(0.85f, 0.85f, 1.0f, 0.7f); // Brighter rain
                spawnRequest--;
            }
        }
    } else {
        rainAccumulator = 0.0f;
    }
    
    // Update existing rain
    for(int i=0; i<MAX_RAIN_PARTICLES; i++) {
        if (rain[i].active) {
            rain[i].position += rain[i].velocity * animDt;
            rain[i].life += animDt;
            // Die on ground or timeout
            if (rain[i].position.y < 0.01f || rain[i].life > rain[i].maxLife) {
                rain[i].active = false;
                // Ideally spawn a ripple splash decoupled from rain, but skip for perf
            }
        }
    }
    
    // Smoke update
    for(int i=0; i<MAX_SMOKE_PARTICLES; i++) {
        if (smoke[i].active) {
            smoke[i].position += smoke[i].velocity * animDt;
            // Smoke drifts up and expands
            smoke[i].velocity.y += 0.5f * animDt; // Buoyancy
            smoke[i].velocity.x += randomFloat(-0.1f, 0.1f) * animDt; // Wind drift
            smoke[i].size += 0.5f * animDt; // Growth
            
            smoke[i].life += animDt;
            // Fade out
            smoke[i].color.a = clamp(1.0f - (smoke[i].life / smoke[i].maxLife), 0.0f, 1.0f) * 0.5f;
            
            if (smoke[i].life > smoke[i].maxLife) {
                smoke[i].active = false;
            }
        }
    }
}

void particlesDraw() {
    glDepthMask(GL_FALSE);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D); // CRITICAL: Lines don't use textures
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Draw Smoke (Quads billboarded to camera)
    float camY = cameraGetYaw();
    for(int i=0; i<MAX_SMOKE_PARTICLES; i++) {
        if (smoke[i].active) {
            smoke[i].color.apply();
            glPushMatrix();
            glTranslatef(smoke[i].position.x, smoke[i].position.y, smoke[i].position.z);
            glRotatef(camY, 0.0f, -1.0f, 0.0f); // Billboard simple
            drawQuad(smoke[i].size, smoke[i].size);
            glPopMatrix();
        }
    }
    
    // Draw Rain (Lines) - Draw if active, even if spawning is off
    glLineWidth(2.0f); // Thicker lines for visibility
    glBegin(GL_LINES);
    for(int i=0; i<MAX_RAIN_PARTICLES; i++) {
        if (rain[i].active) {
            rain[i].color.apply();
            glVertex3f(rain[i].position.x, rain[i].position.y, rain[i].position.z);
            // Draw streak based on velocity to simulate motion blur
            Vector3 tail = rain[i].position - rain[i].velocity * 0.07f; // Longer streaks
            glVertex3f(tail.x, tail.y, tail.z);
        }
    }
    glEnd();
    
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glDepthMask(GL_TRUE);
}

void emitSmoke(const Vector3& pos, const Vector3& vel) {
    for(int i=0; i<MAX_SMOKE_PARTICLES; i++) {
        if (!smoke[i].active) {
            smoke[i].active = true;
            smoke[i].position = pos;
            smoke[i].velocity = vel + Vector3(randomFloat(-0.5f, 0.5f), randomFloat(0.5f, 1.5f), randomFloat(-0.5f, 0.5f));
            smoke[i].life = 0.0f;
            smoke[i].maxLife = randomFloat(1.0f, 2.5f);
            smoke[i].size = randomFloat(0.2f, 0.5f);
            smoke[i].color = Color(0.5f, 0.5f, 0.5f, 0.5f);
            break; // Spawn 1 per call
        }
    }
}

void emitRainDrop(const Vector3& pos) {
    (void)pos;
    // Already handled in update bulk-spawn for efficiency
}
