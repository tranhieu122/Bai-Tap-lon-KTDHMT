#include "utils.h"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

// Provide definition for global mesh declared extern in globals.h
Mesh g_camaroMesh;
Mesh g_humanMesh;
Mesh g_treeMesh;
Mesh g_racingCarMesh;

// ============================================================
// MATH UTILITIES
// ============================================================

float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

float clamp(float v, float lo, float hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

float smoothstep(float edge0, float edge1, float x) {
    float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

// Modern Random Engine
static std::mt19937 g_rng(1337);

void seedRandom(unsigned int seed) {
    g_rng.seed(seed);
}

float randomFloat(float minVal, float maxVal) {
    std::uniform_real_distribution<float> dist(minVal, maxVal);
    return dist(g_rng);
}

int randomInt(int minVal, int maxVal) {
    std::uniform_int_distribution<int> dist(minVal, maxVal);
    return dist(g_rng);
}

float noise1D(float x) {
    int xi = (int)floorf(x);
    float xf = x - xi;
    float u = xf * xf * (3.0f - 2.0f * xf);
    
    // Simple hash
    auto hash = [](int n) -> float {
        n = (n << 13) ^ n;
        return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
    };
    
    return lerp(hash(xi), hash(xi + 1), u);
}

float noise2D(float x, float y) {
    return (noise1D(x + y * 31.17f) + noise1D(y + x * 17.31f)) * 0.5f;
}

// ============================================================
// BASIC SHAPES
// ============================================================

void drawCube(float sx, float sy, float sz) {
    float hx = sx * 0.5f, hy = sy * 0.5f, hz = sz * 0.5f;
    
    glBegin(GL_QUADS);
    // Front face (Z+)
    glNormal3f(0, 0, 1);
    glVertex3f(-hx, -hy,  hz);
    glVertex3f( hx, -hy,  hz);
    glVertex3f( hx,  hy,  hz);
    glVertex3f(-hx,  hy,  hz);
    
    // Back face (Z-)
    glNormal3f(0, 0, -1);
    glVertex3f( hx, -hy, -hz);
    glVertex3f(-hx, -hy, -hz);
    glVertex3f(-hx,  hy, -hz);
    glVertex3f( hx,  hy, -hz);
    
    // Top face (Y+)
    glNormal3f(0, 1, 0);
    glVertex3f(-hx,  hy,  hz);
    glVertex3f( hx,  hy,  hz);
    glVertex3f( hx,  hy, -hz);
    glVertex3f(-hx,  hy, -hz);
    
    // Bottom face (Y-)
    glNormal3f(0, -1, 0);
    glVertex3f(-hx, -hy, -hz);
    glVertex3f( hx, -hy, -hz);
    glVertex3f( hx, -hy,  hz);
    glVertex3f(-hx, -hy,  hz);
    
    // Right face (X+)
    glNormal3f(1, 0, 0);
    glVertex3f( hx, -hy,  hz);
    glVertex3f( hx, -hy, -hz);
    glVertex3f( hx,  hy, -hz);
    glVertex3f( hx,  hy,  hz);
    
    // Left face (X-)
    glNormal3f(-1, 0, 0);
    glVertex3f(-hx, -hy, -hz);
    glVertex3f(-hx, -hy,  hz);
    glVertex3f(-hx,  hy,  hz);
    glVertex3f(-hx,  hy, -hz);
    glEnd();
}

void drawCubeTextured(float sx, float sy, float sz) {
    float hx = sx * 0.5f, hy = sy * 0.5f, hz = sz * 0.5f;
    
    glBegin(GL_QUADS);
    // Front
    glNormal3f(0, 0, 1);
    glTexCoord2f(0, 0); glVertex3f(-hx, -hy, hz);
    glTexCoord2f(1, 0); glVertex3f( hx, -hy, hz);
    glTexCoord2f(1, 1); glVertex3f( hx,  hy, hz);
    glTexCoord2f(0, 1); glVertex3f(-hx,  hy, hz);
    
    // Back
    glNormal3f(0, 0, -1);
    glTexCoord2f(0, 0); glVertex3f( hx, -hy, -hz);
    glTexCoord2f(1, 0); glVertex3f(-hx, -hy, -hz);
    glTexCoord2f(1, 1); glVertex3f(-hx,  hy, -hz);
    glTexCoord2f(0, 1); glVertex3f( hx,  hy, -hz);
    
    // Top
    glNormal3f(0, 1, 0);
    glTexCoord2f(0, 0); glVertex3f(-hx, hy,  hz);
    glTexCoord2f(1, 0); glVertex3f( hx, hy,  hz);
    glTexCoord2f(1, 1); glVertex3f( hx, hy, -hz);
    glTexCoord2f(0, 1); glVertex3f(-hx, hy, -hz);
    
    // Bottom
    glNormal3f(0, -1, 0);
    glTexCoord2f(0, 0); glVertex3f(-hx, -hy, -hz);
    glTexCoord2f(1, 0); glVertex3f( hx, -hy, -hz);
    glTexCoord2f(1, 1); glVertex3f( hx, -hy,  hz);
    glTexCoord2f(0, 1); glVertex3f(-hx, -hy,  hz);
    
    // Right
    glNormal3f(1, 0, 0);
    glTexCoord2f(0, 0); glVertex3f(hx, -hy,  hz);
    glTexCoord2f(1, 0); glVertex3f(hx, -hy, -hz);
    glTexCoord2f(1, 1); glVertex3f(hx,  hy, -hz);
    glTexCoord2f(0, 1); glVertex3f(hx,  hy,  hz);
    
    // Left
    glNormal3f(-1, 0, 0);
    glTexCoord2f(0, 0); glVertex3f(-hx, -hy, -hz);
    glTexCoord2f(1, 0); glVertex3f(-hx, -hy,  hz);
    glTexCoord2f(1, 1); glVertex3f(-hx,  hy,  hz);
    glTexCoord2f(0, 1); glVertex3f(-hx,  hy, -hz);
    glEnd();
}

void drawCylinder(float radius, float height, int slices) {
    float halfH = height * 0.5f;
    float angleStep = 2.0f * (float)M_PI / slices;
    
    // Side
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= slices; i++) {
        float angle = i * angleStep;
        float nx = cosf(angle);
        float nz = sinf(angle);
        glNormal3f(nx, 0, nz);
        glVertex3f(radius * nx, halfH, radius * nz);
        glVertex3f(radius * nx, -halfH, radius * nz);
    }
    glEnd();
    
    // Top cap
    glNormal3f(0, 1, 0);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, halfH, 0);
    for (int i = 0; i <= slices; i++) {
        float angle = i * angleStep;
        glVertex3f(radius * cosf(angle), halfH, radius * sinf(angle));
    }
    glEnd();
    
    // Bottom cap
    glNormal3f(0, -1, 0);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, -halfH, 0);
    for (int i = slices; i >= 0; i--) {
        float angle = i * angleStep;
        glVertex3f(radius * cosf(angle), -halfH, radius * sinf(angle));
    }
    glEnd();
}

