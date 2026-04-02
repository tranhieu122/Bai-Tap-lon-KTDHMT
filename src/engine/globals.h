#pragma once
#ifndef GLOBALS_H
#define GLOBALS_H

#include <GL/freeglut.h>
#include <cmath>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>

// ============================================================
// MATHEMATICAL CONSTANTS
// ============================================================
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define DEG2RAD(x) ((x) * (float)M_PI / 180.0f)
#define RAD2DEG(x) ((x) * 180.0f / (float)M_PI)

extern int g_peopleEnteredToday; // Counter for shop visits per day

// ============================================================
// WINDOW SETTINGS
// ============================================================
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const char* const WINDOW_TITLE = "Pho Phuong Viet Nam 3D - Mo phong Do hoa C++";

// ============================================================
// WORLD DIMENSIONS & CONSTANTS
// ============================================================
namespace World {
    const float STREET_LENGTH = 120.0f;     // Main street length (Z axis)
    const float STREET_WIDTH = 10.0f;       // Rộng hơn theo yêu cầu
    const float LANE_WIDTH = 4.5f;          
    const float SIDEWALK_WIDTH = 5.5f;      
    const float SIDEWALK_HEIGHT = 0.15f;    
    const float MEDIAN_WIDTH = 0.0f;        // No median
    const float BLOCK_DEPTH = 20.0f;        

    const float SIDE_STREET_LENGTH = 0.0f; // Disabled side street
    const float SIDE_STREET_WIDTH = 0.0f;  

    const float CROSSWALK_WIDTH = 4.0f;
    const float CROSSWALK_STRIPE_W = 0.8f;

    // Intersection position along main street
    const float INTERSECTION_Z = 0.0f;
}

// Keep old names for compatibility or use using namespace
using namespace World;

// ============================================================
// BUILDING DIMENSIONS
// ============================================================
const float MIN_BUILDING_WIDTH = 4.0f;
const float MAX_BUILDING_WIDTH = 8.0f;
const float MIN_BUILDING_HEIGHT = 9.0f;  // 3 floors
const float MAX_BUILDING_HEIGHT = 45.0f; // 15 floors
const float FLOOR_HEIGHT = 3.0f;
const float SHOP_FRONT_HEIGHT = 4.0f;
const float BALCONY_DEPTH = 1.0f;
const float BALCONY_HEIGHT = 1.0f;

// ============================================================
// VEGETATION
// ============================================================
const float TREE_SPACING = 12.5f;    // Cách xa nhau thêm nửa mét
const float TREE_MIN_HEIGHT = 6.0f;
const float TREE_MAX_HEIGHT = 10.0f;

// ============================================================
// VEHICLE DIMENSIONS
// ============================================================
const float MOTORBIKE_LENGTH = 1.8f;
const float MOTORBIKE_WIDTH = 0.6f;
const float MOTORBIKE_HEIGHT = 1.1f;

const float CAR_LENGTH = 4.2f;
const float CAR_WIDTH = 1.8f;
const float CAR_HEIGHT = 1.5f;

const float BUS_LENGTH = 10.0f;
const float BUS_WIDTH = 2.4f;
const float BUS_HEIGHT = 3.0f;

const float TRUCK_LENGTH = 6.0f;
const float TRUCK_WIDTH = 2.0f;
const float TRUCK_HEIGHT = 2.5f;

const float BICYCLE_LENGTH = 1.6f;
const float BICYCLE_WIDTH = 0.4f;
const float BICYCLE_HEIGHT = 1.0f;

// ============================================================
// PEOPLE DIMENSIONS
// ============================================================
const float PERSON_HEIGHT = 1.7f;
const float PERSON_WIDTH = 0.4f;

// ============================================================
// ANIMATION SPEEDS
// ============================================================
const float DEFAULT_ANIM_SPEED = 1.0f;
const float VEHICLE_SPEED = 4.5f;       // Reduced for realism and safety
const float PEDESTRIAN_SPEED = 1.5f;    // units/sec
const float DAY_CYCLE_DURATION = 600.0f; // seconds for full day (Slowed down)
const float CLOUD_SPEED = 0.5f;

// Traffic light timing (seconds)
const float GREEN_DURATION = 30.0f;
const float YELLOW_DURATION = 3.0f;
const float RED_DURATION = 33.0f;

