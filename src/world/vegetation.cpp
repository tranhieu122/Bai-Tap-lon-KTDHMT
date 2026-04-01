#include "vegetation.h"
#include "sky.h"
#include "../engine/utils.h"
#include <GL/freeglut.h>

// Draw trunk utility
static void drawTrunk(float height, float radiusBase, float radiusTop) {
  Palette::TREE_TRUNK.applyMaterial();
  glPushMatrix();
  glTranslatef(0, height / 2, 0);
  // Approximate cone trunk
  float hw = height / 2.0f;
  int slices = 8;
  float aStep = 2.0f * (float)M_PI / (float)slices;

  glBegin(GL_QUAD_STRIP);
  for (int i = 0; i <= slices; i++) {
    float a = (float)i * aStep;
    float nx = cosf(a), nz = sinf(a);
    glNormal3f(nx, 0.2f, nz);
    glVertex3f(radiusTop * nx, hw, radiusTop * nz);
    glVertex3f(radiusBase * nx, -hw, radiusBase * nz);
  }
  glEnd();
  glPopMatrix();

  // White painted base (Voi quet goc cay)
  Palette::WALL_WHITE.applyMaterial();
  glPushMatrix();
  glTranslatef(0, 0.4f, 0); // Base height approx 0.8m
  drawCylinder(radiusBase + 0.01f, 0.8f, 8);
  glPopMatrix();
}

// Draw foliage cluster utility (Sphere-like)
static void drawFoliageCluster(const Vector3 &pos, float radius,
                               const Color &col) {
  col.applyMaterial();
  glPushMatrix();
  glTranslatef(pos.x, pos.y, pos.z);

  // Deform sphere slightly for organic look
  glScalef(1.0f, 0.7f, 1.0f);
  drawSphere(radius, 8, 5);
  glPopMatrix();
}

void treeBangDraw(float size) {
  // Bang tree: prominent layered foliage (tán xếp tầng)
  float height = 6.0f * size;
  float radius = 0.3f * size;

  drawTrunk(height * 0.4f, radius, radius * 0.6f);

  // Tiers
  Color leafMain = Palette::LEAF_GREEN;
  Color leafDark = Palette::LEAF_DARK;

  // Bottom tier
  drawFoliageCluster(Vector3(0, height * 0.4f, 0), 2.5f * size, leafDark);

  // Middle tier
  drawFoliageCluster(Vector3(0, height * 0.65f, 0), 2.0f * size, leafMain);

  // Top tier
  drawFoliageCluster(Vector3(0, height * 0.85f, 0), 1.2f * size,
                     Palette::LEAF_LIGHT);
}

void treePhuongDraw(float size) {
  // Phuong tree (Flamboyant): wide umbrella spread, red flowers
  float height = 5.0f * size;
  float radius = 0.4f * size;

  drawTrunk(height * 0.5f, radius, radius * 0.8f);

  // Umbrella shape canopy
  for (int i = 0; i < 5; i++) {
    float a = i * (2.0f * (float)M_PI / 5.0f);
    float r = 1.5f * size;
    Vector3 pos(cosf(a) * r, height * 0.6f, sinf(a) * r);

    drawFoliageCluster(pos, 2.0f * size, Palette::LEAF_DARK);

    // Add red flowers on top of the canopy if in summer
    // For visual flair, we just always draw them
    drawFoliageCluster(pos + Vector3(0, 0.5f * size, 0), 1.0f * size,
                       Palette::FLOWER_RED);
  }

  // Center cluster
  drawFoliageCluster(Vector3(0, height * 0.75f, 0), 2.5f * size,
                     Palette::LEAF_GREEN);
  drawFoliageCluster(Vector3(0, height * 0.9f, 0), 1.5f * size,
                     Palette::FLOWER_RED);
}

void treePalmDraw(float size) {
  // Tall curved trunk
  float height = 8.0f * size;
  float radius = 0.25f * size;

  Palette::TREE_BARK.applyMaterial();

  // Draw segmented trunk with curve
  int segs = 6;
  float segH = height / segs;
  Vector3 cur(0, 0, 0);
  Vector3 dir(0, 1, 0);

  glPushMatrix();
  for (int i = 0; i < segs; i++) {
    glPushMatrix();
    glTranslatef(cur.x, cur.y, cur.z);
    // Tilt slightly more each segment
    glRotatef(i * 3.0f, 1, 0, 0);
    glTranslatef(0, segH / 2, 0);

    // Slightly bulging segment
    float rt = radius * (1.0f - (float)i / segs * 0.3f);
    drawCylinder(rt, segH * 1.05f, 8);
    glPopMatrix();

    // Adjust for next seg
    float nx = 0.0f;
    float ny = cosf(DEG2RAD((float)i * 3.0f));
    float nz = sinf(DEG2RAD((float)i * 3.0f));
    cur += Vector3(nx, ny, nz) * segH;
  }

  // Leaves at the top
  glTranslatef(cur.x, cur.y, cur.z);
  Palette::LEAF_GREEN.applyMaterial();

  for (int i = 0; i < 8; i++) {
    glPushMatrix();
    glRotatef(i * 45.0f, 0, 1, 0); // Spaced around
    glRotatef(45.0f, 1, 0, 0);     // Drooping down

    // Draw leaf
    glTranslatef(0, 0, 1.5f * size);
    glScalef(0.6f * size, 0.05f * size, 3.0f * size);
    drawHemisphere(1.0f, 6, 3);
    glPopMatrix();
  }

  // Coconuts
  Palette::WOOD_DARK.applyMaterial();
  glPushMatrix();
  glTranslatef(0.3f, -0.3f, 0);
  drawSphere(0.2f, 6, 6);
  glPopMatrix();
  glPushMatrix();
  glTranslatef(-0.2f, -0.4f, 0.2f);
  drawSphere(0.2f, 6, 6);
  glPopMatrix();
  glPushMatrix();
  glTranslatef(-0.1f, -0.2f, -0.3f);
  drawSphere(0.2f, 6, 6);
  glPopMatrix();

  glPopMatrix();
}

