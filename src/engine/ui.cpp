#include "ui.h"
#include "utils.h"
#include "camera.h"
#include "sky.h"
#include <GL/freeglut.h>
#include <cstdio>

void uiInit() {
}

void uiDraw() {
    char buf[256];
    
    // Nâng cấp HUD Top-Left với nền trong suốt
    if (g_showHUD) {
        int hudW = 220, hudH = 110;
        int pad = 10;
        int xStart = 15, yStart = g_windowHeight - 15;

        // Vẽ nền Panel cho HUD dùng Alpha Blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_LIGHTING);
        
        glColor4f(0.0f, 0.0f, 0.0f, 0.5f); // Đen trong suốt 50%
        glBegin(GL_QUADS);
        glVertex2i(xStart - pad, yStart + pad);
        glVertex2i(xStart + hudW, yStart + pad);
        glVertex2i(xStart + hudW, yStart - hudH);
        glVertex2i(xStart - pad, yStart - hudH);
        glEnd();

        // Border tinh tế
        glColor4f(0.0f, 1.0f, 0.8f, 0.7f); // Cyan border
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2i(xStart - pad, yStart + pad);
        glVertex2i(xStart + hudW, yStart + pad);
        glVertex2i(xStart + hudW, yStart - hudH);
        glVertex2i(xStart - pad, yStart - hudH);
        glEnd();
        glLineWidth(1.0f);

        float lineH = 22.0f;
        float x = (float)xStart;
        float y = (float)yStart - 15.0f;

        // Performance
        glColor3f(0.0f, 1.0f, 0.8f);
        sprintf(buf, "[ SYSTEM ] FPS: %d", g_fps);
        drawText2D(x, y, buf); y -= lineH;
        
        // World
        glColor3f(1.0f, 1.0f, 1.0f);
        int h = (int)g_timeOfDay;
        int m = (int)((g_timeOfDay - h) * 60);
        sprintf(buf, "[ WORLD  ] Thoi gian: %02d:%02d", h, m);
        drawText2D(x, y, buf); y -= lineH;

        sprintf(buf, "[ WORLD  ] Thoi tiet: %s", skyGetWeatherName());
        drawText2D(x, y, buf); y -= lineH;
        
        // Mode
        glColor3f(1.0f, 0.9f, 0.2f);
        sprintf(buf, "[ VIEW   ] Goc nhin: %s", cameraGetModeName());
        drawText2D(x, y, buf); y -= lineH;

        glEnable(GL_LIGHTING);
        glDisable(GL_BLEND);
    }
    
    // Nâng cấp Panel Hướng dẫn (Bottom-Left)
    if (g_showControls) {
        int ctrlW = 280, ctrlH = 200;
        int xStart = 10, yStart = 10;
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_LIGHTING);
        
        glColor4f(0.0f, 0.0f, 0.0f, 0.6f);
        glBegin(GL_QUADS);
        glVertex2i(xStart, yStart);
        glVertex2i(xStart + ctrlW, yStart);
        glVertex2i(xStart + ctrlW, yStart + ctrlH);
        glVertex2i(xStart, yStart + ctrlH);
        glEnd();

        glColor4f(1.0f, 1.0f, 1.0f, 0.3f);
        glBegin(GL_LINE_LOOP);
        glVertex2i(xStart, yStart);
        glVertex2i(xStart + ctrlW, yStart);
        glVertex2i(xStart + ctrlW, yStart + ctrlH);
        glVertex2i(xStart, yStart + ctrlH);
        glEnd();

        glColor3f(1.0f, 1.0f, 1.0f);
        const char* controls[] = {
            "W/A/S/D: Di chuyen",
            "Kich giu chuot: Xoay goc",
            "Cuon chuot: Thu phong (Zoom)",
            "Phim 1-4: Chon Goc Nhin",
            "Phim T: Ngay/Dem",
            "Phim R: Bat/Tat Mua",
            "Phim F: Bat/Tat Suong",
            "Phim L: Bat/Tat Den duong",
            "Chuot phai: Menu",
            "Phim H: An/Hien HUD"
        };
        for (int i=0; i<10; i++) {
            drawText2D((float)xStart + 10.0f, (float)yStart + 10.0f + (9-i)*18.0f, controls[i]);
        }
        glEnable(GL_LIGHTING);
        glDisable(GL_BLEND);
    }
    
    // Crosshair for FPS
    if (g_cameraMode == CAMERA_FPS || g_cameraMode == CAMERA_FLY) {
        glColor3f(1.0f, 1.0f, 1.0f);
        int cx = g_windowWidth/2;
        int cy = g_windowHeight/2;
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0.0, (double)g_windowWidth, 0.0, (double)g_windowHeight);
        
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        
        glBegin(GL_LINES);
        glVertex2i(cx - 10, cy); glVertex2i(cx + 10, cy);
        glVertex2i(cx, cy - 10); glVertex2i(cx, cy + 10);
        glEnd();
        
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
    }
}
