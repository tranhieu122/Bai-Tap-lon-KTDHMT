#include "street.h"
#include "../engine/globals.h"
#include "sky.h"
#include "furniture.h"
#include "../engine/texture.h"
#include "../engine/utils.h"
#include <GL/freeglut.h>

// ============================================================
// INITIALIZATION
// ============================================================
void streetInit() {
  // Road meshes could be pre-compiled into display lists or VBOs here,
  // but for simplicity we'll just draw them directly in streetDraw()
}

// ============================================================
// UPDATE LOGIC
// ============================================================
void streetUpdate(float dt) {
  (void)dt;
  // Not much to update for static geometry
}

// ============================================================
// DRAWING HELPERS
// ============================================================
static void drawMarkings(float length, float width, bool hasMedian, bool waitLineLeft, bool waitLineRight);
static void drawCrosswalk(float x, float z, bool rotated);
static void drawManhole(float x, float z);
static void drawArrowMarking(float x, float z, float scale, bool leftTurn);

static void drawSewerGrate(float x, float z) {
  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glDisable(GL_LIGHTING);
  
  // Iron frame
  Palette::IRON_BLACK.apply();
  glPushMatrix();
  glTranslatef(x, 0.015f, z);
  drawCube(0.8f, 0.02f, 1.2f);
  
  // Grate bars
  Palette::METAL_GRAY.apply();
  for (float b = -0.5f; b <= 0.5f; b += 0.15f) {
    glPushMatrix();
    glTranslatef(0, 0.01f, b);
    drawCube(0.7f, 0.01f, 0.05f);
    glPopMatrix();
  }
  glPopMatrix();
  
  glPopAttrib();
}

static void draw3DMedian(float length, float width) {
  if (width <= 0) return;
  
  // 1. Concrete curbs for the median
  Palette::CURB.applyMaterial();
  float h = 0.25f; // Raised slightly higher than sidewalk for safety
  
  // Left Curb
  glPushMatrix();
  glTranslatef(-width / 2, h / 2, 0);
  drawCube(0.15f, h, length);
  glPopMatrix();
  
  // Right Curb
  glPushMatrix();
  glTranslatef(width / 2, h / 2, 0);
  drawCube(0.15f, h, length);
  glPopMatrix();

  // 2. Median fill (Grass/Dirt with Vietnamese planters logic)
  Palette::WALL_GREEN.applyMaterial();
  glPushMatrix();
  glTranslatef(0, h - 0.05f, 0);
  drawCube(width - 0.2f, 0.1f, length);
  glPopMatrix();

  // 3. Flower pots / Small trees at intervals
  for (float z = -length / 2 + 5; z < length / 2; z += 15) {
     glPushMatrix();
     glTranslatef(0, h + 0.2f, z);
     Palette::WOOD_DARK.applyMaterial();
     drawCube(0.5f, 0.4f, 0.5f);
     Palette::WALL_GREEN.applyMaterial();
     glTranslatef(0, 0.3f, 0);
     drawCylinder(0.15f, 0.6f, 8);
     glPopMatrix();
  }
}