// ============================================================
// LIGHTING
// ============================================================
const float LAMP_HEIGHT = 5.5f;
const float LAMP_SPACING = 15.0f;
const int   MAX_LIGHTS = 8;  // OpenGL fixed pipeline limit

// ============================================================
// PARTICLE SYSTEM
// ============================================================
const int MAX_RAIN_PARTICLES = 5000;
const int MAX_SMOKE_PARTICLES = 200;
const int MAX_SPARKLE_PARTICLES = 100;

// ============================================================
// ENUMS
// ============================================================
enum CameraMode {
    CAMERA_ORBIT = 0,
    CAMERA_FPS,
    CAMERA_FLY,
    CAMERA_AUTO_TOUR,
    CAMERA_MODE_COUNT
};

enum TimeOfDay {
    TIME_DAWN = 0,
    TIME_MORNING,
    TIME_NOON,
    TIME_AFTERNOON,
    TIME_SUNSET,
    TIME_DUSK,
    TIME_NIGHT,
    TIME_LATE_NIGHT
};

enum WeatherType {
    WEATHER_CLEAR = 0,
    WEATHER_RAIN,
    WEATHER_FOG,
    WEATHER_TYPE_COUNT
};

enum BuildingType {
    BLDG_TUBE_HOUSE = 0,      // Nha ong - Vietnamese tube house
    BLDG_OFFICE,              // Van phong - Modern office
    BLDG_SHOP,                // Cua hang - Small shop
    BLDG_CAFE,                // Quan cafe - Cafe/restaurant
    BLDG_APARTMENT,           // Chung cu cu - Old apartment
    BLDG_TEMPLE,              // Chua - Temple/cultural
    BLDG_MALL,                // Sieu thi - Shopping mall
    BLDG_RESIDENTIAL,         // Nha dan - Residential
    BLDG_COLONIAL,            // Toa nha Phap - Colonial building
    BLDG_SHOPHOUSE,           // Nha pho hang - Shophouse multi-floor
    BLDG_APARTMENT_BLOCK,     // Chung cu nhan tao - Modern apartment block
    BLDG_MODERN_SHOPFRONT,    // Cua hang hien dai - Modern shopfront
    BLDG_MARKET,              // Toa nha cho - Market building
    BLDG_GARAGE,              // Sua xe - Garage/workshop
    BLDG_SKYSCRAPER,          // Toa nha cao toc - Skyscraper
    BLDG_GLASS_TOWER,         // Toa thap kinh - Modern glass tower
    BLDG_MODERN_VILLA,        // Biet thu hien dai - Modern villa
    BLDG_TYPE_COUNT
};

enum VehicleType {
    VEH_MOTORBIKE = 0,
    VEH_CAR_SEDAN,
    VEH_CAR_SUV,
    VEH_BUS,
    VEH_TRUCKS_OPEN,       // Xe tai cho hang ho
    VEH_TRUCKS_CONTAINER,  // Xe tai container
    VEH_AMBULANCE,         // Xe cuu thuong
    VEH_POLICE,            // Xe canh sat
    VEH_TAXI_YELLOW,       // Taxi vang
    VEH_TAXI_GREEN,        // Taxi xanh (Mai Linh)
    VEH_TRICYCLE,          // Xe ba banh (Xich lo/Xe loi)
    VEH_BICYCLE,           // Xe dap
    VEH_TYPE_COUNT
};

enum TrafficLightState {
    TL_OFF = -1,
    TL_GREEN = 0,
    TL_YELLOW,
    TL_RED
};

enum PedestrianState {
    PED_STANDING = 0,
    PED_WALKING,
    PED_SITTING,
    PED_VENDOR,          // Nguoi ban hang rong
    PED_ENTERING_BUILDING,
    PED_EXITING_BUILDING
};

// ============================================================
// STRUCTS
// ============================================================
struct Vector2 {
    float x, y;
    Vector2() : x(0), y(0) {}
    Vector2(float _x, float _y) : x(_x), y(_y) {}
    Vector2 operator+(const Vector2& v) const { return Vector2(x + v.x, y + v.y); }
    Vector2 operator-(const Vector2& v) const { return Vector2(x - v.x, y - v.y); }
    Vector2 operator*(float f) const { return Vector2(x * f, y * f); }
};