void drawCylinderOpen(float radius, float height, int slices) {
    float halfH = height * 0.5f;
    float angleStep = 2.0f * (float)M_PI / slices;
    
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= slices; i++) {
        float angle = i * angleStep;
        float nx = cosf(angle);
        float nz = sinf(angle);
        glNormal3f(nx, 0, nz);
        glVertex3f(radius * nx, halfH, radius * nz);
        glVertex3f(radius * nx, -halfH, radius * nz);
    }
    glEnd();
}

void drawCone(float radius, float height, int slices) {
    float angleStep = 2.0f * (float)M_PI / slices;
    float halfH = height * 0.5f;
    float ny = radius / height;
    
    // Side
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, 1, 0);
    glVertex3f(0, halfH, 0);
    for (int i = 0; i <= slices; i++) {
        float angle = i * angleStep;
        float nx = cosf(angle);
        float nz = sinf(angle);
        glNormal3f(nx * ny, 0.5f, nz * ny);
        glVertex3f(radius * nx, -halfH, radius * nz);
    }
    glEnd();
    
    // Bottom cap
    glNormal3f(0, -1, 0);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, -halfH, 0);
    for (int i = slices; i >= 0; i--) {
        float angle = i * angleStep;
        glVertex3f(radius * cosf(angle), -halfH, radius * sinf(angle));
    }
    glEnd();
}

void drawSphere(float radius, int slices, int stacks) {
    for (int i = 0; i < stacks; i++) {
        float phi1 = (float)M_PI * (-0.5f + (float)i / stacks);
        float phi2 = (float)M_PI * (-0.5f + (float)(i + 1) / stacks);
        
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; j++) {
            float theta = 2.0f * (float)M_PI * (float)j / slices;
            
            float x1 = cosf(phi1) * cosf(theta);
            float y1 = sinf(phi1);
            float z1 = cosf(phi1) * sinf(theta);
            
            float x2 = cosf(phi2) * cosf(theta);
            float y2 = sinf(phi2);
            float z2 = cosf(phi2) * sinf(theta);
            
            glNormal3f(x2, y2, z2);
            glVertex3f(radius * x2, radius * y2, radius * z2);
            glNormal3f(x1, y1, z1);
            glVertex3f(radius * x1, radius * y1, radius * z1);
        }
        glEnd();
    }
}

void drawHemisphere(float radius, int slices, int stacks) {
    for (int i = 0; i < stacks; i++) {
        float phi1 = (float)M_PI * 0.5f * (float)i / stacks;
        float phi2 = (float)M_PI * 0.5f * (float)(i + 1) / stacks;
        
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; j++) {
            float theta = 2.0f * (float)M_PI * (float)j / slices;
            
            float x1 = cosf(phi1) * cosf(theta);
            float y1 = sinf(phi1);
            float z1 = cosf(phi1) * sinf(theta);
            
            float x2 = cosf(phi2) * cosf(theta);
            float y2 = sinf(phi2);
            float z2 = cosf(phi2) * sinf(theta);
            
            glNormal3f(x2, y2, z2);
            glVertex3f(radius * x2, radius * y2, radius * z2);
            glNormal3f(x1, y1, z1);
            glVertex3f(radius * x1, radius * y1, radius * z1);
        }
        glEnd();
    }
}

void drawDisk(float innerR, float outerR, int slices) {
    float angleStep = 2.0f * (float)M_PI / slices;
    
    glNormal3f(0, 1, 0);
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= slices; i++) {
        float angle = i * angleStep;
        float c = cosf(angle), s = sinf(angle);
        glVertex3f(outerR * c, 0, outerR * s);
        glVertex3f(innerR * c, 0, innerR * s);
    }
    glEnd();
}

void drawTorus(float majorR, float minorR, int majorSegs, int minorSegs) {
    for (int i = 0; i < majorSegs; i++) {
        float theta1 = 2.0f * (float)M_PI * i / majorSegs;
        float theta2 = 2.0f * (float)M_PI * (i + 1) / majorSegs;
        
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= minorSegs; j++) {
            float phi = 2.0f * (float)M_PI * j / minorSegs;
            
            for (int k = 1; k >= 0; k--) {
                float theta = (k == 1) ? theta2 : theta1;
                float x = (majorR + minorR * cosf(phi)) * cosf(theta);
                float y = minorR * sinf(phi);
                float z = (majorR + minorR * cosf(phi)) * sinf(theta);
                
                float nx = cosf(phi) * cosf(theta);
                float ny = sinf(phi);
                float nz = cosf(phi) * sinf(theta);
                
                glNormal3f(nx, ny, nz);
                glVertex3f(x, y, z);
            }
        }
        glEnd();
    }
}

void drawQuad(float w, float h) {
    float hw = w * 0.5f, hh = h * 0.5f;
    glNormal3f(0, 0, 1);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-hw, -hh, 0);
    glTexCoord2f(1, 0); glVertex3f( hw, -hh, 0);
    glTexCoord2f(1, 1); glVertex3f( hw,  hh, 0);
    glTexCoord2f(0, 1); glVertex3f(-hw,  hh, 0);
    glEnd();
}

void drawQuadXZ(float w, float d) {
    float hw = w * 0.5f, hd = d * 0.5f;
    glNormal3f(0, 1, 0);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-hw, 0, -hd);
    glTexCoord2f(1, 0); glVertex3f( hw, 0, -hd);
    glTexCoord2f(1, 1); glVertex3f( hw, 0,  hd);
    glTexCoord2f(0, 1); glVertex3f(-hw, 0,  hd);
    glEnd();
}

// ============================================================
// ADVANCED SHAPES
// ============================================================

