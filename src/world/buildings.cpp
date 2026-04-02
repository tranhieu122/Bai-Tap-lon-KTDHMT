#include "buildings.h"
#include "../engine/globals.h"
#include "../engine/utils.h"
#include "../engine/texture.h"
#include <GL/freeglut.h>

// ============================================================
// BUILDING PARTS DRAWING
// ============================================================

// Enhanced balcony railing with details
static void drawBalconyRailing(float w, float h, int segments) {
  Palette::CONCRETE.applyMaterial();
  float segmentWidth = w / segments;

  // Top rail
  glPushMatrix();
  glTranslatef(0, h, 0);
  drawCube(w, 0.08f, 0.08f);
  glPopMatrix();

  // Vertical balusters
  for (int i = 0; i <= segments; i++) {
    float x = -w / 2 + i * segmentWidth;
    glPushMatrix();
    glTranslatef(x, h / 2, 0);
    drawCube(0.06f, h, 0.06f);
    glPopMatrix();
  }

  // Middle rail
  glPushMatrix();
  glTranslatef(0, h / 2, 0);
  drawCube(w, 0.06f, 0.06f);
  glPopMatrix();
}

// Window with frame and details
static void drawDetailedWindow(float w, float h, int panes_h = 2, int panes_w = 2, int variant = 0) {
  // Outer frame
  Palette::WOOD_DARK.applyMaterial();
  glPushMatrix();
  drawCube(w, h, 0.08f);
  glPopMatrix();

  // Glass panes
  Palette::GLASS_BLUE.applyGlossyMaterial(80.0f);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  float paneW = w / panes_w;
  float paneH = h / panes_h;

  for (int py = 0; py < panes_h; py++) {
    for (int px = 0; px < panes_w; px++) {
      float px_pos = -w / 2 + paneW * (px + 0.5f);
      float py_pos = -h / 2 + paneH * (py + 0.5f);

      // Hiệu ứng ánh sáng đêm (Night lighting effect - Emissive interior)
      if (g_timeOfDay > 18.0f || g_timeOfDay < 6.0f) {
           glPushMatrix();
           glTranslatef(px_pos, py_pos, 0.04f);
           
           // Màu đèn ngẫu nhiên dựa trên variant (Căn hộ thường dùng đèn ấm, văn phòng đèn trắng)
           Color lightCol = (variant % 3 == 0) ? Color(1.0f, 0.92f, 0.7f) : Color(0.85f, 0.95f, 1.0f);
           
           // Thêm hiệu ứng nhấp nháy hoặc độ sáng khác nhau cho sinh động
           float intensity = 0.8f + 0.2f * sinf(g_elapsedTime * 0.3f + variant);
           Color emissiveCol = lightCol;
           emissiveCol.r *= intensity; emissiveCol.g *= intensity; emissiveCol.b *= intensity;

           drawGlowBillboard(paneW > paneH ? paneW * 1.2f : paneH * 1.2f, emissiveCol);
           glPopMatrix();
      }

      glPushMatrix();
      glTranslatef(px_pos, py_pos, 0.02f);
      drawQuad(paneW * 0.9f, paneH * 0.9f);
      glPopMatrix();
    }
  }

  glDisable(GL_BLEND);

  // Inner frame grid
  Palette::WOOD_DARK.applyMaterial();
  glPushMatrix();
  glTranslatef(0, 0, 0.03f);
  glLineWidth(1.5f);

  // Horizontal grid lines
  for (int i = 1; i < panes_h; i++) {
    float y = -h / 2 + i * paneH;
    glBegin(GL_LINES);
    glVertex3f(-w / 2, y, 0);
    glVertex3f(w / 2, y, 0);
    glEnd();
  }

  // Vertical grid lines
  for (int i = 1; i < panes_w; i++) {
    float x = -w / 2 + i * paneW;
    glBegin(GL_LINES);
    glVertex3f(x, -h / 2, 0);
    glVertex3f(x, h / 2, 0);
    glEnd();
  }

  glLineWidth(1.0f);
  glPopMatrix();

  // Optional: Shutter on left
  if (variant % 2 == 1) {
    Palette::WOOD_DARK.applyMaterial();
    glPushMatrix();
    glTranslatef(-w / 2 - 0.1f, 0, 0.04f);
    drawCube(0.08f, h, 0.06f);
    glPopMatrix();
  }
}

