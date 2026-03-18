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
    display.setRotation(0);
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
    display.setRotation(3); 
    display.fillRect(0, 0, display.width(), display.height(), 0);
    display.setTextColor(7);

    const int W = display.width();   // expected 1200
    const int H = display.height();  // expected 825


    // Hardcoded cover art: 640x640, centered horizontally
    const int coverSize = 640;
    const int coverX = (W - coverSize) / 2;
    const int coverY = (W - coverSize) / 2;; 

    // Header 
    display.setFont(&FreeSans9pt7b);
    display.setCursor(80, 55);
    //display.print("text");


    // Cover frame + image
    display.drawRect(coverX - 6, coverY - 6, coverSize + 12, coverSize + 12, 7);

    bool imgOk = display.image.drawJpegFromWeb(imageUrl.c_str(), coverX, coverY, 1, 0);
    if (!imgOk)
    {
        Serial.println("WARN: Failed to draw cover image.");
        display.drawRect(coverX, coverY, coverSize, coverSize, 7);
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
    

    // Text area under cover (Spotify-like hierarchy) 
    const int padX = 80;
    const int maxTextW = W - (padX * 2);

    const int yShift=50;

    // Track title (primary). If trackName empty, fall back to albumName.
    String title = trackName.length() ? trackName : albumName;

    display.setFont(&FreeSansBold24pt7b);
    {
        // Ellipsize title to fit maxTextW
        int16_t x1, y1;
        uint16_t tw, th;
        display.getTextBounds(title.c_str(), 0, 0, &x1, &y1, &tw, &th);

        if ((int)tw > maxTextW)
        {
            String base = title;
            const String ell = "...";
            while (base.length() > 1)
            {
                String candidate = base + ell;
                display.getTextBounds(candidate.c_str(), 0, 0, &x1, &y1, &tw, &th);
                if ((int)tw <= maxTextW)
                {
                    title = candidate;
                    break;
                }
                base.remove(base.length() - 1);
            }
        }

        display.getTextBounds(title.c_str(), 0, 0, &x1, &y1, &tw, &th);
        int tx = (W - (int)tw) / 2 - x1;
        int ty = coverY + coverSize + 85 +yShift;
        display.setCursor(tx, ty);
        display.print(title);
    }

    // Artist (secondary)
    display.setFont(&FreeSans18pt7b);
    {
        String artist = artistName;
        int16_t x1, y1;
        uint16_t tw, th;
        display.getTextBounds(artist.c_str(), 0, 0, &x1, &y1, &tw, &th);

        if ((int)tw > maxTextW)
        {
            String base = artist;
            const String ell = "...";
            while (base.length() > 1)
            {
                String candidate = base + ell;
                display.getTextBounds(candidate.c_str(), 0, 0, &x1, &y1, &tw, &th);
                if ((int)tw <= maxTextW)
                {
                    artist = candidate;
                    break;
                }
                base.remove(base.length() - 1);
            }
        }

        display.getTextBounds(artist.c_str(), 0, 0, &x1, &y1, &tw, &th);
        int tx = (W - (int)tw) / 2 - x1;
        int ty = coverY + coverSize + 135+yShift;
        display.setCursor(tx, ty);
        display.print(artist);
    }
    
    // Album (tertiary)
    display.setFont(&FreeSans12pt7b);
    {
        String alb = albumName;
        int16_t x1, y1;
        uint16_t tw, th;
        display.getTextBounds(alb.c_str(), 0, 0, &x1, &y1, &tw, &th);

        if ((int)tw > maxTextW)
        {
            String base = alb;
            const String ell = "...";
            while (base.length() > 1)
            {
                String candidate = base + ell;
                display.getTextBounds(candidate.c_str(), 0, 0, &x1, &y1, &tw, &th);
                if ((int)tw <= maxTextW)
                {
                    alb = candidate;
                    break;
                }
                base.remove(base.length() - 1);
            }
        }

        display.getTextBounds(alb.c_str(), 0, 0, &x1, &y1, &tw, &th);
        int tx = (W - (int)tw) / 2 - x1;
        int ty = coverY + coverSize + 170+yShift;
        display.setCursor(tx, ty);
        display.print(alb);
    }

    
    const int barX = padX;
    const int barW = W - (padX * 2);
    const int barH = 10;
    const int barY = coverY + coverSize + 215+yShift;

    display.drawRect(barX, barY, barW, barH, 7);

    float progress = 0.0f;
    if (durationMs > 0)
    {
        if (progressMs > durationMs) progressMs = durationMs; // clamp
        progress = (float)progressMs / (float)durationMs;
    }
    
    int fillW = (int)(barW * progress);
    display.fillRect(barX + 1, barY + 1, max(0, fillW - 2), barH - 2, 7);

    // Time labels (placeholders)
    display.setFont(&FreeSans9pt7b);
    String leftTime = formatMs(progressMs);
    String rightTime = formatMs(durationMs);

    display.setCursor(barX, barY + 30);
    display.print(leftTime);

    {
        int16_t x1, y1;
        uint16_t tw, th;
        display.getTextBounds(rightTime.c_str(), 0, 0, &x1, &y1, &tw, &th);
        display.setCursor(barX + barW - (int)tw, barY + 30);
        display.print(rightTime);
    }

    
    const int controlsY = barY + 85;
    const int cx = W / 2;

    // Previous
    display.fillTriangle(cx - 220, controlsY,
                         cx - 170, controlsY - 28,
                         cx - 170, controlsY + 28,
                         7);
    display.fillRect(cx - 232, controlsY - 28, 10, 56, 7);

    // Play (circle + triangle)
    display.drawCircle(cx, controlsY, 42, 7);
    {
        const int barW2 = 10;
        const int barH2 = 36;
        const int gap = 10;

        int leftBarX  = cx - gap/2 - barW2;
        int rightBarX = cx + gap/2;
        int barTopY   = controlsY - barH2/2;

        display.fillRect(leftBarX,  barTopY, barW2, barH2, 7);
        display.fillRect(rightBarX, barTopY, barW2, barH2, 7);
    }

    // Next
    display.fillTriangle(cx + 220, controlsY,
                         cx + 170, controlsY - 28,
                         cx + 170, controlsY + 28,
                         7);
    display.fillRect(cx + 222, controlsY - 28, 10, 56, 7);

    // ---- Footer ----
    display.setFont(&FreeSans9pt7b);
    display.setCursor(padX, H - 35);


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