struct Vector3 {
    float x, y, z;
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    
    Vector3 operator+(const Vector3& v) const { return Vector3(x+v.x, y+v.y, z+v.z); }
    Vector3 operator-(const Vector3& v) const { return Vector3(x-v.x, y-v.y, z-v.z); }
    Vector3 operator*(float s) const { return Vector3(x*s, y*s, z*s); }
    Vector3 operator/(float s) const { return Vector3(x/s, y/s, z/s); }
    Vector3& operator+=(const Vector3& v) { x+=v.x; y+=v.y; z+=v.z; return *this; }
    Vector3& operator-=(const Vector3& v) { x-=v.x; y-=v.y; z-=v.z; return *this; }
    
    float length() const { return sqrtf(x*x + y*y + z*z); }
    Vector3 normalized() const {
        float len = length();
        if (len < 0.0001f) return Vector3(0,0,0);
        return *this / len;
    }
    float dot(const Vector3& v) const { return x*v.x + y*v.y + z*v.z; }
    Vector3 cross(const Vector3& v) const {
        return Vector3(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
    }
};

struct MeshFace {
    int v[3], n[3], t[3]; // Indices for vertex, normal, texcoord
};

struct Mesh {
    std::vector<Vector3> vertices;
    std::vector<Vector3> normals;
    std::vector<Vector2> texcoords;
    std::vector<MeshFace> faces;
};


struct Color {
    float r, g, b, a;
    Color() : r(1), g(1), b(1), a(1) {}
    Color(float _r, float _g, float _b, float _a = 1.0f) : r(_r), g(_g), b(_b), a(_a) {}
    
    void apply() const { glColor4f(r, g, b, a); }
    void applyMaterial() const {
        glColor4f(r, g, b, a); 
        float amb[] = { r*0.55f, g*0.55f, b*0.55f, a }; // Higher ambient baseline
        float dif[] = { r, g, b, a };
        float spc[] = { 0.4f, 0.4f, 0.4f, a };
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dif);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spc);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 60.0f);
    }
    // Ap dung vat lieu bong loang (kinh, kim loai)
    void applyGlossyMaterial(float shine = 90.0f) const {
        glColor4f(r, g, b, a); // Bat buoc goi khi GL_COLOR_MATERIAL duoc bat
        float amb[] = { r*0.2f, g*0.2f, b*0.2f, a };
        float dif[] = { r, g, b, a };
        float spc[] = { 0.8f, 0.8f, 0.8f, a };
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dif);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spc);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine);
    }
    
    Color lerp(const Color& other, float t) const {
        return Color(
            r + (other.r - r) * t,
            g + (other.g - g) * t,
            b + (other.b - b) * t,
            a + (other.a - a) * t
        );
    }
};

struct BoundingBox {
    Vector3 min, max;
    BoundingBox() {}
    BoundingBox(const Vector3& _min, const Vector3& _max) : min(_min), max(_max) {}
    
    bool contains(const Vector3& p) const {
        return p.x >= min.x && p.x <= max.x &&
               p.y >= min.y && p.y <= max.y &&
               p.z >= min.z && p.z <= max.z;
    }
    
    bool intersects(const BoundingBox& other) const {
        return min.x <= other.max.x && max.x >= other.min.x &&
               min.y <= other.max.y && max.y >= other.min.y &&
               min.z <= other.max.z && max.z >= other.min.z;
    }
    
    Vector3 center() const { return (min + max) * 0.5f; }
    Vector3 size() const { return max - min; }
};

struct Transform {
    Vector3 position;
    Vector3 rotation; // Euler angles in degrees
    Vector3 scale;
    
    Transform() : scale(1,1,1) {}
    Transform(const Vector3& pos) : position(pos), scale(1,1,1) {}
    Transform(const Vector3& pos, const Vector3& rot) : position(pos), rotation(rot), scale(1,1,1) {}
    Transform(const Vector3& pos, const Vector3& rot, const Vector3& scl) : position(pos), rotation(rot), scale(scl) {}
    
    void applyGL() const {
        glTranslatef(position.x, position.y, position.z);
        glRotatef(rotation.y, 0, 1, 0);
        glRotatef(rotation.x, 1, 0, 0);
        glRotatef(rotation.z, 0, 0, 1);
        glScalef(scale.x, scale.y, scale.z);
    }
};

