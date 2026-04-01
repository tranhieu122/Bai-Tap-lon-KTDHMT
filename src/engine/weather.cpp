#include "weather.h"
#include "sky.h"
#include "particles.h"

void weatherInit() {
    g_weather = WEATHER_CLEAR;
    g_rainEnabled = false;
    g_fogEnabled = false;
}

void weatherUpdate(float dt) {
    if (g_weather == WEATHER_RAIN) {
        g_rainEnabled = true;
        g_fogEnabled = true; // Rain usually comes with limited visibility
    } else if (g_weather == WEATHER_FOG) {
        g_rainEnabled = false;
        g_fogEnabled = true;
    } else {
        g_rainEnabled = false;
        g_fogEnabled = false;
    }
}
