/* BASIC GLFW EXAMPLE AS IN https://www.glfw.org/docs/latest/quick.html */

#define GLFW_DLL
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
 
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
 
#define WOLF_RGB(r, g, b) {(r)*255/63, (g)*255/63, (b)*255/63, 1}

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

    // switch (action)
    // {
    //     // exit if the window is closed
    //     case SDL_QUIT:
    //         Quit(NULL);

    //     // check for keypresses
    //     case GLFW_PRESS:
    //     {
    //         if(key == GLFW_KEY_SCROLL_LOCK || key == GLFW_KEY_F12)
    //         {
    //             GrabInput = !GrabInput;
    //             SDL_WM_GrabInput(GrabInput ? SDL_GRAB_ON : SDL_GRAB_OFF);
    //             return;
    //         }

    //         LastScan = key;
    //         SDLMod mod = SDL_GetModState();
    //         if(Keyboard[sc_Alt])
    //         {
    //             if(LastScan==SDLK_F4)
    //                 Quit(NULL);
    //         }

    //         if(LastScan == SDLK_KP_ENTER) LastScan = SDLK_RETURN;
    //         else if(LastScan == SDLK_RSHIFT) LastScan = SDLK_LSHIFT;
    //         else if(LastScan == SDLK_RALT) LastScan = SDLK_LALT;
    //         else if(LastScan == SDLK_RCTRL) LastScan = SDLK_LCTRL;
    //         else
    //         {
    //             if((mod & KMOD_NUM) == 0)
    //             {
    //                 switch(LastScan)
    //                 {
    //                     case SDLK_KP2: LastScan = SDLK_DOWN; break;
    //                     case SDLK_KP4: LastScan = SDLK_LEFT; break;
    //                     case SDLK_KP6: LastScan = SDLK_RIGHT; break;
    //                     case SDLK_KP8: LastScan = SDLK_UP; break;
    //                 }
    //             }
    //         }

    //         int sym = LastScan;
    //         if(sym >= 'a' && sym <= 'z')
    //             sym -= 32;  // convert to uppercase

    //         if(mod & (KMOD_SHIFT | KMOD_CAPS))
    //         {
    //             if(sym < lengthof(ShiftNames) && ShiftNames[sym])
    //                 LastASCII = ShiftNames[sym];
    //         }
    //         else
    //         {
    //             if(sym < lengthof(ASCIINames) && ASCIINames[sym])
    //                 LastASCII = ASCIINames[sym];
    //         }

	// 		if (LastScan<SDLK_i){
	// 		}

	// 		if(LastScan<SDLK_LAST){
    //             Keyboard[LastScan] = 1;
	// 		}
    //         if(LastScan == SDLK_PAUSE)
    //             Paused = true;
    //         break;
    //     }

    //     case GLFW_RELEASE:
    //     {
            
    //         if(key == SDLK_KP_ENTER) key = SDLK_RETURN;
    //         else if(key == SDLK_RSHIFT) key = SDLK_LSHIFT;
    //         else if(key == SDLK_RALT) key = SDLK_LALT;
    //         else if(key == SDLK_RCTRL) key = SDLK_LCTRL;
    //         else
    //         {
    //             if((SDL_GetModState() & KMOD_NUM) == 0)
    //             {
    //                 switch(key)
    //                 {
    //                     case SDLK_KP2: key = SDLK_DOWN; break;
    //                     case SDLK_KP4: key = SDLK_LEFT; break;
    //                     case SDLK_KP6: key = SDLK_RIGHT; break;
    //                     case SDLK_KP8: key = SDLK_UP; break;
    //                 }
    //             }
    //         }

	// 		if(key<SDLK_LAST){
    //             Keyboard[key] = 0;
	// 		}
    //         break;
    //     }
    // }










    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void resize_callback(GLFWwindow* window, int width, int height) {
    int hr = height * 320;
    int wr = width * 240;
    if (hr > wr) {
        int diff = (hr - wr) / (320*2);
        glViewport(0,diff, wr/240, wr/320);
    } else {
        int diff = (wr - hr) / (240*2);
        glViewport(diff,0, hr/240, hr/320);
    }
    glClear(GL_COLOR_BUFFER_BIT);
}

