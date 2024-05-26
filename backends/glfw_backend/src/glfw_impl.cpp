/* BASIC GLFW EXAMPLE AS IN https://www.glfw.org/docs/latest/quick.html */

#define GLFW_DLL
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
 
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
 
#define WOLF_RGB(r, g, b) {(r)*255/63, (g)*255/63, (b)*255/63, 0}

static GLFWwindow* window;
struct wolfColor_t {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
};

static struct wolfColor_t gamepal[]={
    #if defined (SPEAR) || defined (SPEARDEMO)
    #include "sodpal.inc"
    #else
    #include "wolfpal.inc"
    #endif
};

typedef struct Vertex
{
    float pos[2];
} Vertex;
 
static const Vertex vertices[3] =
{
    { { -0.6f, -0.4f } },
    { {  0.6f, -0.4f } },
    { {   0.f,  0.6f } }
};
 
static GLuint paletteTexture;
static GLuint imageTexture; 

static const char *vertSource = 
"#version 330\n"
"layout(location=0)in vec2 vPos;\n"
"out vec2 pictPos;\n"
"void main() {\n"
"  gl_Position = vec4(vPos, 0.0, 1.0);\n"
"  pictPos = vPos / 2.0 + vec2(0.5, 0.5);\n"
"}";

static const char *fragSource = 
"#version 330\n"
"in vec2 pictPos;\n"
"uniform usampler1D paletteTexture;\n"
"uniform sampler2D imageTexture;\n"
"out vec4 color;\n"
"void main() {\n"
"  vec4 color1 = texture(imageTexture, pictPos);\n"
"  color = texture(paletteTexture, color1.r)/255.0;\n"
"}";
 
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
 
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void resize_callback(GLFWwindow* window, int width, int height) {
    glViewport(0,0, width, height);
}
 



int initGlfw(void)
{
    glfwSetErrorCallback(error_callback);
 
    if (!glfwInit())
        exit(EXIT_FAILURE);
 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
 
    window = glfwCreateWindow(640, 480, "Chocolate Wolfenstein 3d", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
 
    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowSizeCallback(window, resize_callback);
    glfwMakeContextCurrent(window);
    gladLoadGL();
    //gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);
 
 
    /* create palette texture */
    paletteTexture;
    glActiveTexture(GL_TEXTURE1);
    glGenTextures(1, &paletteTexture);
    
    glBindTexture(GL_TEXTURE_1D, paletteTexture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA8UI, 256, 0, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, gamepal);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GLenum glerr;

    /* create image texture */
    imageTexture;
    glActiveTexture(GL_TEXTURE2);
    glGenTextures(1, &imageTexture);
    glBindTexture(GL_TEXTURE_2D, imageTexture);
    uint8_t *tempData = (uint8_t*)malloc(320*200);
    for (int i = 0; i < 320*200; ++i) {
        tempData[i] = 20;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 320, 200, 0, GL_RED, GL_UNSIGNED_BYTE, tempData);
    free(tempData);

    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
 
    const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertSource, NULL);
    glCompileShader(vertex_shader);
    {
        GLint isCompiled = 0;
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &isCompiled);
        if(isCompiled == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            GLchar *errorLog = (GLchar *)malloc(maxLength+1);
            glGetShaderInfoLog(vertex_shader, maxLength, &maxLength, errorLog);
            printf("vs:%s", errorLog);
            free(errorLog);
        }
    }
 
    const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragSource, NULL);
    glCompileShader(fragment_shader);
 
    {
        GLint isCompiled = 0;
        glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &isCompiled);
        if(isCompiled == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            GLchar *errorLog = (GLchar *)malloc(maxLength+1);
            glGetShaderInfoLog(vertex_shader, maxLength, &maxLength, errorLog);
            printf("fs:%s", errorLog);
            free(errorLog);
        }
    }

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    {
        GLint isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
        if (isLinked == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            GLchar *infoLog = (GLchar *)malloc(maxLength+1);
            
            glGetProgramInfoLog(program, maxLength, &maxLength, infoLog);
            printf("prog:%s", infoLog);
            free(infoLog);
        }
    }

    glUseProgram(program);
    const GLint vpos_location = glGetAttribLocation(program, "vPos");
    const GLint paletteTexture_location = glGetUniformLocation(program, "paletteTexture");
    if (paletteTexture_location) {
        glUniform1i(paletteTexture_location, 1);
    }
    const GLint imageTexture_location = glGetUniformLocation(program, "imageTexture");
    if (imageTexture_location) {
        glUniform1i(imageTexture_location, 2);
    }

    GLuint vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), NULL);
 

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    const float ratio = width / (float) height;

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glBindVertexArray(vertex_array);
    glDrawArrays(GL_TRIANGLES, 0, 3);


    glfwSwapBuffers(window);


    return 0;
}

