#include "vanilla.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_BITMAP_H
#include FT_STROKER_H

#define FLOAT_FROM_16_16(n) ((float)((n) >> 16) + (float)((n) & 0xffff) / 65536.0)

#define CAST_PTR(var, type)    (type*)(&var)
#define CAST_VALUE(var, type) *(type*)(&var)

#define OBLIQUE_SLANT 0.25

int initFreetype(Freetype *ft) {
	if (FT_Init_FreeType(CAST_PTR(ft->library, FT_Library)) != 0) {
		fprintf(stderr, "Could not initialise libfreetype\n");
		return 0;
	}

	FT_Stroker_New(
	    CAST_VALUE(ft->library, FT_Library),
	    CAST_PTR(ft->stroker, FT_Stroker),
    );
	return 1;
}

FTHandle_Face loadFontFaceFromFile(Freetype *ft, const char *path) {
    FT_Face face;
    if (FT_New_Face(ft->library, path, 0, &face) != 0) {
		fprintf(stderr, "Error loading font \"%s\"\n", path);
		return NULL;
	}
	return CAST_VALUE(face, FTHandle_Face);
}

FTHandle_Face loadFontFaceFromMemory(Freetype *ft, const uint8_t *data, int size) {
    FT_Face face;
    FT_Open_Args args = {0};
    args.flags = FT_OPEN_MEMORY;
    args.memory_base = data;
    args.memory_size = size;
    if (FT_Open_Face(ft->library, &args, 0, &face) != 0) {
		fprintf(stderr, "Error loading font \"%s\"\n", path);
		return NULL;
	}
	return CAST_VALUE(face, FTHandle_Face);
}

void drawGlyph(FontCache *font, GlyphDesc ch, uint8_t *data, int pxGlyphWidth, int pxGlyphHeight) {
    ARGB fore, back;
    make_argb(attrs->color, &fore);
    make_argb(background, &back);

    float gap = 0;
    FT_Matrix matrix;

    if (ch.italic) {
        gap = OBLIQUE_SLANT * 0.25 * attrs->size;
        matrix = (FT_Matrix) { .xx = 0x10000, .xy = (int)(OBLIQUE_SLANT * 0x10000), .yx = 0, .yy = 0x10000 };
        FT_Set_Transform(face, &matrix, NULL);
    }
    if (ch.bold)
        FT_Stroker_Set(font->ft.stroker, 32, FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);

    FT_Set_Char_Size(face, 0, (int)(attrs->size * 64), font->ft.dpiX, font->ft.dpiY);

    

    if (ch.italic)
        FT_Set_Transform(face, NULL, NULL);
}

void closeFontFace(Freetype *ft, FTHandle_Face face) {
    FT_Done_Face(CAST_VALUE(face, FT_Face));
}

void closeFreetype(Freetype *ft) {
    FT_Stroker_Done(CAST_VALUE(ft->stroker, FT_Stroker));
	FT_Done_FreeType(CAST_VALUE(ft->library, FT_Library));
}