// Detailed door with frame and window
static void drawDetailedDoor(float w, float h, bool hasGlass = false, int variant = 0) {
  // Door frame
  Palette::WOOD_DARK.applyMaterial();
  glPushMatrix();
  drawCube(w, h, 0.08f);
  glPopMatrix();

  if (hasGlass) {
    // Glass panel
    Palette::GLASS_BLUE.applyGlossyMaterial(90.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();
    glTranslatef(0, h * 0.35f, 0.02f);
    drawQuad(w * 0.85f, h * 0.6f);
    glPopMatrix();

    glDisable(GL_BLEND);

    // Glass pane grid
    Palette::METAL_GRAY.applyMaterial();
    glPushMatrix();
    glTranslatef(0, h * 0.35f, 0.03f);
    glLineWidth(1.0f);

    for (int i = 1; i < 3; i++) {
      float x = -w * 0.4f + i * w * 0.4f;
      glBegin(GL_LINES);
      glVertex3f(x, -h * 0.3f, 0);
      glVertex3f(x, h * 0.3f, 0);
      glEnd();
    }

    glLineWidth(1.0f);
    glPopMatrix();
  }

  // Door handle/knob
  Palette::METAL_CHROME.applyGlossyMaterial();
  glPushMatrix();
  glTranslatef(w / 2 - 0.15f, 0, 0.1f);
  drawDisk(0, 0.04f, 8);
  glPopMatrix();

  // Door panel lines (wooden effect)
  if (variant % 2 == 1) {
    Palette::WOOD_DARK.applyMaterial();
    glPushMatrix();
    glTranslatef(0, 0, 0.05f);
    glLineWidth(1.5f);

    glBegin(GL_LINES);
    glVertex3f(-w / 2, -h / 2 + 0.5f, 0);
    glVertex3f(w / 2, -h / 2 + 0.5f, 0);
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(-w / 2, 0, 0);
    glVertex3f(w / 2, 0, 0);
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(-w / 2, h / 2 - 0.5f, 0);
    glVertex3f(w / 2, h / 2 - 0.5f, 0);
    glEnd();

    glLineWidth(1.0f);
    glPopMatrix();
  }
}

// Signage/Shop sign with 3D effect
static void drawStorageSign(float w, float h, Color color, const char *text, int variant = 0) {
  (void)text;
  // Sign background
  color.applyMaterial();
  glPushMatrix();
  drawCube(w, h, 0.15f);
  glPopMatrix();

  // Sign border
  Palette::METAL_DARK.applyMaterial();
  glPushMatrix();
  glTranslatef(0, 0, 0.1f);
  glLineWidth(2.5f);
  glBegin(GL_LINE_LOOP);
  glVertex3f(-w / 2, -h / 2, 0);
  glVertex3f(w / 2, -h / 2, 0);
  glVertex3f(w / 2, h / 2, 0);
  glVertex3f(-w / 2, h / 2, 0);
  glEnd();
  glLineWidth(1.0f);
  glPopMatrix();

  // Corner accents
  if (variant % 2 == 0) {
    Palette::METAL_CHROME.applyGlossyMaterial(100.0f);
    float cornerSize = 0.08f;
    glPushMatrix();
    glTranslatef(w / 2 - cornerSize, h / 2 - cornerSize, 0.12f);
    drawCube(cornerSize * 0.5f, cornerSize * 0.5f, 0.02f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-w / 2 + cornerSize, h / 2 - cornerSize, 0.12f);
    drawCube(cornerSize * 0.5f, cornerSize * 0.5f, 0.02f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(w / 2 - cornerSize, -h / 2 + cornerSize, 0.12f);
    drawCube(cornerSize * 0.5f, cornerSize * 0.5f, 0.02f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-w / 2 + cornerSize, -h / 2 + cornerSize, 0.12f);
    drawCube(cornerSize * 0.5f, cornerSize * 0.5f, 0.02f);
    glPopMatrix();
  }
}

// Decorative wall column/pilaster
static void drawWallColumn(float w, float h, float d, int variant = 0) {
  if (variant % 3 == 0) {
    // Classic rectangular column
    Palette::CONCRETE.applyMaterial();
    glPushMatrix();
    drawCube(w, h, d);
    glPopMatrix();

    // Capital
    Palette::WALL_CREAM.applyMaterial();
    glPushMatrix();
    glTranslatef(0, h / 2, 0);
    drawCube(w * 1.2f, w * 0.3f, d * 1.1f);
    glPopMatrix();

    // Base
    glPushMatrix();
    glTranslatef(0, -h / 2, 0);
    drawCube(w * 1.2f, w * 0.3f, d * 1.1f);
    glPopMatrix();
  } else if (variant % 3 == 1) {
    // Rounded fluted column
    Palette::METAL_GRAY.applyMaterial();
    glPushMatrix();
    drawCylinder(w / 2, h, 16);
    glPopMatrix();

    // Capital decorative ring
    Palette::METAL_CHROME.applyGlossyMaterial();
    glPushMatrix();
    glTranslatef(0, h / 2 - 0.1f, 0);
    drawCylinder(w / 2 + 0.05f, 0.1f, 16);
    glPopMatrix();
  } else {
    // Modern minimalist column
    Palette::METAL_DARK.applyMaterial();
    glPushMatrix();
    drawCube(w * 0.6f, h, d * 0.6f);
    glPopMatrix();

    // Accent stripes
    Palette::METAL_GRAY.applyMaterial();
    for (float y = -h / 2 + 0.5f; y < h / 2; y += 1.5f) {
      glPushMatrix();
      glTranslatef(0, y, 0);
      drawCube(w * 0.7f, 0.08f, d * 0.7f);
      glPopMatrix();
    }
  }
}

// Arched window/entrance
static void drawArchedWindow(float w, float h, int variant = 0) {
  // Arch frame
  Palette::WALL_CREAM.applyMaterial();
  glPushMatrix();
  drawCube(w, h, 0.12f);
  glPopMatrix();

  // Glass area
  Palette::GLASS_DARK.applyGlossyMaterial(100.0f);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glPushMatrix();
  glTranslatef(0, 0, 0.05f);
  drawQuad(w * 0.85f, h * 0.85f);
  glPopMatrix();

  glDisable(GL_BLEND);

  // Decorative arch details
  if (variant % 2 == 1) {
    Palette::METAL_CHROME.applyGlossyMaterial(100.0f);
    glPushMatrix();
    glTranslatef(0, h / 2, 0.08f);
    drawCylinder(0.06f, w * 0.6f, 12);
    glPopMatrix();
  }
}

// Storefront display window
static void drawDisplayWindow(float w, float h, int shelves = 3, int variant = 0) {
  // Glass frame
  Palette::GLASS_BLUE.applyGlossyMaterial(120.0f);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glPushMatrix();
  glTranslatef(0, 0, 0);
  drawCube(w, h, 0.2f);
  glPopMatrix();

  glDisable(GL_BLEND);

  // Metal frame
  Palette::METAL_DARK.applyMaterial();
  glPushMatrix();
  glTranslatef(0, 0, 0.1f);
  glLineWidth(3.0f);
  glBegin(GL_LINE_LOOP);
  glVertex3f(-w / 2, -h / 2, 0);
  glVertex3f(w / 2, -h / 2, 0);
  glVertex3f(w / 2, h / 2, 0);
  glVertex3f(-w / 2, h / 2, 0);
  glEnd();
  glLineWidth(1.0f);
  glPopMatrix();

  // Display shelves
  Palette::METAL_GRAY.applyMaterial();
  float shelfSpacing = h / (shelves + 1);
  for (int i = 1; i <= shelves; i++) {
    float y = -h / 2 + i * shelfSpacing;
    glPushMatrix();
    glTranslatef(0, y, 0.05f);
    drawCube(w * 0.9f, 0.06f, 0.15f);
    glPopMatrix();
  }

  // Display items (colored cubes as products)
  for (int i = 1; i <= shelves; i++) {
    float y = -h / 2 + i * shelfSpacing;
    for (int j = 0; j < 3; j++) {
      float x = -w / 3 + j * w / 3;
      glPushMatrix();
      glTranslatef(x, y + 0.08f, 0.08f);
      Color itemCol = getShirtColor((i + j + variant) % 8);
      itemCol.applyMaterial();
      drawCube(0.1f, 0.12f, 0.08f);
      glPopMatrix();
    }
  }
}

// Roof detail with various styles
static void drawRoofDetails(float w, float d, int colorIdx) {
  // Rooftop elements - varies by color index
  if (colorIdx % 5 == 0) {
    // Solar panels
    Palette::METAL_DARK.applyMaterial();
    for (float x = -w / 3; x <= w / 3; x += w / 4) {
      for (float z = -d / 3; z <= d / 3; z += d / 4) {
        glPushMatrix();
        glTranslatef(x, 0.3f, z);
        drawCube(0.6f, 0.02f, 0.6f);
        glPopMatrix();
      }
    }
  } else if (colorIdx % 5 == 1) {
    // Bồn nước Inox Sơn Hà/Tân Á (Stainless steel rooftop water tank)
    glPushMatrix();
    glTranslatef(-w / 4, 0, -d / 4);
    Palette::METAL_DARK.applyMaterial();
    for(float ox = -0.3f; ox <= 0.3f; ox += 0.6f) {
      for(float oz = -0.4f; oz <= 0.4f; oz += 0.8f) {
        glPushMatrix(); glTranslatef(ox, 0.15f, oz); drawCube(0.05f, 0.3f, 0.05f); glPopMatrix();
      }
    }
    glTranslatef(0, 0.45f, 0);
    Palette::METAL_CHROME.applyGlossyMaterial(120.0f);
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    drawCylinder(0.35f, 1.2f, 16);
    glTranslatef(0, 1.21f, 0); Palette::WALL_BLUE.applyMaterial(); drawDisk(0, 0.15f, 12);
    glPopMatrix();
    glPopMatrix();
  } else if (colorIdx % 5 == 2) {
    // Outdoor furniture (tables/chairs for terrace)
    Palette::WOOD_DARK.applyMaterial();
    for (int i = 0; i < 3; i++) {
      glPushMatrix();
      glTranslatef(-w / 3 + i * w / 3, 0.25f, 0);
      drawCube(0.5f, 0.4f, 0.5f);
      glPopMatrix();
    }
  } else if (colorIdx % 5 == 3) {
    // Satellite dishes and antennas
    Palette::METAL_GRAY.applyMaterial();
    glPushMatrix();
    glTranslatef(w / 3, 0.5f, -d / 3);
    drawDisk(0, 0.4f, 16);
    glPopMatrix();

    Palette::METAL_DARK.applyMaterial();
    glPushMatrix();
    glTranslatef(-w / 3, 0.3f, d / 3);
    drawCylinder(0.08f, 1.2f, 8);
    glPopMatrix();
  } else {
    // Mixed clutter - storage boxes, vents, pipes
    Palette::METAL_GRAY.applyMaterial();
    glPushMatrix();
    glTranslatef(0, 0.25f, d / 3);
    drawCube(0.8f, 0.4f, 0.8f);
    glPopMatrix();

    Palette::CONCRETE.applyMaterial();
    glPushMatrix();
    glTranslatef(w / 3, 0.15f, -d / 3);
    drawCylinder(0.12f, 0.8f, 12);
    glPopMatrix();
  }
}

// Balcony with planters and details
static void drawDetailedBalcony(float w, float d, int variant = 0) {
  // Balcony slab
  Palette::CONCRETE.applyMaterial();
  glPushMatrix();
  glTranslatef(0, 0, d / 2); // Start from wall face
  drawCube(w, 0.15f, d);
  glPopMatrix();

  // Railing
  glPushMatrix();
  glTranslatef(0, 0.15f, d / 2); // Anchored at wall face, sticks out d
  drawBalconyRailing(w, BALCONY_HEIGHT, (int)(w / 0.5f) + 1);
  glPopMatrix();

  // Decorative planters on balcony edge
  if (variant % 2 == 1) {
    for (float x = -w / 2 + 0.5f; x < w / 2; x += w / 4) {
      glPushMatrix();
      glTranslatef(x, 0.15f, d - 0.2f); // Near front edge
      Color potCol = getShirtColor((int)(x * 10 + variant) % 8);
      potCol.applyMaterial();
      drawCube(0.25f, 0.35f, 0.25f);

      // Plants in pot
      Palette::WALL_GREEN.applyMaterial();
      glPushMatrix();
      glTranslatef(0, 0.2f, 0);
      drawCylinder(0.1f, 0.3f, 8);
      glPopMatrix();

      glPopMatrix();
    }
  }
}

// Wall section with various finish types
static void drawWallSection(float w, float h, float d, int textureType, int colorIdx) {
  const Color wallColors[] = {
    Palette::WALL_YELLOW, Palette::WALL_ORANGE, Palette::WALL_PINK,
    Palette::WALL_BLUE, Palette::WALL_GREEN, Palette::WALL_LIME,
    Color(1.0f, 0.4f, 0.0f), Color(0.0f, 0.8f, 0.8f), // Extra vibrant
    Palette::WALL_TERRACOTTA, Palette::WALL_CRIMSON
  };

  Color col = wallColors[colorIdx % 10];
  col.applyMaterial();

  glPushMatrix();
  if (g_texBrick != 0 && textureType % 2 == 0) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, g_texBrick);
      drawCubeTextured(w, h, d);
      glDisable(GL_TEXTURE_2D);
  } else if (g_texConcrete != 0 && textureType % 2 == 1) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, g_texConcrete);
      drawCubeTextured(w, h, d);
      glDisable(GL_TEXTURE_2D);
  } else if (textureType % 2 == 0) {
    textureBind(TEX_BRICK_WALL);
    drawCubeTextured(w, h, d);
    textureUnbind();
  } else {
    drawCube(w, h, d);
  }
  glPopMatrix();

  // Wall accent/trim
  if (colorIdx % 3 == 0) {
    Palette::METAL_GRAY.applyMaterial();
    glPushMatrix();
    glTranslatef(0, h / 2 - 0.1f, d / 2 + 0.01f);
    drawCube(w, 0.1f, 0.05f);
    glPopMatrix();
  }
}

static void drawBalcony(float w, float d) {
  Palette::CONCRETE.applyMaterial();
  // Base slab
  glPushMatrix();
  glTranslatef(0, -0.05f, d / 2);
  drawCube(w, 0.1f, d);
  glPopMatrix();

  // Railing
  glPushMatrix();
  glTranslatef(0, 0, d / 2);
  drawRailing(w, BALCONY_HEIGHT, (int)(w / 0.5f) + 1);
  glPopMatrix();
}

static void drawACUnit(float w, float h, float d, int variant = 0) {
  // Variety of AC compressor unit designs
  const Color acColors[] = {
    Palette::WALL_WHITE, Color(0.85f, 0.85f, 0.85f), Color(0.7f, 0.7f, 0.75f),
    Color(0.9f, 0.88f, 0.82f)
  };
  acColors[variant % 4].applyMaterial();

  // Main unit body
  glPushMatrix();
  drawCube(w, h, d);
  glPopMatrix();

  // Grill/vents
  Palette::METAL_DARK.applyMaterial();
  glPushMatrix();
  glTranslatef(0, 0, d / 2 + 0.01f);

  if (variant % 3 == 0) {
    // Circular grill
    drawDisk(0.0f, h * 0.35f, 12);
  } else if (variant % 3 == 1) {
    // Multiple small vents
    for (float vy = -h * 0.3f; vy <= h * 0.3f; vy += h * 0.15f) {
      glPushMatrix();
      glTranslatef(0, vy, 0.001f);
      drawCube(w * 0.8f, 0.05f, 0.02f);
      glPopMatrix();
    }
  } else {
    // Square grill
    drawCube(w * 0.7f, h * 0.7f, 0.02f);
  }
  glPopMatrix();

  // Metal frame around unit
  Palette::METAL_GRAY.applyMaterial();
  glPushMatrix();
  glTranslatef(0, 0, d / 2 + 0.02f);
  glLineWidth(2.0f);
  glBegin(GL_LINE_LOOP);
  float fw = w * 0.5f, fh = h * 0.5f;
  glVertex3f(-fw, -fh, 0);
  glVertex3f(fw, -fh, 0);
  glVertex3f(fw, fh, 0);
  glVertex3f(-fw, fh, 0);
  glEnd();
  glLineWidth(1.0f);
  glPopMatrix();
}