void drawRoundedBox(float sx, float sy, float sz, float radius, int segments) {
    // Simplified: draw a regular box for now, with slightly beveled edges
    float hx = sx * 0.5f, hy = sy * 0.5f, hz = sz * 0.5f;
    float r = fminf(radius, fminf(hx, fminf(hy, hz)) * 0.3f);
    
    // Main body
    drawCube(sx - 2*r, sy, sz);
    drawCube(sx, sy - 2*r, sz);
    drawCube(sx, sy, sz - 2*r);
    
    // Edge cylinders (simplified - just the 4 vertical edges)
    float edgeH = sy - 2*r;
    if (edgeH > 0 && r > 0.01f) {
        int segs = segments;
        float angleStep = (float)M_PI * 0.5f / segs;
        
        // Four vertical edge fillets
        float corners[4][2] = {
            { hx - r,  hz - r},
            {-hx + r,  hz - r},
            {-hx + r, -hz + r},
            { hx - r, -hz + r}
        };
        float startAngles[4] = { 0, (float)M_PI*0.5f, (float)M_PI, (float)M_PI*1.5f };
        
        for (int c = 0; c < 4; c++) {
            glBegin(GL_QUAD_STRIP);
            for (int i = 0; i <= segs; i++) {
                float angle = startAngles[c] + i * angleStep;
                float nx = cosf(angle);
                float nz = sinf(angle);
                glNormal3f(nx, 0, nz);
                glVertex3f(corners[c][0] + r * nx,  hy - r, corners[c][1] + r * nz);
                glVertex3f(corners[c][0] + r * nx, -hy + r, corners[c][1] + r * nz);
            }
            glEnd();
        }
    }
}

void drawArch(float width, float height, float depth, int segments) {
    float hw = width * 0.5f;
    float hd = depth * 0.5f;
    float archHeight = height * 0.4f;
    float rectHeight = height - archHeight;
    float archRadius = hw;
    
    // Rectangular part
    glPushMatrix();
    glTranslatef(0, rectHeight * 0.5f, 0);
    drawCube(width, rectHeight, depth);
    glPopMatrix();
    
    // Arch part
    float angleStep = (float)M_PI / segments;
    
    // Front arch face
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, 0, 1);
    glVertex3f(0, rectHeight + archRadius, hd);
    for (int i = 0; i <= segments; i++) {
        float angle = (float)M_PI - i * angleStep;
        glVertex3f(archRadius * cosf(angle), rectHeight + archRadius * sinf(angle), hd);
    }
    glEnd();
    
    // Back arch face
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, 0, -1);
    glVertex3f(0, rectHeight + archRadius, -hd);
    for (int i = segments; i >= 0; i--) {
        float angle = (float)M_PI - i * angleStep;
        glVertex3f(archRadius * cosf(angle), rectHeight + archRadius * sinf(angle), -hd);
    }
    glEnd();
    
    // Arch top surface
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; i++) {
        float angle = (float)M_PI - i * angleStep;
        float nx = cosf(angle);
        float ny = sinf(angle);
        glNormal3f(nx, ny, 0);
        glVertex3f(archRadius * nx, rectHeight + archRadius * ny, hd);
        glVertex3f(archRadius * nx, rectHeight + archRadius * ny, -hd);
    }
    glEnd();
}

void drawPipe(float radius, const Vector3& start, const Vector3& end, int slices) {
    Vector3 dir = end - start;
    float len = dir.length();
    if (len < 0.001f) return;
    
    dir = dir / len;
    
    // Find rotation to align cylinder with direction
    Vector3 up(0, 1, 0);
    Vector3 right = up.cross(dir);
    float rightLen = right.length();
    
    glPushMatrix();
    glTranslatef((start.x + end.x)*0.5f, (start.y + end.y)*0.5f, (start.z + end.z)*0.5f);
    
    if (rightLen > 0.001f) {
        right = right / rightLen;
        float angle = RAD2DEG(acosf(clamp(up.dot(dir), -1.0f, 1.0f)));
        glRotatef(angle, right.x, right.y, right.z);
    } else if (dir.y < 0) {
        glRotatef(180, 1, 0, 0);
    }
    
    drawCylinder(radius, len, slices);
    glPopMatrix();
}

void drawWireFrame(float sx, float sy, float sz, float thickness) {
    float hx = sx * 0.5f, hy = sy * 0.5f, hz = sz * 0.5f;
    float t = thickness;
    
    // 12 edges of a box
    // Bottom 4 edges
    glPushMatrix(); glTranslatef(0, -hy, -hz); drawCube(sx, t, t); glPopMatrix();
    glPushMatrix(); glTranslatef(0, -hy,  hz); drawCube(sx, t, t); glPopMatrix();
    glPushMatrix(); glTranslatef(-hx, -hy, 0); drawCube(t, t, sz); glPopMatrix();
    glPushMatrix(); glTranslatef( hx, -hy, 0); drawCube(t, t, sz); glPopMatrix();
    
    // Top 4 edges
    glPushMatrix(); glTranslatef(0, hy, -hz); drawCube(sx, t, t); glPopMatrix();
    glPushMatrix(); glTranslatef(0, hy,  hz); drawCube(sx, t, t); glPopMatrix();
    glPushMatrix(); glTranslatef(-hx, hy, 0); drawCube(t, t, sz); glPopMatrix();
    glPushMatrix(); glTranslatef( hx, hy, 0); drawCube(t, t, sz); glPopMatrix();
    
    // 4 vertical edges
    glPushMatrix(); glTranslatef(-hx, 0, -hz); drawCube(t, sy, t); glPopMatrix();
    glPushMatrix(); glTranslatef( hx, 0, -hz); drawCube(t, sy, t); glPopMatrix();
    glPushMatrix(); glTranslatef(-hx, 0,  hz); drawCube(t, sy, t); glPopMatrix();
    glPushMatrix(); glTranslatef( hx, 0,  hz); drawCube(t, sy, t); glPopMatrix();
}

