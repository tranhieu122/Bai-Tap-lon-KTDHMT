#include "sky.h"
#include "../engine/globals.h"
#include "../engine/texture.h"
#include "../engine/utils.h"

// ============================================================
// SKY STATE
// ============================================================
namespace {
    struct CloudLayer {
        float alt;
        float speedScale;
        float density;
        Vector3 offset;
    };
    
    CloudLayer clouds[3];
    
    // Day cycle
    float realTimeToGameHours = 24.0f / DAY_CYCLE_DURATION;
    
    // Light limits
    Color ambientColor(0.2f, 0.2f, 0.2f);
    Color dirLightColor(1.0f, 1.0f, 1.0f);
    Vector3 lightDir(0.0f, -1.0f, 0.0f); // pointing down
    
    // Precalculated sky colors based on time
    Color skyTop, skyBottom;
}

// ============================================================
// INITIALIZATION
// ============================================================
void skyInit() {
    g_timeOfDay = 12.0f; // Noon
    
    // Init cloud layers
    clouds[0] = { 100.0f, 1.0f, 0.6f, Vector3(0,0,0) };
    clouds[1] = { 150.0f, 0.6f, 0.4f, Vector3(100,0,200) };
    clouds[2] = { 200.0f, 0.3f, 0.3f, Vector3(-200,0,-100) };
}

// ============================================================
// UPDATE LOGIC
// ============================================================
void skyUpdate(float dt) {
    if (!g_paused) {
        // Calculate dynamic realTimeToGameHours
        // 10 minutes total (600s). 
        // 7 minutes for Day (6:00 to 18:00) -> 12 hours in 420s -> 12/420 = 0.02857
        // 3 minutes for Night (18:00 to 6:00) -> 12 hours in 180s -> 12/180 = 0.06667
        
        float rate = (g_timeOfDay >= 6.0f && g_timeOfDay < 18.0f) ? (12.0f / 420.0f) : (12.0f / 180.0f);
        
        g_timeOfDay += dt * rate * g_animSpeed;
        if (g_timeOfDay >= 24.0f) g_timeOfDay -= 24.0f;
    }
    
    // Update simple procedural cloud offsets
    Vector3 windDir(1.0f, 0.0f, 0.5f);
    windDir = windDir.normalized();
    
    for (int i = 0; i < 3; i++) {
        clouds[i].offset += windDir * CLOUD_SPEED * clouds[i].speedScale * dt * g_animSpeed;
    }
    
    // --------------------------------------------------------
    // Calculate lighting and exact colors based on hour
    // --------------------------------------------------------
    float h = g_timeOfDay;
    
    if (h >= 5.0f && h < 7.0f) { // Dawn (5 - 7)
        float t = (h - 5.0f) / 2.0f;
        skyTop = Palette::SKY_NIGHT.lerp(Palette::SKY_DAWN, t);
        skyBottom = Palette::SKY_NIGHT.lerp(Palette::SKY_DAWN, t);
        ambientColor = Color(0.1f, 0.1f, 0.2f).lerp(Color(0.4f, 0.3f, 0.3f), t);
        dirLightColor = Color(0.1f, 0.1f, 0.2f).lerp(Palette::SUN_COLOR, t * 0.5f);
    } 
    else if (h >= 7.0f && h < 10.0f) { // Morning (7 - 10)
        float t = (h - 7.0f) / 3.0f;
        skyTop = Palette::SKY_DAWN.lerp(Palette::SKY_DAY, t);
        skyBottom = Palette::SKY_DAWN.lerp(Palette::SKY_DAY.lerp(Color(1.0f,1.0f,1.0f),0.4f), t);
        ambientColor = Color(0.4f, 0.3f, 0.3f).lerp(Color(0.6f, 0.6f, 0.6f), t);
        dirLightColor = Palette::SUN_COLOR;
    }
    else if (h >= 10.0f && h < 16.0f) { // Day (10 - 16)
        skyTop = Palette::SKY_DAY;
        skyBottom = Palette::SKY_DAY.lerp(Color(1.0f,1.0f,1.0f), 0.4f);
        ambientColor = Color(0.6f, 0.6f, 0.6f);
        dirLightColor = Palette::SUN_COLOR;
    }
    else if (h >= 16.0f && h < 18.0f) { // Chieu + Gio Vang (16 - 18)
        float t = (h - 16.0f) / 2.0f;
        skyTop = Palette::SKY_DAY.lerp(Palette::SKY_SUNSET, t);
        skyBottom = Palette::SKY_GOLDEN_HOUR.lerp(Palette::SKY_SUNSET, t);
        ambientColor = Color(0.6f, 0.6f, 0.6f).lerp(Color(0.55f, 0.40f, 0.28f), t);
        dirLightColor = Palette::SUN_COLOR.lerp(Color(1.0f, 0.50f, 0.18f), t);
    }
    else if (h >= 18.0f && h < 19.5f) { // Hoang hon (18 - 19.5)
        float t = (h - 18.0f) / 1.5f;
        skyTop = Palette::SKY_SUNSET.lerp(Palette::SKY_NIGHT, t);
        skyBottom = Palette::SKY_SUNSET.lerp(Palette::SKY_NIGHT, t);
        ambientColor = Color(0.5f, 0.4f, 0.3f).lerp(Color(0.08f, 0.08f, 0.12f), t);
        dirLightColor = Color(1.0f, 0.5f, 0.2f).lerp(Color(0.08f, 0.08f, 0.18f), t);
    }
    else { // Dem (19.5 - 5)
        skyTop = Palette::SKY_NIGHT;
        skyBottom = Palette::SKY_NIGHT;
        ambientColor = Color(0.08f, 0.08f, 0.12f);
        dirLightColor = Color(0.08f, 0.08f, 0.25f); // Moonlight - xanh nhat
    }
    
    // Apply weather modifiers
    if (g_weather == WEATHER_RAIN || g_weather == WEATHER_FOG) {
        float mix = (g_weather == WEATHER_RAIN) ? 0.6f : 0.8f;
        Color gray(0.4f, 0.4f, 0.4f);
        skyTop = skyTop.lerp(gray, mix);
        skyBottom = skyBottom.lerp(gray, mix);
        ambientColor = ambientColor.lerp(gray, mix * 0.5f);
        dirLightColor = dirLightColor.lerp(gray, mix * 0.8f);
    }
    
    // Sun position (-Z is East, +Z is West roughly)
    // Rises at 6:00 (180 deg), Sets at 18:00 (0 deg)
    float sunAngle = (h - 6.0f) / 12.0f * (float)M_PI;
    lightDir = Vector3(
        cosf(sunAngle) * 0.5f,
        -sinf(sunAngle),
        cosf(sunAngle) * 0.8f
    ).normalized();
    
    // If night, light dir points opposite (from moon)
    if (h < 6.0f || h > 18.0f) {
        float moonAngle = (h - 18.0f) / 12.0f * (float)M_PI;
        if (h < 6.0f) moonAngle = (h + 6.0f) / 12.0f * (float)M_PI;
        
        lightDir = Vector3(
            cosf(moonAngle) * 0.5f,
            -sinf(moonAngle),
            cosf(moonAngle) * 0.8f
        ).normalized();
    }
}

