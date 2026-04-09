#include "vanilla.h"
#include <string.h>

#define LERP(textColor, backColor, lum) \
    (((textColor & 0xff) * lum + (backColor & 0xff) * (255 - lum)) >> 8) | \
    (((((textColor >>  8) & 0xff) * lum + ((backColor >>  8) & 0xff) * (255 - lum)) >> 8) <<  8) | \
    (((((textColor >> 16) & 0xff) * lum + ((backColor >> 16) & 0xff) * (255 - lum)) >> 8) << 16)

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
    /*
    if (!area || area->y < layout->topNavHeight) {
        for (int y = 0; y < layout->toolBarHeight; y++) {
            for (int x = 0; x < layout->toolBarWidth; x++) {
                // draw toolbar (File, Edit, View, Help)
            }
            for (int x = layout->toolBarWidth; x < layout->windowWidth; x++) {
                image[x + width * y] = theme->backToolBar;
            }
        }
        for (int y = layout->toolBarHeight; y < layout->topNavHeight; y++) {
            // draw list of folders from topNavPanX
        }
    }
    */

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

    //log_info("xStart: %d, xEnd: %d, yStart: %d, yEnd: %d\n", xStart, xEnd, yStart, yEnd);

    int chH = (fonts->editorFont.size * 8) / 5;
    int chW = (fonts->editorFont.size * 5) / 8;

    int lineStart = 0;

    // minus 1 to avoid an X11 bug
    for (int y = yStart; y < yEnd - 1; y++) {
        /*
        log_info("y: %d", y);
        if (!area || area->x < layout->sideNavWidth) {
            for (int x = 0; x < layout->sideNavWidth; x++) {
                // draw list of files from sideNavPanY
            }
        }
        */
        int idx = lineStart;
        int foundNl = 0;
        int row = (y - yStart) / chH;
        int chY = (y - yStart) % chH;

        for (int x = xStart; x < xEnd; x += chW) {
            if (idx >= file->size || file->buf[idx] == '\n')
                foundNl = 1;

            if (!foundNl && idx < file->size) {
                //int col = (x - xStart) / chW;
                //int chX = (x - xStart) % chW;
                uint32_t codepoint = (uint32_t)file->buf[idx];
                GlyphDesc gd = *(GlyphDesc*)&codepoint;
                Glyph *glyph = getFontGlyph(&fonts->editorFont, gd);
                /*
                log_info("ch: %u, chW: %d, chH: %d, imgW: %d, imgH: %d, boxW: %d, boxH: %d, offX: %d, offY: %d",
                    glyph->ch, chW, chH, glyph->imgW, glyph->imgH, glyph->boxW, glyph->boxH, glyph->offX, glyph->offY);
                */
                uint8_t *gimg = (uint8_t*)&glyph[1];
                for (int j = 0; j < chW; j++) {
                    int gx = j - glyph->offX; // - (chW - glyph->boxW) / 2;
                    int gy = chY - glyph->boxH + glyph->offY;
                    uint8_t lum = (gx < 0 || gx >= glyph->imgW || gy < 0 || gy >= glyph->imgH) ? 0 : gimg[gx + glyph->imgW * gy];
                    image[x + j + width * y] = LERP(theme->foreEditor, theme->backEditor, lum);
                }
                idx++;
            } else {
                for (int j = 0; j < xEnd - x; j++) {
                    image[x + j + width * y] = theme->backEditor;
                }
                x = xEnd;
            }
        }

        if (chY == chH - 1) {
            if (foundNl) {
                lineStart = idx + 1;
            } else {
                for (int i = idx; i < file->size; i++) {
                    if (file->buf[i] == '\n') {
                        lineStart = i + 1;
                        break;
                    }
                }
            }
        }

        /*
        if (y >= layout->scrollY && y < layout->scrollY + layout->scrollHeight) {
            for (int x = layout->windowWidth - layout->scrollWidth; x < layout->windowWidth; x++) {
                
            }
        }
        if (y >= layout->scrollY && y < layout->scrollY + layout->scrollHeight) {
            for (int x = layout->windowWidth - layout->scrollWidth; x < layout->windowWidth; x++) {
                
            }
        }
        */
    }

    // ALWAYS draw status. Even if it would be excluded, still draw the status area anyway.
    // TODO: maybe check if the status didn't change as well, then skip if it's also outside the invalidation area

    /*
    int statusX = 0;
    if (area && area->y + area->h < layout->windowHeight - layout->statusHeight)
        statusX = layout->windowWidth - layout->statusWidth;

    for (int y = layout->windowHeight - layout->statusHeight; y < layout->windowHeight; y++) {
        for (int x = statusX; x < layout->windowWidth; x++) {
            // draw status
        }
    }
    */
}
