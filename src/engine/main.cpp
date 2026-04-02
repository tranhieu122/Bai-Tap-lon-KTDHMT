#include <cstdio>
#include <ctime>
#include <algorithm>
#include <vector>
#include <string>

#include "globals.h"
#include "camera.h"
#include "lighting.h"
#include "texture.h"
#include "weather.h"
#include "utils.h"
#include "ui.h"

#include "../world/scene.h"
#include "../world/sky.h"
#include "../world/buildings.h"
#include "../world/furniture.h"
#include "../world/street.h"
#include "../world/vegetation.h"

#include "../traffic/animation.h"
#include "../traffic/particles.h"
#include "../traffic/people.h"
#include "../traffic/traffic.h"
#include "../traffic/vehicles.h"

// Define Global State
int g_windowWidth = WINDOW_WIDTH;
int g_windowHeight = WINDOW_HEIGHT;

CameraMode g_cameraMode = CAMERA_ORBIT;
WeatherType g_weather = WEATHER_CLEAR;
float g_timeOfDay = 12.0f;
float g_animSpeed = 1.0f;

bool g_paused = false;
bool g_showHUD = true;
bool g_showMinimap = false;
bool g_showControls = true;
bool g_streetLightsOn = true;
bool g_rainEnabled = false;
bool g_fogEnabled = false;

float g_elapsedTime = 0.0f;
float g_deltaTime = 0.0f;

int g_fps = 0;
int g_frameCount = 0;
float g_lastFpsTime = 0.0f;

std::vector<BuildingInfo> g_buildings;
std::vector<VehicleInfo> g_vehicles;
std::vector<PedestrianInfo> g_pedestrians;

TrafficLightState g_mainTrafficLight = TL_GREEN;
TrafficLightState g_sideTrafficLight = TL_RED;
float g_trafficTimer = 0.0f;
extern Mesh g_camaroMesh;
GLuint g_texAsphalt = 0;
GLuint g_texBrick = 0;
GLuint g_texConcrete = 0;

// Frame timing
int lastTime = 0;

static void setupRC() {
  glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  // === NANG CAP CHAT LUONG HINH ANH ===
  // Khu rang cua (Anti-Aliasing)
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POINT_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  // Alpha blending mac dinh
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Init game systems
  seedRandom((unsigned int)time(NULL));
  textureInit();
  lightingInit();
  sceneInit();
  cameraInit();
  skyInit();
  weatherInit();
  particlesInit();
  initCharacterModels();

  // Scene prep
  streetInit();
  animationInit();
  
  // Load premium assets
  loadMeshFromOBJ("assets/models/camaro2.obj", g_camaroMesh);
  loadMeshFromOBJ("assets/models/human.obj", g_humanMesh);
  loadMeshFromOBJ("assets/models/tree.obj", g_treeMesh);
  loadMeshFromOBJ("assets/models/racing_car.obj", g_racingCarMesh);
  loadTexture("assets/textures/asphalt.png", g_texAsphalt);
  loadTexture("assets/textures/brick.png", g_texBrick);
  loadTexture("assets/textures/concrete.png", g_texConcrete);

  lastTime = glutGet(GLUT_ELAPSED_TIME);

  printf("=== HE THONG DO HOA DA KHOI TAO ===\n");
  printf("  Anti-Aliasing: BAT (GL_LINE_SMOOTH)\n");
  printf("  Bong Do: BAT (Planar Shadows)\n");
  printf("  Suong Khong Khi: BAT\n");
  printf("  Sao Dem: BAT (200 ngoi sao)\n");
}

