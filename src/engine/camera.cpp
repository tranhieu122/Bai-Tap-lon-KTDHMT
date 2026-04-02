#include "camera.h"
#include <GL/freeglut.h>
#include <cmath>
#include <iostream>
#include "utils.h"

// ============================================================
// CAMERA CONSTANTS & STATE
// ============================================================
namespace {
    // Current state
    CameraState st;
    CameraState prevSt;
    CameraState targetSt;

    // Transition state
    bool  isTrans = false;
    float transTimer = 0.0f;
    float transDuration = 1.0f;
    CameraMode preTransMode;

    // Auto-tour path
    std::vector<Waypoint> tourPath;
    int curTourIdx = 0;
    float tourTimer = 0.0f;

    // Input state
    bool k_W = false, k_A = false, k_S = false, k_D = false;
    bool k_Q = false, k_E = false; // Up/Down for fly
    bool m_LeftDown = false;
    bool m_RightDown = false;
    int m_lastX = 0, m_lastY = 0;

    // Bounds limit
    const float BOUND_X = 60.0f;
    const float BOUND_Y_MIN = 1.0f;
    const float BOUND_Y_MAX = 100.0f;
    const float BOUND_Z = 60.0f;
}

// ============================================================
// INITIALIZATION
// ============================================================
void cameraInit() {
    // Default orbit camera
    st.target = Vector3(0, 5.0f, 0);
    st.distance = 60.0f;
    st.yaw = 45.0f;
    st.pitch = 30.0f;
    st.up = Vector3(0, 1, 0);
    st.fov = 60.0f;
    st.moveSpeed = 15.0f;
    st.lookSpeed = 0.2f;
    st.zoomSpeed = 5.0f;

    // Update position based on angles
    float py = st.pitch * (float)M_PI / 180.0f;
    float px = st.yaw * (float)M_PI / 180.0f;
    st.position.x = st.target.x + st.distance * cosf(py) * sinf(px);
    st.position.y = st.target.y + st.distance * sinf(py);
    st.position.z = st.target.z + st.distance * cosf(py) * cosf(px);

    // Setup auto tour path
    tourPath.push_back({ Vector3(0, 40, 80), Vector3(0, 0, 0), 5.0f });
    tourPath.push_back({ Vector3(0, 3, 50), Vector3(0, 3, 0), 4.0f });
    tourPath.push_back({ Vector3(0, 3, -50), Vector3(0, 3, 0), 8.0f });
    tourPath.push_back({ Vector3(40, 20, 0), Vector3(0, 5, 0), 5.0f });
    tourPath.push_back({ Vector3(-40, 20, 0), Vector3(0, 5, 0), 5.0f });

    cameraSetMode(CAMERA_ORBIT);
}

// ============================================================
// UPDATE LOGIC
// ============================================================
static void limitBounds() {
    if (g_cameraMode == CAMERA_ORBIT) {
        if (st.pitch < 5.0f) st.pitch = 5.0f;
        if (st.pitch > 85.0f) st.pitch = 85.0f;
        if (st.distance < 5.0f) st.distance = 5.0f;
        if (st.distance > 150.0f) st.distance = 150.0f;
    } else {
        if (st.pitch < -85.0f) st.pitch = -85.0f;
        if (st.pitch > 85.0f) st.pitch = 85.0f;
    }

    if (g_cameraMode != CAMERA_AUTO_TOUR) {
        // Limit position to workspace
        st.position.x = fmaxf(-BOUND_X, fminf(BOUND_X, st.position.x));
        st.position.y = fmaxf(BOUND_Y_MIN, fminf(BOUND_Y_MAX, st.position.y));
        st.position.z = fmaxf(-BOUND_Z, fminf(BOUND_Z, st.position.z));
    }
}

static Vector3 calcForward() {
    float py = st.pitch * (float)M_PI / 180.0f;
    float px = st.yaw * (float)M_PI / 180.0f;
    return Vector3(
        cosf(py) * sinf(px),
        sinf(py),
        cosf(py) * cosf(px)
    ).normalized();
}

static void updateOrbit(float dt) {
    // Panning map with WASD (no right-click needed, since right-click opens menu)
    Vector3 forward = calcForward();
    Vector3 right = forward.cross(st.up).normalized();
    Vector3 flatForward(forward.x, 0, forward.z);
    flatForward = flatForward.normalized();

    if (k_W) st.target += flatForward * st.moveSpeed * dt;
    if (k_S) st.target -= flatForward * st.moveSpeed * dt;
    if (k_A) st.target -= right * st.moveSpeed * dt;
    if (k_D) st.target += right * st.moveSpeed * dt;

    float py = st.pitch * (float)M_PI / 180.0f;
    float px = st.yaw * (float)M_PI / 180.0f;

    st.position.x = st.target.x + st.distance * cosf(py) * sinf(px);
    st.position.y = st.target.y + st.distance * sinf(py);
    st.position.z = st.target.z + st.distance * cosf(py) * cosf(px);
}