/* IMPL ******************************************************************/
extern void *_GetLatchPic(int which);
void *GetLatchPic(int which) {
    return NULL;
}

extern void _SetLatchPic(int which, void *data);
void SetLatchPic(int which, void *data) {
    ;
}

extern int _GetLatchPicWidth(int which);
int GetLatchPicWidth(int which) {
    return 0;
}

extern int _GetLatchPicHeight(int which);
int GetLatchPicHeight(int which) {
    return 0;
}

extern void _DelayWolfTicks(int ticks);
void DelayWolfTicks(int ticks) {
    ;
}

extern void _DelayMilliseconds(int milliseconds);
void DelayMilliseconds(int milliseconds) {
    ;
}

extern void _DelayVBL(int param);
void DelayVBL(int param) {

    ;
}

extern unsigned int _GetWolfTicks(void); 
unsigned int GetWolfTicks(void) {
    return 0;
}

extern unsigned int _GetMilliseconds(void);
unsigned int GetMilliseconds(void) {
    return 0;
}

extern unsigned char *_GraphicLockBytes(void *surface);
unsigned char *GraphicLockBytes(void *surface)
{
    return NULL;
}

extern void _GraphicUnlockBytes(void *surface);
void GraphicUnlockBytes(void *surface)
{
    ;
}

extern void *_CreateScreenBuffer(void *gamepal, unsigned int *bufferPitch, unsigned int screenWidth, unsigned int screenHeight);
void *CreateScreenBuffer(void *gamepal, unsigned int *bufferPitch, unsigned int screenWidth, unsigned int screenHeight) {
    return NULL;
}

extern void *_GetScreenBuffer(void);
void *GetScreenBuffer(void) {
    return NULL;
}

extern unsigned char _GetScreenBufferPixel(int offset);
unsigned char GetScreenBufferPixel(int offset) {
    return 0;
}

extern void _GetCurrentPaletteColor(int color, int *red, int *green, int *blue);
void GetCurrentPaletteColor(int color, int *red, int *green, int *blue) {
    ;
}

extern void _SetCurrentPaletteColor(int color, int red, int green, int blue, unsigned int screenBits);
void SetCurrentPaletteColor(int color, int red, int green, int blue, unsigned int screenBits) {
    ;
}

extern void _SetWholePalette(void *palette, int forceupdate);
void SetWholePalette(void *palette, int forceupdate) {
    ;
}

extern void _ConvertPalette(unsigned char *srcpal, void *dest, int numColors);
void ConvertPalette(unsigned char *srcpal, void *dest, int numColors) {
    ;
}

extern void _FillPalette(int red, int green, int blue);
void FillPalette(int red, int green, int blue) {
    ;
}

extern void _GetWholePalette(void *palette);
void GetWholePalette(void *palette) {
    ;
}

extern void _SetScreenPalette(void);
void SetScreenPalette(void) {
    ;
}

extern void _SetWindowTitle(const char *title);
void SetWindowTitle(const char *title) {
    ;
}

extern void _SetScreenBits(void);
void SetScreenBits(void) {
    ; 
}

extern unsigned _GetScreenBits(void);
unsigned GetScreenBits(void) {
    return 0;
}