static void displayFunc() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Camera Transform
  cameraApply();

  // Draw Skybox First Mode
  skyDraw();

  // Set Lighting
  // (Lighting is set in sceneDraw)

  // Draw Environment
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  sceneDraw();

  // Dynamic Objects (Vehicles, People)
  for (const auto &v : g_vehicles)
    vehicleDraw(v);
  for (const auto &p : g_pedestrians)
    personDraw(p);

  // Multi-pass Jittered Planar Shadows (Nang cap do bong sieu thuc)
  if (g_timeOfDay > 6.0f && g_timeOfDay < 18.0f && g_weather == WEATHER_CLEAR) {
    Vector3 lightDir = skyGetLightDirection();
    float sunEle = lightDir.y;
    // Allow longer shadows (0.13f clamp) but fade alpha to prevent 'black wall'
    float shadowY = std::max(sunEle, 0.13f); 
    float shadowMat[16] = { 0 };
    float ground[4] = {0.0f, 1.0f, 0.0f, -0.02f}; 
    float lightPos[4] = {lightDir.x, shadowY, lightDir.z, 0.0f};

    float dot = ground[0] * lightPos[0] + ground[1] * lightPos[1] +
                ground[2] * lightPos[2] + ground[3] * lightPos[3];
    shadowMat[0] = dot - lightPos[0] * ground[0];
    shadowMat[4] = 0.0f - lightPos[0] * ground[1];
    shadowMat[8] = 0.0f - lightPos[0] * ground[2];
    shadowMat[12] = 0.0f - lightPos[0] * ground[3];

    shadowMat[1] = 0.0f - lightPos[1] * ground[0];
    shadowMat[5] = dot - lightPos[1] * ground[1];
    shadowMat[9] = 0.0f - lightPos[1] * ground[2];
    shadowMat[13] = 0.0f - lightPos[1] * ground[3];

    shadowMat[2] = 0.0f - lightPos[2] * ground[0];
    shadowMat[6] = 0.0f - lightPos[2] * ground[1];
    shadowMat[10] = dot - lightPos[2] * ground[2];
    shadowMat[14] = 0.0f - lightPos[2] * ground[3];

    shadowMat[3] = 0.0f - lightPos[3] * ground[0];
    shadowMat[7] = 0.0f - lightPos[3] * ground[1];
    shadowMat[11] = 0.0f - lightPos[3] * ground[2];
    shadowMat[15] = dot - lightPos[3] * ground[3];

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0f, -1.0f);

    // Render 8 passes with Poisson-like jitter to simulate deep soft shadows
    struct Jitter { float x, z, a; };
    Jitter samples[8] = {
        {0.00f, 0.00f, 0.25f}, // Central core
        {0.12f, 0.05f, 0.15f},
        {-0.10f, 0.12f, 0.15f},
        {0.08f, -0.12f, 0.15f},
        {-0.12f, -0.06f, 0.15f},
        {0.22f, 0.18f, 0.10f}, // Outer soft edge
        {-0.20f, -0.22f, 0.10f},
        {0.15f, -0.25f, 0.10f}
    };

    // Calculate dynamic fading based on sun elevation
    float alphaScale = std::min(sunEle * 2.0f, 1.0f);
    if (sunEle < 0.2f) alphaScale *= (sunEle / 0.2f); // Fade out near horizon

    for (int i = 0; i < 8; i++) {
      glPushMatrix();
      glTranslatef(samples[i].x, 0, samples[i].z);
      glMultMatrixf(shadowMat);
      
      // Shadow color: Deep navy-black (0.01, 0.01, 0.05) for realism
      glColor4f(0.01f, 0.01f, 0.05f, samples[i].a * alphaScale * 0.8f);

      // Chỉ giữ lại đổ bóng cho người đi bộ để tăng hiệu năng và tập trung thị giác
      for (const auto &p : g_pedestrians)
        personDraw(p, true);
      glPopMatrix();
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_LIGHTING);
  }

  // Draw Weather / Transparent Elements
  weatherEffectDraw();
  drawTransparentObjects();
  particlesDraw();

  // === SUONG KHONG KHI DONG (Dynamic Atmospheric Haze) ===
  if (g_weather == WEATHER_CLEAR && !g_fogEnabled) {
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_EXP2);
    
    // Suong mu doi mau theo sky ambient de hoa quyen
    Color hazeCol = skyGetAmbientLight(); 
    float intensity = (g_timeOfDay > 6.0f && g_timeOfDay < 18.0f) ? 0.7f : 0.4f;
    float fogCol[] = {hazeCol.r * intensity, hazeCol.g * intensity, hazeCol.b * intensity, 1.0f};
    
    glFogfv(GL_FOG_COLOR, fogCol);
    glFogf(GL_FOG_DENSITY, (g_timeOfDay > 18.0f || g_timeOfDay < 6.0f) ? 0.0035f : 0.002f); 
    glHint(GL_FOG_HINT, GL_NICEST);
  }

  // Tat fog truoc khi ve UI de khong anh huong text
  glDisable(GL_FOG);

  // UI Override
  uiDraw();

  glutSwapBuffers();

  // FPS counter
  g_frameCount++;
  float curTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
  if (curTime - g_lastFpsTime > 1.0f) {
    g_fps = g_frameCount;
    g_frameCount = 0;
    g_lastFpsTime = curTime;
  }
}

