#pragma once

#include <stdint.h>

#define HEADER_VECTOR(pref, type) \
typedef struct { \
    type *data; \
    int capacity; \
    int size; \
} pref ## Vector; \
void pref ## Vector_resize(pref ## Vector *vec, int newSize); \
void pref ## Vector_add(pref ## Vector *vec, type elem); \
void pref ## Vector_extend(pref ## Vector *vec, type *data, int sz);

HEADER_VECTOR(Byte, uint8_t)

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
    int x;
    int y;
} Point;

typedef struct {
    uint32_t bold : 1;
    uint32_t italic : 1;
    uint32_t underline : 1;
    uint32_t codepoint : 29;
} GlyphDesc;

typedef struct { void *ptr; } FTHandle_Library;
typedef struct { void *ptr; } FTHandle_Stroker;
typedef struct { void *ptr; } FTHandle_Face;

typedef struct {
    FTHandle_Library library;
    FTHandle_Stroker stroker;
    int dpiX;
    int dpiY;
} Freetype;

typedef struct {
    Freetype *ft;
    FTHandle_Face face;
    HashTable32 glyphToImageMap;
    ByteVector atlas;
    int pxGlyphWidth;
    int pxGlyphHeight;
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

typedef struct {
    
} Input;

typedef struct {
    
} FileView;

typedef struct {
    
} Syntax;

// font.c
int initFreetype(Freetype *ft);
int initFontFace(FontCache *font, Freetype *ft, const char *path);
FTHandle_Face loadFontFaceFromFile(Freetype *ft, const char *path);
FTHandle_Face loadFontFaceFromMemory(Freetype *ft, const uint8_t *data, int size);
void drawGlyph(FontCache *font, GlyphDesc ch, uint8_t *data, int pxGlyphWidth, int pxGlyphHeight);
void closeFontFace(Freetype *ft, FTHandle_Face face);
void closeFreetype(Freetype *ft);

// render.c
void draw(
    Input *input,
    FileView *file,
    Syntax *syntax,
    Layout *layout,
    Theme *theme,
    AllFontCaches *fonts,
    Rect *area,
    uint32_t *image
);
void closeAllFonts(AllFontCaches *fonts);

// util.c
KeyValue32 *HashTable32_locate(HashTable32 *tbl, uint32_t key);
void HashTable32_rebalance(HashTable32 *tbl, KeyValue32 *pair);
uint32_t HashTable32_put(HashTable32 *tbl, uint32_t key, uint32_t value);
int HashTable32_get(HashTable32 *tbl, uint32_t key, uint32_t *outValue);