int initGlfw(void)
{
    glfwSetErrorCallback(error_callback);
 
    if (!glfwInit())
        exit(EXIT_FAILURE);
 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 0);
 
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

    glfwSwapInterval(1);
 
    GLuint vboRects;
    glGenVertexArrays(1, &vboRects);
    glBindVertexArray(vboRects);

    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    float fData[] = {-1.f, -1.f, 1.f, -1.f, -1.f, 1.f,
                    1.f, 1.f, 1.f, -1.f, -1.f, 1.f};
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), fData, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

 
    /* create palette texture */
    paletteTexture;
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &paletteTexture);
    
    // fix alpha
    gamepal[255].alpha = 0;
    glBindTexture(GL_TEXTURE_1D, paletteTexture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA8UI, 256, 0, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, gamepal);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_1D);
    GLenum glerr;

    /* create image texture */
    imageTexture;
    glActiveTexture(GL_TEXTURE1);
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
    glGenerateMipmap(GL_TEXTURE_2D);
    free(tempData);
 
    const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertSource, NULL);
    glCompileShader(vertex_shader);
    
 
    const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragSource, NULL);
    glCompileShader(fragment_shader);

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glUseProgram(program);

    const GLint paletteTexture_location = glGetUniformLocation(program, "paletteTexture");
    if (paletteTexture_location >= 0) {
        glUniform1i(paletteTexture_location, 0);
    }
    const GLint imageTexture_location = glGetUniformLocation(program, "imageTexture");
    if (imageTexture_location >= 0) {
        glUniform1i(imageTexture_location, 1);
    }
 

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    const float ratio = width / (float) height;

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glBindVertexArray(vboRects);
    glDrawArrays(GL_TRIANGLES, 0, 6);


    glfwSwapBuffers(window);


    return 0;
}

/* IMPL ******************************************************************/

void DelayWolfTicks(int ticks) {
    ;
}

void DelayMilliseconds(int milliseconds) {
    ;
}

void DelayVBL(int param) {

    ;
}

unsigned int GetWolfTicks(void) {
    return 0;
}

unsigned int GetMilliseconds(void) {
    return 0;
}

/* This function should swap buffers */
void SetWholePalette(void *palette, int forceupdate) {
    ;
}

void ClearCurrentSurface(unsigned int color) {
   ;
}

void *GetGamePal(void) {
    return NULL;
}