static void updateFPS(float dt) {
    Vector3 forward = calcForward();
    Vector3 right = forward.cross(st.up).normalized();

    // FPS mode ignores vertical component for movement
    Vector3 flatFwd(forward.x, 0, forward.z);
    flatFwd = flatFwd.normalized();

    if (k_W) st.position += flatFwd * st.moveSpeed * dt;
    if (k_S) st.position -= flatFwd * st.moveSpeed * dt;
    if (k_A) st.position -= right * st.moveSpeed * dt;
    if (k_D) st.position += right * st.moveSpeed * dt;

    // Lock height to simulate walking
    st.position.y = 1.7f; // PERSON_HEIGHT

    // Calculate target from position and forward vector
    st.target = st.position + forward;
}

static void updateFly(float dt) {
    Vector3 forward = calcForward();
    Vector3 right = forward.cross(st.up).normalized();

    // Fly mode moves in direction we look
    if (k_W) st.position += forward * st.moveSpeed * dt * 2.0f; // Fly faster
    if (k_S) st.position -= forward * st.moveSpeed * dt * 2.0f;
    if (k_A) st.position -= right * st.moveSpeed * dt * 2.0f;
    if (k_D) st.position += right * st.moveSpeed * dt * 2.0f;
    if (k_Q) st.position -= st.up * st.moveSpeed * dt * 2.0f;
    if (k_E) st.position += st.up * st.moveSpeed * dt * 2.0f;

    st.target = st.position + forward;
}

static void updateAutoTour(float dt) {
    if (tourPath.empty()) return;

    tourTimer += dt;
    Waypoint& wp1 = tourPath[curTourIdx];
    int nextIdx = (curTourIdx + 1) % tourPath.size();
    Waypoint& wp2 = tourPath[nextIdx];

    if (tourTimer >= wp1.duration) {
        tourTimer -= wp1.duration;
        curTourIdx = nextIdx;
        wp1 = tourPath[curTourIdx];
        nextIdx = (curTourIdx + 1) % tourPath.size();
        wp2 = tourPath[nextIdx];
    }

    float t = tourTimer / wp1.duration;
    // Smooth step for nicer interpolation
    t = t * t * (3.0f - 2.0f * t);

    st.position = wp1.position + (wp2.position - wp1.position) * t;
    st.target = wp1.lookAt + (wp2.lookAt - wp1.lookAt) * t;

    // Calc yaw pitch from look direction
    Vector3 dir = (st.target - st.position).normalized();
    st.pitch = asinf(dir.y) * 180.0f / (float)M_PI;
    st.yaw = atan2f(dir.x, dir.z) * 180.0f / (float)M_PI;
}

static void updateTransition(float dt) {
    transTimer += dt;
    if (transTimer >= transDuration) {
        isTrans = false;
        st = targetSt;
        return;
    }

    float t = transTimer / transDuration;
    t = t * t * (3.0f - 2.0f * t); // Smoothstep

    st.position = prevSt.position + (targetSt.position - prevSt.position) * t;
    st.target = prevSt.target + (targetSt.target - prevSt.target) * t;
    st.yaw = prevSt.yaw + (targetSt.yaw - prevSt.yaw) * t;
    st.pitch = prevSt.pitch + (targetSt.pitch - prevSt.pitch) * t;
    st.distance = prevSt.distance + (targetSt.distance - prevSt.distance) * t;

    // Special fix for shortest path rotation
    if (targetSt.yaw - prevSt.yaw > 180) {
        st.yaw = prevSt.yaw + (targetSt.yaw - 360 - prevSt.yaw) * t;
    } else if (targetSt.yaw - prevSt.yaw < -180) {
        st.yaw = prevSt.yaw + (targetSt.yaw + 360 - prevSt.yaw) * t;
    }
}

void cameraUpdate(float dt) {
    if (isTrans) {
        updateTransition(dt);
        return;
    }

    switch (g_cameraMode) {
        case CAMERA_ORBIT: updateOrbit(dt); break;
        case CAMERA_FPS: updateFPS(dt); break;
        case CAMERA_FLY: updateFly(dt); break;
        case CAMERA_AUTO_TOUR: updateAutoTour(dt); break;
        default: break;
    }

    limitBounds();
}

void cameraApply() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(st.fov, (double)g_windowWidth / (double)g_windowHeight, 0.1, 1000.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
        st.position.x, st.position.y, st.position.z,
        st.target.x, st.target.y, st.target.z,
        st.up.x, st.up.y, st.up.z
    );
}