static void idleFunc() {
  int curTime = glutGet(GLUT_ELAPSED_TIME);
  g_deltaTime = (curTime - lastTime) / 1000.0f;

  // Cap DT to prevent large jumps
  if (g_deltaTime > 0.1f)
    g_deltaTime = 0.1f;
  lastTime = curTime;

  g_elapsedTime += g_deltaTime;

  // Updates
  cameraUpdate(g_deltaTime);
  skyUpdate(g_deltaTime);
  weatherUpdate(g_deltaTime);
  trafficUpdate(g_deltaTime);
  animationUpdate(g_deltaTime);
  particlesUpdate(g_deltaTime);

  glutPostRedisplay();
}

static void reshapeFunc(int w, int h) {
  g_windowWidth = w;
  g_windowHeight = h;
  if (h == 0)
    h = 1;
  glViewport(0, 0, w, h);
}

// Menu handling
enum MenuOption {
  MENU_CAM_ORBIT,
  MENU_CAM_FPS,
  MENU_CAM_FLY,
  MENU_CAM_TOUR,
  MENU_TIME_NOON,
  MENU_TIME_NIGHT,
  MENU_WEATHER_CLEAR,
  MENU_WEATHER_RAIN,
  MENU_WEATHER_FOG,
  MENU_TOGGLE_LIGHTS,
  MENU_TOGGLE_HUD,
  MENU_TOGGLE_PAUSE
};

static void menuProcess(int option) {
  switch (option) {
  case MENU_CAM_ORBIT:
    cameraSetMode(CAMERA_ORBIT);
    break;
  case MENU_CAM_FPS:
    cameraSetMode(CAMERA_FPS);
    break;
  case MENU_CAM_FLY:
    cameraSetMode(CAMERA_FLY);
    break;
  case MENU_CAM_TOUR:
    cameraSetMode(CAMERA_AUTO_TOUR);
    break;
  case MENU_TIME_NOON:
    g_timeOfDay = 12.0f;
    break;
  case MENU_TIME_NIGHT:
    g_timeOfDay = 21.0f;
    break;
  case MENU_WEATHER_CLEAR:
    g_weather = WEATHER_CLEAR;
    break;
  case MENU_WEATHER_RAIN:
    g_weather = WEATHER_RAIN;
    break;
  case MENU_WEATHER_FOG:
    g_weather = WEATHER_FOG;
    break;
  case MENU_TOGGLE_LIGHTS:
    g_streetLightsOn = !g_streetLightsOn;
    break;
  case MENU_TOGGLE_HUD:
    g_showHUD = !g_showHUD;
    g_showControls = g_showHUD;
    break;
  case MENU_TOGGLE_PAUSE:
    g_paused = !g_paused;
    break;
  }
}