void CenterWindow(void) {
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

void ReadMouseState(int *btns, int *mx, int *my) {
    ;
}

void CenterMouse(int width, int height) {
    ;
}

void InitRedShifts (void) {
    ;
}

void InitWhiteShifts (void) {
    ;
}

int GetWhitePaletteShifts(void) {
    return 0;
}

int GetRedPaletteShifts(void) {
    return 0;
}

int GetWhitePaletteSwapMs(void) {
    return 0;
}

void* GetRedPaletteShifted(int which) {
    return NULL;
}

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

/* This function should swap buffers */
extern int _SubFizzleFade (void *src, int x1, int y1,
                       unsigned width, unsigned height, 
                       unsigned frames, int abortable,
                       int rndbits_y, int rndmask);
int SubFizzleFade (int x1, int y1,
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

// static void processEvent(SDL_Event *event)
// {
//     switch (event->type)
//     {
//         // exit if the window is closed
//         case SDL_QUIT:
//             Quit(NULL);

//         // check for keypresses
//         case SDL_KEYDOWN:
//         {
//             if(event->key.keysym.sym==SDLK_SCROLLOCK || event->key.keysym.sym==SDLK_F12)
//             {
//                 GrabInput = !GrabInput;
//                 SDL_WM_GrabInput(GrabInput ? SDL_GRAB_ON : SDL_GRAB_OFF);
//                 return;
//             }

//             LastScan = event->key.keysym.sym;
//             SDLMod mod = SDL_GetModState();
//             if(Keyboard[sc_Alt])
//             {
//                 if(LastScan==SDLK_F4)
//                     Quit(NULL);
//             }

//             if(LastScan == SDLK_KP_ENTER) LastScan = SDLK_RETURN;
//             else if(LastScan == SDLK_RSHIFT) LastScan = SDLK_LSHIFT;
//             else if(LastScan == SDLK_RALT) LastScan = SDLK_LALT;
//             else if(LastScan == SDLK_RCTRL) LastScan = SDLK_LCTRL;
//             else
//             {
//                 if((mod & KMOD_NUM) == 0)
//                 {
//                     switch(LastScan)
//                     {
//                         case SDLK_KP2: LastScan = SDLK_DOWN; break;
//                         case SDLK_KP4: LastScan = SDLK_LEFT; break;
//                         case SDLK_KP6: LastScan = SDLK_RIGHT; break;
//                         case SDLK_KP8: LastScan = SDLK_UP; break;
//                     }
//                 }
//             }

//             int sym = LastScan;
//             if(sym >= 'a' && sym <= 'z')
//                 sym -= 32;  // convert to uppercase

//             if(mod & (KMOD_SHIFT | KMOD_CAPS))
//             {
//                 if(sym < lengthof(ShiftNames) && ShiftNames[sym])
//                     LastASCII = ShiftNames[sym];
//             }
//             else
//             {
//                 if(sym < lengthof(ASCIINames) && ASCIINames[sym])
//                     LastASCII = ASCIINames[sym];
//             }

// 			if (LastScan<SDLK_i){
// 			}

// 			if(LastScan<SDLK_LAST){
//                 Keyboard[LastScan] = 1;
// 			}
//             if(LastScan == SDLK_PAUSE)
//                 Paused = true;
//             break;
//         }

//         case SDL_KEYUP:
//         {
//             int key = event->key.keysym.sym;
//             if(key == SDLK_KP_ENTER) key = SDLK_RETURN;
//             else if(key == SDLK_RSHIFT) key = SDLK_LSHIFT;
//             else if(key == SDLK_RALT) key = SDLK_LALT;
//             else if(key == SDLK_RCTRL) key = SDLK_LCTRL;
//             else
//             {
//                 if((SDL_GetModState() & KMOD_NUM) == 0)
//                 {
//                     switch(key)
//                     {
//                         case SDLK_KP2: key = SDLK_DOWN; break;
//                         case SDLK_KP4: key = SDLK_LEFT; break;
//                         case SDLK_KP6: key = SDLK_RIGHT; break;
//                         case SDLK_KP8: key = SDLK_UP; break;
//                     }
//                 }
//             }

// 			if(key<SDLK_LAST){
//                 Keyboard[key] = 0;
// 			}
//             break;
//         }
//     }
// }

extern void ProcessEvents(void);
void WaitAndProcessEvents(void) {
    if (window) {
        glfwWaitEvents();
        glfwPollEvents();
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glfwSwapBuffers(window);
        if (glfwWindowShouldClose(window)) {
            exit(0);
        }
    }
}

void ProcessEvents(void) {
    if (window) {
        glfwPollEvents();
        glDrawArrays(GL_TRIANGLES, 0, 6);
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

    for (int j = 0; j < 4; ++j) {
        for (int k = height-1; k >= 0; --k) {
            int offset = j;
            for (int i = 0; i < width/4; ++i) {
                pixels[k * width + offset] = *source++;
                offset = (offset + 4) % width;
            }
       }
    }
    glActiveTexture(GL_TEXTURE1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_2D);
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

void LatchDrawPic (unsigned x, unsigned y, unsigned picnum)
{
    ;
}

void LatchDrawPicScaledCoord (unsigned scx, unsigned scy, unsigned picnum)
{
    ;
}

unsigned char *VL_LockSurface(void* surface)
{
    return NULL;
}

void VL_UnlockSurface(void *surface)
{
    ;
}

static void VL_ScreenToScreen (void *source, void *dest)
{
    ;
}

/* this function should swap buffers*/
void VH_UpdateScreen()
{
    ;
}

void    ThreeDRefresh (void)
{
    ;
}

void VWB_DrawPropString(const char* string)
{
    ;
}

void BlitPictureToScreen(unsigned char *pic) {
    ;
}