static void drawRoadSurface(float length, float width) {
  // 1. MAIN ASPHALT SURFACE
  if (g_texAsphalt != 0) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, g_texAsphalt);
  } else {
      textureBind(TEX_ASPHALT);
  }
  
  Palette::ASPHALT.applyMaterial();
  float tyScale = length / 4.0f;
  float txScale = width / 2.0f;

  glBegin(GL_QUADS);
  glNormal3f(0, 1, 0);
  
  // Left side of road (from median to left curb)
  glTexCoord2f(0, 0);        glVertex3f(-width / 2, 0, -length / 2);
  glTexCoord2f(txScale, 0);  glVertex3f(-MEDIAN_WIDTH/2, 0, -length / 2);
  glTexCoord2f(txScale, tyScale); glVertex3f(-MEDIAN_WIDTH/2, 0, length / 2);
  glTexCoord2f(0, tyScale);  glVertex3f(-width / 2, 0, length / 2);

  // Right side of road (from median to right curb)
  glTexCoord2f(0, 0);        glVertex3f(MEDIAN_WIDTH/2, 0, -length / 2);
  glTexCoord2f(txScale, 0);  glVertex3f(width / 2, 0, -length / 2);
  glTexCoord2f(txScale, tyScale); glVertex3f(width / 2, 0, length / 2);
  glTexCoord2f(0, tyScale);  glVertex3f(MEDIAN_WIDTH/2, 0, length / 2);
  glEnd();

  if (g_texAsphalt != 0) glDisable(GL_TEXTURE_2D);
  else textureUnbind();

  // 2. 3D MEDIAN
  draw3DMedian(length, MEDIAN_WIDTH);

  // 3. ROAD MARKINGS (Raised slightly to avoid Z-fighting)
  drawMarkings(length, width, true, false, false);
  
  // 4. WATER ACCUMULATION (Rain)
  if (g_weather == WEATHER_RAIN) {
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDisable(GL_LIGHTING);
    Color(0.2f, 0.2f, 0.2f, 0.3f).apply();
    glBegin(GL_QUADS);
    glVertex3f(-width / 2, 0.005f, -length / 2);
    glVertex3f(width / 2, 0.005f, -length / 2);
    glVertex3f(width / 2, 0.005f, length / 2);
    glVertex3f(-width / 2, 0.005f, length / 2);
    glEnd();
    glPopAttrib();
  }
}

static void drawSidewalk(float length, float width, bool isRight) {
  float side = isRight ? 1.0f : -1.0f;
  float shift = side * (width/2.0f + SIDEWALK_WIDTH/2.0f);
  float h = SIDEWALK_HEIGHT;

  glPushMatrix();
  glTranslatef(shift, 0, 0);

  // 1. Main surface
  textureBind(TEX_SIDEWALK);
  Palette::SIDEWALK_BRICK.applyMaterial();
  glBegin(GL_QUADS);
  glNormal3f(0, 1, 0);
  float txScale = SIDEWALK_WIDTH / 2.0f;
  float tyScale = length / 2.0f;
  glTexCoord2f(0, 0);        glVertex3f(-SIDEWALK_WIDTH / 2, h, -length / 2);
  glTexCoord2f(txScale, 0);  glVertex3f(SIDEWALK_WIDTH / 2, h, -length / 2);
  glTexCoord2f(txScale, tyScale); glVertex3f(SIDEWALK_WIDTH / 2, h, length / 2);
  glTexCoord2f(0, tyScale);  glVertex3f(-SIDEWALK_WIDTH / 2, h, length / 2);
  glEnd();
  textureUnbind();

  // 2. 3D Side Curb (Bó vỉa)
  Palette::CURB.applyMaterial();
  glPushMatrix();
  glTranslatef(-side * (SIDEWALK_WIDTH / 2 + 0.05f), h / 2, 0);
  drawCube(0.2f, h, length);
  glPopMatrix();

  // 3. 3D Sewer Grates along the curb
  for (float sz = -length / 2 + 15; sz < length / 2; sz += 30) {
    drawSewerGrate(-side * (SIDEWALK_WIDTH / 2 + 0.4f), sz);
  }

  glPopMatrix();
  glDisable(GL_TEXTURE_2D);
}