void bushDraw(float size) {
  drawFoliageCluster(Vector3(0, 0.5f * size, 0), 1.0f * size,
                     Palette::LEAF_DARK);
  drawFoliageCluster(Vector3(0.5f * size, 0.4f * size, 0.3f * size),
                     0.8f * size, Palette::LEAF_GREEN);
  drawFoliageCluster(Vector3(-0.4f * size, 0.3f * size, 0.5f * size),
                     0.7f * size, Palette::LEAF_LIGHT);
  drawFoliageCluster(Vector3(-0.3f * size, 0.4f * size, -0.6f * size),
                     0.9f * size, Palette::LEAF_GREEN);
}

void flowerBedDraw(float width, float depth) {
  // Dirt base
  Palette::TREE_BARK.applyMaterial();
  glPushMatrix();
  glTranslatef(0, 0.1f, 0);
  drawCube(width, 0.2f, depth);
  glPopMatrix();

  // Small plants and flowers
  seedRandom(12345); // Fixed seed for same flower bed look - uses mt19937 not srand
  int numFlowers = (int)(width * depth * 4.0f);

  for (int i = 0; i < numFlowers; i++) {
    float x = randomFloat(-width / 2 + 0.2f, width / 2 - 0.2f);
    float z = randomFloat(-depth / 2 + 0.2f, depth / 2 - 0.2f);
    float h = randomFloat(0.3f, 0.6f);

    // Stem
    Palette::GRASS.applyMaterial();
    glPushMatrix();
    glTranslatef(x, 0.2f + h / 2, z);
    drawCylinder(0.02f, h, 4);
    glPopMatrix();

    // Flower Head
    Color fcol = Palette::FLOWER_YELLOW;
    int t = randomInt(0, 2);
    if (t == 1)
      fcol = Palette::FLOWER_RED;
    else if (t == 2)
      fcol = Palette::FLOWER_PURPLE;

    fcol.applyMaterial();
    glPushMatrix();
    glTranslatef(x, 0.2f + h, z);
    drawHemisphere(0.1f, 6, 4);
    glPopMatrix();
  }
}

void vegetationDraw(const TreeInfo &info) {
  glPushMatrix();
  info.transform.applyGL();

  // Premium Wind Sway Animation
  float windStrength = 1.2f;
  float t = g_elapsedTime * 1.5f + (info.transform.position.x + info.transform.position.z) * 0.1f;
  // Use sin combinations for semi-random feel
  float swayX = sinf(t) * cosf(t * 0.7f) * windStrength; 
  float swayZ = sinf(t * 1.1f) * windStrength * 0.8f;
  
  glRotatef(swayX, 1.0f, 0.0f, 0.0f);
  glRotatef(swayZ, 0.0f, 0.0f, 1.0f);

  switch (info.type) {
    case TREE_BANG:
    case TREE_PALM:
        // Sử dụng mô hình cây thực tế mới (Chế độ Y-up cho cây đứng thẳng)
        setMaterial(Color(0.2f, 0.45f, 0.1f), 8.0f); 
        // Tăng tỉ lệ lên 0.5f để cây cao khoảng 5m trên vỉa hè
        drawModel(g_treeMesh, 0.5f * info.age, false, 0.0f, info.age * 90.0f);
        break;
  case TREE_PHUONG:
    // Cây phượng vĩ tự vẽ đặc sắc với hoa đỏ
    treePhuongDraw(info.age); 
    break;
  case TREE_BUSH:
    // Bụi cây cảnh quan
    setMaterial(Color(0.1f, 0.6f, 0.2f), 15.0f);
    bushDraw(info.age);
    break;
  case TREE_FLOWER_BED:
    // Bồn hoa công viên
    flowerBedDraw(2.0f * info.age, 2.0f * info.age);
    break;
  default:
    setMaterial(Color(0.2f, 0.4f, 0.2f), 5.0f);
    drawModel(g_treeMesh, 0.05f * info.age, true, 0.0f, 0.0f);
    break;
  }

  glPopMatrix();
}
