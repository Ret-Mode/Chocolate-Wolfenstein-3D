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
    float col[3];
} Vertex;
 
static const Vertex vertices[3] =
{
    { { -0.6f, -0.4f }, { 1.f, 0.f, 0.f } },
    { {  0.6f, -0.4f }, { 0.f, 1.f, 0.f } },
    { {   0.f,  0.6f }, { 0.f, 0.f, 1.f } }
};
 
static GLuint paletteTexture;
static GLuint imageTexture; 

static const char* vertex_shader_text =
"#version 330\n"
"uniform mat4 MVP;\n"
"in vec3 vCol;\n"
"in vec2 vPos;\n"
"out vec2 pPos;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    pPos = vPos;\n"
"}\n";
 
static const char* fragment_shader_text =
"#version 330\n"
"in vec2 pPos;\n"
"uniform sampler1D paletteTexture;\n"
"uniform usampler2D imageTexture;\n"
"out vec4 fragment;\n"
"void main()\n"
"{\n"
"uvec4 coords = texture(imageTexture,pPos);\n"
"    fragment = texture(paletteTexture, float(coords.x) / 255.0);\n"
"}\n";
 
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
 
    // NOTE: OpenGL error checks have been omitted for brevity
 
    /* create palette texture */
    paletteTexture;
    glActiveTexture(GL_TEXTURE1);
    glGenTextures(1, &paletteTexture);
    
    glBindTexture(GL_TEXTURE_1D, paletteTexture);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, gamepal);
    GLenum glerr;

    /* create image texture */
    imageTexture;
    glActiveTexture(GL_TEXTURE2);
    glGenTextures(1, &imageTexture);
    glBindTexture(GL_TEXTURE_2D, imageTexture);
    uint8_t *tempData = (uint8_t*)malloc(320*200);
    for (int i = 0; i < 320*200; ++i) {
        tempData[i] = 0;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, 320, 200, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, tempData);
    free(tempData);

    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
 
    const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
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
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
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

    const GLint mvp_location = glGetUniformLocation(program, "MVP");
    const GLint vpos_location = glGetAttribLocation(program, "vPos");
    const GLint vcol_location = glGetAttribLocation(program, "vCol");
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
                          sizeof(Vertex), (void*) offsetof(Vertex, pos));
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*) offsetof(Vertex, col));
 

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    const float ratio = width / (float) height;

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    float mvp[16];
    mvp[0] = 1.0f;
    mvp[1] = 0.0f;
    mvp[2] = 0.0f;
    mvp[3] = 0.0f;
    mvp[4] = 0.0f;
    mvp[5] = 1.0f;
    mvp[6] = 0.0f;
    mvp[7] = 0.0f;
    mvp[8] = 0.0f;
    mvp[9] = 0.0f;
    mvp[10] = 1.0f;
    mvp[11] = 0.0f;
    mvp[12] = 0.0f;
    mvp[13] = 0.0f;
    mvp[14] = 0.0f;
    mvp[15] = 1.0f;
    

    glUseProgram(program);
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) &mvp);
    glBindVertexArray(vertex_array);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);

    return 0;
}

/* IMPL ******************************************************************/
extern void *_GetLatchPic(int which);
void *GetLatchPic(int which) {
    return _GetLatchPic(which);
}

extern void _SetLatchPic(int which, void *data);
void SetLatchPic(int which, void *data) {
    _SetLatchPic(which, data);
}

extern int _GetLatchPicWidth(int which);
int GetLatchPicWidth(int which) {
    return _GetLatchPicWidth(which);
}

extern int _GetLatchPicHeight(int which);
int GetLatchPicHeight(int which) {
    return _GetLatchPicHeight(which);
}

extern void _DelayWolfTicks(int ticks);
void DelayWolfTicks(int ticks) {
    _DelayWolfTicks(ticks);
}

extern void _DelayMilliseconds(int milliseconds);
void DelayMilliseconds(int milliseconds) {
    _DelayMilliseconds(milliseconds);
}

extern void _DelayVBL(int param);
void DelayVBL(int param) {
    _DelayVBL(param);
}

extern unsigned int _GetWolfTicks(void); 
unsigned int GetWolfTicks(void) {
    return _GetWolfTicks();
}

extern unsigned int _GetMilliseconds(void);
unsigned int GetMilliseconds(void) {
    return _GetMilliseconds();
}

extern unsigned char *_GraphicLockBytes(void *surface);
unsigned char *GraphicLockBytes(void *surface)
{
    return _GraphicLockBytes(surface);
}

extern void _GraphicUnlockBytes(void *surface);
void GraphicUnlockBytes(void *surface)
{
    _GraphicUnlockBytes(surface);
}

