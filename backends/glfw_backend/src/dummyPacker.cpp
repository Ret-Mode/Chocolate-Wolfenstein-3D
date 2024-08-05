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
#define FLAG_EMPTY   (0)
#define FLAG_STARTED (1)
#define FLAG_FULL    (2)
#define FLAG_DATA    (3)
#define PACK_UL(x) (x & FLAG_DATA)
#define PACK_UR(x) ((x & FLAG_DATA) << 2)
#define PACK_DL(x) ((x & FLAG_DATA) << 4)
#define PACK_DR(x) ((x & FLAG_DATA) << 6)
#define PACK_UL_STARTED (FLAG_STARTED)
#define PACK_UL_FULL (FLAG_FULL)
#define PACK_UL_DATA (FLAG_DATA)
#define PACK_UR_STARTED (FLAG_STARTED << 2)
#define PACK_UR_FULL (FLAG_FULL << 2)
#define PACK_UR_DATA (FLAG_DATA << 2)
#define PACK_DL_STARTED (FLAG_STARTED << 4)
#define PACK_DL_FULL (FLAG_FULL << 4)
#define PACK_DL_DATA (FLAG_DATA << 4)
#define PACK_DR_STARTED (FLAG_STARTED << 6)
#define PACK_DR_FULL (FLAG_FULL << 6)
#define PACK_DR_DATA (FLAG_DATA << 6)
#define UNPACK_UL_STARTED(x) ( x     & FLAG_STARTED)
#define UNPACK_UR_STARTED(x) ((x>>2) & FLAG_STARTED)
#define UNPACK_DL_STARTED(x) ((x>>4) & FLAG_STARTED)
#define UNPACK_DR_STARTED(x) ((x>>6) & FLAG_STARTED)
#define UNPACK_UL_FULL(x) ( x     & FLAG_FULL) 
#define UNPACK_UR_FULL(x) ((x>>2) & FLAG_FULL)
#define UNPACK_DL_FULL(x) ((x>>4) & FLAG_FULL) 
#define UNPACK_DR_FULL(x) ((x>>6) & FLAG_FULL) 
#define UNPACK_UL_DATA(x) ( x     & FLAG_DATA) 
#define UNPACK_UR_DATA(x) ((x>>2) & FLAG_DATA)
#define UNPACK_DL_DATA(x) ((x>>4) & FLAG_DATA) 
#define UNPACK_DR_DATA(x) ((x>>6) & FLAG_DATA) 
#define ALL_ARE_FULL(x) ((x >> 1) & (x >> 3) & (x >> 5) & (x >> 7) & 0x1)
#define ANY_FULL_OR_STARTED(x) (((x) | (x >> 2) | (x >> 4) | (x >> 6)) & 0x3)

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
        level->flagsRed = PACK_UL(red) | PACK_UR(red) | PACK_DL(red) | PACK_DR(red);
        level->flagsGreen = PACK_UL(green) | PACK_UR(green) | PACK_DL(green) | PACK_DR(green);
        level->flagsBlue = PACK_UL(blue) | PACK_UR(blue) | PACK_DL(blue) | PACK_DR(blue);
        level->flagsAlpha = PACK_UL(alpha) | PACK_UR(alpha) | PACK_DL(alpha) | PACK_DR(alpha);
        return level;
    }
    assert(0);
    return NULL;
}

inline static uint8_t* GetColorFlag(uint8_t colormask, level_t *level) {
    uint8_t* result = &(level->flagsRed);
    switch (colormask) {
        case COLORMASK_GREEN:
        result = &(level->flagsGreen);
        break;
        case COLORMASK_BLUE:
        result = &(level->flagsBlue);
        break;
        case COLORMASK_ALPHA:
        result = &(level->flagsAlpha);
        break;
    }
    return result;
}