static void drawWaterTank(float r, float l) {
  // Stainless steel rooftop water tank (Son Ha/Tan A style)
  Palette::METAL_CHROME.applyGlossyMaterial();
  glPushMatrix();
  glRotatef(90, 0, 1, 0); // Horizontal cylinder
  drawCylinder(r, l, 12);
  // Caps
  glPushMatrix();
  glTranslatef(0, 0, l / 2);
  drawDisk(0, r, 12);
  glPopMatrix();
  glPushMatrix();
  glTranslatef(0, 0, -l / 2);
  drawDisk(0, r, 12);
  glPopMatrix();
  glPopMatrix();

  // Support frame
  Palette::METAL_GRAY.applyMaterial();
  for (float x = -l / 2 + 0.1f; x <= l / 2 - 0.1f; x += 0.4f) {
    glPushMatrix();
    glTranslatef(x, -r, 0);
    drawCube(0.05f, 0.1f, r * 2.2f);
    glPopMatrix();
  }
}

static void drawShopFront(float w, float h, float d, const std::string &name, int colorIdx = 0) {
  // Protect all states (blending, materials, matrix)
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  // Glass front
  Palette::GLASS_BLUE.applyGlossyMaterial(100.0f);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glPushMatrix();
  glTranslatef(0, h / 2 - 0.5f, d / 2);
  drawQuad(w - 0.4f, h - 1.0f);
  glPopMatrix();
  glDisable(GL_BLEND);

  // Doors
  glPushMatrix();
  glTranslatef(w / 4, h / 2 - 0.5f, d / 2 + 0.05f);
  drawDoor(1.5f, 2.5f, true); 
  glPopMatrix();

  // 1. MAIN HEADER SIGN (Biển hiệu chính)
  const char* shopText = name.empty() ? "CUA HANG" : name.c_str();
  
  // Vibrant Vietnamese shop board colors
  Color boardColors[] = {
      Palette::VEH_RED, Palette::VEH_BLUE, Palette::VEH_GREEN,
      Palette::VEH_YELLOW, Palette::VEH_BLACK, Color(0.1f, 0.4f, 0.2f)
  };
  Color boardCol = boardColors[abs(colorIdx) % 6];
  
  glPushMatrix();
  glTranslatef(0, h - 0.6f, d / 2 + 0.15f);
  
  // Drawing the large board
  drawSignBoard(w * 0.95f, 0.9f, boardCol, shopText);
  
  // 2. SECONDARY DETAILS (Số nhà, Tên phố)
  const char* streets[] = {"PHO HUE", "HANG BAC", "NGUYEN TRAI", "DIEN BIEN PHU", "LE LOI"};
  char details[64];
  // SAFETY: Use snprintf and abs() for index to prevent stack corruption
  snprintf(details, sizeof(details), "SO %d - %s", (abs(colorIdx) * 17) % 200 + 1, streets[abs(colorIdx) % 5]);
  
  // Miniature text at bottom left of the board
  glPushMatrix();
  glTranslatef(-w * 0.4f, -0.3f, 0.08f);
  drawTextStroke(0, 0, 0, details, 0.0015f, false);
  glPopMatrix();

  // "Hotline" on bottom right
  glPushMatrix();
  glTranslatef(w * 0.2f, -0.3f, 0.08f);
  drawTextStroke(0, 0, 0, "HOTLINE: 09xx", 0.0012f, false);
  glPopMatrix();
  
  glPopMatrix();

  // 3. PROJECTING SIGN (Biển vẫy) - Common in VN, sticks out from wall
  if ((abs(colorIdx) + 2) % 3 != 0) {
      glPushMatrix();
      glTranslatef(w/2 + 0.02f, h * 0.65f, d/2);
      glRotatef(90, 0, 1, 0); // Sick out from the facade
      
      const char* vSigns[] = {"TOP", "SALE", "HOT", "24H", "SPA", "FREE"};
      drawProjectingSign(0.6f, 0.6f, boardColors[(abs(colorIdx) + 2) % 6], vSigns[abs(colorIdx) % 6]);
      glPopMatrix();
  }

  // Awning
  glPushMatrix();
  glTranslatef(0, h - 1.2f, d / 2);
  drawAwning(w, 1.2f, Palette::NEON_RED); 
  glPopMatrix();

  glPopAttrib(); // Restore all states
}

static void drawWindowsArrangement(float w, float h, float d, int count) {
  float spacing = w / (count + 1);
  float winW = spacing * 0.65f;
  float winH = h * 0.65f;

  for (int i = 0; i < count; i++) {
    float x = -w / 2 + spacing * (i + 1);
    glPushMatrix();
    glTranslatef(x, h / 2, d / 2 + 0.02f);
    drawWindow(winW, winH, false, ((i + count) % 4 == 0)); // deterministic shutters
    glPopMatrix();

    // Random AC unit next to some windows
    if ((i * 7 + count) % 7 == 0) {
      glPushMatrix();
      glTranslatef(x + winW * 0.6f + 0.3f, h / 2 - 0.2f, d / 2 + 0.15f);
      drawACUnit(0.5f, 0.45f, 0.25f, count + i);
      glPopMatrix();
    }
  }
}

// ============================================================
// COLONIAL STYLE BUILDING (Toa Nha Phap)
// ============================================================
void drawColonialBuilding(float w, float h, float d, int colorIdx) {
  // Colonial architecture colors (Indochina Yellow/Beige)
  Color baseCol = getBuildingColor(colorIdx % 4); 
  // Force typical colonial colors for this type
  if (colorIdx % 2 == 0) baseCol = Color(0.95f, 0.85f, 0.65f); // Classic Yellow
  else baseCol = Color(0.92f, 0.88f, 0.78f); // Classic Cream
  baseCol.applyMaterial();

  // Main body
  glPushMatrix();
  glTranslatef(0, h / 2, 0);
  drawCubeTextured(w, h, d);
  glPopMatrix();

  // Decorative columns on facade
  for (float z = -d / 2 + 0.5f; z <= d / 2 - 0.5f; z += d) {
    glPushMatrix();
    glTranslatef(-w / 2 - 0.2f, h / 4, z);
    drawWallColumn(0.25f, h / 2, 0.2f, colorIdx);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(w / 2 + 0.2f, h / 4, z);
    drawWallColumn(0.25f, h / 2, 0.2f, colorIdx + 1);
    glPopMatrix();
  }

  // Arched windows
  int numWindows = (int)(w / 2.5f);
  float windowSpacing = w / (numWindows + 1);

  for (int floor = 0; floor < 2; floor++) {
    float floorY = floor * (h / 2) + h / 4;

    for (int i = 0; i < numWindows; i++) {
      float x = -w / 2 + windowSpacing * (i + 1);
      glPushMatrix();
      glTranslatef(x, floorY, d / 2 + 0.05f);
      drawArchedWindow(1.2f, 1.5f, colorIdx + i);
      glPopMatrix();
    }
  }

  // Decorative balcony (Front)
  glPushMatrix();
  glTranslatef(0, h / 2, d / 2);
  drawDetailedBalcony(w - 0.5f, 1.2f, colorIdx);
  glPopMatrix();

  // Ornamental cornice/trim
  Palette::WALL_CREAM.applyMaterial();
  glPushMatrix();
  glTranslatef(0, h - 0.1f, 0);
  drawCube(w + 0.4f, 0.2f, d + 0.4f);
  glPopMatrix();

  // Roof with chimneys and ornamental peak
  glPushMatrix();
  glTranslatef(0, h, 0);
  drawTileRoof(w + 0.2f, d + 0.2f, 35.0f);
  
  // Add a chimney
  glPushMatrix();
  glTranslatef(w*0.3f, 0.6f, d*0.1f);
  Palette::WALL_TERRACOTTA.applyMaterial();
  drawCube(0.45f, 1.2f, 0.45f);
  Palette::METAL_DARK.applyMaterial();
  glTranslatef(0, 0.7f, 0);
  drawDisk(0, 0.3f, 12);
  glPopMatrix();

  if (w > 6.0f) {
    // Second chimney for symmetry
    glPushMatrix();
    glTranslatef(-w*0.3f, 0.6f, -d*0.1f);
    Palette::WALL_TERRACOTTA.applyMaterial();
    drawCube(0.45f, 1.2f, 0.45f);
    Palette::METAL_DARK.applyMaterial();
    glTranslatef(0, 0.7f, 0);
    drawDisk(0, 0.3f, 12);
    glPopMatrix();
  }
  glPopMatrix();
}

