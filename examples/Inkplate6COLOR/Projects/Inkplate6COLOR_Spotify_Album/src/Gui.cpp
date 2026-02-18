#include "Gui.h"

Gui::Gui(Inkplate& disp) : display(disp)
{
}

void Gui::begin()
{
    display.begin();
}

void Gui::drawWrappedText(const String& text, int x, int y, int w, int h, int lineGapPx)
{
    int16_t x1, y1;
    uint16_t tw, th;

    int cursorX = x;
    int cursorY = y;

    String remaining = text;
    remaining.trim();

    while (remaining.length() > 0)
    {
        display.getTextBounds("Ag", 0, 0, &x1, &y1, &tw, &th);
        if (cursorY + (int)th > y + h) break;

        int bestCut = -1;
        int cut = 0;

        bool hasSpace = remaining.indexOf(' ') >= 0;

        while (cut < (int)remaining.length())
        {
            int nextSpace = remaining.indexOf(' ', cut);
            if (nextSpace < 0) nextSpace = remaining.length();

            String candidate = remaining.substring(0, nextSpace);
            candidate.trim();

            display.getTextBounds(candidate.c_str(), 0, 0, &x1, &y1, &tw, &th);
            if ((int)tw <= w)
            {
                bestCut = nextSpace;
                cut = nextSpace + 1;
                if (nextSpace == (int)remaining.length()) break;
            }
            else
            {
                break;
            }
        }

        if (bestCut < 0)
        {
            if (!hasSpace)
            {
                int hard = min((int)remaining.length(), 20);
                String line = remaining.substring(0, hard);
                display.setCursor(cursorX, cursorY);
                display.print(line);
                remaining = remaining.substring(hard);
                remaining.trim();
            }
            else
            {
                int sp = remaining.indexOf(' ');
                if (sp < 0) sp = remaining.length();
                String line = remaining.substring(0, sp);
                display.setCursor(cursorX, cursorY);
                display.print(line);
                remaining = remaining.substring(sp);
                remaining.trim();
            }
        }
        else
        {
            String line = remaining.substring(0, bestCut);
            line.trim();
            display.setCursor(cursorX, cursorY);
            display.print(line);
            remaining = remaining.substring(bestCut);
            remaining.trim();
        }

        cursorY += (int)th + lineGapPx;
    }
}

void Gui::renderNothingPlaying()
{
    display.setRotation(1);
    Serial.println("Rendering: Nothing playing screen");
    display.clearDisplay();

    display.fillRect(0, 0, display.width(), display.height(), BG_DARK);

    display.setTextColor(FG_LIGHT);
    display.setFont(&FreeSansBold24pt7b);

    const char* msg = "Nothing playing";
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(msg, 0, 0, &x1, &y1, &w, &h);

    int cx = (display.width() - (int)w) / 2;
    int cy = (display.height() + (int)h) / 2;

    display.setCursor(cx, cy);
    display.print(msg);

    display.display();
}

