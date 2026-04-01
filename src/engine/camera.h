#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include "globals.h"

// ============================================================
// CAMERA SYSTEM
// ============================================================

// Camera state
struct CameraState {
    Vector3 position;
    Vector3 target;
    Vector3 up;
    float yaw;         // Horizontal angle (degrees)
    float pitch;       // Vertical angle (degrees)
    float distance;    // Distance from target (orbit mode)
    float fov;         // Field of view
    float moveSpeed;
    float lookSpeed;
    float zoomSpeed;
};

// Auto tour waypoint
struct Waypoint {
    Vector3 position;
    Vector3 lookAt;
    float duration; // seconds at this point
};

// Initialize camera system
void cameraInit();
void cameraUpdate(float dt);
void cameraApply();
void cameraSetMode(CameraMode mode);
CameraMode cameraGetMode();
const char* cameraGetModeName();

// Input handlers
void cameraOnMouseMove(int x, int y);
void cameraOnMouseButton(int button, int state, int x, int y);
void cameraOnMouseWheel(int wheel, int dir, int x, int y);
void cameraOnKeyDown(unsigned char key);
void cameraOnKeyUp(unsigned char key);
void cameraOnSpecialKey(int key);

// Getters
Vector3 cameraGetPosition();
Vector3 cameraGetForward();
Vector3 cameraGetRight();
float cameraGetYaw();
float cameraGetPitch();

// Smooth transition between modes
void cameraBeginTransition(const CameraState& target, float duration);
bool cameraIsTransitioning();

#endif // CAMERA_H
