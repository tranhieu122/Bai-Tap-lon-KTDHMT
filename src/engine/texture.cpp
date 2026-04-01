#include "texture.h"
#include "utils.h"

namespace {
    GLuint glTextures[TEX_COUNT];
    
    // Texture generators (CPU side image building)
    
    void genAsphalt(unsigned char* data, int w, int h) {
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                // High frequency noise
                float n = noise2D(x * 0.5f, y * 0.5f) * 0.5f + 0.5f;
                // Add some macro variations
                float m = noise2D(x * 0.02f, y * 0.02f);
                
                float v = 0.35f + 0.15f * n + 0.05f * m; // Brighter gray asphalt
                
                int idx = (y * w + x) * 3;
                data[idx] = data[idx+1] = data[idx+2] = (unsigned char)(clamp(v, 0.0f, 1.0f) * 255.0f);
            }
        }
    }
    
    void genSidewalk(unsigned char* data, int w, int h) {
        int tileSize = w / 8; // 8x8 tiles
        
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                int tx = x % tileSize;
                int ty = y % tileSize;
                int gridX = x / tileSize;
                int gridY = y / tileSize;
                
                // Mortar lines
                bool isMortar = (tx < 2 || ty < 2);
                
                float r, g, b;
                if (isMortar) {
                    r = g = b = 0.65f; // Brighter mortar
                } else {
                    // Slight color variation per tile based on tile grid index
                    float tv = noise2D(gridX * 10.0f, gridY * 10.0f) * 0.2f;
                    
                    // Internal tile noise
                    float in = noise2D(x * 0.1f, y * 0.1f) * 0.05f;
                    
                    // Reddish/brownish Hanoi sidewalk tiles - more saturated
                    r = 0.85f + tv + in;
                    g = 0.5f + tv * 0.5f + in;
                    b = 0.4f + tv * 0.3f + in;
                }
                
                int idx = (y * w + x) * 3;
                data[idx] = (unsigned char)(clamp(r, 0.0f, 1.0f) * 255.0f);
                data[idx+1] = (unsigned char)(clamp(g, 0.0f, 1.0f) * 255.0f);
                data[idx+2] = (unsigned char)(clamp(b, 0.0f, 1.0f) * 255.0f);
            }
        }
    }
    
    void genBrickWall(unsigned char* data, int w, int h) {
        int rows = 16;
        int brickH = h / rows;
        int brickW = brickH * 3;
        
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                int r = y / brickH;
                int offsetX = (r % 2 == 0) ? 0 : brickW / 2;
                int shiftedX = (x + offsetX);
                int c = shiftedX / brickW;
                
                int bx = shiftedX % brickW;
                int by = y % brickH;
                
                bool isMortar = (bx < 2 || by < 2);
                
                float colR, colG, colB;
                if (isMortar) {
                    colR = colG = colB = 0.6f;
                } else {
                    float bv = noise2D(c * 15.2f, r * 31.7f) * 0.2f; // Brick variance
                    float in = noise2D(x * 0.2f, y * 0.2f) * 0.05f; // Internal grit
                    
                    colR = 0.7f + bv + in;
                    colG = 0.35f + bv * 0.5f + in;
                    colB = 0.25f + bv * 0.3f + in;
                }
                
                int idx = (y * w + x) * 3;
                data[idx] = (unsigned char)(clamp(colR, 0.0f, 1.0f) * 255.0f);
                data[idx+1] = (unsigned char)(clamp(colG, 0.0f, 1.0f) * 255.0f);
                data[idx+2] = (unsigned char)(clamp(colB, 0.0f, 1.0f) * 255.0f);
            }
        }
    }
    
    void genGrass(unsigned char* data, int w, int h) {
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                float baseNoise = noise2D(x * 0.05f, y * 0.05f);
                float fineNoise = noise2D(x * 0.3f, y * 0.3f);
                
                float r = 0.2f + baseNoise * 0.1f + fineNoise * 0.05f;
                float g = 0.5f + baseNoise * 0.2f + fineNoise * 0.1f;
                float b = 0.2f + baseNoise * 0.1f;
                
                int idx = (y * w + x) * 3;
                data[idx] = (unsigned char)(clamp(r, 0.0f, 1.0f) * 255.0f);
                data[idx+1] = (unsigned char)(clamp(g, 0.0f, 1.0f) * 255.0f);
                data[idx+2] = (unsigned char)(clamp(b, 0.0f, 1.0f) * 255.0f);
            }
        }
    }
    
    void uploadTexture(TextureID id, void (*genFunc)(unsigned char*, int, int)) {
        const int TEX_SIZE = 256;
        unsigned char* data = new unsigned char[TEX_SIZE * TEX_SIZE * 3];
        
        genFunc(data, TEX_SIZE, TEX_SIZE);
        
        glBindTexture(GL_TEXTURE_2D, glTextures[id]);
        
        // Setup params
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        // Upload & build mipmaps
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TEX_SIZE, TEX_SIZE, GL_RGB, GL_UNSIGNED_BYTE, data);
        
        delete[] data;
    }
}

// ============================================================
// INITIALIZATION
// ============================================================
void textureInit() {
    // Generate OpenGL texture IDs
    glGenTextures(TEX_COUNT, glTextures);
    
    // Generate and upload procedural textures
    uploadTexture(TEX_ASPHALT, genAsphalt);
    uploadTexture(TEX_SIDEWALK, genSidewalk);
    uploadTexture(TEX_BRICK_WALL, genBrickWall);
    uploadTexture(TEX_GRASS, genGrass);
    
    // Other simple colors can be just plain untextured drawing, but 
    // we set them up if needed.
    
    textureUnbind();
}

// ============================================================
// BINDING
// ============================================================
void textureBind(TextureID id) {
    if (id == TEX_NONE) {
        glDisable(GL_TEXTURE_2D);
    } else {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, glTextures[id]);
        // Set white color so texture is not tinted (unless intended)
        glColor3f(1.0f, 1.0f, 1.0f);
    }
}

void textureUnbind() {
    glDisable(GL_TEXTURE_2D);
}
