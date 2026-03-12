#include "vanilla.h"

#define LERP(textColor, backColor, lum) ((((textColor) >> 8) * lum + ((backColor) >> 8) * (256 - lum)) | 0xffU);

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
	if (!area || area->y < topNavHeight) {
		for (int y = 0; y < layout->toolBarHeight; y++) {
			// draw toolbar (File, Edit, View, Help)
			for (int x = 0, i = 0; x < layout->toolBarWidth; x += fonts->toolBarFont.pxFontWidth, i++) {
				uint32_t ch = (uint32_t)(" File  Edit  View  Help "[i]) & 0xff;
				uint8_t *image = getFontGlyphPixel(&fonts->toolBarFont, ch) + (fonts->toolBarFont.pxFontWidth * y);
				for (int j = 0; j < fonts->toolBarFont.pxFontWidth; j++)
					image[x + j + width * y] = LERP(theme->textColor, theme->toolBarColor, image[j]);
			}
			for (int x = layout->toolBarWidth; x < layout->windowWidth; x++) {
				image[x + width * y] = theme->toolBarColor;
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
		if (!area || area->x < sideNavWidth) {
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

// 100% transparent = 0, 100% opaque = 256 (not 255)
uint8_t *getFontGlyph(FontCache *font, uint32_t ch) {
	KeyValue32 *glyphToOffset = HashTable32_locate(&font->glyphToImageMap, c);
	int imageOffset = 0;
	if (!glyphToOffset || glyphToOffset->key != ch) {
		// With a handle to FreeType2, render this glyph to an image.
		// Then copy it as grayscale to our atlas and record the offset in the atlas back to font->glyphToImageMap
		HashTable32_put(&font->glyphToImageMap, ch, imageOffset);
	}
	return &font->atlas[imageOffset];
}
