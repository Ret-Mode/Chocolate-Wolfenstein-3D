#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define COLORMASK_RED   0
#define COLORMASK_GREEN 1
#define COLORMASK_BLUE  2
#define COLORMASK_ALPHA 3

#define PACK_EMPTY   (0)
#define PACK_UL_FULL (2)
#define PACK_UL_DATA (3)
#define PACK_UR_FULL (2 << 2)
#define PACK_UR_DATA (3 << 2)
#define PACK_DL_FULL (2 << 4)
#define PACK_DL_DATA (3 << 4)
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
    level_t *ul;
    level_t *ur;
    level_t *dl;
    level_t *dr;
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
    uint32_t textureDataSize;
    uint32_t textureStackSize;
    uint32_t dataStackSize;
    uint32_t textureStackCurrent;
    uint32_t dataStackCurrent;
} textureHead;

static void DuPackUploadPixels(pixelData_t *pixelData, uint16_t stride, uint16_t textureWidth, uint16_t textureHeight, uint8_t *data) {
    pixelData->width = textureWidth;
    pixelData->height = textureHeight;
    for (uint32_t y = pixelData->y; y < pixelData->y + textureHeight; ++y) {
        uint32_t yoffset = (stride - y - 1) * stride;
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

static level_t *DuPackAddLevel(uint16_t dimension) {
    if (textureHead.textureStackCurrent < textureHead.textureStackSize) {
        level_t *level = textureHead.textureStack + textureHead.textureStackCurrent;
        textureHead.textureStackCurrent++;
        level->dimension = dimension;
        level->ul = level->ur = level->dl = level->dr = NULL;
        level->flagsRed = level->flagsGreen = level->flagsBlue = level->flagsAlpha = 0;
        return level;
    }
    return NULL;
}



static pixelData_t *DuPackAddTextureRec(uint16_t dimension, uint16_t left, uint16_t bottom, level_t *level) {
    if (dimension <= level->dimension / 2) {
        pixelData_t *pixelResult = NULL;
        // check up left
        uint8_t flagAnd = level->flagsRed & level->flagsGreen & level->flagsBlue & level->flagsAlpha;
        if ( !(flagAnd & PACK_UL_FULL)) {
            if (level->ul == NULL) {
                level->ul = DuPackAddLevel(level->dimension / 2);
            }
            pixelResult = DuPackAddTextureRec(dimension, left, bottom + level->dimension / 2, level->ul);
            if (pixelResult) {
                uint8_t tmpRedFlags = level->ul->flagsRed;
                if (tmpRedFlags & (tmpRedFlags >> 2) & (tmpRedFlags >> 4) & (tmpRedFlags >> 6) & 0x2) {
                    level->flagsRed |= PACK_UL_FULL;
                }
                uint8_t tmpGreenFlags = level->ul->flagsGreen;
                if (tmpGreenFlags & (tmpGreenFlags >> 2) & (tmpGreenFlags >> 4) & (tmpGreenFlags >> 6) & 0x2) {
                    level->flagsGreen |= PACK_UL_FULL;
                }
                uint8_t tmpBlueFlags = level->ul->flagsBlue;
                if (tmpBlueFlags & (tmpBlueFlags >> 2) & (tmpBlueFlags >> 4) & (tmpBlueFlags >> 6) & 0x2) {
                    level->flagsBlue |= PACK_UL_FULL;
                }
                uint8_t tmpAlphaFlags = level->ul->flagsAlpha;
                if (tmpAlphaFlags & (tmpAlphaFlags >> 2) & (tmpAlphaFlags >> 4) & (tmpAlphaFlags >> 6) & 0x2) {
                    level->flagsAlpha |= PACK_UL_FULL;
                }
                return pixelResult;
            }
        }

        if ( !(flagAnd & PACK_UR_FULL)) {
            if (level->ur == NULL) {
                level->ur = DuPackAddLevel(level->dimension / 2);
            }
            pixelResult = DuPackAddTextureRec(dimension, left + level->dimension / 2, bottom + level->dimension / 2, level->ur);
            if (pixelResult) {
                uint8_t tmpRedFlags = level->ur->flagsRed;
                if (tmpRedFlags & (tmpRedFlags >> 2) & (tmpRedFlags >> 4) & (tmpRedFlags >> 6) & 0x2) {
                    level->flagsRed |= PACK_UR_FULL;
                }
                uint8_t tmpGreenFlags = level->ur->flagsGreen;
                if (tmpGreenFlags & (tmpGreenFlags >> 2) & (tmpGreenFlags >> 4) & (tmpGreenFlags >> 6) & 0x2) {
                    level->flagsGreen |= PACK_UR_FULL;
                }
                uint8_t tmpBlueFlags = level->ur->flagsBlue;
                if (tmpBlueFlags & (tmpBlueFlags >> 2) & (tmpBlueFlags >> 4) & (tmpBlueFlags >> 6) & 0x2) {
                    level->flagsBlue |= PACK_UR_FULL;
                }
                uint8_t tmpAlphaFlags = level->ur->flagsAlpha;
                if (tmpAlphaFlags & (tmpAlphaFlags >> 2) & (tmpAlphaFlags >> 4) & (tmpAlphaFlags >> 6) & 0x2) {
                    level->flagsAlpha |= PACK_UR_FULL;
                }
                return pixelResult;
            }
        }

        if ( !(flagAnd & PACK_DL_FULL)) {
            if (level->dl == NULL) {
                level->dl = DuPackAddLevel(level->dimension / 2);
            }
            pixelResult = DuPackAddTextureRec(dimension, left, bottom, level->dl);
            if (pixelResult) {
                uint8_t tmpRedFlags = level->dl->flagsRed;
                if (tmpRedFlags & (tmpRedFlags >> 2) & (tmpRedFlags >> 4) & (tmpRedFlags >> 6) & 0x2) {
                    level->flagsRed |= PACK_DL_FULL;
                }
                uint8_t tmpGreenFlags = level->dl->flagsGreen;
                if (tmpGreenFlags & (tmpGreenFlags >> 2) & (tmpGreenFlags >> 4) & (tmpGreenFlags >> 6) & 0x2) {
                    level->flagsGreen |= PACK_DL_FULL;
                }
                uint8_t tmpBlueFlags = level->dl->flagsBlue;
                if (tmpBlueFlags & (tmpBlueFlags >> 2) & (tmpBlueFlags >> 4) & (tmpBlueFlags >> 6) & 0x2) {
                    level->flagsBlue |= PACK_DL_FULL;
                }
                uint8_t tmpAlphaFlags = level->dl->flagsAlpha;
                if (tmpAlphaFlags & (tmpAlphaFlags >> 2) & (tmpAlphaFlags >> 4) & (tmpAlphaFlags >> 6) & 0x2) {
                    level->flagsAlpha |= PACK_DL_FULL;
                }
                return pixelResult;
            }
        }

        if ( !(flagAnd & PACK_DR_FULL)) {
            if (level->dr == NULL) {
                level->dr = DuPackAddLevel(level->dimension / 2);
            }
            pixelResult = DuPackAddTextureRec(dimension, left + level->dimension / 2, bottom, level->dr);
            if (pixelResult) {
                uint8_t tmpRedFlags = level->dr->flagsRed;
                if (tmpRedFlags & (tmpRedFlags >> 2) & (tmpRedFlags >> 4) & (tmpRedFlags >> 6) & 0x2) {
                    level->flagsRed |= PACK_DR_FULL;
                }
                uint8_t tmpGreenFlags = level->dr->flagsGreen;
                if (tmpGreenFlags & (tmpGreenFlags >> 2) & (tmpGreenFlags >> 4) & (tmpGreenFlags >> 6) & 0x2) {
                    level->flagsGreen |= PACK_DR_FULL;
                }
                uint8_t tmpBlueFlags = level->dr->flagsBlue;
                if (tmpBlueFlags & (tmpBlueFlags >> 2) & (tmpBlueFlags >> 4) & (tmpBlueFlags >> 6) & 0x2) {
                    level->flagsBlue |= PACK_DR_FULL;
                }
                uint8_t tmpAlphaFlags = level->dr->flagsAlpha;
                if (tmpAlphaFlags & (tmpAlphaFlags >> 2) & (tmpAlphaFlags >> 4) & (tmpAlphaFlags >> 6) & 0x2) {
                    level->flagsAlpha |= PACK_DR_FULL;
                }
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
void DuPackAddTexture(int width, int height, unsigned char *data) {
    uint16_t dimension = (uint16_t) (width > height ? width : height);

    pixelData_t * pixelData = DuPackAddTextureRec(dimension, 0, 0, textureHead.textureStack);
    if (pixelData) {
        DuPackUploadPixels(pixelData, textureHead.textureStack->dimension,width, height, data);
    } else {
        exit(-1);
    }

    FILE *fp = fopen("data.raw", "wb");
    fwrite(textureHead.textureData, textureHead.textureDataSize * textureHead.textureDataSize * 4, 1, fp);
    fclose(fp);
}

unsigned char *DuPackGetPalettizedTexture(void) {
    return textureHead.textureData;
}

void DuPackGetTextureCoords(int index, float *left, float *right, float *bottom, float *top, int *rgbaOffset) {
    pixelData_t * pixelData = textureHead.dataStack + index;
    float textureDimension = (float)textureHead.textureDataSize;
    *left = ((float)pixelData->x) / textureDimension;
    *right = ((float)pixelData->x + (float)pixelData->width) / textureDimension;
    *bottom = ((float)pixelData->y) / textureDimension;
    *top = ((float)pixelData->y + (float)pixelData->height) / textureDimension;
    *rgbaOffset = pixelData->colorMask;
}

void DuPackGetColorCoords(int color, float *left, float *right, float *bottom, float *top, int *rgbaOffset) {
    pixelData_t * pixelData = textureHead.colorTexture;
    uint16_t xOffset = color & 0xF;
    uint16_t yOffset = 0xF - ((color >> 4) & 0xF);
    float textureDimension = (float)textureHead.textureDataSize;
    *right = *left = ((float)(pixelData->x + xOffset)) / textureDimension;
    *top = *bottom = ((float)(pixelData->y + yOffset)) / textureDimension;
    *rgbaOffset = pixelData->colorMask;
}

void DuPackInit(unsigned int textureSize, 
                unsigned int levelStackSize, 
                unsigned int dataStackSize) {
    textureHead.textureDataSize = textureSize;
    textureHead.textureStackSize = levelStackSize;
    textureHead.dataStackSize = dataStackSize;

    textureHead.textureData = (uint8_t*)malloc(textureSize * textureSize * 4); // square rgb texture
    if (!textureHead.textureData) {
        exit(-1);
    }
    memset(textureHead.textureData, 0, textureSize * textureSize * 4);
    textureHead.textureStack = (level_t*)malloc(levelStackSize * sizeof (level_t)); 
    if (!textureHead.textureStack) {
        exit(-1);
    }
    textureHead.dataStack = (pixelData_t*)malloc(dataStackSize * sizeof(pixelData_t)); 
    if (!textureHead.dataStack) {
        exit(-1);
    }
    textureHead.textureStackCurrent = 0;
    textureHead.dataStackCurrent = 0;

    DuPackAddLevel(textureSize);
    textureHead.colorTexture = DuPackCreateAllColorsTexture();
}

void DuPackFinish(void) {
    free(textureHead.textureData);
    free(textureHead.textureStack);
    free(textureHead.dataStack);
}