#include "gui.h"
#include "sensorReadings.h"

static void drawLabelValue(int x, int y, const char *label, const String &value, int valueOffset = 150);
static void drawProgressBar(int x, int y, int w, int h, float percent, const String &caption);
static void drawMotionBadge(int x, int y, bool motionDetected);
static void drawTiltWidget(int x, int y, int w, int h, float pitchDeg, float rollDeg);

void guiInit()
{
    display.clearDisplay();
    display.setTextColor(BLACK);
    display.setTextWrap(false);
}

void guiDrawStaticFrame()
{
    display.clearDisplay();

    // Outer border
    display.drawRect(0, 0, SCREEN_W, SCREEN_H, BLACK);

    // Top status bar
    display.drawRect(MARGIN, MARGIN, SCREEN_W - 2 * MARGIN, TOP_H, BLACK);

    // Main row
    int mainY = MARGIN + TOP_H + GAP;

    display.drawRect(MARGIN, mainY, LEFT_W, MID_H, BLACK);
    display.drawRect(MARGIN + LEFT_W + GAP, mainY, CENTER_W, MID_H, BLACK);
    display.drawRect(MARGIN + LEFT_W + GAP + CENTER_W + GAP, mainY, RIGHT_W, MID_H, BLACK);

    // Bottom panel
    display.drawRect(MARGIN, SCREEN_H - MARGIN - BOTTOM_H, SCREEN_W - 2 * MARGIN, BOTTOM_H, BLACK);

    // Titles
    display.setTextSize(3);
    display.setCursor(MARGIN + 16, MARGIN + 18);
    display.print("Inkplate Sensor Hub");

    display.setTextSize(2);
    display.setCursor(MARGIN + 18, mainY + 16);
    display.print("ENVIRONMENT");

    display.setCursor(MARGIN + LEFT_W + GAP + 18, mainY + 16);
    display.print("TILT / IMU");

    display.setCursor(MARGIN + LEFT_W + GAP + CENTER_W + GAP + 18, mainY + 16);
    display.print("POWER / LIGHT / STATE");

    display.setCursor(MARGIN + 18, SCREEN_H - MARGIN - BOTTOM_H + 16);
    display.print("POTENTIOMETER");
}

void guiDrawData(const SensorData &data, uint32_t updateCount, bool fullRefresh)
{
    guiDrawStaticFrame();

    int mainY = MARGIN + TOP_H + GAP;

    // ---------------- Top bar ----------------
    display.setTextSize(2);

    display.setCursor(SCREEN_W - 310, MARGIN + 18);
    display.print(fullRefresh ? "FULL REFRESH" : "PARTIAL REFRESH");

    display.setCursor(SCREEN_W - 310, MARGIN + 44);
    display.print("Update #");
    display.print(updateCount);

    drawMotionBadge(SCREEN_W - 500, MARGIN + 16, data.pirDetected);

    // ---------------- Left panel: environment ----------------
    int lx = MARGIN + 22;
    int ly = mainY + 54;

    drawLabelValue(lx, ly + 0,   "Temp", data.bmeOk ? String(data.temperatureC, 1) + " C" : "--");
    drawLabelValue(lx, ly + 36,  "Hum",  data.bmeOk ? String(data.humidityPct, 1) + " %" : "--");
    drawLabelValue(lx, ly + 72,  "Pres", data.bmeOk ? String(data.pressurehPa, 1) + " hPa" : "--");

    drawLabelValue(lx, ly + 138, "Soil", data.soilOk ? String(soilPercentFromRaw(data.soilRaw), 0) + " %" : "--");
    drawLabelValue(lx, ly + 174, "Hall", data.hallOk ? String(hallPercentFromRaw(data.hallRaw), 0) + " %" : "--");

    display.setTextSize(2);
    display.setCursor(lx, ly + 240);
    display.print("Context");

    display.setTextSize(1);
    display.setCursor(lx, ly + 270);
    display.print("Temp / Hum / Pres from BME280");

    display.setCursor(lx, ly + 288);
    display.print("Soil: estimated moisture percent");

    display.setCursor(lx, ly + 306);
    display.print("Hall: changes when magnet moves");

    // ---------------- Center panel: IMU ----------------
    int cx = MARGIN + LEFT_W + GAP + 20;
    int cy = mainY + 52;
    int cw = CENTER_W - 40;
    int ch = MID_H - 72;

    drawTiltWidget(cx, cy, cw, ch, data.pitchDeg, data.rollDeg);

    // ---------------- Right panel ----------------
    int rx = MARGIN + LEFT_W + GAP + CENTER_W + GAP + 22;
    int ry = mainY + 54;

    drawLabelValue(rx, ry + 0,   "Bus",    data.inaOk ? String(data.busVoltageV, 2) + " V" : "--");
    drawLabelValue(rx, ry + 36,  "Curr",   data.inaOk ? String(data.currentmA, 1) + " mA" : "--");
    drawLabelValue(rx, ry + 72,  "Power",  data.inaOk ? String(data.powermW, 1) + " mW" : "--");
    drawLabelValue(rx, ry + 210, "Motion", data.pirOk ? String(data.pirDetected ? "DETECTED" : "idle") : "--");

    display.setTextSize(1);
    display.setCursor(rx, ry + 270);
    display.print("INA219 = electrical measurements");

    display.setCursor(rx, ry + 306);
    display.print("PIR = movement trigger");

    // ---------------- Bottom panel: potentiometer ----------------
    float potPercent = data.potOk ? potPercentFromRaw(data.potentiometer) : 0.0f;

    drawProgressBar(
        MARGIN + 24,
        SCREEN_H - MARGIN - BOTTOM_H + 48,
        SCREEN_W - 2 * MARGIN - 48,
        36,
        potPercent,
        data.potOk ? String((int)potPercent) + "%" : "--"
    );
}