void drawTrapezoid(float bottomW, float topW, float height, float depth) {
    float hd = depth * 0.5f;
    float hb = bottomW * 0.5f;
    float ht = topW * 0.5f;
    
    glBegin(GL_QUADS);
    // Front
    glNormal3f(0, 0, 1);
    glVertex3f(-hb, 0, hd);
    glVertex3f( hb, 0, hd);
    glVertex3f( ht, height, hd);
    glVertex3f(-ht, height, hd);
    
    // Back
    glNormal3f(0, 0, -1);
    glVertex3f( hb, 0, -hd);
    glVertex3f(-hb, 0, -hd);
    glVertex3f(-ht, height, -hd);
    glVertex3f( ht, height, -hd);
    
    // Top
    glNormal3f(0, 1, 0);
    glVertex3f(-ht, height, hd);
    glVertex3f( ht, height, hd);
    glVertex3f( ht, height, -hd);
    glVertex3f(-ht, height, -hd);
    
    // Bottom
    glNormal3f(0, -1, 0);
    glVertex3f(-hb, 0, -hd);
    glVertex3f( hb, 0, -hd);
    glVertex3f( hb, 0, hd);
    glVertex3f(-hb, 0, hd);
    
    // Right
    float slopeX = height / (hb - ht);
    float nx = slopeX > 0.01f ? 1.0f : 0.0f;
    float nny = (hb - ht) > 0.01f ? (hb - ht) / height : 0.0f;
    glNormal3f(nx, nny, 0);
    glVertex3f(hb, 0, hd);
    glVertex3f(hb, 0, -hd);
    glVertex3f(ht, height, -hd);
    glVertex3f(ht, height, hd);
    
    // Left
    glNormal3f(-nx, nny, 0);
    glVertex3f(-hb, 0, -hd);
    glVertex3f(-hb, 0, hd);
    glVertex3f(-ht, height, hd);
    glVertex3f(-ht, height, -hd);
    glEnd();
}

void drawPrism(float width, float height, float depth) {
    float hw = width * 0.5f;
    float hd = depth * 0.5f;
    
    // Triangular prism (roof shape)
    glBegin(GL_TRIANGLES);
    // Front triangle
    glNormal3f(0, 0, 1);
    glVertex3f(-hw, 0, hd);
    glVertex3f( hw, 0, hd);
    glVertex3f(0, height, hd);
    
    // Back triangle
    glNormal3f(0, 0, -1);
    glVertex3f( hw, 0, -hd);
    glVertex3f(-hw, 0, -hd);
    glVertex3f(0, height, -hd);
    glEnd();
    
    // Slope faces
    float slopeLen = sqrtf(hw * hw + height * height);
    float snx = height / slopeLen;
    float sny = hw / slopeLen;
    
    glBegin(GL_QUADS);
    // Right slope
    glNormal3f(snx, sny, 0);
    glVertex3f(hw, 0, hd);
    glVertex3f(hw, 0, -hd);
    glVertex3f(0, height, -hd);
    glVertex3f(0, height, hd);
    
    // Left slope
    glNormal3f(-snx, sny, 0);
    glVertex3f(-hw, 0, -hd);
    glVertex3f(-hw, 0, hd);
    glVertex3f(0, height, hd);
    glVertex3f(0, height, -hd);
    
    // Bottom
    glNormal3f(0, -1, 0);
    glVertex3f(-hw, 0, -hd);
    glVertex3f( hw, 0, -hd);
    glVertex3f( hw, 0, hd);
    glVertex3f(-hw, 0, hd);
    glEnd();
}

void drawStairs(int steps, float totalWidth, float totalHeight, float depth) {
    float stepH = totalHeight / steps;
    float stepD = depth / steps;
    float hw = totalWidth * 0.5f;
    
    for (int i = 0; i < steps; i++) {
        glPushMatrix();
        glTranslatef(0, stepH * (i + 0.5f), -stepD * (i + 0.5f));
        drawCube(totalWidth, stepH, stepD);
        glPopMatrix();
    }
}

// ============================================================
// COMPLEX DRAWING HELPERS
// ============================================================

void drawWindow(float w, float h, bool open, bool hasShutters) {
    float frameT = 0.05f;

    // Window frame
    Palette::WOOD_DARK.applyMaterial();
    drawWireFrame(w, h, 0.08f, frameT);

    // Glass pane
    if (!open) {
        Palette::GLASS_BLUE.applyMaterial();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glPushMatrix();
        glTranslatef(0, 0, 0.01f); // Small physical offset to prevent z-fighting
        drawQuad(w - frameT*2, h - frameT*2);
        glPopMatrix();
        glDisable(GL_BLEND);
    }

    // Cross divider
    Palette::WOOD_DARK.applyMaterial();
    glPushMatrix();
    drawCube(w, frameT, 0.06f); // horizontal
    glPopMatrix();
    glPushMatrix();
    drawCube(frameT, h, 0.06f); // vertical
    glPopMatrix();

    // Shutters
    if (hasShutters) {
        Color shutterColor(0.35f, 0.55f, 0.35f);
        shutterColor.applyMaterial();
        float shutterW = w * 0.3f;
        // Left shutter
        glPushMatrix();
        glTranslatef(-w*0.5f - shutterW*0.5f + 0.02f, 0, 0.03f); // Increased z offset
        drawCube(shutterW, h * 0.95f, 0.03f);
        glPopMatrix();
        // Right shutter
        glPushMatrix();
        glTranslatef(w*0.5f + shutterW*0.5f - 0.02f, 0, 0.03f); // Increased z offset
        drawCube(shutterW, h * 0.95f, 0.03f);
        glPopMatrix();
    }
}

void drawDoor(float w, float h, bool isRollerDoor) {
    if (isRollerDoor) {
        // Roller door (cua cuon)
        Palette::METAL_GRAY.applyMaterial();
        // Draw horizontal slats
        int slats = (int)(h / 0.08f);
        for (int i = 0; i < slats; i++) {
            float y = -h*0.5f + (i + 0.5f) * (h / slats);
            glPushMatrix();
            glTranslatef(0, y, 0);
            drawCube(w, h / slats * 0.9f, 0.03f);
            glPopMatrix();
        }
        // Frame
        Palette::METAL_DARK.applyMaterial();
        glPushMatrix();
        glTranslatef(-w*0.5f, 0, 0);
        drawCube(0.06f, h, 0.08f);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(w*0.5f, 0, 0);
        drawCube(0.06f, h, 0.08f);
        glPopMatrix();
    } else {
        // Regular door
        Palette::WOOD_LIGHT.applyMaterial();
        drawCube(w, h, 0.06f);
        
        // Panels
        Palette::WOOD_DARK.applyMaterial();
        float panelW = w * 0.35f;
        float panelH = h * 0.35f;
        glPushMatrix();
        glTranslatef(0, h*0.15f, 0.035f);
        drawCube(panelW, panelH, 0.015f);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(0, -h*0.15f, 0.035f);
        drawCube(panelW, panelH, 0.015f);
        glPopMatrix();
        
        // Handle
        Palette::METAL_GRAY.applyMaterial();
        glPushMatrix();
        glTranslatef(w * 0.3f, 0, 0.05f);
        drawSphere(0.03f, 6, 4);
        glPopMatrix();
    }
}