// ============================================================
// SHOPHOUSE MULTI-FLOOR (Nha Pho Hang)
// ============================================================
void drawShophouse(float w, float h, float d, int floors, int colorIdx, const std::string &shopName) {
  // Shophouse color scheme - Vibrant Indochina / Modern
  Color wallCol = getBuildingColor(colorIdx);
  Color roofCol = getRoofColor(colorIdx);
  wallCol.applyMaterial();

  // Main wall
  glPushMatrix();
  glTranslatef(0, h / 2, 0);
  drawWallSection(w, h, d, colorIdx, colorIdx);
  glPopMatrix();

  // Ground floor - Shop front
  glPushMatrix();
  drawShopFront(w, FLOOR_HEIGHT, d, shopName, colorIdx);
  glPopMatrix();

  // Upper floors - residential with windows and AC units
  for (int i = 1; i < floors; i++) {
    float floorY = i * FLOOR_HEIGHT;

    // Decorative band between floors
    Palette::METAL_GRAY.applyMaterial();
    glPushMatrix();
    glTranslatef(0, floorY + 0.05f, 0);
    drawCube(w + 0.1f, 0.1f, d + 0.1f);
    glPopMatrix();

    // Windows on this floor
    int windowCount = (int)(w / 1.5f);
    float windowSpacing = w / (windowCount + 1);

    for (int j = 0; j < windowCount; j++) {
      float wx = -w / 2 + windowSpacing * (j + 1);

      // Main window
      glPushMatrix();
      glTranslatef(wx, floorY + FLOOR_HEIGHT * 0.55f, d / 2 + 0.02f);
      drawDetailedWindow(0.9f, 0.95f, 2, 2, colorIdx + i + j);
      glPopMatrix();

      // AC unit next to some windows
      if ((j + i) % 3 == 0) {
        glPushMatrix();
        glTranslatef(wx + 0.6f, floorY + FLOOR_HEIGHT * 0.4f, d / 2 + 0.2f);
        drawACUnit(0.5f, 0.45f, 0.3f, colorIdx + i + j);
        glPopMatrix();
      }
    }

    // Side window/light well
    glPushMatrix();
    glTranslatef(-w / 2 - 0.05f, floorY + FLOOR_HEIGHT * 0.55f, 0);
    drawDetailedWindow(0.6f, 0.8f, 1, 1, colorIdx + i);
    glPopMatrix();
  }

  // Roof section
  glPushMatrix();
  glTranslatef(0, h, 0);
  drawFlatRoof(w, d);
  drawRoofDetails(w, d, colorIdx);
  glPopMatrix();
}

// ============================================================
// RESIDENTIAL APARTMENT BLOCK (Chung Cu Nhan Tao)
// ============================================================
void drawApartmentBlock(float w, float h, float d, int floors, int colorIdx) {
  // Modern apartment colors - Slightly more muted but varied
  Color wallCol = getBuildingColor(colorIdx + 8); // Shift to later palette for modern apps
  wallCol.applyMaterial();

  // Main structure
  glPushMatrix();
  glTranslatef(0, h / 2, 0);
  drawCube(w, h, d);
  glPopMatrix();

  // Horizontal floor divisions (bands)
  Palette::METAL_GRAY.applyMaterial();
  for (int i = 0; i < floors; i++) {
    float floorY = i * FLOOR_HEIGHT;
    glPushMatrix();
    glTranslatef(0, floorY + 0.02f, 0);
    // Draw perimeter beams (front, back, left, right) instead of a solid slab
    // Front band
    glPushMatrix(); glTranslatef(0, 0, d/2 + 0.05f); drawCube(w + 0.1f, 0.12f, 0.1f); glPopMatrix();
    // Back band
    glPushMatrix(); glTranslatef(0, 0, -d/2 - 0.05f); drawCube(w + 0.1f, 0.12f, 0.1f); glPopMatrix();
    // Left band
    glPushMatrix(); glTranslatef(-w/2 - 0.05f, 0, 0); drawCube(0.1f, 0.12f, d + 0.1f); glPopMatrix();
    // Right band
    glPushMatrix(); glTranslatef(w/2 + 0.05f, 0, 0); drawCube(0.1f, 0.12f, d + 0.1f); glPopMatrix();
    glPopMatrix();
  }

  // Vertical mullions (structural grid)
  Palette::METAL_DARK.applyMaterial();
  int verticalDivisions = (int)(w / 3.5f) + 1;
  float divisionSpacing = w / verticalDivisions;

  for (int i = 0; i <= verticalDivisions; i++) {
    float x = -w / 2 + i * divisionSpacing;
    glPushMatrix();
    glTranslatef(x, h / 2, d / 2 + 0.01f);
    drawCube(0.08f, h, 0.05f);
    glPopMatrix();
  }

  // Apartment units with windows
  for (int floor = 0; floor < floors; floor++) {
    float floorY = floor * FLOOR_HEIGHT;

    for (int unit = 0; unit < 4; unit++) {
      float ux = -w / 2 + unit * (w / 4) + w / 8;

      // Window 1
      glPushMatrix();
      glTranslatef(ux - 0.35f, floorY + FLOOR_HEIGHT * 0.65f, d / 2 + 0.03f);
      drawDetailedWindow(0.6f, 0.7f, 2, 1, colorIdx + floor + unit);
      glPopMatrix();

      // Window 2
      glPushMatrix();
      glTranslatef(ux + 0.35f, floorY + FLOOR_HEIGHT * 0.65f, d / 2 + 0.03f);
      drawDetailedWindow(0.6f, 0.7f, 2, 1, colorIdx + floor + unit + 1);
      glPopMatrix();

      // Balcony
      glPushMatrix();
      glTranslatef(ux, floorY + FLOOR_HEIGHT * 0.3f, d / 2);
      drawDetailedBalcony(0.8f, 0.8f, colorIdx + unit);
      glPopMatrix();
    }
  }

  // Side face details - stairwell/service area
  Palette::WALL_CREAM.applyMaterial();
  glPushMatrix();
  glTranslatef(-w / 2 - 0.05f, h / 2, 0);
  drawCube(0.3f, h, d);
  glPopMatrix();

  // Stairwell windows
  for (int i = 0; i < floors; i++) {
    float floorY = i * FLOOR_HEIGHT;
    glPushMatrix();
    glTranslatef(-w / 2 - 0.08f, floorY + FLOOR_HEIGHT * 0.6f, 0);
    drawDetailedWindow(0.25f, 0.5f, 1, 1, colorIdx);
    glPopMatrix();
  }

  // Roof
  glPushMatrix();
  glTranslatef(0, h, 0);
  drawFlatRoof(w + 0.3f, d + 0.3f);
  drawRoofDetails(w, d, colorIdx);
  glPopMatrix();
}

// ============================================================
// MODERN SHOPFRONT (Cua Hang Hien Dai)
// ============================================================
void drawModernShopfront(float w, float h, float d, int colorIdx, const std::string &shopName) {
  (void)shopName;
  // Shopfront color scheme
  const Color shopfrontColors[] = {
    Color(0.95f, 0.6f, 0.6f), Color(0.6f, 0.9f, 0.7f), Color(0.9f, 0.85f, 0.6f),
    Color(0.85f, 0.7f, 0.95f), Palette::WALL_BLUE
  };

  Color baseCol = shopfrontColors[colorIdx % 5];
  baseCol.applyMaterial();

  // Main wall
  glPushMatrix();
  glTranslatef(0, h / 2, 0);
  drawCube(w, h, d);
  glPopMatrix();

  // Large display window - divided into sections
  Palette::GLASS_BLUE.applyGlossyMaterial(110.0f);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glPushMatrix();
  glTranslatef(0, h * 0.4f, d / 2 + 0.01f);
  drawCube(w - 0.5f, h * 0.7f, 0.1f);
  glPopMatrix();

  glDisable(GL_BLEND);

  // Display shelves and products
  glPushMatrix();
  glTranslatef(0, h * 0.4f, d / 2 + 0.08f);
  drawDisplayWindow(w - 0.6f, h * 0.6f, 3, colorIdx);
  glPopMatrix();

  // Door section
  glPushMatrix();
  glTranslatef(w / 3, h * 0.2f, d / 2 + 0.05f);
  drawDetailedDoor(1.0f, 2.0f, true, colorIdx);
  glPopMatrix();

  // Side door
  glPushMatrix();
  glTranslatef(-w / 3, h * 0.2f, d / 2 + 0.05f);
  drawDetailedDoor(0.8f, 2.0f, false, colorIdx + 1);
  glPopMatrix();

  // Top signage area with Neon / LED effect
  const char* vnSignsModern[] = { 
    "SIE U THI MINI", "APPLE STORE", "CA PHE HIGHLANDS", "CONG CA PHE",
    "THE GIOI DI DONG", "FOCOCY TEA", "NET CAFE 24/7", "FASHION SHOP"
  };
  const char* currentSign = vnSignsModern[colorIdx % 8];

  glPushMatrix();
  glTranslatef(0, h - 0.6f, d / 2 + 0.1f);
  // Cửa hàng hiện đại dùng biển hiệu Neon sáng hơn
  drawNeonSign(w * 0.95f, 0.7f, baseCol, currentSign);
  glPopMatrix();

  // Decorative awning
  glPushMatrix();
  glTranslatef(0, h - 1.0f, d / 2);
  drawAwning(w + 0.5f, 1.2f, baseCol); // reduced from 1.5f
  glPopMatrix();

  // Roof
  glPushMatrix();
  glTranslatef(0, h, 0);
  drawFlatRoof(w, d);
  glPopMatrix();
}