// ============================================================
// BUILDING PLACEMENT DATA
// ============================================================
struct BuildingInfo {
    BuildingType type = BLDG_TUBE_HOUSE;
    Transform transform = Transform();
    float width = MIN_BUILDING_WIDTH;
    float height = MIN_BUILDING_HEIGHT;
    float depth = BLOCK_DEPTH;
    int floors = 1;
    int colorVariant = 0;
    bool hasShopFront = false;
    bool hasBalcony = false;
    std::string shopName = std::string();
};

// ============================================================
// VEHICLE DATA
// ============================================================
struct VehicleInfo {
    VehicleType type;
    Vector3 position;
    float rotation;     // Y-axis rotation
    float pitch, roll;  // For suspension animation
    float speed;
    int lane;           // Lane index
    int direction;      // 1 or -1
    int colorVariant;
    bool stopped;       // At traffic light
    float pathParam;    // 0-1 along the path
    std::string bienSo; // Bien so xe Viet Nam
};

// ============================================================
// PEDESTRIAN DATA
// ============================================================
struct PedestrianInfo {
    PedestrianState state;
    Vector3 position;
    float rotation;
    float walkPhase;    // Animation phase
    float speed;
    int colorVariant;   // Still useful for overall palette
    int shirtColor;     // New: Persistent shirt color index
    int direction;      // 1 or -1
    bool crossingStreet;
    bool isWaving;      
    float waveTimer;    
    
    // AI Advanced Fields
    float targetX;      // X goal for steering/entering
    float originalX;    // Base sidewalk X
    bool hasBag;        // Visual accessory
    bool isInside;      // Hidden inside building
    float stayTimer;    // Time to stay inside
    int buildingIdx;    // Which building they are visiting
};

// ============================================================
// PARTICLE DATA
// ============================================================
struct Particle {
    Vector3 position;
    Vector3 velocity;
    float life;
    float maxLife;
    float size;
    Color color;
    bool active;
};

// ============================================================
// COLOR PALETTES - Vietnamese Street Style
// ============================================================
namespace Palette {
    // === TUONG NHA - Mau Pastel Duong Pho Viet Nam ===
    const Color WALL_YELLOW(0.98f, 0.85f, 0.15f);     // Vang Hoi An ruc ro - SÁNG
    const Color WALL_CREAM(0.95f, 0.88f, 0.70f);      // Kem vang - RUC
    const Color WALL_PINK(0.95f, 0.65f, 0.60f);       // Hong dam
    const Color WALL_BLUE(0.35f, 0.75f, 0.95f);       // Xanh troi dam
    const Color WALL_GREEN(0.45f, 0.85f, 0.55f);      // Xanh ngoc dam
    const Color WALL_WHITE(0.92f, 0.92f, 0.85f);      // Trang hien dai
    const Color WALL_ORANGE(0.98f, 0.60f, 0.25f);     // Cam dam ruc ro
    const Color WALL_LAVENDER(0.75f, 0.60f, 0.90f);   // Tim Lavender dam
    const Color WALL_TERRACOTTA(0.85f, 0.45f, 0.25f); // Mau gach nung ruc
    const Color WALL_MINT(0.68f, 0.95f, 0.85f);       // Xanh Bac Ha
    // === THEM MAU RUC RO ===
    const Color WALL_LIME(0.75f, 0.95f, 0.15f);       // Luc neon
    const Color WALL_CRIMSON(0.95f, 0.15f, 0.40f);    // Do dam
    const Color WALL_CORAL(0.95f, 0.50f, 0.40f);      // San ho
    const Color WALL_TEAL(0.15f, 0.75f, 0.85f);       // Xanh dam
    const Color WALL_PLUM(0.80f, 0.35f, 0.75f);       // Tim lam
    
    // === MAI NHA ===
    const Color ROOF_RED(0.85f, 0.25f, 0.18f);        // Do ngoi ruc
    const Color ROOF_BROWN(0.45f, 0.28f, 0.15f);
    const Color ROOF_GRAY(0.42f, 0.42f, 0.45f);
    const Color ROOF_TERRACOTTA(0.88f, 0.45f, 0.25f);
    
