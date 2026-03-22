#include "vanilla.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_BITMAP_H
#include FT_STROKER_H

#define FLOAT_FROM_16_16(n) ((float)((n) >> 16) + (float)((n) & 0xffff) / 65536.0)

#define CAST_PTR(var, type)    (type*)(&var)
#define CAST_VALUE(var, type) *(type*)(&var)
#define CAST_DEREF(var, type) *(type*)(var)

#define OBLIQUE_SLANT 0.25

int initFreetype(Freetype *ft, int dpiX, int dpiY) {
	if (FT_Init_FreeType(CAST_PTR(ft->library, FT_Library)) != 0) {
		fprintf(stderr, "Could not initialise libfreetype\n");
		return 0;
	}

	FT_Stroker_New(
	    CAST_VALUE(ft->library, FT_Library),
	    CAST_PTR(ft->stroker, FT_Stroker)
    );

    ft->dpiX = dpiX;
    ft->dpiY = dpiY;
	return 1;
}

FTHandle_Face loadFontFaceFromFile(Freetype *ft, const char *path) {
    FT_Face face;
    if (FT_New_Face(
        CAST_VALUE(ft->library, FT_Library),
        path,
        0,
        &face
    ) != 0) {
		fprintf(stderr, "Error loading font \"%s\"\n", path);
		return (FTHandle_Face) {NULL};
	}
	return CAST_VALUE(face, FTHandle_Face);
}

FTHandle_Face loadFontFaceFromMemory(Freetype *ft, const uint8_t *data, int size) {
    FT_Face face;
    FT_Open_Args args = {0};
    args.flags = FT_OPEN_MEMORY;
    args.memory_base = data;
    args.memory_size = size;
    if (FT_Open_Face(
        CAST_VALUE(ft->library, FT_Library),
        &args,
        0,
        &face
    ) != 0) {
		fprintf(stderr, "Error loading font from memory \"%p:%d\"\n", data, size);
		return (FTHandle_Face) {NULL};
	}
	return CAST_VALUE(face, FTHandle_Face);
}

int initFontCache(FontCache *font, Freetype *ft, FTHandle_Face *faceRef, int fontHeight) {
    *font = (FontCache) {0};
    font->ft = ft;
    font->face = faceRef;
    font->pxGlyphWidth = fontHeight;
    font->pxGlyphHeight = fontHeight;
}

int initAllFonts(AllFontCaches *fonts, Freetype *ft) {
    fonts->codeFontFace = loadFontFaceFromFile(ft, "fonts/code.ttf");
    fonts->uiFontFace = loadFontFaceFromFile(ft, "fonts/ui.otf");
    if (!fonts->codeFontFace.ptr || !fonts->uiFontFace.ptr)
        return 0;

    initFontCache(&fonts->toolBarFont, ft, &fonts->uiFontFace, 20);
    initFontCache(&fonts->sideNavFont, ft, &fonts->uiFontFace, 30);
    initFontCache(&fonts->editorFont, ft, &fonts->codeFontFace, 30);
    return 1;
}

// 100% transparent = 0, 100% opaque = 256 (not 255)
uint8_t *getFontGlyph(FontCache *font, GlyphDesc ch) {
    uint32_t chUint = *(uint32_t*)&ch;
    KeyValue32 *glyphToOffset = HashTable32_locate(&font->glyphToImageMap, chUint);
    int imageOffset = 0;

    if (!glyphToOffset || glyphToOffset->key != chUint) {
        imageOffset = font->atlas.size;
        HashTable32_put(&font->glyphToImageMap, chUint, imageOffset);

        int glyphArea = sizeof(Glyph) + font->pxGlyphWidth * font->pxGlyphHeight;
        ByteVector_resize(&font->atlas, imageOffset + glyphArea);
        drawGlyph(font, ch, imageOffset);
    }
    else {
        imageOffset = glyphToOffset->value;
    }

    return &font->atlas.data[imageOffset];
}

void drawGlyph(FontCache *font, GlyphDesc ch, int dataOffset) {
    FT_Face face = CAST_DEREF(*font->face, FT_Face);

    int maxWidth = font->pxGlyphWidth;
    int maxHeight = font->pxGlyphHeight;

    FT_Set_Char_Size(face, 0, (int)(attrs->size * 64), font->ft->dpiX, font->ft->dpiY);

    FT_Bitmap bmp;
	FT_Glyph glyph;
	int left, top;

    float gap = 0;
    FT_Matrix matrix;

    if (ch.italic) {
        gap = OBLIQUE_SLANT * 0.25 * attrs->size;
        matrix = (FT_Matrix) { .xx = 0x10000, .xy = (int)(OBLIQUE_SLANT * 0x10000), .yx = 0, .yy = 0x10000 };
        FT_Set_Transform(face, &matrix, NULL);
    }

    if (ch.bold) {
        FT_Stroker_Set(
            CAST_VALUE(font->ft->stroker, FT_Stroker),
            32,
            FT_STROKER_LINECAP_ROUND,
            FT_STROKER_LINEJOIN_ROUND,
            0
        );

		FT_Load_Char(face, ch.codepoint, FT_LOAD_NO_BITMAP);
		FT_Get_Glyph(face->glyph, &glyph);

		FT_Glyph_StrokeBorder(&glyph, stroker, 0, 1);
		FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, NULL, 1);
		FT_BitmapGlyph bg = (FT_BitmapGlyph)glyph;

		bmp = bg->bitmap;
		left = bg->left;
		top = bg->top;
	}
	else {
		FT_Load_Char(face, ch.codepoint, FT_LOAD_RENDER);

		bmp = face->glyph->bitmap;
		left = face->glyph->bitmap_left;
		top = face->glyph->bitmap_top;
	}

	Glyph *info = (Glyph*)&font->atlas.data[dataOffset];
	info->imgW = bmp.width;
    info->imgH = bmp.row;
    info->boxW = FLOAT_FROM_16_16(face->glyph->linearHoriAdvance) + gap,
    info->boxH = FLOAT_FROM_16_16(face->glyph->linearVertAdvance);
    info->left  = left;
    info->top   = top;

    for (int i = 0; i < gl->img_w * gl->img_h; i++) {
        float lum = (float)bmp.buffer[i] / 255.0;
    }

    XPutImage(
		display, draw_ctx->window, draw_ctx->gc, (XImage*)&glyphs[idx].ximage,
		0, 0,
		x + glyphs[idx].left, y - glyphs[idx].top,
		glyphs[idx].img_w, glyphs[idx].img_h
	);

    #define FONT_WIDTH(glyph) (int)(glyph.box_w + 0.5)
    #define FONT_HEIGHT(glyph) (int)(glyph.box_h + 0.5)

    x += FONT_WIDTH(glyphs[0]);

	if (ch.bold)
		FT_Done_Glyph(glyph);

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

void closeAllFonts(AllFontCaches *fonts) {
    Freetype *ft = fonts->toolBarFont.ft;
    closeFontFace(ft, fonts->codeFontFace);
    closeFontFace(ft, fonts->uiFontFace);
    closeFreetype(ft);
}