extern void _SetScreen(void *screenPtr);
void SetScreen(void *screenPtr) {
    ;
}

extern void *_GetScreen(void);
void *GetScreen(void) {
    return NULL;
}

extern short _GetScreenFlags(void);
short GetScreenFlags(void) {
    return NULL;
}

extern unsigned short _GetScreenPitch(void);
unsigned short GetScreenPitch(void) {
    return 0;
}

extern void *_GetScreenFormat(void);
void *GetScreenFormat(void) {
    return NULL;
}

extern unsigned char _GetScreenBytesPerPixel(void);
unsigned char GetScreenBytesPerPixel(void) {
    return 0;
}

extern void *_GetCurSurface(void);
void *GetCurSurface(void) {
    return NULL;
}

extern void _SetCurSurface(void *current);
void SetCurSurface(void *current) {
    ;
}

extern unsigned char *_GetCurSurfacePixels(void);
unsigned char *GetCurSurfacePixels(void) {
    return NULL;
}

extern void _ClearCurrentSurface(unsigned int color);
void ClearCurrentSurface(unsigned int color) {
   ;
}

extern unsigned char *_GetSurfacePixels(void *surface);
unsigned char *GetSurfacePixels(void *surface) {
    return NULL;
}

extern unsigned short _GetSurfacePitch(void *surface);
unsigned short GetSurfacePitch(void *surface) {
    return 0;
}

extern void *_GetGamePal(void);
void *GetGamePal(void) {
    return NULL;
}

extern void _CenterWindow(void);
void CenterWindow(void) {
    ;
}

extern void _ConvertPaletteToRGB(unsigned char *pixelPointer, int width, int height);
void ConvertPaletteToRGB(unsigned char *pixelPointer, int width, int height) {
    ;
}

extern void _ScreenToScreen (void *source, void *dest);
void ScreenToScreen (void *source, void *dest) {
    ;
}

extern void _LatchToScreenScaledCoord(int which, int xsrc, int ysrc, int width, int height, int scxdest, int scydest);
void LatchToScreenScaledCoord(int which, int xsrc, int ysrc, int width, int height, int scxdest, int scydest) {
    ;
}

static void cleanup(void) {
    glfwDestroyWindow(window);
    window = NULL;
    glfwTerminate();
}

extern void _InitGraphics(void);
void InitGraphics(void) {
    initGlfw();
    atexit(cleanup);
}

extern void _ReadMouseState(int *btns, int *mx, int *my);
void ReadMouseState(int *btns, int *mx, int *my) {
    ;
}

extern void _CenterMouse(int width, int height);
void CenterMouse(int width, int height) {
    ;
}

extern void _InitRedShifts (void);
void InitRedShifts (void) {
    ;
}

extern void _InitWhiteShifts (void);
void InitWhiteShifts (void) {
    ;
}

extern int _GetWhitePaletteShifts(void);
int GetWhitePaletteShifts(void) {
    return 0;
}

extern int _GetRedPaletteShifts(void);
int GetRedPaletteShifts(void) {
    return 0;
}

extern int _GetWhitePaletteSwapMs(void);
int GetWhitePaletteSwapMs(void) {
    return 0;
}

extern void* _GetRedPaletteShifted(int which);
void* GetRedPaletteShifted(int which) {
    return NULL;
}

extern void* _GetWhitePaletteShifted(int which);
void* GetWhitePaletteShifted(int which) {
    return NULL;
}

extern void _PaletteFadeOut (int start, int end, int red, int green, int blue, int steps);
void PaletteFadeOut (int start, int end, int red, int green, int blue, int steps) {
    ;
}

extern void _PaletteFadeIn(int start, int end, void *platettePtr, int steps);
void PaletteFadeIn(int start, int end, void *platettePtr, int steps) {
    ;
}

extern void _SaveBitmap(char *filename);
void SaveBitmap(char *filename) {
    ;
}

extern int _GetMouseButtons(void);
int GetMouseButtons(void) {
    return 0;
}

extern int _GetNuberOfJoysticks(void);
int GetNuberOfJoysticks(void) {
    return 0;
}