extern void *_CreateScreenBuffer(void *gamepal, unsigned int *bufferPitch, unsigned int screenWidth, unsigned int screenHeight);
void *CreateScreenBuffer(void *gamepal, unsigned int *bufferPitch, unsigned int screenWidth, unsigned int screenHeight) {
    return _CreateScreenBuffer(gamepal, bufferPitch, screenWidth, screenHeight);
}

extern void *_GetScreenBuffer(void);
void *GetScreenBuffer(void) {
    return _GetScreenBuffer();
}

extern unsigned char _GetScreenBufferPixel(int offset);
unsigned char GetScreenBufferPixel(int offset) {
    return _GetScreenBufferPixel(offset);
}

extern void _GetCurrentPaletteColor(int color, int *red, int *green, int *blue);
void GetCurrentPaletteColor(int color, int *red, int *green, int *blue) {
    _GetCurrentPaletteColor(color, red, green, blue);
}

extern void _SetCurrentPaletteColor(int color, int red, int green, int blue, unsigned int screenBits);
void SetCurrentPaletteColor(int color, int red, int green, int blue, unsigned int screenBits) {
    _SetCurrentPaletteColor(color, red, green, blue, screenBits);
}

extern void _SetWholePalette(void *palette, int forceupdate);
void SetWholePalette(void *palette, int forceupdate) {
    _SetWholePalette(palette, forceupdate);
}

extern void _ConvertPalette(unsigned char *srcpal, void *dest, int numColors);
void ConvertPalette(unsigned char *srcpal, void *dest, int numColors) {
    _ConvertPalette(srcpal, dest, numColors);
}

extern void _FillPalette(int red, int green, int blue);
void FillPalette(int red, int green, int blue) {
    _FillPalette(red, green, blue);
}

extern void _GetWholePalette(void *palette);
void GetWholePalette(void *palette) {
    _GetWholePalette(palette);
}

extern void _SetScreenPalette(void);
void SetScreenPalette(void) {
    _SetScreenPalette();
}

extern void _SetWindowTitle(const char *title);
void SetWindowTitle(const char *title) {
    _SetWindowTitle(title);
}

extern void _SetScreenBits(void);
void SetScreenBits(void) {
    _SetScreenBits(); 
}

extern unsigned _GetScreenBits(void);
unsigned GetScreenBits(void) {
    return _GetScreenBits();
}

extern void _SetScreen(void *screenPtr);
void SetScreen(void *screenPtr) {
    _SetScreen(screenPtr);
}

extern void *_GetScreen(void);
void *GetScreen(void) {
    return _GetScreen();
}

extern short _GetScreenFlags(void);
short GetScreenFlags(void) {
    return _GetScreenFlags();
}

extern unsigned short _GetScreenPitch(void);
unsigned short GetScreenPitch(void) {
    return _GetScreenPitch();
}

extern void *_GetScreenFormat(void);
void *GetScreenFormat(void) {
    return _GetScreenFormat();
}

extern unsigned char _GetScreenBytesPerPixel(void);
unsigned char GetScreenBytesPerPixel(void) {
    return _GetScreenBytesPerPixel();
}

extern void *_GetCurSurface(void);
void *GetCurSurface(void) {
    return _GetCurSurface();
}

extern void _SetCurSurface(void *current);
void SetCurSurface(void *current) {
    return _SetCurSurface(current);
}

extern unsigned char *_GetCurSurfacePixels(void);
unsigned char *GetCurSurfacePixels(void) {
    return _GetCurSurfacePixels();
}

extern void _ClearCurrentSurface(unsigned int color);
void ClearCurrentSurface(unsigned int color) {
    return _ClearCurrentSurface(color);
}

extern unsigned char *_GetSurfacePixels(void *surface);
unsigned char *GetSurfacePixels(void *surface) {
    return _GetSurfacePixels(surface);
}

extern unsigned short _GetSurfacePitch(void *surface);
unsigned short GetSurfacePitch(void *surface) {
    return _GetSurfacePitch(surface);
}

extern void *_GetGamePal(void);
void *GetGamePal(void) {
    return _GetGamePal();
}

extern void _CenterWindow(void);
void CenterWindow(void) {
    _CenterWindow();
}

extern void _ConvertPaletteToRGB(unsigned char *pixelPointer, int width, int height);
void ConvertPaletteToRGB(unsigned char *pixelPointer, int width, int height) {
    _ConvertPaletteToRGB(pixelPointer, width, height);
}

extern void _ScreenToScreen (void *source, void *dest);
void ScreenToScreen (void *source, void *dest) {
    _ScreenToScreen (source, dest);
}

extern void _LatchToScreenScaledCoord(int which, int xsrc, int ysrc, int width, int height, int scxdest, int scydest);
void LatchToScreenScaledCoord(int which, int xsrc, int ysrc, int width, int height, int scxdest, int scydest) {
    _LatchToScreenScaledCoord(which, xsrc, ysrc, width, height, scxdest, scydest);
}

extern void _InitGraphics(void);
void InitGraphics(void) {
    _InitGraphics();
    initGlfw();
    atexit(glfwTerminate);
}

