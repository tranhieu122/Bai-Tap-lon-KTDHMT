#pragma once
#include "globals.h"

// ============================================================
// PROCEDURAL TEXTURE SYSTEM
// Generate textures mathematically without loading image files
// ============================================================

enum TextureID {
    TEX_NONE = 0,
    TEX_ASPHALT,        // Mac duong nhua
    TEX_SIDEWALK,       // Gach via he
    TEX_BRICK_WALL,     // Tuong gach do
    TEX_CONCRETE,       // Be tong
    TEX_GRASS,          // Tham co
    TEX_WINDOW_GLASS,   // Cua kinh co reflection fake
    TEX_ROOF_TILE,      // Mai ngoi
    TEX_COUNT
};

// Generate and upload all procedural textures to OpenGL
void textureInit();
void textureBind(TextureID id);
void textureUnbind();