static void drawLabelValue(int x, int y, const char *label, const String &value, int valueOffset)
{
    display.setTextSize(2);
    display.setCursor(x, y);
    display.print(label);

    display.setCursor(x + valueOffset, y);
    display.print(value);
}

static void drawProgressBar(int x, int y, int w, int h, float percent, const String &caption)
{
    percent = clampf(percent, 0.0f, 100.0f);

    display.drawRect(x, y, w, h, BLACK);

    int fillWidth = (int)((w - 4) * percent / 100.0f);
    if (fillWidth > 0)
    {
        display.fillRect(x + 2, y + 2, fillWidth, h - 4, BLACK);
    }

    display.setTextSize(2);

    int textX = x + (w / 2) - 28;
    int textY = y + 8;

    if (fillWidth > w / 2)
    {
        display.setTextColor(WHITE);
    }
    else
    {
        display.setTextColor(BLACK);
    }

    display.setCursor(textX, textY);
    display.print(caption);

    display.setTextColor(BLACK);
}

static void drawMotionBadge(int x, int y, bool motionDetected)
{
    int w = 150;
    int h = 34;

    display.drawRect(x, y, w, h, BLACK);

    if (motionDetected)
    {
        display.fillRect(x + 2, y + 2, w - 4, h - 4, BLACK);
        display.setTextColor(WHITE);
    }
    else
    {
        display.setTextColor(BLACK);
    }

    display.setTextSize(2);
    display.setCursor(x + 16, y + 8);
    display.print(motionDetected ? "MOTION" : "STILL");

    display.setTextColor(BLACK);
}

static void drawTiltWidget(int x, int y, int w, int h, float pitchDeg, float rollDeg)
{
    display.drawRect(x, y, w, h, BLACK);

    int centerX = x + w / 2;
    int centerY = y + h / 2;

    // Crosshair
    display.drawLine(centerX, y + 24, centerX, y + h - 24, BLACK);
    display.drawLine(x + 24, centerY, x + w - 24, centerY, BLACK);

    // Ball position based on pitch / roll
    int dx = (int)(clampf(rollDeg, -45.0f, 45.0f) / 45.0f * (w / 3));
    int dy = (int)(clampf(pitchDeg, -45.0f, 45.0f) / 45.0f * (h / 3));

    int ballX = centerX + dx;
    int ballY = centerY + dy;

    display.drawCircle(ballX, ballY, 18, BLACK);
    display.fillCircle(ballX, ballY, 9, BLACK);

    // Rotating square impression using roll
    float angle = rollDeg * PI / 180.0f;
    int size = 78;

    int x1 = centerX + (int)(-size * cos(angle) - (-size) * sin(angle));
    int y1 = centerY + (int)(-size * sin(angle) + (-size) * cos(angle));

    int x2 = centerX + (int)( size * cos(angle) - (-size) * sin(angle));
    int y2 = centerY + (int)( size * sin(angle) + (-size) * cos(angle));

    int x3 = centerX + (int)( size * cos(angle) - size * sin(angle));
    int y3 = centerY + (int)( size * sin(angle) + size * cos(angle));

    int x4 = centerX + (int)(-size * cos(angle) - size * sin(angle));
    int y4 = centerY + (int)(-size * sin(angle) + size * cos(angle));

    display.drawLine(x1, y1, x2, y2, BLACK);
    display.drawLine(x2, y2, x3, y3, BLACK);
    display.drawLine(x3, y3, x4, y4, BLACK);
    display.drawLine(x4, y4, x1, y1, BLACK);

    // Text values
    display.setTextSize(2);
    display.setCursor(x + 18, y + h - 58);
    display.print("Pitch: ");
    display.print(pitchDeg, 1);
    display.print(" deg");

    display.setCursor(x + 18, y + h - 30);
    display.print("Roll : ");
    display.print(rollDeg, 1);
    display.print(" deg");
}