void drawRailing(float length, float height, int posts) {
    Palette::IRON_BLACK.applyMaterial();
    float spacing = length / (posts - 1);
    
    // Top rail
    glPushMatrix();
    glTranslatef(0, height, 0);
    drawCube(length, 0.04f, 0.04f);
    glPopMatrix();
    
    // Bottom rail
    glPushMatrix();
    glTranslatef(0, 0.1f, 0);
    drawCube(length, 0.04f, 0.04f);
    glPopMatrix();
    
    // Vertical posts
    for (int i = 0; i < posts; i++) {
        float x = -length * 0.5f + i * spacing;
        glPushMatrix();
        glTranslatef(x, height * 0.5f, 0);
        drawCube(0.03f, height, 0.03f);
        glPopMatrix();
    }
}

void drawFence(float length, float height, int posts) {
    float spacing = length / (posts - 1);
    
    // Horizontal bars
    Palette::WOOD_LIGHT.applyMaterial();
    for (int h_idx = 0; h_idx < 3; h_idx++) {
        float y = height * 0.2f + height * 0.3f * h_idx;
        glPushMatrix();
        glTranslatef(0, y, 0);
        drawCube(length, 0.08f, 0.04f);
        glPopMatrix();
    }
    
    // Posts
    Palette::WOOD_DARK.applyMaterial();
    for (int i = 0; i < posts; i++) {
        float x = -length * 0.5f + i * spacing;
        glPushMatrix();
        glTranslatef(x, height * 0.5f, 0);
        drawCube(0.08f, height, 0.08f);
        glPopMatrix();
        
        // Post cap
        glPushMatrix();
        glTranslatef(x, height + 0.03f, 0);
        drawCube(0.12f, 0.06f, 0.12f);
        glPopMatrix();
    }
}

void drawBrickPattern(float w, float h, int rows, int cols) {
    float brickH = h / rows;
    float brickW = w / cols;
    float mortarT = 0.005f;
    
    glBegin(GL_QUADS);
    for (int r = 0; r < rows; r++) {
        float offset = (r % 2 == 0) ? 0 : brickW * 0.5f;
        for (int c = 0; c < cols; c++) {
            float x = -w * 0.5f + c * brickW + offset;
            float y = -h * 0.5f + r * brickH;
            
            if (x + brickW > w * 0.5f) continue;
            
            float bx0 = x + mortarT;
            float bx1 = x + brickW - mortarT;
            float by0 = y + mortarT;
            float by1 = y + brickH - mortarT;
            
            // Slight color variation per brick
            float v = noise2D(x * 10, y * 10) * 0.05f;
            glColor3f(0.75f + v, 0.35f + v * 0.5f, 0.25f + v * 0.3f);
            
            glNormal3f(0, 0, 1);
            glVertex3f(bx0, by0, 0.001f);
            glVertex3f(bx1, by0, 0.001f);
            glVertex3f(bx1, by1, 0.001f);
            glVertex3f(bx0, by1, 0.001f);
        }
    }
    glEnd();
}

void drawTileRoof(float width, float depth, float angle) {
    Palette::ROOF_TERRACOTTA.applyMaterial();
    
    float hw = width * 0.5f;
    float roofHeight = hw * tanf(DEG2RAD(angle));
    
    // Main roof shape (prism)
    drawPrism(width + 0.2f, roofHeight, depth + 0.3f);
    
    // Tile ridges
    Palette::ROOF_RED.applyMaterial();
    float tileRows = 8;
    float tileH = roofHeight / tileRows;
    float slopeLen = sqrtf(hw * hw + roofHeight * roofHeight);
    
    for (int i = 0; i < (int)tileRows; i++) {
        float t = (float)i / tileRows;
        float y = t * roofHeight;
        float x = hw * (1.0f - t);
        
        // Right side ridge
        glPushMatrix();
        float rx = x * 0.5f;
        float ry = y + tileH * 0.5f;
        glTranslatef(rx, ry, 0.0f);
        float slopeAngle = RAD2DEG(atan2f(roofHeight, hw));
        glRotatef(-slopeAngle, 0.0f, 0.0f, 1.0f);
        drawCube(slopeLen / tileRows * 1.05f, 0.03f, depth + 0.2f);
        glPopMatrix();
    }
}

void drawFlatRoof(float width, float depth) {
    Palette::CONCRETE.applyMaterial();
    glPushMatrix();
    drawCube(width + 0.1f, 0.15f, depth + 0.1f);
    glPopMatrix();
    
    // Roof edge parapet
    Palette::WALL_WHITE.applyMaterial();
    float parapetH = 0.6f;
    // Front
    glPushMatrix();
    glTranslatef(0, parapetH * 0.5f + 0.075f, depth * 0.5f + 0.05f);
    drawCube(width + 0.2f, parapetH, 0.1f);
    glPopMatrix();
    // Back
    glPushMatrix();
    glTranslatef(0, parapetH * 0.5f + 0.075f, -depth * 0.5f - 0.05f);
    drawCube(width + 0.2f, parapetH, 0.1f);
    glPopMatrix();
    // Left
    glPushMatrix();
    glTranslatef(-width * 0.5f - 0.05f, parapetH * 0.5f + 0.075f, 0);
    drawCube(0.1f, parapetH, depth + 0.2f);
    glPopMatrix();
    // Right
    glPushMatrix();
    glTranslatef(width * 0.5f + 0.05f, parapetH * 0.5f + 0.075f, 0);
    drawCube(0.1f, parapetH, depth + 0.2f);
    glPopMatrix();
}

