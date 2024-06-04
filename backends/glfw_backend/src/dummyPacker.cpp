#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#define COLORMASK_RED   0
#define COLORMASK_GREEN 1
#define COLORMASK_BLUE  2
#define COLORMASK_ALPHA 3

#define EMPTY_NODE   (0xFFFF)
#define PACK_EMPTY   (0)
#define PACK_UL_STARTED (1)
#define PACK_UL_FULL (2)
#define PACK_UL_DATA (3)
#define PACK_UR_STARTED (1 << 2)
#define PACK_UR_FULL (2 << 2)
#define PACK_UR_DATA (3 << 2)
#define PACK_DL_STARTED (1 << 4)
#define PACK_DL_FULL (2 << 4)
#define PACK_DL_DATA (3 << 4)
#define PACK_DR_STARTED (1 << 6)
#define PACK_DR_FULL (2 << 6)
#define PACK_DR_DATA (3 << 6)

struct pixelData_t {
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    uint16_t cellSize;
    uint16_t colorMask;
    uint16_t index;
};

struct level_t {
    uint16_t ul;
    uint16_t ur;
    uint16_t dl;
    uint16_t dr;
    uint16_t dimension;
    uint8_t flagsRed;
    uint8_t flagsGreen;
    uint8_t flagsBlue;
    uint8_t flagsAlpha;
    uint16_t padding;
};

static struct textureHead_t {
    uint8_t *textureData;
    level_t *textureStack;
    pixelData_t *dataStack;
    pixelData_t *colorTexture;
    uint32_t textureSizeInBytes;
    uint16_t textureDimension;
    uint16_t textureStackSize;
    uint16_t dataStackSize;
    uint16_t textureStackCurrent;
    uint16_t dataStackCurrent;
} textureHead;

static void DuPackUploadPixels(pixelData_t *pixelData, uint16_t stride, uint16_t textureWidth, uint16_t textureHeight, uint8_t *data) {
    pixelData->width = textureWidth;
    pixelData->height = textureHeight;
    for (uint32_t y = pixelData->y; y < pixelData->y + textureHeight; ++y) {
        uint32_t yoffset = y * stride;
        for (uint32_t x = pixelData->x; x < pixelData->x + textureWidth; ++x) {
            uint32_t offset = 4 * (x + yoffset) + pixelData->colorMask;
            textureHead.textureData[offset] = *data++;
        }
    }
}

static pixelData_t *DuPackAddPixelData(uint8_t colorMask, uint16_t x, uint16_t y, uint16_t cellSize) {
    if (textureHead.dataStackCurrent < textureHead.dataStackSize) {
        pixelData_t *pixelData = textureHead.dataStack + textureHead.dataStackCurrent;
        pixelData->index = textureHead.dataStackCurrent;
        pixelData->colorMask = colorMask;
        pixelData->x = x;
        pixelData->y = y;
        pixelData->width = 0;
        pixelData->height = 0;
        pixelData->cellSize = cellSize;
        textureHead.dataStackCurrent++;
        // initailize
        return pixelData;
    }
    return NULL;
}

static level_t *DuPackAddLevel(uint16_t dimension, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
    if (textureHead.textureStackCurrent < textureHead.textureStackSize) {
        level_t *level = textureHead.textureStack + textureHead.textureStackCurrent;
        textureHead.textureStackCurrent++;
        level->dimension = dimension;
        level->ul = level->ur = level->dl = level->dr = EMPTY_NODE;
        level->flagsRed = (red << 6) | (red << 4) | (red << 2) | red;
        level->flagsGreen = (green << 6) | (green << 4) | (green << 2) | green;
        level->flagsBlue = (blue << 6) | (blue << 4) | (blue << 2) | (blue);
        level->flagsAlpha = (alpha << 6) | (alpha << 4) | (alpha << 2) | alpha;
        return level;
    }
    assert(0);
    return NULL;
}