// ============================================================
// DRAWING
// ============================================================
void skyDraw() {
    // We draw sky as a large inverted hemisphere
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    
    // Dome center based on camera so it never moves past
    // Note: defined globally but accessed through globals? Actually camera pos
    // Wait, let's just use big coordinates relative to center
    
    glPushMatrix();
    glTranslatef(0, -20.0f, 0); // slightly below horizon
    
    float radius = 500.0f;
    int slices = 32;
    int stacks = 16;
    
    for (int i = 0; i < stacks; i++) {
        float phi1 = (float)M_PI * 0.5f * (float)i / stacks;
        float phi2 = (float)M_PI * 0.5f * (float)(i + 1) / stacks;
        
        float t1 = (float)i / stacks;
        float t2 = (float)(i + 1) / stacks;
        
        Color c1 = skyBottom.lerp(skyTop, t1);
        Color c2 = skyBottom.lerp(skyTop, t2);
        
        // Add horizon haze effect
        if (i < stacks / 4) {
            float haze = 1.0f - (float)i / (stacks / 4);
            Color hazeCol = Color(0.8f, 0.85f, 0.95f, 0.4f); // soft white-blue haze
            if (g_timeOfDay > 17.0f || g_timeOfDay < 7.0f) hazeCol = Palette::SKY_DAWN.lerp(Color(1.0f,0.5f,0.2f), 0.5f);
            c1 = c1.lerp(hazeCol, haze * 0.5f);
            c2 = c2.lerp(hazeCol, haze * 0.3f);
        }

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; j++) {
            float theta = 2.0f * (float)M_PI * (float)j / slices;
            
            float x1 = cosf(phi1) * cosf(theta);
            float y1 = sinf(phi1);
            float z1 = cosf(phi1) * sinf(theta);
            
            float x2 = cosf(phi2) * cosf(theta);
            float y2 = sinf(phi2);
            float z2 = cosf(phi2) * sinf(theta);
            
            c2.apply();
            glVertex3f(radius * x2, radius * y2, radius * z2);
            
            c1.apply();
            glVertex3f(radius * x1, radius * y1, radius * z1);
        }
        glEnd();
    }
    glPopMatrix();
    
    // --------------------------------------------------------
    // Draw Sun and Moon
    // --------------------------------------------------------
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    float h = g_timeOfDay;
    // Sun
    if (h >= 5.0f && h <= 19.5f) {
        float sunAngle = (h - 6.0f) / 12.0f * (float)M_PI;
        Vector3 sunPos(
            -cosf(sunAngle) * 0.5f * 400.0f,
            sinf(sunAngle) * 400.0f,
            -cosf(sunAngle) * 0.8f * 400.0f
        );
        
        glPushMatrix();
        glTranslatef(sunPos.x, sunPos.y, sunPos.z);
        // Look at origin
        glRotatef(RAD2DEG(atan2f(sunPos.x, sunPos.z)), 0, 1, 0);
        glRotatef(RAD2DEG(asin(sunPos.y / 400.0f)), -1, 0, 0);
        
        // Sun disc
        Palette::SUN_COLOR.apply();
        drawQuad(40.0f, 40.0f);
        
        // Premium Sun Bloom (multilayered)
        glDepthMask(GL_FALSE);
        Color bloom1 = Palette::SKY_GOLDEN_HOUR; bloom1.a = 0.3f;
        bloom1.apply(); drawQuad(120.0f, 120.0f);
        
        Color bloom2 = Palette::SKY_DAWN; bloom2.a = 0.15f;
        bloom2.apply(); drawQuad(250.0f, 250.0f);
        
        if (h < 7.0f || h > 17.0f) {
            Color sunsetGlow(1.0f, 0.3f, 0.1f, 0.2f);
            sunsetGlow.apply(); drawQuad(400.0f, 300.0f);
        }
        glDepthMask(GL_TRUE);
        
        glPopMatrix();
    }
    
    // Moon
    if (h < 6.5f || h > 17.5f) {
        float moonAngle = (h - 18.0f) / 12.0f * (float)M_PI;
        if (h < 6.5f) moonAngle = (h + 6.0f) / 12.0f * (float)M_PI;
        
        Vector3 moonPos(
            -cosf(moonAngle) * 0.5f * 400.0f,
            sinf(moonAngle) * 400.0f,
            -cosf(moonAngle) * 0.8f * 400.0f
        );
        
        glPushMatrix();
        glTranslatef(moonPos.x, moonPos.y, moonPos.z);
        glRotatef(RAD2DEG(atan2f(moonPos.x, moonPos.z)), 0, 1, 0);
        glRotatef(RAD2DEG(asin(moonPos.y / 400.0f)), -1, 0, 0);
        
        // Moon glow (hao quang trang)
        Color moonGlow(0.7f, 0.75f, 0.9f, 0.25f);
        moonGlow.apply();
        drawQuad(80.0f, 80.0f);
        
        Palette::MOON_COLOR.apply();
        drawQuad(30.0f, 30.0f);
        glPopMatrix();
    }
    
    // === SAO DEM ===
    if (h < 5.5f || h > 19.0f) {
        glPointSize(2.0f);
        float starAlpha = 1.0f;
        if (h >= 5.0f && h < 5.5f) starAlpha = 1.0f - (h - 5.0f) / 0.5f;
        if (h >= 19.0f && h < 19.5f) starAlpha = (h - 19.0f) / 0.5f;
        
        // Draw stars using deterministic pattern based on index
        float starSeed = 42.0f;
        glBegin(GL_POINTS);
        for (int s = 0; s < 200; s++) {
            float sx = (fmod(s * 123.456f + starSeed, 1000.0f) - 500.0f) * 0.8f;
            float sy = 50.0f + fmod(s * 789.012f + starSeed, 350.0f);
            float sz = (fmod(s * 345.678f + starSeed, 1000.0f) - 500.0f) * 0.8f;
            float bright = 0.5f + fmod(s * 901.234f + starSeed, 50.0f) / 100.0f;
            // Nhap nhay nhe
            float twinkle = 0.8f + 0.2f * sinf(g_elapsedTime * (1.0f + s * 0.01f) + s * 1.7f);
            glColor4f(bright, bright, bright + 0.05f, starAlpha * twinkle);
            glVertex3f(sx, sy, sz);
        }
        // Shooting stars - deterministic based on elapsed time
        glEnd();
        glPointSize(1.0f);
    }
    
    // --------------------------------------------------------
    // Draw clouds (Improved Volumetric Clusters)
    // --------------------------------------------------------
    if (g_weather != WEATHER_RAIN) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        // Base cloud color depends on time of day
        Color cloudBase(1.0f, 1.0f, 1.0f);
        h = g_timeOfDay;
        
        if (h < 7.0f || h > 17.0f) { // Dawn/Sunset
            cloudBase = Color(1.0f, 0.7f, 0.6f); // Pinkish
            if (h < 5.5f || h > 19.5f) cloudBase = Color(0.2f, 0.25f, 0.35f); // Night dark clouds
        }
        
        for (int i = 0; i < 3; i++) {
            glPushMatrix();
            // Larger wrap for wider view
            float tx = fmodf(clouds[i].offset.x, 800.0f) - 400.0f;
            float tz = fmodf(clouds[i].offset.z, 800.0f) - 400.0f;
            glTranslatef(tx, clouds[i].alt, tz);
            
            // Draw a grid of cloud clusters
            for(int cx = -3; cx <= 3; cx++) {
                for(int cz = -3; cz <= 3; cz++) {
                    if ((cx + cz) % 2 != 0) continue; 

                    glPushMatrix();
                    glTranslatef(cx * 150.0f, noise1D(cx*3.0f+cz*7.0f)*15.0f, cz * 150.0f);
                    
                    // Each cluster has 6 "puffs" of varying size
                    for (int puff = 0; puff < 6; puff++) {
                        float seed = (float)puff + cx * 2.0f + cz * 3.0f;
                        float px = noise1D(seed * 1.123f) * 45.0f;
                        float py = noise1D(seed * 2.234f) * 12.0f;
                        float pz = noise1D(seed * 3.345f) * 45.0f;
                        float pSize = 25.0f + noise1D(seed * 4.456f) * 20.0f;
                        float pAlpha = clouds[i].density * (0.3f + noise1D(seed * 5.567f) * 0.45f);

                        glPushMatrix();
                        glTranslatef(px, py, pz);
                        glScalef(1.8f, 0.45f, 1.5f); // Flattened puffs
                        
                        Color puffCol = cloudBase;
                        puffCol.a = pAlpha;
                        puffCol.apply();
                        
                        drawHemisphere(pSize, 12, 6);
                        glPopMatrix();
                    }
                    glPopMatrix();
                }
            }
            glPopMatrix();
        }
    }
    
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
}

