#pragma once

#include <stdint.h>

typedef struct {
    uint32_t key;
    uint32_t value;
} KeyValue32;

typedef struct {
    KeyValue32 *pairs;
    int sizeLog2;
    int capacity;
    int occupied;
} HashTable32;

typedef struct {
    int x;
    int y;
    int w;
    int h;
} Rect;

typedef struct {
    uint32_t bold : 1;
    uint32_t italic : 1;
    uint32_t underline : 1;
    uint32_t codepoint : 29;
} GlyphDesc;

typedef struct {
    HashTable32 glyphToImageMap;
    uint8_t *atlas;
    int capacity;
    int fontSize;
    int pxFontWidth;
    int pxFontHeight;
} FontCache;

typedef struct {
    FontCache toolBarFont;
    FontCache sideNavFont;
    FontCache editorFont;
} AllFontCaches;

typedef struct {
    uint32_t textColor;
} Theme;

typedef struct {
    int windowWidth;
    int windowHeight;
    int topNavHeight;
    int topNavPanX;
    int statusHeight;
    int sideNavWidth;
    int sideNavPanY;
    int panX;
    int panY;
    int scrollY;
    int scrollWidth;
    int scrollHeight;
} Layout;