void drawAwning(float width, float depth, const Color& color) {
    color.applyMaterial();
    
    float hw = width * 0.5f;
    float angle = 15.0f;
    float dropH = depth * sinf(DEG2RAD(angle));
    
    // Awning surface (angled quad)
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    glVertex3f(-hw, 0, 0);
    glVertex3f( hw, 0, 0);
    glVertex3f( hw, -dropH, depth);
    glVertex3f(-hw, -dropH, depth);
    glEnd();
    
    // Bottom side
    glBegin(GL_QUADS);
    glNormal3f(0, -1, 0);
    glVertex3f(-hw, -0.02f, 0);
    glVertex3f( hw, -0.02f, 0);
    glVertex3f( hw, -dropH - 0.02f, depth);
    glVertex3f(-hw, -dropH - 0.02f, depth);
    glEnd();
    
    // Scalloped edge
    Color edgeColor = color.lerp(Color(1,1,1), 0.3f);
    edgeColor.applyMaterial();
    int scallops = (int)(width / 0.3f);
    float scW = width / scallops;
    for (int i = 0; i < scallops; i++) {
        float x = -hw + (i + 0.5f) * scW;
        glPushMatrix();
        glTranslatef(x, -dropH - 0.08f, depth);
        glScalef(scW * 0.8f, 0.12f, 0.02f);
        drawSphere(0.5f, 6, 4);
        glPopMatrix();
    }
    
    // Support poles
    Palette::METAL_GRAY.applyMaterial();
    glPushMatrix();
    glTranslatef(-hw + 0.1f, -dropH * 0.5f - 0.5f, depth);
    drawCylinder(0.02f, dropH + 1.0f, 6);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(hw - 0.1f, -dropH * 0.5f - 0.5f, depth);
    drawCylinder(0.02f, dropH + 1.0f, 6);
    glPopMatrix();
}

void drawSignBoard(float w, float h, const Color& bgColor, const char* text) {
    // Background board
    bgColor.applyMaterial();
    glPushMatrix();
    glTranslatef(0, 0, 0.01f); // Small offset to prevent z-fighting
    drawCube(w, h, 0.08f);
    glPopMatrix();

    // Border
    Color borderColor = bgColor.lerp(Color(0,0,0), 0.3f);
    borderColor.applyMaterial();
    drawWireFrame(w + 0.02f, h + 0.02f, 0.09f, 0.03f);

    // Text
    if (text) {
        glColor3f(1, 1, 1);
        glDisable(GL_LIGHTING);
        drawText3D(0, 0, 0.05f, text);
        glEnable(GL_LIGHTING);
    }
}

void drawNeonSign(float w, float h, const Color& glowColor, const char* text) {
    // Dark background
    Color(0.1f, 0.1f, 0.1f).applyMaterial();
    glPushMatrix();
    glTranslatef(0, 0, 0.01f); // Small offset to prevent z-fighting
    drawCube(w, h, 0.06f);
    glPopMatrix();

    // Check if it's night time to show glow
    bool isNight = (g_timeOfDay < 6.0f || g_timeOfDay > 18.0f);

    if (isNight) {
        // Neon glow effect
        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        // Outer glow
        Color glow = glowColor;
        glow.a = 0.3f;
        glow.apply();
        glPushMatrix();
        glTranslatef(0, 0, 0.04f);
        drawQuad(w + 0.3f, h + 0.3f);
        glPopMatrix();

        // Inner bright
        glow.a = 0.8f;
        glow.apply();
        glPushMatrix();
        glTranslatef(0, 0, 0.05f);
        drawQuad(w - 0.1f, h - 0.1f);
        glPopMatrix();

        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);
    }

    // Text
    if (text) {
        setEmissiveMaterial(glowColor, isNight ? glowColor : Color(0,0,0));
        drawText3D(0, 0, 0.04f, text);
        resetMaterial();
    }
}

void drawWire(const Vector3& start, const Vector3& end, float sag, int segments) {
    glDisable(GL_LIGHTING);
    Palette::IRON_BLACK.apply();
    glLineWidth(1.5f);
    
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= segments; i++) {
        float t = (float)i / segments;
        Vector3 p = start + (end - start) * t;
        // Add catenary sag
        float sagAmount = sag * (4.0f * t * (1.0f - t));
        p.y -= sagAmount;
        glVertex3f(p.x, p.y, p.z);
    }
    glEnd();
    
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
}

// ============================================================
// TEXT RENDERING
// ============================================================

void drawText2D(float x, float y, const char* text, void* font) {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, g_windowWidth, 0, g_windowHeight);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glRasterPos2f(x, y);
    while (*text) {
        glutBitmapCharacter(font, *text);
        text++;
    }
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void drawText3D(float x, float y, float z, const char* text, void* font) {
    glRasterPos3f(x, y, z);
    if (!text) return;
    while (*text) {
        glutBitmapCharacter(font, *text);
        text++;
    }
}

void drawTextStroke(float x, float y, float z, const char* text, float scale) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(scale, scale, scale);
    while (*text) {
        glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, *text);
        text++;
    }
    glPopMatrix();
}

// ============================================================
// COLOR HELPERS
// ============================================================

Color hslToColor(float h, float s, float l, float a) {
    float c = (1.0f - fabsf(2.0f * l - 1.0f)) * s;
    float x = c * (1.0f - fabsf(fmodf(h / 60.0f, 2.0f) - 1.0f));
    float m = l - c * 0.5f;
    
    float r, g, b;
    if (h < 60)       { r = c; g = x; b = 0; }
    else if (h < 120) { r = x; g = c; b = 0; }
    else if (h < 180) { r = 0; g = c; b = x; }
    else if (h < 240) { r = 0; g = x; b = c; }
    else if (h < 300) { r = x; g = 0; b = c; }
    else              { r = c; g = 0; b = x; }
    
    return Color(r + m, g + m, b + m, a);
}

Color getBuildingColor(int variant) {
    const Color colors[] = {
        Palette::WALL_YELLOW,      // Vàng nghệ (Cổ điển)
        Palette::WALL_ORANGE,      // Cam cháy
        Palette::WALL_LIME,        // Xanh cốm rực rỡ
        Palette::WALL_MINT,        // Xanh bạc hà
        Palette::WALL_BLUE,        // Xanh da trời
        Palette::WALL_PINK,        // Hồng nhạt
        Palette::WALL_TERRACOTTA,  // Đỏ gạch nung
        Palette::WALL_CREAM,       // Kem sữa
        Color(0.95f, 0.95f, 0.80f), // Vàng nhạt (Modern)
        Color(0.70f, 0.90f, 0.95f), // Xanh lơ
        Color(0.90f, 0.70f, 0.95f), // Tím nhạt
        Color(0.70f, 0.95f, 0.70f), // Xanh lá mạ
        Color(0.85f, 0.85f, 0.85f), // Xám xi măng
        Color(0.98f, 0.98f, 0.90f), // Trắng ngà
        Palette::WALL_TEAL,        // Xanh Teal
        Palette::WALL_CORAL        // Màu San hô
    };
    return colors[variant % 16];
}