extern void _SetVGAMode(unsigned *scrWidth, unsigned *scrHeight, 
                unsigned *scrPitch, unsigned *bufPitch, 
                unsigned *currPitch, unsigned *sclFactor);
void SetVGAMode(unsigned *scrWidth, unsigned *scrHeight, 
                unsigned *scrPitch, unsigned *bufPitch, 
                unsigned *currPitch, unsigned *sclFactor) {
                    ;
}

extern void _LoadLatchMemory (void);
void LoadLatchMemory (void) {
    ;
}

extern int _SubFizzleFade (void *src, int x1, int y1,
                       unsigned width, unsigned height, 
                       unsigned frames, int abortable,
                       int rndbits_y, int rndmask);
int SubFizzleFade (void *src, int x1, int y1,
                       unsigned width, unsigned height, 
                       unsigned frames, int abortable,
                       int rndbits_y, int rndmask){
    return 0;
}

extern void _GetJoystickDelta(int *dx, int *dy);
void GetJoystickDelta(int *dx, int *dy) {
    ;
}

extern void _GetJoystickFineDelta(int *dx, int *dy);
void GetJoystickFineDelta(int *dx, int *dy) {
    ;
}

extern int _GetJoystickButtons(void);
int GetJoystickButtons(void) {
    return 0;
}

extern int _IsJoystickPresent(void);
int IsJoystickPresent(void) {
    return 0;
}

extern void ProcessEvents(void);
void WaitAndProcessEvents(void) {
    if (window) {
        glfwWaitEvents();
        glfwPollEvents();
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        if (glfwWindowShouldClose(window)) {
            exit(0);
        }
    }
}

void ProcessEvents(void) {
    if (window) {
        glfwPollEvents();
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        if (glfwWindowShouldClose(window)) {
            exit(0);
        }
    }
}

extern int _IsInputGrabbed(void);
int IsInputGrabbed(void) {
    return 0;
}

extern void _JoystickShutdown(void);
void JoystickShutdown(void) {
    ;
}

extern void _JoystickStartup(void);
void JoystickStartup(void) {
    ;
}


extern void _CheckIsJoystickCorrect(void);
void CheckIsJoystickCorrect(void) {
    ;
}






void VL_MemToScreenScaledCoord (unsigned char *source, int width, int height, int destx, int desty)
{
    unsigned char *pixels = (unsigned char *)malloc(width * height);
    // for (int j = 0; j < height; ++j) {
    //     for (int i = 0; i < width; ++i) {
    //         int foffset = ((i & 3) * (width >> 2) * height) + (i >> 2) + j * (width >>2);
    //         pixels[(height - j - 1) * width + i] = source[foffset];
    //    }
    // }

    for (int j = 0; j < 4; ++j) {
        for (int k = height-1; k >= 0; --k) {
            int offset = j;
            for (int i = 0; i < width/4; ++i) {
                pixels[k * width + offset] = *source++;
                offset = (offset + 4) % width;
            }
       }
    }
    glActiveTexture(GL_TEXTURE2);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    //glActiveTexture(GL_TEXTURE2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glGenerateMipmap(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    free(pixels);


}



void VL_Plot (int x, int y, int color)
{
    ;
}


unsigned char VL_GetPixel (int x, int y)
{
    return 0;
}

void VL_Hlin (unsigned x, unsigned y, unsigned width, int color)
{
    ;
}

void VL_Vlin (int x, int y, int height, int color)
{
    ;
}


void VL_BarScaledCoord (int scx, int scy, int scwidth, int scheight, int color)
{
    ;
}


void VL_MemToLatch(unsigned char *source, int width, int height,
    void *destSurface, int x, int y)
{
    ;
}

void VL_MemToScreenScaledCoord (unsigned char *source, int origwidth, int origheight, int srcx, int srcy,
                                int destx, int desty, int width, int height)
{
    ;
}


void VL_LatchToScreenScaledCoord(int which, int xsrc, int ysrc,
    int width, int height, int scxdest, int scydest)
{
    ;
}