static void drawMarkings(float length, float width, bool hasMedian,
                         bool waitLineLeft, bool waitLineRight) {
  glDisable(GL_LIGHTING);
  Palette::LANE_MARK.apply(); // White markings

  float y = 0.08f; // slightly above road to prevent Z-fighting

  // Median yellow lines
  if (hasMedian) {
    Color(0.8f, 0.8f, 0.1f).apply();
    glBegin(GL_QUADS);
    glVertex3f(-MEDIAN_WIDTH/2 - 0.1f, y, -length/2); glVertex3f(-MEDIAN_WIDTH/2, y, -length/2); glVertex3f(-MEDIAN_WIDTH/2, y, length/2); glVertex3f(-MEDIAN_WIDTH/2 - 0.1f, y, length/2);
    glVertex3f(MEDIAN_WIDTH/2, y, -length/2); glVertex3f(MEDIAN_WIDTH/2 + 0.1f, y, -length/2); glVertex3f(MEDIAN_WIDTH/2 + 0.1f, y, length/2); glVertex3f(MEDIAN_WIDTH/2, y, length/2);
    glEnd();
  } else {
    Color(0.8f, 0.8f, 0.1f).apply();
    int numDashes = (int)(length / 3.0f);
    glBegin(GL_QUADS);
    for (int i = 0; i < numDashes; i++) {
      float sz = -length/2 + i*3.0f + 0.5f; float ez = sz + 1.5f;
      glVertex3f(-0.1f, y, sz); glVertex3f(0.1f, y, sz); glVertex3f(0.1f, y, ez); glVertex3f(-0.1f, y, ez);
    }
    glEnd();
  }

  // Dash lines & Wait Lines
  Palette::LANE_MARK.apply();
  float laneW = (width/2 - (hasMedian ? MEDIAN_WIDTH/2 : 0)) / (hasMedian ? 2.0f : 1.0f);
  int numDashes = (int)(length / 4.0f);
  
  glBegin(GL_QUADS);
  for (int i = 0; i < numDashes; i++) {
    float sz = -length/2 + i*4.0f + 1.0f; float ez = sz + 2.0f;
    float offset = (hasMedian ? MEDIAN_WIDTH/2 : 0) + laneW;
    // Drawn on both left and right sides
    glVertex3f(-offset - 0.15f, y, sz); glVertex3f(-offset + 0.15f, y, sz); glVertex3f(-offset + 0.15f, y, ez); glVertex3f(-offset - 0.15f, y, ez);
    glVertex3f(offset - 0.15f, y, sz); glVertex3f(offset + 0.15f, y, sz); glVertex3f(offset + 0.15f, y, ez); glVertex3f(offset - 0.15f, y, ez);
  }

  // STOP LINES at intersection boundaries
  float stopLineThickness = 0.6f;
  if (waitLineLeft) { // Intersection is at local Z = +length/2
    float sz = length / 2.0f - 1.2f; // Offset back to make room for crosswalk
    glVertex3f(-width/2, y, sz); glVertex3f(width/2, y, sz); glVertex3f(width/2, y, sz - stopLineThickness); glVertex3f(-width/2, y, sz - stopLineThickness);
    
    // Add Turn Arrows near intersection
    drawArrowMarking(-width/4, sz - 3.0f, 1.0f, true);  // Left turn
    drawArrowMarking(width/4, sz - 3.0f, 1.0f, false); // Straight
  }
  if (waitLineRight) { // Intersection is at local Z = -length/2
    float sz = -length / 2.0f + 1.2f; // Offset back
    glVertex3f(-width/2, y, sz); glVertex3f(width/2, y, sz); glVertex3f(width/2, y, sz + 0.5f); glVertex3f(-width/2, y, sz + 0.5f);
    
    // Add Turn Arrows (needs rotation or mirrored logic)
    glPushMatrix();
    glRotatef(180, 0, 1, 0);
    drawArrowMarking(-width/4, -sz - 3.0f, 1.0f, true);
    drawArrowMarking(width/4, -sz - 3.0f, 1.0f, false);
    glPopMatrix();
  }
  glEnd();

  glEnable(GL_LIGHTING);
}

static void drawArrowMarking(float x, float z, float scale, bool leftTurn) {
  float y = 0.085f;
  glDisable(GL_LIGHTING);
  Palette::LANE_MARK.apply();

  glPushMatrix();
  glTranslatef(x, y, z);
  glScalef(scale, 1.0f, scale);

  glBegin(GL_QUADS);
  // Shaft
  glVertex3f(-0.1f, 0, -1.0f); glVertex3f(0.1f, 0, -1.0f); glVertex3f(0.1f, 0, 0.2f); glVertex3f(-0.1f, 0, 0.2f);
  
  if (leftTurn) {
      // Left turn elbow
      glVertex3f(-0.5f, 0, 0.2f); glVertex3f(0.1f, 0, 0.2f); glVertex3f(0.1f, 0, 0.4f); glVertex3f(-0.5f, 0, 0.4f);
      // Arrow head (triangle as quad)
      glVertex3f(-0.5f, 0, 0.1f); glVertex3f(-0.7f, 0, 0.3f); glVertex3f(-0.5f, 0, 0.5f); glVertex3f(-0.5f, 0, 0.3f);
  } else {
      // Straight head
      glVertex3f(-0.3f, 0, 0.2f); glVertex3f(0.3f, 0, 0.2f); glVertex3f(0, 0, 0.8f); glVertex3f(0, 0, 0.8f);
  }
  glEnd();
  
  glPopMatrix();
}