extern void _ReadMouseState(int *btns, int *mx, int *my);
void ReadMouseState(int *btns, int *mx, int *my) {
    _ReadMouseState(btns, mx, my);
}

extern void _CenterMouse(int width, int height);
void CenterMouse(int width, int height) {
    _CenterMouse(width, height);
}

extern void _InitRedShifts (void);
void InitRedShifts (void) {
    _InitRedShifts ();
}

extern void _InitWhiteShifts (void);
void InitWhiteShifts (void) {
    _InitWhiteShifts ();
}

extern int _GetWhitePaletteShifts(void);
int GetWhitePaletteShifts(void) {
    return _GetWhitePaletteShifts();
}

extern int _GetRedPaletteShifts(void);
int GetRedPaletteShifts(void) {
    return _GetRedPaletteShifts();
}

extern int _GetWhitePaletteSwapMs(void);
int GetWhitePaletteSwapMs(void) {
    return _GetWhitePaletteSwapMs();
}

extern void* _GetRedPaletteShifted(int which);
void* GetRedPaletteShifted(int which) {
    return _GetRedPaletteShifted(which);
}

extern void* _GetWhitePaletteShifted(int which);
void* GetWhitePaletteShifted(int which) {
    return _GetWhitePaletteShifted(which);
}

extern void _PaletteFadeOut (int start, int end, int red, int green, int blue, int steps);
void PaletteFadeOut (int start, int end, int red, int green, int blue, int steps) {
    _PaletteFadeOut (start, end, red, green, blue, steps);
}

extern void _PaletteFadeIn(int start, int end, void *platettePtr, int steps);
void PaletteFadeIn(int start, int end, void *platettePtr, int steps) {
    _PaletteFadeIn(start, end, platettePtr, steps);
}

extern void _SaveBitmap(char *filename);
void SaveBitmap(char *filename) {
    _SaveBitmap(filename);
}

extern int _GetMouseButtons(void);
int GetMouseButtons(void) {
    return _GetMouseButtons();
}

extern int _GetNuberOfJoysticks(void);
int GetNuberOfJoysticks(void) {
    return _GetNuberOfJoysticks();
}

extern void _SetVGAMode(unsigned *scrWidth, unsigned *scrHeight, 
                unsigned *scrPitch, unsigned *bufPitch, 
                unsigned *currPitch, unsigned *sclFactor);
void SetVGAMode(unsigned *scrWidth, unsigned *scrHeight, 
                unsigned *scrPitch, unsigned *bufPitch, 
                unsigned *currPitch, unsigned *sclFactor) {
    _SetVGAMode(scrWidth, scrHeight, 
                scrPitch, bufPitch, 
                currPitch, sclFactor);
    
}

extern void _LoadLatchMemory (void);
void LoadLatchMemory (void) {
    _LoadLatchMemory ();
}

extern int _SubFizzleFade (void *src, int x1, int y1,
                       unsigned width, unsigned height, 
                       unsigned frames, int abortable,
                       int rndbits_y, int rndmask);
int SubFizzleFade (void *src, int x1, int y1,
                       unsigned width, unsigned height, 
                       unsigned frames, int abortable,
                       int rndbits_y, int rndmask){
    return _SubFizzleFade(src, x1, y1,
                       width, height, 
                       frames, abortable,
                       rndbits_y, rndmask);
}

extern void _GetJoystickDelta(int *dx, int *dy);
void GetJoystickDelta(int *dx, int *dy) {
    _GetJoystickDelta(dx, dy);
}

extern void _GetJoystickFineDelta(int *dx, int *dy);
void GetJoystickFineDelta(int *dx, int *dy) {
    _GetJoystickFineDelta(dx, dy);
}

extern int _GetJoystickButtons(void);
int GetJoystickButtons(void) {
    return _GetJoystickButtons();
}

extern int _IsJoystickPresent(void);
int IsJoystickPresent(void) {
    return _IsJoystickPresent();
}

extern void _WaitAndProcessEvents(void);
void WaitAndProcessEvents(void) {
    _WaitAndProcessEvents();
}

extern void _ProcessEvents(void);
void ProcessEvents(void) {
    _ProcessEvents();
    if (window) {
        if(!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            glfwSwapBuffers(window);
        }
        else {
            glfwDestroyWindow(window);
            window = NULL;
        }
    }
}

extern int _IsInputGrabbed(void);
int IsInputGrabbed(void) {
    return _IsInputGrabbed();
}

extern void _JoystickShutdown(void);
void JoystickShutdown(void) {
    _JoystickShutdown();
}

extern void _JoystickStartup(void);
void JoystickStartup(void) {
    _JoystickStartup();
}


extern void _CheckIsJoystickCorrect(void);
void CheckIsJoystickCorrect(void) {
    _CheckIsJoystickCorrect();
}