// ============================================================
// MODE SWITCHING & TRANSITIONS
// ============================================================
void cameraSetMode(CameraMode mode) {
    if (mode == g_cameraMode) return;

    prevSt = st;
    g_cameraMode = mode;

    if (mode == CAMERA_FPS) {
        st.position.y = 1.7f;
        st.pitch = 0.0f;
        st.target = st.position + calcForward();
    } else if (mode == CAMERA_ORBIT) {
        // Try to keep looking at same target, just back up
        st.target.y = 0.0f;
        st.distance = 40.0f;
    }

    // Camera cut, no transition
    isTrans = false;
}

const char* cameraGetModeName() {
    switch (g_cameraMode) {
        case CAMERA_ORBIT: return "Xoay Quanh";
        case CAMERA_FPS: return "Goc nhin FPS";
        case CAMERA_FLY: return "Bay Tu Do";
        case CAMERA_AUTO_TOUR: return "Tham Quan";
        default: return "Khong Ro";
    }
}

CameraMode cameraGetMode() {
    return g_cameraMode;
}

void cameraBeginTransition(const CameraState& target, float duration) {
    prevSt = st;
    targetSt = target;
    transDuration = duration;
    transTimer = 0.0f;
    isTrans = true;
    preTransMode = g_cameraMode;
}

bool cameraIsTransitioning() {
    return isTrans;
}

// ============================================================
// INPUT HANDLING
// ============================================================
void cameraOnMouseMove(int x, int y) {
    if (isTrans || g_cameraMode == CAMERA_AUTO_TOUR) return;

    int dx = x - m_lastX;
    int dy = y - m_lastY;
    m_lastX = x;
    m_lastY = y;

    if (m_LeftDown) {
        if (g_cameraMode == CAMERA_ORBIT) {
            st.yaw += dx * st.lookSpeed * 2.0f;
            st.pitch += dy * st.lookSpeed * 2.0f;
        } else { // FPS, Fly
            st.yaw -= dx * st.lookSpeed;
            st.pitch -= dy * st.lookSpeed;
        }

        // Normalize yaw
        while (st.yaw >= 360.0f) st.yaw -= 360.0f;
        while (st.yaw < 0.0f) st.yaw += 360.0f;

        // Limit pitch inside limitBounds() later
    }
}

void cameraOnMouseButton(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            m_LeftDown = true;
            m_lastX = x;
            m_lastY = y;
        } else {
            m_LeftDown = false;
        }
    } else if (button == GLUT_RIGHT_BUTTON) {
        if (state == GLUT_DOWN) {
            m_RightDown = true;
        } else {
            m_RightDown = false;
        }
    }
}

void cameraOnMouseWheel(int wheel, int dir, int x, int y) {
    (void)wheel; (void)x; (void)y;
    if (isTrans) return;

    if (g_cameraMode == CAMERA_ORBIT) {
        if (dir > 0) st.distance -= st.zoomSpeed;
        else st.distance += st.zoomSpeed;
    } else if (g_cameraMode == CAMERA_FLY || g_cameraMode == CAMERA_FPS) {
        if (dir > 0) st.fov -= 5.0f;
        else st.fov += 5.0f;
        st.fov = clamp(st.fov, 30.0f, 100.0f);
    }
}

// ASCII keys
void cameraOnKeyDown(unsigned char key) {
    key = (unsigned char)tolower(key);
    if (key == 'w') k_W = true;
    if (key == 's') k_S = true;
    if (key == 'a') k_A = true;
    if (key == 'd') k_D = true;
    if (key == 'q') k_Q = true;
    if (key == 'e') k_E = true;

    // Switch modes with 1,2,3,4
    if (key == '1') cameraSetMode(CAMERA_ORBIT);
    if (key == '2') cameraSetMode(CAMERA_FPS);
    if (key == '3') cameraSetMode(CAMERA_FLY);
    if (key == '4') cameraSetMode(CAMERA_AUTO_TOUR);
}

void cameraOnKeyUp(unsigned char key) {
    key = (unsigned char)tolower(key);
    if (key == 'w') k_W = false;
    if (key == 's') k_S = false;
    if (key == 'a') k_A = false;
    if (key == 'd') k_D = false;
    if (key == 'q') k_Q = false;
    if (key == 'e') k_E = false;
}

void cameraOnSpecialKey(int key) {
    (void)key;
    // We could map arrows to WASD here if wanted
}

// ============================================================
// GETTERS
// ============================================================
Vector3 cameraGetPosition() { return st.position; }
Vector3 cameraGetForward() { return calcForward(); }
Vector3 cameraGetRight() { return calcForward().cross(st.up).normalized(); }
float cameraGetYaw() { return st.yaw; }
float cameraGetPitch() { return st.pitch; }