void Gui::renderAlbumScreen(const String& albumName,
                            const String& artistName,
                            const String& trackName,
                            const String& imageUrl,
                            uint32_t progressMs,
                            uint32_t durationMs)
{
    Serial.println("Rendering Album Screen");

    // ---- Base canvas ----
    display.clearDisplay();
    display.image.setDitherKernel(ImageColor::DitherKernel::JarvisJudiceNinke);
    display.setRotation(3); // set for portrait orientation on your panel
    display.fillRect(0, 0, display.width(), display.height(), INKPLATE_BLACK);
    display.setTextColor(INKPLATE_WHITE);

    const int W = display.width();   // expected 488 (portrait)
    const int H = display.height();  // expected 600 (portrait)

    // ---- Cover placement (portrait-friendly) ----
    const int coverSize = 300;
    const int coverX = (W - coverSize) / 2;

    // Put cover near the top so we have room for text + controls under it
    const int topMargin = 24;
    const int coverY = topMargin;

    // Cover frame + image
    display.drawRect(coverX - 6, coverY - 6, coverSize + 12, coverSize + 12, INKPLATE_WHITE);

    bool imgOk = display.image.drawJpegFromWeb(imageUrl.c_str(), coverX, coverY, 1, 0);
    if (!imgOk)
    {
        Serial.println("WARN: Failed to draw cover image.");
        display.drawRect(coverX, coverY, coverSize, coverSize, INKPLATE_WHITE);
        display.setFont(&FreeSans12pt7b);

        int16_t x1, y1;
        uint16_t tw, th;
        const char* msg = "Cover image failed";
        display.getTextBounds(msg, 0, 0, &x1, &y1, &tw, &th);
        int tx = (W - (int)tw) / 2 - x1;
        int ty = coverY + coverSize / 2;
        display.setCursor(tx, ty);
        display.print(msg);
    }

    // ---- Text + progress layout (portrait-friendly stack) ----
    const int padX = 36;                    // smaller side padding for 488px wide
    const int maxTextW = W - (padX * 2);

    // Helper: center + ellipsize to maxTextW at a given baseline Y
    auto drawCenteredEllipsized = [&](const String& in, const GFXfont* font, int baselineY)
    {
        display.setFont(font);

        String s = in;
        int16_t x1, y1;
        uint16_t tw, th;

        display.getTextBounds(s.c_str(), 0, 0, &x1, &y1, &tw, &th);
        if ((int)tw > maxTextW)
        {
            String base = s;
            const String ell = "...";
            while (base.length() > 1)
            {
                String candidate = base + ell;
                display.getTextBounds(candidate.c_str(), 0, 0, &x1, &y1, &tw, &th);
                if ((int)tw <= maxTextW)
                {
                    s = candidate;
                    break;
                }
                base.remove(base.length() - 1);
            }
        }

        display.getTextBounds(s.c_str(), 0, 0, &x1, &y1, &tw, &th);
        int tx = (W - (int)tw) / 2 - x1;
        display.setCursor(tx, baselineY);
        display.print(s);
    };

    // Track title (primary). If trackName empty, fall back to albumName.
    const String title = trackName.length() ? trackName : albumName;

    // Baselines under cover (tuned for 488x600)
    const int textTopY = coverY + coverSize + 42; // first baseline under cover
    const int titleY   = textTopY;                // bold 12pt
    const int artistY  = titleY + 30;             // 9pt
    const int albumY   = artistY + 24;            // 9pt

    drawCenteredEllipsized(title,      &FreeSansBold12pt7b, titleY);
    drawCenteredEllipsized(artistName, &FreeSans9pt7b,      artistY);
    drawCenteredEllipsized(albumName,  &FreeSans9pt7b,      albumY);

    // Progress bar under text
    const int barX = padX;
    const int barW = W - (padX * 2);
    const int barH = 10;
    const int barY = albumY + 26;

    display.drawRect(barX, barY, barW, barH, INKPLATE_WHITE);

    float progress = 0.0f;
    if (durationMs > 0)
    {
        if (progressMs > durationMs) progressMs = durationMs; // clamp
        progress = (float)progressMs / (float)durationMs;
    }

    int fillW = (int)(barW * progress);
    display.fillRect(barX + 1, barY + 1, max(0, fillW - 2), barH - 2, INKPLATE_WHITE);

    // Time labels
    display.setFont(&FreeSans9pt7b);
    String leftTime = formatMs(progressMs);
    String rightTime = formatMs(durationMs);

    const int timeY = barY + 28;
    display.setCursor(barX, timeY);
    display.print(leftTime);

    {
        int16_t x1, y1;
        uint16_t tw, th;
        display.getTextBounds(rightTime.c_str(), 0, 0, &x1, &y1, &tw, &th);
        display.setCursor(barX + barW - (int)tw, timeY);
        display.print(rightTime);
    }

    // Controls
    const int controlsY = timeY + 60;
    const int cx = W / 2;

    // Previous
    display.fillTriangle(cx - 170, controlsY,
                         cx - 130, controlsY - 24,
                         cx - 130, controlsY + 24,
                         INKPLATE_WHITE);
    display.fillRect(cx - 182, controlsY - 24, 8, 48, INKPLATE_WHITE);

    // Play/Pause (circle + bars) -- same as your original "pause" icon
    display.drawCircle(cx, controlsY, 38, INKPLATE_WHITE);
    {
        const int barW2 = 9;
        const int barH2 = 32;
        const int gap = 10;

        int leftBarX  = cx - gap / 2 - barW2;
        int rightBarX = cx + gap / 2;
        int barTopY   = controlsY - barH2 / 2;

        display.fillRect(leftBarX,  barTopY, barW2, barH2, INKPLATE_WHITE);
        display.fillRect(rightBarX, barTopY, barW2, barH2, INKPLATE_WHITE);
    }

    // Next
    display.fillTriangle(cx + 170, controlsY,
                         cx + 130, controlsY - 24,
                         cx + 130, controlsY + 24,
                         INKPLATE_WHITE);
    display.fillRect(cx + 174, controlsY - 24, 8, 48, INKPLATE_WHITE);

    // ---- Footer ----
    display.setFont(&FreeSans9pt7b);
    display.setCursor(padX, H - 22);

    display.display();
}


String Gui::formatMs(uint32_t ms){
    uint32_t totalSeconds = ms / 1000;
    uint32_t minutes = totalSeconds / 60;
    uint32_t seconds = totalSeconds % 60;

    char buf[16];
    snprintf(buf, sizeof(buf), "%lu:%02lu",
             (unsigned long)minutes, (unsigned long)seconds);
    return String(buf);
}