// Getters
Color skyGetAmbientLight() { return ambientColor; }
Color skyGetDirectionalLight() { return dirLightColor; }
Vector3 skyGetLightDirection() { return lightDir; }

TimeOfDay skyGetTimeState() {
    float h = g_timeOfDay;
    if (h >= 5.0f && h < 7.0f)  return TIME_DAWN;
    if (h >= 7.0f && h < 11.0f) return TIME_MORNING;
    if (h >= 11.0f && h < 14.0f) return TIME_NOON;
    if (h >= 14.0f && h < 17.0f) return TIME_AFTERNOON;
    if (h >= 17.0f && h < 18.5f) return TIME_SUNSET;
    if (h >= 18.5f && h < 19.5f) return TIME_DUSK;
    if (h >= 19.5f || h < 2.0f)  return TIME_NIGHT;
    return TIME_LATE_NIGHT;
}

void skySetTime(float hours) {
    g_timeOfDay = hours;
    while(g_timeOfDay >= 24.0f) g_timeOfDay -= 24.0f;
    while(g_timeOfDay < 0.0f) g_timeOfDay += 24.0f;
}

float skyGetTime() { return g_timeOfDay; }

void skySetWeather(WeatherType type) { g_weather = type; }
WeatherType skyGetWeather() { return g_weather; }
const char* skyGetWeatherName() {
    switch (g_weather) {
        case WEATHER_CLEAR: return "Quang Dang";
        case WEATHER_RAIN: return "Troi Mua";
        case WEATHER_FOG: return "Suong Mu";
        default: return "Khong Ro";
    }
}

void weatherEffectDraw() {
    // Note: Rain particles will actually be handled by the particle system
    // Here we handle the fog setup
    if (g_fogEnabled || g_weather != WEATHER_CLEAR) {
        glEnable(GL_FOG);
        glFogi(GL_FOG_MODE, GL_EXP2);
        
        float density = 0.002f; // Clear night/day baseline
        Color fogColor = skyBottom;
        
        if (g_weather == WEATHER_FOG) {
            density = 0.015f;
            fogColor = Color(0.6f, 0.6f, 0.65f).lerp(skyBottom, 0.3f);
        } 
        else if (g_weather == WEATHER_RAIN) {
            density = 0.012f; // Increased fog for rain
            fogColor = Color(0.3f, 0.32f, 0.35f).lerp(skyBottom, 0.2f); // Darker, gloomier fog
        }
        else if (g_fogEnabled) {
            density = 0.008f;
        }
        
        float col[] = { fogColor.r, fogColor.g, fogColor.b, 1.0f };
        glFogfv(GL_FOG_COLOR, col);
        glFogf(GL_FOG_DENSITY, density);
        glHint(GL_FOG_HINT, GL_NICEST);
    } else {
        glDisable(GL_FOG);
    }
}