// ============================================================
// MARKET STALL BUILDING (Toa Nha Cho)
// ============================================================
void drawMarketBuilding(float w, float h, float d, int colorIdx) {
  // Market building (semi-open)
  Color marketCol = Color(0.85f, 0.78f, 0.65f);
  marketCol.applyMaterial();

  // Support structure
  glPushMatrix();
  glTranslatef(0, h / 2, 0);
  drawCube(w, h, d);
  glPopMatrix();

  // Open sides with pillars
  float pillarSpacing = w / 5;
  for (float x = -w / 2; x <= w / 2; x += pillarSpacing) {
    // Front pillar
    glPushMatrix();
    glTranslatef(x, h / 2, d / 2 + 0.2f);
    drawWallColumn(0.35f, h, 0.3f, colorIdx);
    glPopMatrix();

    // Back pillar
    glPushMatrix();
    glTranslatef(x, h / 2, -d / 2 - 0.2f);
    drawWallColumn(0.35f, h, 0.3f, colorIdx + 1);
    glPopMatrix();
  }

  // Display stalls/counters
  Palette::WOOD_DARK.applyMaterial();
  for (int i = 0; i < 3; i++) {
    float z = -d / 2 + 0.5f + i * (d - 1.0f) / 3;
    glPushMatrix();
    glTranslatef(0, 0.3f, z);
    drawCube(w - 1.0f, 0.6f, d / 4);
    glPopMatrix();

    // Market goods (small colored cubes on stalls)
    for (float x = -w / 2 + 0.5f; x < w / 2; x += w / 5) {
      Color goodsCol = getShirtColor((i + (int)x) % 8);
      goodsCol.applyMaterial();
      glPushMatrix();
      glTranslatef(x, 1.0f, z);
      drawCube(0.3f, 0.3f, d / 5);
      glPopMatrix();
    }
  }

  // Metal roof frame
  Palette::METAL_DARK.applyMaterial();
  glPushMatrix();
  glTranslatef(0, h, 0);
  drawCube(w, 0.2f, d);
  glPopMatrix();

  // Decorative hanging lights
  Palette::METAL_CHROME.applyGlossyMaterial(100.0f);
  for (float x = -w / 2 + 1.0f; x < w / 2; x += w / 3) {
    for (float z = -d / 2 + 0.5f; z < d / 2; z += d / 2) {
      glPushMatrix();
      glTranslatef(x, h - 0.3f, z);
      drawDisk(0, 0.08f, 12);
      glPopMatrix();
    }
  }
}

// ============================================================
// TEMPLE/CULTURAL BUILDING (Dien Tho/Nha Tho)
// ============================================================
void drawTempleBuilding(float w, float h, float d, int colorIdx) {
  (void)colorIdx;
  // Temple colors
  Color templeRed = Color(0.8f, 0.2f, 0.2f);
  Color templeGold = Palette::METAL_CHROME;
  Color templeWood = Palette::WOOD_DARK;

  templeRed.applyMaterial();

  // Main structure
  glPushMatrix();
  glTranslatef(0, h / 2, 0);
  drawCube(w, h, d);
  glPopMatrix();

  // Decorative pillars at entrance
  for (float x = -w / 4; x <= w / 4; x += w / 2) {
    glPushMatrix();
    glTranslatef(x, h / 2, d / 2 + 0.3f);
    drawWallColumn(0.3f, h * 0.8f, 0.25f, 1);
    glPopMatrix();
  }

  // Gold decorative bands
  templeGold.applyGlossyMaterial(100.0f);
  glPushMatrix();
  glTranslatef(0, h * 0.7f, 0);
  drawCube(w + 0.5f, 0.15f, d + 0.3f);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0, h * 0.3f, 0);
  drawCube(w + 0.5f, 0.15f, d + 0.3f);
  glPopMatrix();

  // Arched entrance doors
  glPushMatrix();
  glTranslatef(-w / 4, h * 0.3f, d / 2 + 0.08f);
  drawArchedWindow(1.0f, 2.5f, 1);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(w / 4, h * 0.3f, d / 2 + 0.08f);
  drawArchedWindow(1.0f, 2.5f, 0);
  glPopMatrix();

  // Side door
  glPushMatrix();
  glTranslatef(-w / 2 + 0.3f, h * 0.25f, d / 2 + 0.08f);
  drawDetailedDoor(0.8f, 2.0f, false, 0);
  glPopMatrix();

  // Roof - curved/pitched style
  glPushMatrix();
  glTranslatef(0, h, 0);
  drawTileRoof(w + 0.5f, d + 0.5f, 40.0f);
  glPopMatrix();

  // Rooftop ornament (gold sphere/finial)
  templeGold.applyGlossyMaterial(100.0f);
  glPushMatrix();
  glTranslatef(0, h + 0.8f, 0);
  drawDisk(0, 0.15f, 12);
  glPopMatrix();
}

// ============================================================
// GARAGE/WORKSHOP BUILDING (Sua Xe/Xưởng)
// ============================================================
void drawGarageBuilding(float w, float h, float d, int colorIdx) {
  // Industrial colors
  const Color garageColors[] = {
    Color(0.6f, 0.6f, 0.6f), Color(0.75f, 0.75f, 0.75f),
    Color(0.65f, 0.65f, 0.7f)
  };

  garageColors[colorIdx % 3].applyMaterial();

  // Main structure
  glPushMatrix();
  glTranslatef(0, h / 2, 0);
  drawCube(w, h, d);
  glPopMatrix();

  // Large roller doors
  Palette::METAL_DARK.applyMaterial();
  int numDoors = 2;
  float doorWidth = w / numDoors;

  for (int i = 0; i < numDoors; i++) {
    float dx = -w / 2 + doorWidth * (i + 0.5f);

    // Door frame
    glPushMatrix();
    glTranslatef(dx, h * 0.35f, d / 2 + 0.05f);
    drawCube(doorWidth - 0.2f, h * 0.6f, 0.1f);
    glPopMatrix();

    // Horizontal slats on door
    Palette::METAL_GRAY.applyMaterial();
    for (float y = -h * 0.25f; y < h * 0.25f; y += 0.15f) {
      glPushMatrix();
      glTranslatef(dx, y, d / 2 + 0.08f);
      drawCube(doorWidth * 0.9f, 0.08f, 0.05f);
      glPopMatrix();
    }
  }

  // Small office window
  glPushMatrix();
  glTranslatef(w / 2 - 0.5f, h * 0.7f, d / 2 + 0.03f);
  drawDetailedWindow(0.8f, 0.6f, 1, 2, colorIdx);
  glPopMatrix();

  // Roof
  glPushMatrix();
  glTranslatef(0, h, 0);
  drawFlatRoof(w, d);

  // Ventilation pipes
  Palette::METAL_GRAY.applyMaterial();
  for (float x = -w / 3; x <= w / 3; x += w / 3) {
    glPushMatrix();
    glTranslatef(x, 0.3f, 0);
    drawCylinder(0.1f, 0.6f, 12);
    glPopMatrix();
  }

  glPopMatrix();
}

// ============================================================
// VIETNAMESE TUBE HOUSE (Nha Ong)
// ============================================================
void drawTubeHouse(float w, float h, float d, int floors, int colorIdx,
                   bool hasShop, bool hasBalcony, const std::string &shopName) {
  // Vibrant Vietnamese Tube House colors
  Color wallCol = getBuildingColor(colorIdx);
  Color roofCol = getRoofColor(colorIdx);
  wallCol.applyMaterial();

  // Main body with enhanced texture
  textureBind(TEX_BRICK_WALL);
  wallCol.applyMaterial();
  glPushMatrix();
  glTranslatef(0, h / 2, 0);
  drawCubeTextured(w, h, d);
  glPopMatrix();
  textureUnbind();

  // Floors
  for (int i = 0; i < floors; i++) {
    float floorY = i * FLOOR_HEIGHT;

    if (i == 0 && hasShop) {
      // Ground floor shop
      drawShopFront(w, FLOOR_HEIGHT, d, shopName, colorIdx);
    } else {
      // Upper floors
      if (hasBalcony) {
        // Set back the wall slightly to make room for balcony
        glPushMatrix();
        glTranslatef(0, floorY, d / 2);
        drawBalcony(w - 0.2f, BALCONY_DEPTH);
        glPopMatrix();

        // Door to balcony
        glPushMatrix();
        glTranslatef(0, floorY + 1.25f, d / 2 - 0.05f);
        drawDoor(1.2f, 2.3f, false);
        glPopMatrix();

        // Add AC units on balcony side (varied designs)
        glPushMatrix();
        glTranslatef(w / 2 - 0.6f, floorY + 0.5f, d / 2 + 0.2f);
        drawACUnit(0.6f, 0.5f, 0.3f, colorIdx + i);
        glPopMatrix();

        // Windows next to door if wide enough
        if (w > 5.0f) {
          glPushMatrix();
          glTranslatef(-w / 4 - 0.2f, floorY + 1.0f, d / 2 - 0.05f);
          drawWindow(1.0f, 1.5f);
          glPopMatrix();
          glPushMatrix();
          glTranslatef(w / 4 + 0.2f, floorY + 1.0f, d / 2 - 0.05f);
          drawWindow(1.0f, 1.5f);
          glPopMatrix();
        }
      } else {
        // Just windows
        glPushMatrix();
        glTranslatef(0, floorY, 0);
        drawWindowsArrangement(w, FLOOR_HEIGHT, d, (int)(w / 3.0f));
        glPopMatrix();
      }
    }
  }

  // Roof (Flat roof with parapet or tile roof)
  glPushMatrix();
  glTranslatef(0, h, 0);
  if (colorIdx % 2 == 0) {
    drawFlatRoof(w, d);
    // Add water tank, typical in VN
    glPushMatrix();
    glTranslatef(w / 4, 0.5f, -d / 4);
    drawWaterTank(0.35f, 1.1f);
    glPopMatrix();

    // Enhanced rooftop clutter
    for (int j = 0; j < 3; j++) {
      if ((j + colorIdx) % 3 == 0) {
        glPushMatrix();
        glTranslatef((j - 1) * w / 3.0f, 0.3f, (j - 1) * d / 3.0f);
        if ((j + colorIdx) % 6 == 0) {
          Palette::METAL_DARK.applyMaterial();
          drawCube(0.6f, 0.6f, 0.6f);
        } else {
          Palette::METAL_GRAY.applyMaterial();
          drawCylinder(0.15f, 0.4f, 8);
        }
        glPopMatrix();
      }
    }
  } else {
    drawTileRoof(w, d, 25.0f);
  }
  glPopMatrix();
}