    // === MAT DUONG ===
    const Color ASPHALT(0.22f, 0.22f, 0.26f);         // Nhua duong moi - xam dam
    const Color ASPHALT_DARK(0.35f, 0.35f, 0.38f);    // Brighter asphalt
    const Color ASPHALT_WET(0.30f, 0.32f, 0.40f);
    const Color SIDEWALK(0.75f, 0.72f, 0.65f);
    const Color SIDEWALK_BRICK(0.90f, 0.65f, 0.50f);  // Brighter brick
    const Color CURB(0.85f, 0.85f, 0.82f);
    const Color LANE_MARK(0.97f, 0.97f, 0.92f);
    const Color CROSSWALK_WHITE(0.99f, 0.99f, 0.96f);
    const Color MEDIAN_GREEN(0.28f, 0.58f, 0.22f);
    
    // === CAY XANH ===
    const Color TREE_TRUNK(0.42f, 0.28f, 0.16f);
    const Color TREE_BARK(0.35f, 0.22f, 0.12f);
    const Color LEAF_GREEN(0.18f, 0.52f, 0.18f);
    const Color LEAF_DARK(0.12f, 0.38f, 0.12f);
    const Color LEAF_LIGHT(0.38f, 0.68f, 0.32f);
    const Color LEAF_TROPICAL(0.15f, 0.60f, 0.25f);   // La nhiet doi
    const Color FLOWER_RED(0.92f, 0.18f, 0.12f);      // Hoa Phuong
    const Color FLOWER_YELLOW(0.97f, 0.88f, 0.18f);   // Hoa Mai
    const Color FLOWER_PURPLE(0.62f, 0.28f, 0.78f);   // Hoa Bang Lang
    const Color FLOWER_BOUGAINVILLEA(0.90f, 0.15f, 0.50f); // Hoa Giay
    const Color GRASS(0.32f, 0.62f, 0.22f);
    
    // === KIM LOAI & VAT LIEU ===
    const Color METAL_GRAY(0.58f, 0.58f, 0.62f);
    const Color METAL_DARK(0.32f, 0.32f, 0.36f);
    const Color METAL_LIGHT(0.82f, 0.82f, 0.85f);
    const Color METAL_CHROME(0.88f, 0.88f, 0.92f);    // Inox bong
    const Color IRON_BLACK(0.12f, 0.12f, 0.15f);
    const Color GLASS_BLUE(0.48f, 0.68f, 0.85f, 0.55f);   // Kinh xanh
    const Color GLASS_DARK(0.18f, 0.28f, 0.40f, 0.65f);   // Kinh toi
    const Color GLASS_CLEAR(0.85f, 0.90f, 0.95f, 0.35f);  // Kinh trong
    const Color WOOD_LIGHT(0.75f, 0.58f, 0.38f);
    const Color WOOD_DARK(0.42f, 0.28f, 0.12f);
    const Color CONCRETE(0.70f, 0.68f, 0.65f);
    const Color TILE_WHITE(0.92f, 0.92f, 0.90f);      // Gach men trang
    const Color LEATHER_BROWN(0.45f, 0.25f, 0.15f);   // Mau da cho tui xach
    
    // === MAU XE ===
    const Color VEH_RED(0.85f, 0.12f, 0.10f);
    const Color VEH_BLUE(0.12f, 0.32f, 0.78f);
    const Color VEH_WHITE(0.94f, 0.94f, 0.94f);
    const Color VEH_BLACK(0.10f, 0.10f, 0.12f);
    const Color VEH_SILVER(0.78f, 0.78f, 0.82f);
    const Color VEH_YELLOW(0.97f, 0.82f, 0.08f);
    const Color VEH_GREEN(0.08f, 0.58f, 0.28f);
    const Color VEH_WINE_RED(0.55f, 0.08f, 0.12f);    // Do Ruou Vang
    const Color BUS_GREEN(0.12f, 0.48f, 0.32f);
    
    // === MAU NGUOI ===
    const Color SKIN_LIGHT(0.92f, 0.78f, 0.62f);
    const Color SKIN_MEDIUM(0.82f, 0.65f, 0.48f);
    const Color SKIN_DARK(0.68f, 0.52f, 0.38f);
    const Color HAIR_BLACK(0.08f, 0.06f, 0.06f);
    
