#include "../backends/glfw_backend/src/dummyPacker.cpp"


void PrintPacker(level_t *level, int id) {
    printf("id:%d dimensions:%d\n", id, level->dimension);
    printf("redFlags DL:%d DR:%d UL:%d UR:%d\n", UNPACK_DL_DATA(level->flagsRed), UNPACK_DR_DATA(level->flagsRed), UNPACK_UL_DATA(level->flagsRed), UNPACK_UR_DATA(level->flagsRed));
    printf("greenFlags DL:%d DR:%d UL:%d UR:%d\n", UNPACK_DL_DATA(level->flagsGreen), UNPACK_DR_DATA(level->flagsGreen), UNPACK_UL_DATA(level->flagsGreen), UNPACK_UR_DATA(level->flagsGreen));
    printf("blueFlags DL:%d DR:%d UL:%d UR:%d\n", UNPACK_DL_DATA(level->flagsBlue), UNPACK_DR_DATA(level->flagsBlue), UNPACK_UL_DATA(level->flagsBlue), UNPACK_UR_DATA(level->flagsBlue));
    printf("alphaFlags DL:%d DR:%d UL:%d UR:%d\n", UNPACK_DL_DATA(level->flagsAlpha), UNPACK_DR_DATA(level->flagsAlpha), UNPACK_UL_DATA(level->flagsAlpha), UNPACK_UR_DATA(level->flagsAlpha));
    
    printf("Nodes: DL:%d DR:%d UL:%d UR:%d\n\n", 
        level->dl == EMPTY_NODE ? EMPTY_NODE : id + level->dl, 
        level->dr == EMPTY_NODE ? EMPTY_NODE : id + level->dr, 
        level->ul == EMPTY_NODE ? EMPTY_NODE : id + level->ul, 
        level->ur == EMPTY_NODE ? EMPTY_NODE : id + level->ur);
    if (level->ul != EMPTY_NODE) {
        PrintPacker(level + level->ul, id + level->ul);
    }
    if (level->ur != EMPTY_NODE) {
        PrintPacker(level + level->ur, id + level->ur);
    }
    if (level->dl != EMPTY_NODE) {
        PrintPacker(level + level->dl, id + level->dl);
    }
    if (level->dr != EMPTY_NODE) {
        PrintPacker(level + level->dr, id + level->dr);
    }

}


int main() {
    unsigned char *pixels = (unsigned char *)malloc(1024*1024);

    DuPackInit(1024, 100, 300);

    memset(pixels, 0xF0, 1024*1024);
    DuPackAddTexture(260, 260, pixels);

    memset(pixels, 0xF3, 1024*1024);
    DuPackAddTexture(260, 260, pixels);

    memset(pixels, 0xF4, 1024*1024);
    DuPackAddTexture(260, 260, pixels);

    memset(pixels, 0xF5, 1024*1024);
    DuPackAddTexture(260, 260, pixels);

    memset(pixels, 0xF6, 1024*1024);
    DuPackAddTexture(128, 128, pixels);

    memset(pixels, 0xF7, 1024*1024);
    DuPackAddTexture(128, 128, pixels);

    memset(pixels, 0xF8, 1024*1024);
    DuPackAddTexture(128, 128, pixels);

    memset(pixels, 0xF9, 1024*1024);
    DuPackAddTexture(128, 128, pixels);

    memset(pixels, 0xF6, 1024*1024);
    DuPackAddTexture(16, 16, pixels);

    memset(pixels, 0xF7, 1024*1024);
    DuPackAddTexture(16, 16, pixels);

    memset(pixels, 0xF8, 1024*1024);
    DuPackAddTexture(16, 16, pixels);

    memset(pixels, 0xF9, 1024*1024);
    DuPackAddTexture(16, 16, pixels);

    memset(pixels, 0xF6, 1024*1024);
    DuPackAddTexture(16, 16, pixels);

    memset(pixels, 0xF7, 1024*1024);
    DuPackAddTexture(16, 16, pixels);

    memset(pixels, 0xF8, 1024*1024);
    DuPackAddTexture(16, 16, pixels);

    memset(pixels, 0xF9, 1024*1024);
    DuPackAddTexture(16, 16, pixels);

    memset(pixels, 0xF6, 1024*1024);
    DuPackAddTexture(16, 16, pixels);

    memset(pixels, 0xF7, 1024*1024);
    DuPackAddTexture(16, 16, pixels);

    memset(pixels, 0xF8, 1024*1024);
    DuPackAddTexture(16, 16, pixels);

    memset(pixels, 0xF9, 1024*1024);
    DuPackAddTexture(16, 16, pixels);

    PrintPacker(textureHead.textureStack, 0);
    return 0;
}