// ============================================================
// OFFICE BUILDING (Toa Nha Van Phong)
// ============================================================
void drawOfficeBuilding(float w, float h, float d, int floors, int colorIdx) {
  (void)floors;
  // Modern office building colors
  const Color officeColors[] = {
    Palette::GLASS_DARK, Color(0.75f, 0.8f, 0.85f), Color(0.7f, 0.75f, 0.8f),
    Color(0.8f, 0.75f, 0.7f), Color(0.65f, 0.7f, 0.75f)
  };

  // Glass curtain wall typical of modern offices
  officeColors[colorIdx % 5].applyMaterial();

  // Core structure
  glPushMatrix();
  glTranslatef(0, h / 2, 0);
  drawCube(w - 0.2f, h, d - 0.2f);
  glPopMatrix();

  // Glass exterior with varying transparency
  Palette::GLASS_DARK.applyGlossyMaterial(120.0f + (colorIdx % 3) * 10.0f);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glPushMatrix();
  glTranslatef(0, h / 2, 0);
  drawCube(w, h, d);
  glPopMatrix();
  glDisable(GL_BLEND);

  // Mullions (Grid lines) with variation
  Palette::METAL_GRAY.applyMaterial();

  float glassGridW = 2.0f + (colorIdx % 2);
  float glassGridH = FLOOR_HEIGHT;

  // Front grid
  glPushMatrix();
  glTranslatef(0, 0, d / 2 + 0.02f);
  for (float y = 0; y <= h; y += glassGridH) {
    glPushMatrix();
    glTranslatef(0, y, 0);
    drawCube(w, 0.1f, 0.05f);
    glPopMatrix();
  }
  for (float x = -w / 2; x <= w / 2; x += glassGridW) {
    glPushMatrix();
    glTranslatef(x, h / 2, 0);
    drawCube(0.1f, h, 0.05f);
    glPopMatrix();
  }
  glPopMatrix();

  // Ground floor entrance with style variation
  Palette::METAL_DARK.applyMaterial();
  glPushMatrix();
  glTranslatef(0, 2.0f, d / 2 + 0.05f);
  drawCube(w * (0.5f + (colorIdx % 2) * 0.1f), 0.3f, 3.0f);
  glPopMatrix();

  // Large glass doors
  glPushMatrix();
  glTranslatef(0, 1.5f, d / 2 + 0.02f);
  drawWindow(w * 0.5f, 3.0f, true);
  // Entrance Glow
  if (g_timeOfDay < 6.0f || g_timeOfDay > 18.0f) {
      glTranslatef(0, 0, 0.1f);
      drawGlowBillboard(4.0f, Color(0.8f, 0.9f, 1.0f));
  }
  glPopMatrix();

  // GIANT NEON SIGN ON TOP
  glPushMatrix();
  glTranslatef(0, h + 1.5f, d / 2 - 0.5f);
  drawNeonSign(w * 0.7f, 2.5f, Palette::NEON_CYAN, "TECH CENTER");
  glPopMatrix();

  // Rooftop elements - AC units with variation
  glPushMatrix();
  glTranslatef(0, h + 0.5f, 0);
  if (colorIdx % 3 == 0) {
    Palette::METAL_GRAY.applyMaterial();
    drawCube(2.0f, 1.0f, 2.0f);
  } else if (colorIdx % 3 == 1) {
    Palette::METAL_DARK.applyMaterial();
    drawCylinder(0.4f, 1.5f, 16);
  } else {
    Palette::METAL_GRAY.applyMaterial();
    drawCube(1.5f, 0.8f, 1.5f);
    glPushMatrix();
    glTranslatef(0.8f, 0.4f, 0);
    Palette::METAL_DARK.applyMaterial();
    drawCylinder(0.2f, 0.6f, 12);
    glPopMatrix();
  }
  glPopMatrix();
}

// ============================================================
// SHOP (Cua Hang Nho) 1-2 tang
// ============================================================
void drawShop(float w, float h, float d, int colorIdx,
              const std::string &shopName) {
  const Color shopColors[] = {
      Palette::WALL_YELLOW, Palette::WALL_CREAM, Palette::WALL_PINK,
      Palette::WALL_BLUE, Palette::WALL_GREEN, Palette::WALL_WHITE,
      Palette::WALL_ORANGE, Palette::WALL_LAVENDER,
      Color(0.95f, 0.6f, 0.6f),   // Coral
      Color(0.6f, 0.9f, 0.7f),    // Mint
      Color(0.9f, 0.85f, 0.6f),   // Gold
      Color(0.85f, 0.7f, 0.95f)   // Lilac
  };
  Color baseCol = shopColors[colorIdx % 12];
  baseCol.applyMaterial();

  glPushMatrix();
  glTranslatef(0, h / 2, 0);
  drawCube(w, h, d);
  glPopMatrix();

  // Enhanced shop front
  drawShopFront(w, FLOOR_HEIGHT, d, shopName, colorIdx);

  // Decorative stripe on shop
  glPushMatrix();
  glTranslatef(0, FLOOR_HEIGHT * 0.7f, d / 2 + 0.02f);
  Color stripeCol = shopColors[(colorIdx + 3) % 12];
  stripeCol.applyMaterial();
  drawCube(w, 0.15f, 0.05f);
  glPopMatrix();

  // Flat roof with style
  glPushMatrix();
  glTranslatef(0, h, 0);
  drawFlatRoof(w, d);

  // Small rooftop sign/decoration
  if (colorIdx % 2 == 0) {
    Palette::METAL_DARK.applyMaterial();
    glPushMatrix();
    glTranslatef(0, 0.2f, 0);
    drawCube(w * 0.6f, 0.3f, 0.1f);
    glPopMatrix();
  }
  glPopMatrix();
}

// ============================================================
// CAFE / RESTAURANT
// ============================================================
void drawCafe(float w, float h, float d, int colorIdx,
              const std::string &shopName) {
  (void)colorIdx;
  Color baseCol = Color(0.9f, 0.85f, 0.75f); // Warm cafe colors
  baseCol.applyMaterial();

  glPushMatrix();
  glTranslatef(0, h / 2, 0);
  drawCubeTextured(w, h, d); // Assume Brick or painted brick
  glPopMatrix();

  // 1. Phào chỉ trang trí (Moldings/Cornices)
  Palette::CONCRETE.applyMaterial();
  glPushMatrix();
  glTranslatef(0, FLOOR_HEIGHT - 0.05f, d / 2 + 0.1f);
  drawCube(w + 0.2f, 0.1f, 0.2f); // Floor divider moulding
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0, h - 0.05f, d / 2 + 0.15f);
  drawCube(w + 0.4f, 0.15f, 0.3f); // Roof cornice
  glPopMatrix();

  // Open front (folding doors)
  Palette::WOOD_DARK.applyMaterial();
  glPushMatrix();
  glTranslatef(0, FLOOR_HEIGHT / 2, d / 2 + 0.02f);
  drawWireFrame(w - 1.0f, FLOOR_HEIGHT - 0.5f, 0.1f, 0.1f);
  glPopMatrix();

  // Large Awning over sidewalk area
  glPushMatrix();
  glTranslatef(0, FLOOR_HEIGHT - 0.2f, d / 2);
  drawAwning(w + 1.0f, 1.2f, Color(0.15f, 0.35f, 0.15f)); // reduced from 3.5f
  glPopMatrix();

  // Sign (Neon for shop name)
  glPushMatrix();
  glTranslatef(0, FLOOR_HEIGHT + 0.5f, d / 2 + 0.2f);
  drawNeonSign(w * 0.9f, 0.8f, Palette::NEON_PINK, shopName.c_str());
  glPopMatrix();
}

