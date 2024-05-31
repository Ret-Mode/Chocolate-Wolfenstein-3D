#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

#define PACK_EMPTY 0
#define PACK_STARTED 1
#define PACK_FULL 2
#define PACK_DATA 4

struct pixelData_t {
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    uint16_t rgba;
};

struct level_t {
    union {
        level_t *ul;
        pixelData_t *uld;
    };
    union {
        level_t *ur;
        pixelData_t *urd;
    };
    union {
        level_t *dl;
        pixelData_t *dld;
    };
    union {
        level_t *ur;
        pixelData_t *urd;
    };
    uint16_t dimension;
    uint8_t flags[4];
    uint8_t padding[2];
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

static level_t *DuPackAddLevel(uint16_t dimension) {
    if (textureHead.textureStackCurrent < textureHead.textureStackSize) {
        level_t *level = textureHead.textureStack + textureHead.textureStackCurrent;
        textureHead.textureStackCurrent++;
        level->dimension = dimension;
        level->flags[0] = PACK_EMPTY;
        level->flags[1] = PACK_EMPTY;
        level->flags[2] = PACK_EMPTY;
        level->flags[3] = PACK_EMPTY;
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

static level_t *DuPackAddTextureRec(uint16_t dimension, uint8_t *data, level_t *level) {
    if (dimension <= level->dimension / 2) {
        // descend
    } else if (dimension <= level->dimension) {
        if (level->flags[0] == PACK_EMPTY && level->flags[1] == PACK_EMPTY && level->flags[2] == PACK_EMPTY && level->flags[3] == PACK_EMPTY) {
            return level;
        }
    }
}

void DuPackAddTexture(int width, int height, unsigned char *data) {
    uint16_t dimension = (uint16_t) (width > height ? width : height);

    level_t * level = DuPackAddTextureRec(dimension, data, textureHead.textureStack);

}