static void DuPackUpdateFlags(level_t *level, level_t *child, uint8_t fullFlag, uint8_t startedFlag){
    uint8_t tmpFlags = child->flagsRed;
    uint8_t tmpAndFlags = ((tmpFlags >> 1) & (tmpFlags >> 3) & (tmpFlags >> 5) & (tmpFlags >> 7) & 0x1);
    uint8_t tmpOrFlags = (((tmpFlags) | (tmpFlags >> 2) | (tmpFlags >> 4) | (tmpFlags >> 6)) & 0x3);
    if (tmpAndFlags) {
        level->flagsRed |= fullFlag;
    } else if (tmpOrFlags) {
        level->flagsRed |= startedFlag;
    }
    tmpFlags = child->flagsGreen;
    tmpAndFlags = ((tmpFlags >> 1) & (tmpFlags >> 3) & (tmpFlags >> 5) & (tmpFlags >> 7) & 0x1);
    tmpOrFlags = (((tmpFlags) | (tmpFlags >> 2) | (tmpFlags >> 4) | (tmpFlags >> 6)) & 0x3);
    if (tmpAndFlags) {
        level->flagsGreen |= fullFlag;
    } else if (tmpOrFlags) {
        level->flagsGreen |= startedFlag;
    }
    tmpFlags = child->flagsBlue;
    tmpAndFlags = ((tmpFlags >> 1) & (tmpFlags >> 3) & (tmpFlags >> 5) & (tmpFlags >> 7) & 0x1);
    tmpOrFlags = (((tmpFlags) | (tmpFlags >> 2) | (tmpFlags >> 4) | (tmpFlags >> 6)) & 0x3);
    if (tmpAndFlags) {
        level->flagsBlue |= fullFlag;
    } else if (tmpOrFlags) {
        level->flagsBlue |= startedFlag;
    }
    tmpFlags = child->flagsAlpha;
    tmpAndFlags = ((tmpFlags >> 1) & (tmpFlags >> 3) & (tmpFlags >> 5) & (tmpFlags >> 7) & 0x1);
    tmpOrFlags = (((tmpFlags) | (tmpFlags >> 2) | (tmpFlags >> 4) | (tmpFlags >> 6)) & 0x3);
    if (tmpAndFlags) {
        level->flagsAlpha |= fullFlag;
    } else if (tmpOrFlags) {
        level->flagsAlpha |= startedFlag;
    }
}

static pixelData_t *DuPackAddTextureRec(uint16_t dimension, uint16_t left, uint16_t bottom, level_t *level) {
    if (dimension <= level->dimension / 2) {
        pixelData_t *pixelResult = NULL;
        // check up left
        uint8_t flagAnd = level->flagsRed & level->flagsGreen & level->flagsBlue & level->flagsAlpha;
        if ( !(flagAnd & PACK_UL_FULL)) {
            if (level->ul == EMPTY_NODE) {
                level->ul = DuPackAddLevel(level->dimension / 2,
                                           (level->flagsRed & PACK_UL_FULL), 
                                           (level->flagsGreen & PACK_UL_FULL), 
                                           (level->flagsBlue & PACK_UL_FULL), 
                                           (level->flagsAlpha & PACK_UL_FULL)) - level;
            }
            pixelResult = DuPackAddTextureRec(dimension, left, bottom + level->dimension / 2, level + level->ul);
            if (pixelResult) {
                DuPackUpdateFlags(level, level + level->ul, PACK_UL_FULL, PACK_UL_STARTED);
                return pixelResult;
            }
        }

        if ( !(flagAnd & PACK_UR_FULL)) {
            if (level->ur == EMPTY_NODE) {
                level->ur = DuPackAddLevel(level->dimension / 2, 
                                           (level->flagsRed & PACK_UR_FULL) >> 2, 
                                           (level->flagsGreen & PACK_UR_FULL) >> 2, 
                                           (level->flagsBlue & PACK_UR_FULL) >> 2, 
                                           (level->flagsAlpha & PACK_UR_FULL) >> 2) - level;
            }
            pixelResult = DuPackAddTextureRec(dimension, left + level->dimension / 2, bottom + level->dimension / 2, level + level->ur);
            if (pixelResult) {
                DuPackUpdateFlags(level, level + level->ur, PACK_UR_FULL, PACK_UR_STARTED);
                return pixelResult;
            }
        }

        if ( !(flagAnd & PACK_DL_FULL)) {
            if (level->dl == EMPTY_NODE) {
                level->dl = DuPackAddLevel(level->dimension / 2,
                                           (level->flagsRed & PACK_DL_FULL) >> 4, 
                                           (level->flagsGreen & PACK_DL_FULL) >> 4, 
                                           (level->flagsBlue & PACK_DL_FULL) >> 4, 
                                           (level->flagsAlpha & PACK_DL_FULL) >> 4) - level;
            }
            pixelResult = DuPackAddTextureRec(dimension, left, bottom, level + level->dl);
            if (pixelResult) {
                DuPackUpdateFlags(level, level + level->dl, PACK_DL_FULL, PACK_DL_STARTED);
                return pixelResult;
            }
        }

        if ( !(flagAnd & PACK_DR_FULL)) {
            if (level->dr == EMPTY_NODE) {
                level->dr = DuPackAddLevel(level->dimension / 2,
                                           (level->flagsRed & PACK_DR_FULL) >> 6, 
                                           (level->flagsGreen & PACK_DR_FULL) >> 6, 
                                           (level->flagsBlue & PACK_DR_FULL) >> 6, 
                                           (level->flagsAlpha & PACK_DR_FULL) >> 6) - level;
            }
            pixelResult = DuPackAddTextureRec(dimension, left + level->dimension / 2, bottom, level + level->dr);
            if (pixelResult) {
                DuPackUpdateFlags(level, level + level->dr, PACK_DR_FULL, PACK_DR_STARTED);
                return pixelResult;
            }
        }

    } else if (dimension <= level->dimension) {
        // check red
        if (level->flagsRed == PACK_EMPTY) {
            pixelData_t *pixelData = DuPackAddPixelData(COLORMASK_RED, left, bottom, level->dimension);
            level->flagsRed = PACK_DL_DATA | PACK_DR_DATA | PACK_UL_DATA | PACK_UR_DATA;
            return pixelData;
        }
        if (level->flagsGreen == PACK_EMPTY) {
            pixelData_t *pixelData = DuPackAddPixelData(COLORMASK_GREEN, left, bottom, level->dimension);
            level->flagsGreen = PACK_DL_DATA | PACK_DR_DATA | PACK_UL_DATA | PACK_UR_DATA;
            return pixelData;
        }
        if (level->flagsBlue == PACK_EMPTY) {
            pixelData_t *pixelData = DuPackAddPixelData(COLORMASK_BLUE, left, bottom, level->dimension);
            level->flagsBlue = PACK_DL_DATA | PACK_DR_DATA | PACK_UL_DATA | PACK_UR_DATA;
            return pixelData;
        }
        if (level->flagsAlpha == PACK_EMPTY) {
            pixelData_t *pixelData = DuPackAddPixelData(COLORMASK_ALPHA, left, bottom, level->dimension);
            level->flagsAlpha = PACK_DL_DATA | PACK_DR_DATA | PACK_UL_DATA | PACK_UR_DATA;
            return pixelData;
        }
    }
    return NULL;
}