static void setupMenu() {
  int camMenu = glutCreateMenu(menuProcess);
  glutAddMenuEntry("Xoay Quanh", MENU_CAM_ORBIT);
  glutAddMenuEntry("Goc nhin FPS", MENU_CAM_FPS);
  glutAddMenuEntry("Bay Tu Do", MENU_CAM_FLY);
  glutAddMenuEntry("Tham Quan Tu Dong", MENU_CAM_TOUR);

  int envMenu = glutCreateMenu(menuProcess);
  glutAddMenuEntry("Troi Sang", MENU_TIME_NOON);
  glutAddMenuEntry("Ban Dem", MENU_TIME_NIGHT);
  glutAddMenuEntry("Troi Quang", MENU_WEATHER_CLEAR);
  glutAddMenuEntry("Troi Mua", MENU_WEATHER_RAIN);
  glutAddMenuEntry("Suong Mu", MENU_WEATHER_FOG);
  glutAddMenuEntry("Bat/Tat Den Duong", MENU_TOGGLE_LIGHTS);

  glutCreateMenu(menuProcess);
  glutAddSubMenu("Che Do Goc Nhin", camMenu);
  glutAddSubMenu("Moi Truong", envMenu);
  glutAddMenuEntry("An/Hien HUD", MENU_TOGGLE_HUD);
  glutAddMenuEntry("Tam Dung", MENU_TOGGLE_PAUSE);

  glutAttachMenu(GLUT_RIGHT_BUTTON);
}

static void keyboardFunc(unsigned char key, int x, int y) {
  (void)x; (void)y;
  if (key == 27)
    exit(0); // ESC

  if (key == 't' || key == 'T')
    g_timeOfDay = (g_timeOfDay < 18.0f && g_timeOfDay > 6.0f) ? 21.0f : 12.0f;
  if (key == 'r' || key == 'R')
    g_weather = (g_weather == WEATHER_RAIN) ? WEATHER_CLEAR : WEATHER_RAIN;
  if (key == 'f' || key == 'F')
    g_weather = (g_weather == WEATHER_FOG) ? WEATHER_CLEAR : WEATHER_FOG;
  if (key == 'l' || key == 'L')
    g_streetLightsOn = !g_streetLightsOn;
  if (key == 'h' || key == 'H') {
    g_showHUD = !g_showHUD;
    g_showControls = g_showHUD;
  }
  if (key == ' ')
    g_paused = !g_paused;
  if (key == '=')
    g_animSpeed += 0.2f;
  if (key == '-') {
    g_animSpeed -= 0.2f;
    if (g_animSpeed < 0.2f)
      g_animSpeed = 0.2f;
  }

  cameraOnKeyDown(key);
}

static void keyboardUpFunc(unsigned char key, int x, int y) { (void)x; (void)y; cameraOnKeyUp(key); }

static void specialFunc(int key, int x, int y) { (void)x; (void)y; cameraOnSpecialKey(key); }

static void mouseFunc(int button, int state, int x, int y) {
  cameraOnMouseButton(button, state, x, y);
}

static void motionFunc(int x, int y) { cameraOnMouseMove(x, y); }

static void mouseWheelFunc(int wheel, int dir, int x, int y) {
  cameraOnMouseWheel(wheel, dir, x, y);
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(WINDOW_TITLE);

  setupRC();
  setupMenu();

  // Prevent key repeat from causing ghost movement
  glutIgnoreKeyRepeat(1);

  glutDisplayFunc(displayFunc);
  glutReshapeFunc(reshapeFunc);
  glutIdleFunc(idleFunc);

  glutKeyboardFunc(keyboardFunc);
  glutKeyboardUpFunc(keyboardUpFunc);
  glutSpecialFunc(specialFunc);
  glutMouseFunc(mouseFunc);
  glutMotionFunc(motionFunc);
  glutMouseWheelFunc(mouseWheelFunc);

  printf("=========================================\n");
  printf(" Do an: Mo phong Duong Pho Viet Nam 3D   \n");
  printf("=========================================\n");
  printf(" HDSD:                                   \n");
  printf(" Phim W,A,S,D : Di chuyen                \n");
  printf(" Giu Chuot Trai : Xoay goc nhin          \n");
  printf(" Chuot Phai : Mo thiet lap               \n");
  printf(" Phim T/R/F : Doi Ngay, Dem/Mua/Suong mu \n");

  glutMainLoop();
  return 0;
}
