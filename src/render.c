#include "vanilla.h"
#include <string.h>

#define LERP(textColor, backColor, lum) ((((textColor) >> 8) * lum + ((backColor) >> 8) * (256 - lum)) | 0xffU);

void draw(
    Input *input,
    FileView *file,
    Syntax *syntax,
    Layout *layout,
    Theme *theme,
    AllFontCaches *fonts,
    Rect *area,
    uint32_t *image
) {
    int width = layout->windowWidth;
    if (!area || area->y < layout->topNavHeight) {
        for (int y = 0; y < layout->toolBarHeight; y++) {
            // draw toolbar (File, Edit, View, Help)
            for (int x = 0, i = 0; x < layout->toolBarWidth; x += fonts->toolBarFont.pxGlyphWidth, i++) {
                uint32_t ch = (uint32_t)(" File  Edit  View  Help "[i]) & 0xff;
                GlyphDesc gd = *(GlyphDesc*)&ch;
                uint8_t *image = getFontGlyph(&fonts->toolBarFont, gd) + (fonts->toolBarFont.pxGlyphWidth * y);
                for (int j = 0; j < fonts->toolBarFont.pxGlyphWidth; j++)
                    image[x + j + width * y] = LERP(theme->foreToolBar, theme->backToolBar, image[j]);
            }
            for (int x = layout->toolBarWidth; x < layout->windowWidth; x++) {
                image[x + width * y] = theme->backToolBar;
            }
        }
        for (int y = layout->toolBarHeight; y < layout->topNavHeight; y++) {
            // draw list of folders from topNavPanX
        }
    }

    int xStart = layout->sideNavWidth;
    int xEnd = layout->windowWidth;
    int yStart = layout->topNavHeight;
    int yEnd = layout->windowHeight - layout->statusHeight;
    if (area) {
        if (area->x > xStart)
            xStart = area->x;
        if (area->x + area->w > xEnd)
            xEnd = area->x + area->w;
        if (area->y > yStart)
            yStart = area->y;
        if (area->y + area->h < yEnd)
            yEnd = area->y + area->h;
    }

    for (int y = yStart; y < yEnd; y++) {
        if (!area || area->x < layout->sideNavWidth) {
            for (int x = 0; x < layout->sideNavWidth; x++) {
                // draw list of files from sideNavPanY
            }
        }
        for (int x = xStart; x < xEnd; x++) {
            // draw file text
        }
        if (y >= layout->scrollY && y < layout->scrollY + layout->scrollHeight) {
            for (int x = layout->windowWidth - layout->scrollWidth; x < layout->windowWidth; x++) {
                
            }
        }
        if (y >= layout->scrollY && y < layout->scrollY + layout->scrollHeight) {
            for (int x = layout->windowWidth - layout->scrollWidth; x < layout->windowWidth; x++) {
                
            }
        }
    }

    // ALWAYS draw status. Even if it would be excluded, still draw the status area anyway.
    // TODO: maybe check if the status didn't change as well, then skip if it's also outside the invalidation area

    int statusX = 0;
    if (area && area->y + area->h < layout->windowHeight - layout->statusHeight)
        statusX = layout->windowWidth - layout->statusWidth;

    for (int y = layout->windowHeight - layout->statusHeight; y < layout->windowHeight; y++) {
        for (int x = statusX; x < layout->windowWidth; x++) {
            // draw status
        }
    }
}