// ============================================================
// OLD APARTMENT (Chung Cu Cu / Tap The)
// ============================================================
void drawOldApartment(float w, float h, float d, int floors, int colorIdx) {
  // Varied apartment colors
  const Color apartmentColors[] = {
    Palette::WALL_YELLOW, Color(0.92f, 0.88f, 0.75f), Color(0.95f, 0.88f, 0.8f),
    Color(0.88f, 0.92f, 0.85f), Color(0.9f, 0.85f, 0.75f)
  };
  apartmentColors[colorIdx % 5].applyMaterial();

  glPushMatrix();
  glTranslatef(0, h / 2, 0);
  drawCube(w, h, d);
  glPopMatrix();

  // Corridor style
  float corridorDepth = 1.5f;

  for (int i = 0; i < floors; i++) {
    float floorY = i * FLOOR_HEIGHT;

    // Push corridor inwards with floor line
    Palette::WALL_CREAM.applyMaterial();
    glPushMatrix();
    glTranslatef(0, floorY + FLOOR_HEIGHT / 2, d / 2 - corridorDepth / 2);
    drawCube(w - 0.4f, FLOOR_HEIGHT - 0.2f, corridorDepth);
    glPopMatrix();

    // Floor divider line for visual interest
    if (i > 0) {
      Palette::CONCRETE.applyMaterial();
      glPushMatrix();
      glTranslatef(0, floorY + 0.02f, d / 2 - corridorDepth / 2);
      drawCube(w - 0.4f, 0.05f, corridorDepth);
      glPopMatrix();
    }

    // Corridor railing & pillars
    Palette::CONCRETE.applyMaterial();
    glPushMatrix();
    glTranslatef(0, floorY, d / 2);
    drawRailing(w, 1.0f, (int)(w / 1.5f) + 1);
    glPopMatrix();

    // Support pillars
    for (float x = -w / 2; x <= w / 2 + 0.1f; x += 4.0f) {
      glPushMatrix();
      glTranslatef(x, floorY + FLOOR_HEIGHT / 2, d / 2);
      drawCube(0.3f, FLOOR_HEIGHT, 0.3f);
      glPopMatrix();
    }

    // Window/door elements on corridor
    if (i > 0 && w > 3.0f) {
      for (float x = -w / 2 + 1.0f; x <= w / 2 - 1.0f; x += w / 2.5f) {
        glPushMatrix();
        glTranslatef(x, floorY + FLOOR_HEIGHT * 0.65f, d / 2 - corridorDepth / 2 + 0.01f);
        drawWindow(0.8f, 1.0f, false, false);
        glPopMatrix();
      }
    }
  }

  // Add random details on balconies (cages, clothes) typical in VN
  for (int i = 0; i < floors; i++) {
    float floorY = i * FLOOR_HEIGHT;
    int cageSpace = (int)(w - 4.0f);
    if (i > 0 && cageSpace > 0 && (i * 37 + colorIdx) % 3 == 0) {
      // "Chuong cop" (Tiger cage - iron bars expanding balcony)
      Palette::IRON_BLACK.applyMaterial();
      glPushMatrix();
      float cageX = -w / 2 + 2.0f + ((colorIdx + i * 13) % cageSpace);
      glTranslatef(cageX, floorY + FLOOR_HEIGHT / 2, d / 2 + 0.35f);
      drawWireFrame(3.5f, FLOOR_HEIGHT - 0.2f, 0.7f, 0.02f); // Thin bars
      glPopMatrix();

      // Enhanced laundry with varied colors
      if ((i + colorIdx) % 2 == 0) {
        for (int l = 0; l < 3; l++) {
          glPushMatrix();
          glTranslatef(cageX + (l - 1) * 0.6f,
                       floorY + FLOOR_HEIGHT * 0.6f, d / 2 + 0.2f);
          // More clothing color variety
          int shirtIdx = (l + i + colorIdx) % 8;
          if (l % 2 == 0) {
            getShirtColor(shirtIdx).applyMaterial();
          } else {
            getShirtColor((shirtIdx + 4) % 8).applyMaterial();
          }
          drawQuad(0.45f, 0.55f);
          glPopMatrix();

          // Add hanging ropes
          Palette::CONCRETE.applyMaterial();
          glPushMatrix();
          glTranslatef(cageX + (l - 1) * 0.6f - 0.25f,
                       floorY + FLOOR_HEIGHT * 0.65f, d / 2 + 0.22f);
          glLineWidth(1.5f);
          glBegin(GL_LINES);
          glVertex3f(0, 0.35f, 0);
          glVertex3f(0, -0.35f, 0);
          glEnd();
          glLineWidth(1.0f);
          glPopMatrix();
        }
      }

      // Add variation - sometimes add plant pot
      if ((i * 13 + colorIdx) % 5 == 0) {
        glPushMatrix();
        glTranslatef(cageX - 1.5f, floorY + FLOOR_HEIGHT * 0.3f, d / 2 + 0.3f);
        Color potColor = getShirtColor((i + colorIdx * 3) % 8);
        potColor.applyMaterial();
        drawCube(0.35f, 0.4f, 0.35f);
        glPopMatrix();
      }
    }
  }

  // Flat roof with enhanced details
  glPushMatrix();
  glTranslatef(0, h, 0);
  drawFlatRoof(w, d);

  // Rooftop antennas and clutter with variation
  for (int j = 0; j < 3; j++) {
    if ((j + colorIdx) % 3 == 0) {
      glPushMatrix();
      glTranslatef((j - 1) * w / 3.0f, 1.0f, (j - 1) * d / 3.0f);
      if ((j * colorIdx) % 4 == 0) {
        Palette::METAL_DARK.applyMaterial();
        drawCylinder(0.04f, 3.0f, 4);
      } else if ((j * colorIdx) % 4 == 1) {
        Palette::METAL_GRAY.applyMaterial();
        drawCylinder(0.06f, 2.5f, 6);
      } else {
        Palette::METAL_DARK.applyMaterial();
        drawCube(0.2f, 2.5f, 0.2f);
      }
      glPopMatrix();
    }
  }

  // Water tank on roof if space available
  if (w > 4.0f && (colorIdx % 4) < 2) {
    glPushMatrix();
    glTranslatef(-w / 3, 0.3f, d / 4);
    drawWaterTank(0.25f, 0.8f);
    glPopMatrix();
  }

  glPopMatrix();
}

// ============================================================
// MODERN GLASS TOWER (Toa Thap Kinh)
// ============================================================
void drawGlassTower(float w, float h, float d, int floors, int colorIdx) {
  // Vibrant modern glass colors
  const Color glassColors[] = {
    Color(0.2f, 0.5f, 0.9f, 0.6f),  // Electric Blue
    Color(0.1f, 0.8f, 0.7f, 0.5f),  // Cyan/Teal
    Color(0.4f, 0.4f, 0.45f, 0.8f), // Smoke Gray
    Color(0.1f, 0.1f, 0.15f, 0.9f)  // Obsidian
  };
  Color glassCol = glassColors[colorIdx % 4];

  // 1. Core Structure (Concrete inner)
  Palette::CONCRETE.applyMaterial();
  glPushMatrix();
  glTranslatef(0, h / 2, 0);
  drawCube(w * 0.85f, h, d * 0.85f);
  glPopMatrix();

  // 2. Interior "Proxy" Floors
  Palette::WALL_WHITE.applyMaterial();
  for (int i = 0; i < floors; i++) {
    float fy = i * (h / floors);
    glPushMatrix();
    glTranslatef(0, fy + 0.05f, 0);
    drawCube(w * 0.98f, 0.1f, d * 0.98f);
    
    // Add some random "office furniture" boxes inside
    if (i % 2 == 0) {
      for (int j = 0; j < 4; j++) {
        float ox = (j % 2 == 0 ? 1 : -1) * w * 0.3f;
        float oz = (j / 2 == 0 ? 1 : -1) * d * 0.3f;
        glPushMatrix();
        glTranslatef(ox, 0.4f, oz);
        Color deskCol = (j % 2 == 0) ? Palette::WOOD_DARK : Palette::METAL_GRAY;
        deskCol.applyMaterial();
        drawCube(0.8f, 0.6f, 1.2f);
        glPopMatrix();
      }
    }
    glPopMatrix();
  }

  // 3. Glass Curtain Wall (Outer)
  glassCol.applyGlossyMaterial(128.0f);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glPushMatrix();
  glTranslatef(0, h / 2, 0);
  drawCube(w, h, d);
  glPopMatrix();
  glDisable(GL_BLEND);

  // 4. External Structural Grid (Mullions)
  Palette::METAL_DARK.applyMaterial();
  float gridW = w / 4.0f;
  float gridH = h / floors;
  
  for (float x = -w/2; x <= w/2 + 0.01f; x += gridW) {
    glPushMatrix();
    glTranslatef(x, h / 2, 0);
    drawCube(0.08f, h, d + 0.02f);
    glPopMatrix();
  }
  for (float y = 0; y <= h + 0.01f; y += gridH) {
    glPushMatrix();
    glTranslatef(0, y, 0);
    drawCube(w + 0.02f, 0.08f, d + 0.02f);
    glPopMatrix();
  }

  // 5. Rooftop Helipad & Antennas
  glPushMatrix();
  glTranslatef(0, h, 0);
  drawFlatRoof(w, d);
  Palette::METAL_LIGHT.applyMaterial();
  glPushMatrix();
  glTranslatef(0, 0.05f, 0);
  drawDisk(w * 0.3f, w * 0.35f, 24);
  drawCube(0.2f, 0.01f, w * 0.4f);
  glPushMatrix(); glTranslatef(w * 0.3f, 0, 0); drawCube(0.2f, 0.01f, w * 0.4f); glPopMatrix();
  glPushMatrix(); glTranslatef(-w * 0.3f, 0, 0); drawCube(0.2f, 0.01f, w * 0.4f); glPopMatrix();
  drawCube(w * 0.6f, 0.01f, 0.2f);
  glPopMatrix();
  glPopMatrix();
}

// ============================================================
// SKYSCRAPER (Toa Nha Sieu Cao)
// ============================================================
void drawSkyscraper(float w, float h, float d, int colorIdx) {
  float currentH = 0;
  float currentW = w;
  float currentD = d;
  int tiers = 3 + (colorIdx % 2);
  float tierH = h / tiers;

  for (int i = 0; i < tiers; i++) {
    float scale = 1.0f - i * 0.2f;
    float tw = currentW * scale;
    float td = currentD * scale;
    glPushMatrix();
    glTranslatef(0, currentH + tierH / 2, 0);
    if (i % 2 == 0) Palette::METAL_GRAY.applyMaterial();
    else Palette::CONCRETE.applyMaterial();
    drawCube(tw, tierH, td);
    Palette::GLASS_BLUE.applyGlossyMaterial();
    glEnable(GL_BLEND);
    glPushMatrix(); glTranslatef(0, 0, td/2 + 0.01f); drawQuad(tw * 0.9f, tierH * 0.8f); glPopMatrix();
    glDisable(GL_BLEND);
    glPopMatrix();
    if (i < tiers - 1) {
      Palette::METAL_DARK.applyMaterial();
      glPushMatrix(); glTranslatef(0, currentH + tierH, 0); drawCube(tw + 0.2f, 0.2f, td + 0.2f); glPopMatrix();
    }
    currentH += tierH;
  }
}

// ============================================================
// MARKET / MALL (Cho / Trung Tam Thuong Mai)
// ============================================================
void drawMarketMall(float w, float h, float d, int colorIdx, const std::string& name) {
    // Modern mall look: mix of glass and decorative panels
    Color panelCol = (colorIdx % 2 == 0) ? Color(0.9f, 0.9f, 0.95f) : Color(0.2f, 0.2f, 0.25f);
    
    // 1. Base structure
    panelCol.applyMaterial();
    glPushMatrix();
    glTranslatef(0, h/2, 0);
    drawCube(w, h, d);
    glPopMatrix();

    // 2. Large Glass Atrium at front
    Palette::GLASS_BLUE.applyGlossyMaterial(120.0f);
    glEnable(GL_BLEND);
    glPushMatrix();
    glTranslatef(0, h * 0.55f, d/2 + 0.1f);
    drawCube(w * 0.7f, h * 0.7f, 0.5f);
    glPopMatrix();
    glDisable(GL_BLEND);

    // 3. Decorative Frame around atrium
    Palette::METAL_CHROME.applyGlossyMaterial();
    glPushMatrix();
    glTranslatef(0, h * 0.55f, d/2 + 0.15f);
    drawWireFrame(w * 0.72f, h * 0.72f, 0.6f, 0.15f);
    glPopMatrix();

    // 4. Advertising LED Screen
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(-w/3, h * 0.7f, d/2 + 0.12f);
    // Glow effect
    if (fmodf(g_elapsedTime, 2.0f) < 1.0f) glColor3f(1.0f, 0.1f, 0.1f);
    else glColor3f(0.1f, 0.1f, 1.0f);
    drawCube(w * 0.25f, h * 0.3f, 0.05f);
    
    // Screen Text
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText3D(-1.0f, 0, 0.03f, "SALE 50%");
    glPopMatrix();
    glEnable(GL_LIGHTING);

    // 5. Entrance Doors (Multiple)
    for (float x = -w/4; x <= w/4; x += w/4) {
        glPushMatrix();
        glTranslatef(x, 1.25f, d/2 + 0.1f);
        drawWindow(1.8f, 2.5f, true);
        glPopMatrix();
    }

    // Mall Name Neon
    glPushMatrix();
    glTranslatef(0, h - 1.5f, d/2 + 0.2f);
    drawNeonSign(w * 0.5f, 1.2f, Palette::NEON_RED, name.c_str());
    glPopMatrix();
}