    // === MAU TROI ===
    const Color SKY_DAY(0.35f, 0.75f, 0.98f);
    const Color SKY_DAWN(1.00f, 0.55f, 0.35f);
    const Color SKY_SUNSET(1.00f, 0.45f, 0.20f);
    const Color SKY_NIGHT(0.02f, 0.02f, 0.10f);
    const Color SKY_GOLDEN_HOUR(1.00f, 0.80f, 0.40f);
    const Color SUN_COLOR(1.00f, 1.00f, 0.90f);
    const Color MOON_COLOR(0.95f, 0.95f, 1.00f);
    const Color STAR_COLOR(1.00f, 1.00f, 1.00f);
    
    // === DEN NEON - Bien Quang Cao ===
    const Color NEON_RED(1.0f, 0.12f, 0.08f);
    const Color NEON_GREEN(0.08f, 1.0f, 0.28f);
    const Color NEON_BLUE(0.08f, 0.35f, 1.0f);
    const Color NEON_YELLOW(1.0f, 0.96f, 0.08f);
    const Color NEON_PINK(1.0f, 0.18f, 0.58f);
    const Color NEON_CYAN(0.08f, 0.92f, 0.92f);       // Xanh Cyan
    const Color NEON_ORANGE(1.0f, 0.55f, 0.08f);      // Cam Neon
    const Color NEON_PURPLE(0.70f, 0.15f, 1.0f);      // Tim Neon
}

// ============================================================
// GLOBAL STATE (extern declarations - defined in main.cpp)
// ============================================================
extern CameraMode g_cameraMode;
extern WeatherType g_weather;
extern float g_timeOfDay;         // 0-24 hours
extern float g_animSpeed;
extern float g_elapsedTime;
extern float g_deltaTime;
extern bool  g_paused;
extern bool  g_showHUD;
extern bool  g_showMinimap;
extern bool  g_showControls;
extern bool  g_streetLightsOn;
extern bool  g_rainEnabled;
extern bool  g_fogEnabled;
extern int   g_windowWidth;
extern int   g_windowHeight;
extern int   g_fps;
extern Mesh  g_camaroMesh;
extern GLuint g_texAsphalt;
extern GLuint g_texBrick;
extern GLuint g_texConcrete;
extern int   g_frameCount;
extern float g_lastFpsTime;

// === STREET FURNITURE (from furniture.h) ===
enum FurnitureType {
    FURN_STREET_LAMP = 0,
    FURN_BENCH,
    FURN_TRASH_CAN,
    FURN_POWER_POLE,
    FURN_STREET_SIGN,
    FURN_BUS_STOP,
    FURN_PLANTER_BOX,
    FURN_STOOL,
    FURN_TEA_TABLE,
    FURN_VENDOR_STALL,
    FURN_TEA_GROUP,
    FURN_ELECTRIC_POLE,
    FURN_TYPE_COUNT
};

struct FurnitureInfo {
    FurnitureType type;
    Transform transform;
    Color color;
};

// === VEGETATION (from vegetation.h) ===
enum TreeType {
    TREE_BANG,
    TREE_PHUONG,
    TREE_PALM,
    TREE_BUSH,
    TREE_FLOWER_BED
};

struct TreeInfo {
    TreeType type;
    Transform transform;
    float age;
};

// === TRAFFIC SIGNS (from traffic.h) ===
enum TrafficSignType {
    SIGN_STOP,
    SIGN_ONE_WAY,
    SIGN_SPEED_LIMIT,
    SIGN_NO_PARKING,
    SIGN_CROSSWALK
};

struct TrafficSignInfo {
    TrafficSignType type;
    Transform transform;
};

// Scene data
extern std::vector<BuildingInfo> g_buildings;
extern std::vector<VehicleInfo> g_vehicles;
extern std::vector<PedestrianInfo> g_pedestrians;
extern std::vector<FurnitureInfo> g_sceneFurniture;
extern std::vector<TreeInfo> g_sceneTrees;
extern std::vector<TrafficSignInfo> g_sceneSigns;

// Traffic light state
extern TrafficLightState g_mainTrafficLight;
extern TrafficLightState g_sideTrafficLight;
extern float g_trafficTimer;
extern float g_phaseTimeLeft;

// Global Meshes
extern Mesh g_camaroMesh;
extern Mesh g_humanMesh;
extern Mesh g_treeMesh;
extern Mesh g_racingCarMesh;

#endif // GLOBALS_H