Color getRoofColor(int variant) {
    const Color colors[] = {
        Palette::ROOF_RED,         // Ngói đỏ tươi
        Palette::ROOF_TERRACOTTA,  // Ngói đất nung
        Palette::ROOF_GRAY,        // Tôn xám
        Palette::ROOF_BROWN,       // Ngói nâu
        Color(0.2f, 0.2f, 0.4f),   // Mái xanh đen modern
        Color(0.1f, 0.3f, 0.2f)    // Mái tôn xanh lá
    };
    return colors[variant % 6];
}

Color getShirtColor(int variant) {
    const Color colors[] = {
        Color(0.85f, 0.25f, 0.20f),  // Red
        Color(0.20f, 0.45f, 0.80f),  // Blue
        Color(0.90f, 0.90f, 0.90f),  // White
        Color(0.15f, 0.55f, 0.30f),  // Green
        Color(0.95f, 0.80f, 0.15f),  // Yellow
        Color(0.60f, 0.30f, 0.70f),  // Purple
        Color(0.95f, 0.55f, 0.20f),  // Orange
        Color(0.30f, 0.30f, 0.30f),  // Dark gray
    };
    return colors[variant % 8];
}

Color getPantsColor(int variant) {
    const Color colors[] = {
        Color(0.15f, 0.20f, 0.35f),  // Dark blue (jeans)
        Color(0.10f, 0.10f, 0.10f),  // Black
        Color(0.45f, 0.35f, 0.25f),  // Khaki
        Color(0.25f, 0.25f, 0.28f),  // Dark gray
    };
    return colors[variant % 4];
}

Color getVehicleColor(int variant) {
    const Color colors[] = {
        Palette::VEH_RED, Palette::VEH_BLUE, Palette::VEH_WHITE,
        Palette::VEH_BLACK, Palette::VEH_SILVER, Palette::VEH_YELLOW,
        Palette::VEH_GREEN,
    };
    return colors[variant % 7];
}

// ============================================================
// MATERIAL HELPERS
// ============================================================

void setMaterial(const Color& diffuse, float shininess, const Color& specular) {
    float amb[] = { diffuse.r * 0.3f, diffuse.g * 0.3f, diffuse.b * 0.3f, diffuse.a };
    float dif[] = { diffuse.r, diffuse.g, diffuse.b, diffuse.a };
    float spc[] = { specular.r, specular.g, specular.b, specular.a };
    float emi[] = { 0, 0, 0, 1 };
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dif);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spc);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emi);
}

void setEmissiveMaterial(const Color& diffuse, const Color& emission) {
    float amb[] = { diffuse.r * 0.3f, diffuse.g * 0.3f, diffuse.b * 0.3f, diffuse.a };
    float dif[] = { diffuse.r, diffuse.g, diffuse.b, diffuse.a };
    float spc[] = { 0.1f, 0.1f, 0.1f, 1 };
    float emi[] = { emission.r, emission.g, emission.b, emission.a };
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dif);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spc);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10.0f);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emi);
}

void resetMaterial() {
    float amb[] = { 0.2f, 0.2f, 0.2f, 1 };
    float dif[] = { 0.8f, 0.8f, 0.8f, 1 };
    float spc[] = { 0, 0, 0, 1 };
    float emi[] = { 0, 0, 0, 1 };
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dif);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spc);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emi);
}

// ============================================================
// 3D MODEL SYSTEM
// ============================================================
static Mesh characterMesh;

bool loadOBJFromStream(std::istream& in, Mesh& mesh) {
    mesh.vertices.clear();
    mesh.normals.clear();
    mesh.faces.clear();
    
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        if (line[0] == 'v' && line[1] == ' ') {
            std::istringstream s(line.substr(2));
            Vector3 v; s >> v.x >> v.y >> v.z;
            mesh.vertices.push_back(v);
        } else if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ') {
            std::istringstream s(line.substr(3));
            Vector3 n; s >> n.x >> n.y >> n.z;
            mesh.normals.push_back(n);
        } else if (line[0] == 'f' && line[1] == ' ') {
            std::istringstream s(line.substr(2));
            MeshFace f;
            // Initialize face indices to -1 to avoid using uninitialized values
            for (int k = 0; k < 3; k++) { f.v[k] = f.n[k] = f.t[k] = -1; }
            std::string part;
            for (int i = 0; i < 3; i++) {
                if (!(s >> part)) break;
                size_t firstSlash = part.find('/');
                size_t lastSlash = part.find_last_of('/');
                
                f.v[i] = std::stoi(part.substr(0, firstSlash)) - 1;
                f.n[i] = -1; f.t[i] = -1;
                
                if (firstSlash != std::string::npos && lastSlash != std::string::npos) {
                    if (lastSlash > firstSlash + 1) {
                        std::string tStr = part.substr(firstSlash + 1, lastSlash - firstSlash - 1);
                        if (!tStr.empty()) f.t[i] = std::stoi(tStr) - 1;
                    }
                    std::string nStr = part.substr(lastSlash + 1);
                    if (!nStr.empty()) f.n[i] = std::stoi(nStr) - 1;
                }
            }
            mesh.faces.push_back(f);
        }
    }
    return true;
}