// ============================================================
// MODERN VILLA (Biet Thu Hien Dai)
// ============================================================
void drawModernVilla(float w, float h, float d, int colorIdx) {
    Color starkWhite(0.95f, 0.95f, 0.95f);
    Color accentCol = getBuildingColor(colorIdx); // Rich accent color
    Color woodAccent(0.4f, 0.25f, 0.15f);
    Color stoneGray(0.5f, 0.5f, 0.52f);

    // Asymmetric block design
    // Block 1: Ground floor (Centered)
    starkWhite.applyMaterial();
    glPushMatrix();
    glTranslatef(0, h*0.25f, 0); // Removed -w*0.1f offset
    drawCube(w*0.8f, h*0.5f, d);
    glPopMatrix();

    // Block 2: Overhanging second floor (Centered)
    glPushMatrix();
    glTranslatef(0, h*0.75f, 0); // Removed offset
    starkWhite.applyMaterial();
    drawCube(w*0.8f, h*0.5f, d*0.8f);
    
    // Wood panel accent
    woodAccent.applyMaterial();
    glPushMatrix();
    glTranslatef(w*0.4f, 0, 0);
    drawCube(0.1f, h*0.4f, d*0.6f);
    glPopMatrix();
    glPopMatrix();

    // Large floor-to-ceiling windows (Centered)
    Palette::GLASS_DARK.applyGlossyMaterial();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPushMatrix();
    glTranslatef(0, h*0.25f, d/2 + 0.02f);
    drawQuad(w*0.6f, h*0.4f);
    glPopMatrix();
    
    // Modern Sliding glass doors on second floor (Centered)
    glPushMatrix();
    glTranslatef(0, h*0.75f, d/2 + 0.02f);
    drawQuad(w*0.7f, h*0.4f);
    glPopMatrix();
    glDisable(GL_BLEND);

    // Glass Railing for the overhanging balcony
    glEnable(GL_BLEND);
    Palette::GLASS_BLUE.applyMaterial();
    glPushMatrix();
    glTranslatef(w*0.1f, h*0.6f, d/2 + 0.1f);
    drawCube(w*0.8f, 1.0f, 0.05f);
    glPopMatrix();
    glDisable(GL_BLEND);

    // Swimming pool (Optional detail)
    if (colorIdx % 2 == 0) {
        Palette::GLASS_BLUE.applyGlossyMaterial();
        glEnable(GL_BLEND);
        glPushMatrix();
        glTranslatef(w*0.6f, 0.05f, 0);
        drawCube(w*0.5f, 0.15f, d*0.7f);
        glPopMatrix();
        glDisable(GL_BLEND);
    }
}

// ============================================================
// ENHANCED TRADITIONAL TEMPLE (Chua Viet Nam)
// ============================================================
void drawTraditionalTemple(float w, float h, float d, int colorIdx) {
  // Foundation (Concrete/Stone)
  Palette::CONCRETE.applyMaterial();
  glPushMatrix();
  glTranslatef(0, 0.25f, 0);
  drawCube(w + 2.0f, 0.5f, d + 2.0f);
  glPopMatrix();

  // Main columns (Dark Wood/Red Lacquer)
  Color templeRed(0.6f, 0.1f, 0.1f);
  templeRed.applyMaterial();
  float colSpacingW = w * 0.4f;
  float colSpacingD = d * 0.4f;
  for (float x = -colSpacingW; x <= colSpacingW + 0.01f; x += colSpacingW) {
    for (float z = -colSpacingD; z <= colSpacingD + 0.01f; z += colSpacingD) {
      glPushMatrix(); 
      glTranslatef(x, h/2 + 0.5f, z); 
      drawCylinder(0.25f, h, 16); // Thicker columns
      glPopMatrix();
    }
  }

  // Sanctuary Walls (Red)
  templeRed.applyMaterial();
  glPushMatrix();
  glTranslatef(0, h/2 + 0.5f, 0);
  drawCube(w * 0.8f, h * 0.9f, d * 0.8f);
  glPopMatrix();

  // Multiple Layered Roofs (Mái Chồng Diêm)
  int layers = 2 + (colorIdx % 2);
  for (int layer = 0; layer < layers; layer++) {
    float ly = h + 0.5f + layer * 1.5f;
    float lw = w * (1.3f - layer * 0.25f);
    float ld = d * (1.3f - layer * 0.25f);
    
    // Roof slope
    glPushMatrix(); 
    glTranslatef(0, ly, 0); 
    drawTileRoof(lw, ld, 35.0f); 
    
    // Dragon/Cloud ornaments on corners (Dragons of VN)
    Palette::METAL_CHROME.applyGlossyMaterial(100.0f); // Gold look
    for (int c = 0; c < 4; c++) {
      float cx = (c%2==0 ? 1 : -1) * lw/2;
      float cz = (c/2==0 ? 1 : -1) * ld/2;
      glPushMatrix(); 
      glTranslatef(cx, 0.2f, cz); 
      glRotatef((float)(45 + c * 90), 0, 1, 0); 
      // Curved "dragon tail" ornament
      for(int segment=0; segment<5; segment++) {
          glTranslatef(0, 0.1f, 0.1f);
          glRotatef(-15, 1, 0, 0);
          drawCylinder(0.12f - segment*0.02f, 0.3f, 8);
      }
      glPopMatrix();
    }

    // Roof ridge ornament (Two dragons facing the moon)
    glPushMatrix();
    glTranslatef(0, 0.8f, 0);
    drawDisk(0.15f, 0.4f, 16); // The "Moon" (Gold disk)
    glPopMatrix();

    glPopMatrix();
  }

  // Incense burner at front
  Palette::METAL_DARK.applyMaterial();
  glPushMatrix();
  glTranslatef(0, 0.8f, d/2 + 1.2f);
  drawCylinder(0.4f, 0.6f, 12);
  // Sticks
  Palette::NEON_YELLOW.applyMaterial();
  for(int s=0; s<3; s++) {
      glPushMatrix();
      glTranslatef((s-1)*0.1f, 0.6f, 0);
      glRotatef((float)(10 * (s - 1)), 0, 0, 1);
      drawCylinder(0.02f, 0.5f, 4);
      glPopMatrix();
  }
  glPopMatrix();
}

// ============================================================
// MAIN DISPATCHER
// ============================================================
void drawBuilding(const BuildingInfo &info, bool isShadow) {
  glPushMatrix();
  info.transform.applyGL();

  // If it's a shadow pass, we can skip complex logic and just draw the building
  // but for the best shadow shape, we let the specific functions run.
  // The glDisable(GL_LIGHTING) and color overrides in main.cpp will handle the rest.

  switch (info.type) {
  case BLDG_TUBE_HOUSE:
    drawTubeHouse(info.width, info.height, info.depth, info.floors,
                  info.colorVariant, info.hasShopFront, info.hasBalcony,
                  info.shopName);
    break;
  case BLDG_OFFICE:
    drawOfficeBuilding(info.width, info.height, info.depth, info.floors,
                       info.colorVariant);
    break;
  case BLDG_SHOP:
    drawShop(info.width, info.height, info.depth, info.colorVariant,
             info.shopName);
    break;
  case BLDG_CAFE:
    drawCafe(info.width, info.height, info.depth, info.colorVariant,
             info.shopName);
    break;
  case BLDG_APARTMENT:
    drawOldApartment(info.width, info.height, info.depth, info.floors,
                     info.colorVariant);
    break;
  case BLDG_COLONIAL:
    drawColonialBuilding(info.width, info.height, info.depth, info.colorVariant);
    break;
  case BLDG_SHOPHOUSE:
    drawShophouse(info.width, info.height, info.depth, info.floors,
                  info.colorVariant, info.shopName);
    break;
  case BLDG_APARTMENT_BLOCK:
    drawApartmentBlock(info.width, info.height, info.depth, info.floors,
                       info.colorVariant);
    break;
  case BLDG_MODERN_SHOPFRONT:
    drawModernShopfront(info.width, info.height, info.depth, info.colorVariant,
                        info.shopName);
    break;
  case BLDG_MARKET:
    drawMarketMall(info.width, info.height, info.depth, info.colorVariant, info.shopName);
    break;
  case BLDG_TEMPLE:
    drawTraditionalTemple(info.width, info.height, info.depth, info.colorVariant);
    break;
  case BLDG_GARAGE:
    drawGarageBuilding(info.width, info.height, info.depth, info.colorVariant);
    break;
  case BLDG_SKYSCRAPER:
    drawSkyscraper(info.width, info.height, info.depth, info.colorVariant);
    break;
  case BLDG_GLASS_TOWER:
    drawGlassTower(info.width, info.height, info.depth, info.floors, info.colorVariant);
    break;
  case BLDG_MODERN_VILLA:
    drawModernVilla(info.width, info.height, info.depth, info.colorVariant);
    break;
  case BLDG_MALL:
    drawMarketMall(info.width, info.height, info.depth, info.colorVariant, info.shopName);
    break;
  default:
    if (!isShadow) getBuildingColor(info.colorVariant).applyMaterial();
    glPushMatrix();
    glTranslatef(0, info.height / 2.0f, 0);
    drawCube(info.width, info.height, info.depth);
    glPopMatrix();
    break;
  }

  glPopMatrix();
}