static void DescendWithFlags(uint8_t colormask, level_t *level, uint8_t ul, uint8_t ur, uint8_t dl, uint8_t dr) {
    uint8_t *flag = GetColorFlag(colormask, level);
    *flag = *flag | PACK_UL(ul) | PACK_UR(ur) | PACK_DL(dl) | PACK_DR(dr);
    if (level->ul != EMPTY_NODE) {
        DescendWithFlags(colormask, level + level->ul, ul, ul, ul, ul);
    }
    if (level->ur != EMPTY_NODE) {
        DescendWithFlags(colormask, level + level->ur, ur, ur, ur, ur);
    }
    if (level->dl != EMPTY_NODE) {
        DescendWithFlags(colormask, level + level->dl, dl, dl, dl, dl);
    }
    if (level->dr != EMPTY_NODE) {
        DescendWithFlags(colormask, level + level->dr, dr, dr, dr, dr);
    }
}

static pixelData_t *AddToLowerLeftEdge(uint8_t colormask, uint16_t left, uint16_t bottom, level_t* level, bool lessThanThreeFourths) {
    pixelData_t *pixelData = DuPackAddPixelData(colormask, left, bottom, level->dimension);
    if (pixelData == NULL) {
        return NULL;
    }
    if (lessThanThreeFourths) {
        if (level->ul == EMPTY_NODE) {
            level->ul = DuPackAddLevel(level->dimension / 2, 
                                    UNPACK_UL_FULL(level->flagsRed),
                                    UNPACK_UL_FULL(level->flagsGreen), 
                                    UNPACK_UL_FULL(level->flagsBlue), 
                                    UNPACK_UL_FULL(level->flagsAlpha)) - level;

        }

        if (level->ur == EMPTY_NODE) {
            level->ur = DuPackAddLevel(level->dimension / 2, 
                                    UNPACK_UR_FULL(level->flagsRed),
                                    UNPACK_UR_FULL(level->flagsGreen), 
                                    UNPACK_UR_FULL(level->flagsBlue), 
                                    UNPACK_UR_FULL(level->flagsAlpha)) - level;

        }

        if (level->dr == EMPTY_NODE) {
            level->dr = DuPackAddLevel(level->dimension / 2, 
                                    UNPACK_DR_FULL(level->flagsRed),
                                    UNPACK_DR_FULL(level->flagsGreen), 
                                    UNPACK_DR_FULL(level->flagsBlue), 
                                    UNPACK_DR_FULL(level->flagsAlpha)) - level;
        }
        uint8_t* flag;
        DescendWithFlags(colormask, level + level->ul, FLAG_EMPTY, FLAG_EMPTY, FLAG_DATA, FLAG_DATA); 
        DescendWithFlags(colormask, level + level->ur, FLAG_EMPTY, FLAG_EMPTY, FLAG_DATA, FLAG_EMPTY); 
        DescendWithFlags(colormask, level + level->dr, FLAG_DATA, FLAG_EMPTY, FLAG_DATA, FLAG_EMPTY); 
        flag = GetColorFlag(colormask, level);
        *flag = PACK_DL_DATA | PACK_DR_STARTED | PACK_UL_STARTED | PACK_UR_STARTED;
    } else {
        DescendWithFlags(colormask, level, FLAG_DATA, FLAG_DATA, FLAG_DATA, FLAG_DATA); 
    }
    return pixelData;
}