// Realistic low-poly human proxy model (OBJ format)
const char* HUMAN_MODEL_DATA = 
"v 0.00 1.63 0.08\nv -0.11 1.61 0.16\nv 0.11 1.61 0.16\nv 0.11 1.61 -0.01\nv -0.11 1.61 -0.01\n"
"v -0.07 1.54 0.13\nv 0.07 1.54 0.13\nv 0.07 1.54 0.03\nv -0.07 1.54 0.03\n"
"v -0.22 1.48 0.08\nv 0.22 1.48 0.08\nv 0.22 1.48 -0.08\nv -0.22 1.48 -0.08\n"
"v -0.18 0.92 0.08\nv 0.18 0.92 0.08\nv 0.18 0.92 -0.08\nv -0.18 0.92 -0.08\n"
"v -0.15 0.50 0.10\nv 0.15 0.50 0.10\nv 0.15 0.50 -0.06\nv -0.15 0.50 -0.06\n"
"v -0.15 0.00 0.20\nv 0.15 0.00 0.20\nv 0.15 0.00 0.00\nv -0.15 0.00 0.00\n"
"vn 0 1 0\nvn 0 0 1\nvn 1 0 0\nvn -1 0 0\nvn 0 0 -1\n"
"f 1//1 2//1 3//1\nf 1//1 3//1 4//1\nf 1//1 4//1 5//1\nf 1//1 5//1 2//1\n"
"f 6//2 7//2 11//2\nf 6//2 11//2 10//2\nf 7//2 11//2 12//2\nf 7//2 8//2 12//2\n"
"f 10//2 11//2 15//2\nf 10//2 15//2 14//2\nf 11//2 12//2 16//2\nf 11//2 16//2 15//2\n"
"f 14//2 15//2 19//2\nf 14//2 19//2 18//2\nf 15//2 16//2 20//2\nf 15//2 20//2 19//2\n"
"f 18//2 19//2 23//2\nf 18//2 23//2 22//2\nf 19//2 20//2 24//2\nf 19//2 24//2 23//2\n";

void initCharacterModels() {
    std::string data(HUMAN_MODEL_DATA);
    std::istringstream iss(data);
    loadOBJFromStream(iss, characterMesh);
}

Mesh& getCharacterMesh() {
    return characterMesh;
}
// ============================================================
// OBJ LOADING (Using tinyobjloader)
// ============================================================
#define TINYOBJLOADER_IMPLEMENTATION
#include "external/tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

bool loadTexture(const char* filename, GLuint& texID) {
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);
    
    if (!data) {
        printf("Error loading texture %s: %s\n", filename, stbi_failure_reason());
        return false;
    }

    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    gluBuild2DMipmaps(GL_TEXTURE_2D, channels, width, height, format, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    printf("Loaded texture: %s (%dx%d, %d channels)\n", filename, width, height, channels);
    return true;
}

bool loadMeshFromOBJ(const char* filename, Mesh& outMesh) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename)) {
        printf("Error loading OBJ %s: %s\n", filename, err.c_str());
        return false;
    }

    outMesh.vertices.clear();
    outMesh.normals.clear();
    outMesh.texcoords.clear();
    outMesh.faces.clear();

    // Transfer vertices
    for (size_t i = 0; i < attrib.vertices.size(); i += 3) {
        outMesh.vertices.push_back(Vector3(attrib.vertices[i], attrib.vertices[i+1], attrib.vertices[i+2]));
    }
    // Transfer normals
    for (size_t i = 0; i < attrib.normals.size(); i += 3) {
        outMesh.normals.push_back(Vector3(attrib.normals[i], attrib.normals[i+1], attrib.normals[i+2]));
    }
    // Transfer texcoords
    for (size_t i = 0; i < attrib.texcoords.size(); i += 2) {
        outMesh.texcoords.push_back(Vector2(attrib.texcoords[i], attrib.texcoords[i+1]));
    }

    // Transfer faces
    for (const auto& shape : shapes) {
        for (size_t f = 0; f < shape.mesh.indices.size(); f += 3) {
            MeshFace face;
            for (int v = 0; v < 3; v++) {
                face.v[v] = shape.mesh.indices[f + v].vertex_index;
                face.n[v] = shape.mesh.indices[f + v].normal_index;
                face.t[v] = shape.mesh.indices[f + v].texcoord_index;
            }
            outMesh.faces.push_back(face);
        }
    }

    printf("Loaded OBJ: %s (%zu vertices, %zu faces)\n", filename, outMesh.vertices.size(), outMesh.faces.size());
    return true;
}

void drawMesh(const Mesh& mesh) {
    // Save and adjust some GL state
    glPushAttrib(GL_ENABLE_BIT);
    // Only enable texturing when texcoords are available
    if (!mesh.texcoords.empty()) glEnable(GL_TEXTURE_2D);
    glDisable(GL_CULL_FACE);

    glBegin(GL_TRIANGLES);
    for (const auto& face : mesh.faces) {
        for (int i = 0; i < 3; i++) {
            if (face.n[i] >= 0 && face.n[i] < (int)mesh.normals.size()) {
                const Vector3& n = mesh.normals[face.n[i]];
                glNormal3f(n.x, n.y, n.z);
            }
            if (face.t[i] >= 0 && face.t[i] < (int)mesh.texcoords.size()) {
                const Vector2& uv = mesh.texcoords[face.t[i]];
                glTexCoord2f(uv.x, uv.y);
            }
            if (face.v[i] >= 0 && face.v[i] < (int)mesh.vertices.size()) {
                const Vector3& v = mesh.vertices[face.v[i]];
                glVertex3f(v.x, v.y, v.z);
            }
        }
    }
    glEnd();

    if (!mesh.texcoords.empty()) glDisable(GL_TEXTURE_2D);
    glPopAttrib();
}

void drawModel(const Mesh& mesh, float scale, bool isZUp, float yOffset, float intrinsicRotation) {
    if (mesh.vertices.empty()) return;

    glPushMatrix();
    
    // 1. Dịch chuyển (Translation) - thường để căn chỉnh tâm hoặc độ cao chân
    glTranslatef(0.0f, yOffset, 0.0f);

    // 2. Tỷ lệ (Scaling)
    glScalef(scale, scale, scale);

    // 3. Sửa hướng (Orientation Fixes)
    if (isZUp) {
        // Chuyển từ hệ tọa độ Z-up (thường thấy trong Blender/3ds Max) sang Y-up của OpenGL
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    }
    
    // 4. Xoay nội tại (Intrinsic Rotation) - sửa hướng mũi xe hoặc hướng mặt người
    if (fabs(intrinsicRotation) > 0.001f) {
        if (isZUp) {
            glRotatef(intrinsicRotation, 0.0f, 0.0f, 1.0f); // Xoay quanh trục Z sau khi đã đổi hướng Z-up
        } else {
            glRotatef(intrinsicRotation, 0.0f, 1.0f, 0.0f); // Xoay quanh trục Y cho mô hình Y-up thông thường
        }
    }

    drawMesh(mesh);
    
    glPopMatrix();
}
