#pragma once
#include "includes.h"

class Gui
{
public:
    explicit Gui(Inkplate& disp);

    void begin();

    void renderNothingPlaying();

    void renderAlbumScreen(const String& albumName,
                           const String& artistName,
                           const String& trackName,
                           const String& imageUrl,
                           uint32_t progressMs,
                           uint32_t durationMs
                        );

private:
    Inkplate& display;

    void drawWrappedText(const String& text,
                         int x, int y, int w, int h,
                         int lineGapPx);
    String formatMs(uint32_t ms);
};
