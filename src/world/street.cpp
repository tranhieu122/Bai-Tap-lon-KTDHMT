#include "street.h"
#include "../engine/globals.h"
#include "sky.h"
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
static void drawRoadSurface(float length, float width) {
  // Asphalt
  if (g_texAsphalt != 0) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, g_texAsphalt);
  } else {
      textureBind(TEX_ASPHALT);
  }
  
  Palette::ASPHALT.applyMaterial();

  glBegin(GL_QUADS);
  glNormal3f(0, 1, 0);
  // Tiling texture coordinates - adjust scaling for high-res PNG
  float tyScale = length / 2.0f;
  float txScale = width / 2.0f;

  glTexCoord2f(0, 0);
  glVertex3f(-width / 2, 0, -length / 2);
  glTexCoord2f(txScale, 0);
  glVertex3f(width / 2, 0, -length / 2);
  glTexCoord2f(txScale, tyScale);
  glVertex3f(width / 2, 0, length / 2);
  glTexCoord2f(0, tyScale);
  glVertex3f(-width / 2, 0, length / 2);
  glEnd();

  if (g_texAsphalt != 0) {
      glDisable(GL_TEXTURE_2D);
  } else {
      textureUnbind();
  }

  // Dash lines for lane separation (2 lanes per side)
  // Each side is (8 - 0.5) = 7.5m wide. Middle is 3.75m from median edge.
  // Center of side is 0.5 + 3.75 = 4.25
  Palette::LANE_MARK.applyMaterial();
  float lanePos = 4.25f; 
  for (float sz = -length / 2.0f; sz < length / 2.0f; sz += 4.0f) {
    glBegin(GL_QUADS);
    glVertex3f(-lanePos - 0.05f, 0.01f, sz);
    glVertex3f(-lanePos + 0.05f, 0.01f, sz);
    glVertex3f(-lanePos + 0.05f, 0.01f, sz + 2.0f);
    glVertex3f(-lanePos - 0.05f, 0.01f, sz + 2.0f);

    glVertex3f(lanePos - 0.05f, 0.01f, sz);
    glVertex3f(lanePos + 0.05f, 0.01f, sz);
    glVertex3f(lanePos + 0.05f, 0.01f, sz + 2.0f);
    glVertex3f(lanePos - 0.05f, 0.01f, sz + 2.0f);
    glEnd();
  }

  // Rain reflection on road surface
  if (g_weather == WEATHER_RAIN) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDisable(GL_LIGHTING);

    Color spec(0.2f, 0.2f, 0.2f, 0.3f);
    spec.apply();
    glBegin(GL_QUADS);
    glVertex3f(-width / 2, 0.01f, -length / 2);
    glVertex3f(width / 2, 0.01f, -length / 2);
    glVertex3f(width / 2, 0.01f, length / 2);
    glVertex3f(-width / 2, 0.01f, length / 2);
    glEnd();

    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
  }
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
  }
  if (waitLineRight) { // Intersection is at local Z = -length/2
    float sz = -length / 2.0f + 1.2f; // Offset back
    glVertex3f(-width/2, y, sz); glVertex3f(width/2, y, sz); glVertex3f(width/2, y, sz + 0.5f); glVertex3f(-width/2, y, sz + 0.5f);
  }
  glEnd();
  glEnable(GL_LIGHTING);
}

static void drawSidewalk(float length, float width, bool isRight) {
  // Sidewalk block (Curb + pavement)
  textureBind(TEX_SIDEWALK);
  Palette::SIDEWALK_BRICK.applyMaterial();

  float y = SIDEWALK_HEIGHT;
  float side = isRight ? 1.0f : -1.0f;
  float shift = side * (width/2.0f + SIDEWALK_WIDTH/2.0f);

  glPushMatrix();
  glTranslatef(shift, 0, 0);

  glBegin(GL_QUADS);
  glNormal3f(0, 1, 0);
  float txScale = SIDEWALK_WIDTH / 2.0f;
  float tyScale = length / 2.0f;

  glTexCoord2f(0, 0);
  glVertex3f(-SIDEWALK_WIDTH / 2, y, -length / 2);
  glTexCoord2f(txScale, 0);
  glVertex3f(SIDEWALK_WIDTH / 2, y, -length / 2);
  glTexCoord2f(txScale, tyScale);
  glVertex3f(SIDEWALK_WIDTH / 2, y, length / 2);
  glTexCoord2f(0, tyScale);
  glVertex3f(-SIDEWALK_WIDTH / 2, y, length / 2);
  glEnd();

  textureUnbind();

  // Curbs
  Palette::CURB.applyMaterial();
  glBegin(GL_QUADS);
  glNormal3f(-side, 0, 0);
  glVertex3f(-side * SIDEWALK_WIDTH / 2, 0, -length / 2);
  glVertex3f(-side * SIDEWALK_WIDTH / 2, 0, length / 2);
  glVertex3f(-side * SIDEWALK_WIDTH / 2, y, length / 2);
  glVertex3f(-side * SIDEWALK_WIDTH / 2, y, -length / 2);
  glEnd();
  glPopMatrix();
  glDisable(GL_TEXTURE_2D);
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
  // 1. SINGLE CLEAN ROAD (Z-axis, no markings for minimalist look)
  drawRoadSurface(STREET_LENGTH, STREET_WIDTH);

  // 2. SIDEWALKS
  drawSidewalk(STREET_LENGTH, STREET_WIDTH, true); // Right side
  drawSidewalk(STREET_LENGTH, STREET_WIDTH, false); // Left side

  // 4. FURNITURE & DECOR (Sparse)
  for (float sz = -STREET_LENGTH / 2 + 10; sz < STREET_LENGTH / 2; sz += 25) {
      drawManhole(1.5f, sz);
      drawManhole(-1.5f, sz + 12);
  }

}
