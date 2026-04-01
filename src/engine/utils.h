#pragma once
#ifndef UTILS_H
#define UTILS_H

#include "globals.h"
#include <random>

// ============================================================
// MATH UTILITIES
// ============================================================
float lerp(float a, float b, float t);
float clamp(float v, float lo, float hi);
float smoothstep(float edge0, float edge1, float x);

// Modern Random Generation
float randomFloat(float min, float max);
int   randomInt(int min, int max);
void  seedRandom(unsigned int seed);
float noise1D(float x);
float noise2D(float x, float y);

// ============================================================
// GEOMETRY DRAWING - Basic Shapes
// ============================================================
void drawCube(float sx, float sy, float sz);
void drawCubeTextured(float sx, float sy, float sz);
void drawCylinder(float radius, float height, int slices = 16);
void drawCylinderOpen(float radius, float height, int slices = 16);
void drawCone(float radius, float height, int slices = 16);
void drawSphere(float radius, int slices = 12, int stacks = 8);
void drawHemisphere(float radius, int slices = 12, int stacks = 6);
void drawDisk(float innerR, float outerR, int slices = 24);
void drawTorus(float majorR, float minorR, int majorSegs = 16, int minorSegs = 8);
void drawQuad(float w, float h);
void drawQuadXZ(float w, float d);

// ============================================================
// GEOMETRY DRAWING - Advanced Shapes
// ============================================================
void drawRoundedBox(float sx, float sy, float sz, float radius, int segments = 4);
void drawArch(float width, float height, float depth, int segments = 12);
void drawPipe(float radius, const Vector3& start, const Vector3& end, int slices = 8);
void drawWireFrame(float sx, float sy, float sz, float thickness);
void drawTrapezoid(float bottomW, float topW, float height, float depth);
void drawPrism(float width, float height, float depth);
void drawStairs(int steps, float totalWidth, float totalHeight, float depth);

// ============================================================
// COMPLEX DRAWING HELPERS
// ============================================================
void drawWindow(float w, float h, bool open = false, bool hasShutters = false);
void drawDoor(float w, float h, bool isRollerDoor = false);
void drawRailing(float length, float height, int posts = 5);
void drawFence(float length, float height, int posts = 8);
void drawBrickPattern(float w, float h, int rows = 8, int cols = 16);
void drawTileRoof(float width, float depth, float angle = 25.0f);
void drawFlatRoof(float width, float depth);
void drawAwning(float width, float depth, const Color& color);
void drawSignBoard(float w, float h, const Color& bgColor, const char* text = nullptr);
bool loadMeshFromOBJ(const char* filename, Mesh& outMesh);
void drawMesh(const Mesh& mesh);
void drawModel(const Mesh& mesh, float scale, bool isZUp = false, float yOffset = 0.0f, float intrinsicRotation = 0.0f);
bool loadTexture(const char* filename, GLuint& texID);
void drawNeonSign(float w, float h, const Color& glowColor, const char* text = nullptr);

// 3D MODELS
bool loadOBJFromStream(std::istream& in, Mesh& mesh);
void initCharacterModels();
Mesh& getCharacterMesh();
void drawWire(const Vector3& start, const Vector3& end, float sag = 0.5f, int segments = 12);

// ============================================================
// TEXT RENDERING
// ============================================================
void drawText2D(float x, float y, const char* text, void* font = GLUT_BITMAP_HELVETICA_12);
void drawText3D(float x, float y, float z, const char* text, void* font = GLUT_BITMAP_HELVETICA_10);
void drawTextStroke(float x, float y, float z, const char* text, float scale = 0.005f);

// ============================================================
// COLOR HELPERS
// ============================================================
Color hslToColor(float h, float s, float l, float a = 1.0f);
Color getBuildingColor(int variant);
Color getRoofColor(int variant);
Color getShirtColor(int variant);
Color getPantsColor(int variant);
Color getVehicleColor(int variant);

// ============================================================
// MATERIAL HELPERS
// ============================================================
void setMaterial(const Color& diffuse, float shininess = 30.0f, const Color& specular = Color(0.2f,0.2f,0.2f));
void setEmissiveMaterial(const Color& diffuse, const Color& emission);
void resetMaterial();

// ============================================================
// DEPTH BIAS / Z-FIGHTING PREVENTION
// ============================================================
inline void enableDecalOffset() {
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(-0.5f, -0.5f); // Subtle bias - reduce z-fighting
}

inline void disableDecalOffset() {
  glDisable(GL_POLYGON_OFFSET_FILL);
}

#endif // UTILS_H