static void DuPackUpdateFlags(level_t *level, level_t *child, uint8_t fullFlag, uint8_t startedFlag){
    uint8_t tmpFlags = child->flagsRed;
    uint8_t allFull = ALL_ARE_FULL(tmpFlags);
    uint8_t anyNonEmpty = ANY_FULL_OR_STARTED(tmpFlags);
    if (allFull) {
        level->flagsRed |= fullFlag;
    } else if (anyNonEmpty) {
        level->flagsRed |= startedFlag;
    }
    tmpFlags = child->flagsGreen;
    allFull = ALL_ARE_FULL(tmpFlags);
    anyNonEmpty = ANY_FULL_OR_STARTED(tmpFlags);
    if (allFull) {
        level->flagsGreen |= fullFlag;
    } else if (anyNonEmpty) {
        level->flagsGreen |= startedFlag;
    }
    tmpFlags = child->flagsBlue;
    allFull = ALL_ARE_FULL(tmpFlags);
    anyNonEmpty = ANY_FULL_OR_STARTED(tmpFlags);
    if (allFull) {
        level->flagsBlue |= fullFlag;
    } else if (anyNonEmpty) {
        level->flagsBlue |= startedFlag;
    }
    tmpFlags = child->flagsAlpha;
    allFull = ALL_ARE_FULL(tmpFlags);
    anyNonEmpty = ANY_FULL_OR_STARTED(tmpFlags);
    if (allFull) {
        level->flagsAlpha |= fullFlag;
    } else if (anyNonEmpty) {
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
                                           UNPACK_UL_FULL(level->flagsRed), 
                                           UNPACK_UL_FULL(level->flagsGreen), 
                                           UNPACK_UL_FULL(level->flagsBlue), 
                                           UNPACK_UL_FULL(level->flagsAlpha)) - level;
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
                                           UNPACK_UR_FULL(level->flagsRed), 
                                           UNPACK_UR_FULL(level->flagsGreen), 
                                           UNPACK_UR_FULL(level->flagsBlue), 
                                           UNPACK_UR_FULL(level->flagsAlpha)) - level;
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
                                           UNPACK_DL_FULL(level->flagsRed), 
                                           UNPACK_DL_FULL(level->flagsGreen), 
                                           UNPACK_DL_FULL(level->flagsBlue), 
                                           UNPACK_DL_FULL(level->flagsAlpha)) - level;
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
                                           UNPACK_DR_FULL(level->flagsRed), 
                                           UNPACK_DR_FULL(level->flagsGreen), 
                                           UNPACK_DR_FULL(level->flagsBlue), 
                                           UNPACK_DR_FULL(level->flagsAlpha)) - level;
            }
            pixelResult = DuPackAddTextureRec(dimension, left + level->dimension / 2, bottom, level + level->dr);
            if (pixelResult) {
                DuPackUpdateFlags(level, level + level->dr, PACK_DR_FULL, PACK_DR_STARTED);
                return pixelResult;
            }
        }

    } else if (dimension <= level->dimension) {
        bool lessThanThreeFourths = (level->dimension > 2 && dimension <= (level->dimension - level->dimension/4));
        pixelData_t *pixelData = NULL;
        if (level->flagsRed == FLAG_EMPTY) {
            pixelData = AddToLowerLeftEdge(COLORMASK_RED, left, bottom, level, lessThanThreeFourths);
            if (pixelData != NULL) {
                return pixelData;
            }
        }
        if (level->flagsGreen == FLAG_EMPTY) {
            pixelData = AddToLowerLeftEdge(COLORMASK_GREEN, left, bottom, level, lessThanThreeFourths);
            if (pixelData != NULL) {
                return pixelData;
            }
        }
        if (level->flagsBlue == FLAG_EMPTY) {
            pixelData = AddToLowerLeftEdge(COLORMASK_BLUE, left, bottom, level, lessThanThreeFourths);
            if (pixelData != NULL) {
                return pixelData;
            }
        }
        if (level->flagsAlpha == FLAG_EMPTY) {
            pixelData = AddToLowerLeftEdge(COLORMASK_ALPHA, left, bottom, level, lessThanThreeFourths);
            if (pixelData != NULL) {
                return pixelData;
            }
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

void DuPackGetTextureIntCoords(int index, int *left, int *right, int *bottom, int *top) {
    pixelData_t * pixelData = textureHead.dataStack + index;
    *left = pixelData->x;
    *right = pixelData->x + pixelData->width;
    *bottom = pixelData->y;
    *top = pixelData->y + pixelData->height;
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
    memset(textureHead.textureData, 0xFF, textureHead.textureSizeInBytes);
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

    DuPackAddLevel(textureSize, FLAG_EMPTY, FLAG_EMPTY, FLAG_EMPTY, FLAG_EMPTY);
    textureHead.colorTexture = DuPackCreateAllColorsTexture();
}

void DuPackFinish(void) {
    free(textureHead.textureData);
    free(textureHead.textureStack);
    free(textureHead.dataStack);
}