static void drawCrosswalk(float x, float z, bool rotated) {
  glDisable(GL_LIGHTING);
  Palette::CROSSWALK_WHITE.apply();

  float cwWidth = CROSSWALK_WIDTH;
  float cwLength = STREET_WIDTH;
  if (rotated)
    cwLength = SIDE_STREET_WIDTH;

  glPushMatrix();
  glTranslatef(x, 0.11f, z);
  if (rotated)
    glRotatef(90, 0, 1, 0);

  // Draw zebra stripes
  int numStripes = (int)((cwLength - 1.0f) / (CROSSWALK_STRIPE_W * 2.0f));
  float startX = -cwLength / 2 + CROSSWALK_STRIPE_W;

  glBegin(GL_QUADS);
  for (int i = 0; i < numStripes; i++) {
    float px = startX + i * CROSSWALK_STRIPE_W * 1.8f; // Closer spacing for realistic look
    glVertex3f(px, 0, -cwWidth / 2);
    glVertex3f(px + CROSSWALK_STRIPE_W, 0, -cwWidth / 2);
    glVertex3f(px + CROSSWALK_STRIPE_W, 0, cwWidth / 2);
    glVertex3f(px, 0, cwWidth / 2);
  }
  glEnd();

  glPopMatrix();
  glEnable(GL_LIGHTING);
}

static void drawManhole(float x, float z) {
  glDisable(GL_LIGHTING);
  Palette::IRON_BLACK.apply();
  glPushMatrix();
  glTranslatef(x, 0.05f, z);
  drawDisk(0.0f, 0.45f, 12); // outer rim
  Palette::METAL_GRAY.apply();
  drawDisk(0.0f, 0.4f, 12); // center cover
  glPopMatrix();
  glEnable(GL_LIGHTING);
}

// ============================================================
// MAIN DRAWING
// ============================================================
void streetDraw() {
  // 1. 3D ROAD MODEL (Asphalt + Median + Curbs)
  drawRoadSurface(STREET_LENGTH, STREET_WIDTH);

  // 2. 3D SIDEWALKS
  drawSidewalk(STREET_LENGTH, STREET_WIDTH, true); 
  drawSidewalk(STREET_LENGTH, STREET_WIDTH, false); 

  // 3. CROSSWALKS & MANHOLES
  for (float sz = -STREET_LENGTH / 2 + 20; sz < STREET_LENGTH / 2; sz += 40) {
      drawManhole(2.5f, sz);
      drawManhole(-2.5f, sz + 8);
  }

  // 4. FURNITURE (ELECTRIC POLES & LAMPS)
  // These are grounded on the sidewalk edge
  extern void drawElectricPole();
  extern void streetLampDraw(bool isOn);
  extern void drawStreetWires();

  float poleX = STREET_WIDTH / 2 + 1.25f;
  for (float sz = -STREET_LENGTH / 2 + 10; sz < STREET_LENGTH / 2; sz += 30) {
      // Right side
      glPushMatrix();
      glTranslatef(poleX, 0, sz);
      drawElectricPole();
      glPopMatrix();
      
      // Lamps halfway between poles
      glPushMatrix();
      glTranslatef(poleX - 0.2f, 0, sz + 15);
      streetLampDraw(true);
      glPopMatrix();
      
      // Left side (mirrored)
      glPushMatrix();
      glTranslatef(-poleX, 0, sz + 5);
      glRotatef(180, 0, 1, 0);
      drawElectricPole();
      glPopMatrix();
  }
  
  // Connect wires between poles
  drawStreetWires();
}