static pixelData_t *DuPackCreateAllColorsTexture(void) {
    uint8_t palette[256];
    for (uint32_t i = 0; i < 256; ++i) {
        palette[i] = i;
    }
    pixelData_t * pixelData = DuPackAddTextureRec(16, 0, 0, textureHead.textureStack);
    if (pixelData) {
        DuPackUploadPixels(pixelData, textureHead.textureStack->dimension,16, 16, palette);
    }
    return pixelData;
}

/* PUBLIC FUNCTIONS */
int DuPackAddTexture(int width, int height, unsigned char *data) {
    uint16_t dimension = (uint16_t) (width > height ? width : height);

    pixelData_t * pixelData = DuPackAddTextureRec(dimension, 0, 0, textureHead.textureStack);
    if (pixelData) {
        DuPackUploadPixels(pixelData, textureHead.textureStack->dimension,width, height, data);
    } else {
        exit(-1);
    }

    FILE *fp = fopen("data.raw", "wb");
    fwrite(textureHead.textureData, textureHead.textureSizeInBytes, 1, fp);
    fclose(fp);

    return pixelData->index;
}

unsigned char *DuPackGetPalettizedTexture(void) {
    return textureHead.textureData;
}

int DuPackGetTextureDimension(void) {
    return textureHead.textureDimension;
}

void DuPackGetTextureCoords(int index, float *left, float *right, float *bottom, float *top, int *rgbaOffset) {
    pixelData_t * pixelData = textureHead.dataStack + index;
    float textureDimension = (float)textureHead.textureDimension;
    *left = ((float)pixelData->x) / textureDimension;
    *right = ((float)pixelData->x + (float)pixelData->width) / textureDimension;
    *bottom = ((float)pixelData->y) / textureDimension;
    *top = ((float)pixelData->y + (float)pixelData->height) / textureDimension;
    *rgbaOffset = pixelData->colorMask;
}

void DuPackGetColorCoords(int color, float *left, float *right, float *bottom, float *top, int *rgbaOffset) {
    pixelData_t * pixelData = textureHead.colorTexture;
    uint16_t xOffset = color & 0xF;
    uint16_t yOffset = (color >> 4) & 0xF;
    float textureDimension = (float)textureHead.textureDimension;
    *right = *left = ((float)(pixelData->x + xOffset)) / textureDimension;
    *top = *bottom = ((float)(pixelData->y + yOffset)) / textureDimension;
    *rgbaOffset = pixelData->colorMask;
}

void DuPackInit(unsigned int textureSize, 
                unsigned int levelStackSize, 
                unsigned int dataStackSize) {
    textureHead.textureDimension = textureSize;
    textureHead.textureSizeInBytes = textureSize * textureSize * 4;
    textureHead.textureStackSize = levelStackSize;
    textureHead.dataStackSize = dataStackSize;

    textureHead.textureData = (uint8_t*)malloc(textureHead.textureSizeInBytes); // square rgb texture
    if (!textureHead.textureData) {
        exit(-1);
    }
    memset(textureHead.textureData, 0, textureHead.textureSizeInBytes);
    textureHead.textureStack = (level_t*)malloc(levelStackSize * sizeof(level_t)); 
    if (!textureHead.textureStack) {
        exit(-1);
    }
    textureHead.dataStack = (pixelData_t*)malloc(dataStackSize * sizeof(pixelData_t)); 
    if (!textureHead.dataStack) {
        exit(-1);
    }
    textureHead.textureStackCurrent = 0;
    textureHead.dataStackCurrent = 0;

    DuPackAddLevel(textureSize, PACK_EMPTY, PACK_EMPTY, PACK_EMPTY, PACK_EMPTY);
    textureHead.colorTexture = DuPackCreateAllColorsTexture();
}

void DuPackFinish(void) {
    free(textureHead.textureData);
    free(textureHead.textureStack);
    free(textureHead.dataStack);
}