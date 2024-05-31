#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

#define PACK_EMPTY 0
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
    uint16_t rgba;
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
    uint32_t textureDataSize;
    uint32_t textureStackSize;
    uint32_t dataStackSize;
    uint32_t textureStackCurrent;
    uint32_t dataStackCurrent;
} textureHead;

static pixelData_t *DuPackAddPixelData(uint8_t colorMask, uint8_t *data) {
    if (textureHead.dataStackCurrent < textureHead.dataStackSize) {
        pixelData_t *level = textureHead.dataStack + textureHead.dataStackCurrent;
        textureHead.dataStackCurrent++;
        // initailize
        return level;
    }
    return NULL;
}

static level_t *DuPackAddLevel(uint16_t dimension) {
    if (textureHead.textureStackCurrent < textureHead.textureStackSize) {
        level_t *level = textureHead.textureStack + textureHead.textureStackCurrent;
        textureHead.textureStackCurrent++;
        level->dimension = dimension;
        level->flagsRed = level->flagsGreen = level->flagsBlue = level->flagsAlpha = 0;
        return level;
    }
    return NULL;
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

}

static pixelData_t *DuPackAddTextureRec(uint16_t dimension, uint8_t *data, uint16_t left, uint16_t bottom, level_t *level) {
    if (dimension <= level->dimension / 2) {

    } else if (dimension <= level->dimension) {
        if (level->flagsRed == PACK_EMPTY) {
            // allocate red
            level->flagsRed = PACK_DL_DATA | PACK_DR_DATA | PACK_UL_DATA | PACK_UR_DATA;
            // return allocation
        }
    }
    return NULL;
}

void DuPackAddTexture(int width, int height, unsigned char *data) {
    uint16_t dimension = (uint16_t) (width > height ? width : height);

    pixelData_t * level = DuPackAddTextureRec(dimension, data, 0, 0, textureHead.textureStack);

}