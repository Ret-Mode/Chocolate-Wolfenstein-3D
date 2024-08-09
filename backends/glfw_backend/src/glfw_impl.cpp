/* BASIC GLFW EXAMPLE AS IN https://www.glfw.org/docs/latest/quick.html */

#define GLFW_DLL
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
 
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

 #ifdef APIENTRY
 #undef APIENTRY
 #endif

#include "wl_def.h"
#define WOLF_RGB(r, g, b) {(r)*255/63, (g)*255/63, (b)*255/63, 255}

extern void DuPackInit(unsigned int textureSize, 
                unsigned int levelStackSize, 
                unsigned int dataStackSize);
extern int DuPackAddTexture(int width, int height, unsigned char *data);
extern unsigned char *DuPackGetPalettizedTexture(void);
extern int DuPackGetTextureDimension(void); 
extern void DuPackGetTextureIntCoords(int index, int *left, int *right, int *bottom, int *top);
extern void DuPackGetTextureCoords(int index, float *left, float *right, float *bottom, float *top, int *rgbaOffset);
extern void DuPackGetColorCoords(int color, float *left, float *right, float *bottom, float *top, int *rgbaOffset);

static inline void ShufflePicColumns(uint8_t *dest, uint8_t *src, uint32_t width, uint32_t height) {
    for (int j = 0; j < 4; ++j) {
        for (int k = height-1; k >= 0; --k) {
            int offset = j;
            for (int l = 0; l < width/4; ++l) {
                dest[k * width + offset] = *src++;
                offset = (offset + 4) % width;
            }
        }
    }
}

static GLFWwindow* window;

struct wolfColor_t {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
};

struct boxCoords_t {
    float left;
    float right;
    float bottom;
    float top;
};

struct WindowCoords_t {
    int viewportX;
    int viewportY;
    int width;
    int height;
    int viewportWidth;
    int viewportHeight;
} WindowCoords;

static struct bufferInternals_t {
    float vertices[2*320*3];
    float texCoords[2*320*3];
    float colorMasks[320*3];
    int gpuCapacity;
    int capacity;
    int index;
} bufferInternals;

static struct wolfColor_t gamepal[]={
    #if defined (SPEAR) || defined (SPEARDEMO)
    #include "sodpal.inc"
    #else
    #include "wolfpal.inc"
    #endif
};
 
static int latchpics[NUMLATCHPICS];

 static byte ASCIINames[] =      // Unshifted ASCII for scan codes       // TODO: keypad
{
//   0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
    0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,8  ,9  ,0  ,0  ,0  ,13 ,0  ,0  ,    // 0
    0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,27 ,0  ,0  ,0  ,    // 1
    ' ',0  ,0  ,0  ,0  ,0  ,0  ,39 ,0  ,0  ,'*','+',',','-','.','/',    // 2
    '0','1','2','3','4','5','6','7','8','9',0  ,';',0  ,'=',0  ,0  ,    // 3
    '`','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',    // 4
    'p','q','r','s','t','u','v','w','x','y','z','[',92 ,']',0  ,0  ,    // 5
    0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,    // 6
    0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0       // 7
};
static byte ShiftNames[] =     // Shifted ASCII for scan codes
{
//   0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
    0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,8  ,9  ,0  ,0  ,0  ,13 ,0  ,0  ,    // 0
    0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,27 ,0  ,0  ,0  ,    // 1
    ' ',0  ,0  ,0  ,0  ,0  ,0  ,34 ,0  ,0  ,'*','+','<','_','>','?',    // 2
    ')','!','@','#','$','%','^','&','*','(',0  ,':',0  ,'+',0  ,0  ,    // 3
    '~','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',    // 4
    'P','Q','R','S','T','U','V','W','X','Y','Z','{','|','}',0  ,0  ,    // 5
    0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,    // 6
    0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0  ,0       // 7
};

static GLuint paletteTexture;
static GLuint imageTexture;

static GLuint backgroundTexture;
static GLuint vgaFramebuffer;

static GLuint vertexBuffer;
static GLuint paletteCoordBuffer;
static GLuint paletteMaskBuffer;

static GLuint programBackground;
static GLuint programBlit;
static GLuint vboRects;

static int GrabInput = false;



static const char *vertBackgroundSource = 
"#version 330\n"
"layout(location=1)in vec2 vVertPos;\n"
"layout(location=2)in vec2 vTexPos;\n"
"layout(location=3)in float vColorMask;\n"
"out vec2 fPictPos;\n"
"flat out float fColorMask;\n"
"void main() {\n"
"  gl_Position = vec4(vVertPos, 0.0, 1.0);\n"
"  fPictPos = vTexPos;\n"
"  fColorMask = vColorMask;\n"
"}";

static const char *fragBackgroundSource = 
"#version 330\n"
"in vec2 fPictPos;\n"
"flat in float fColorMask;\n"
"uniform sampler2D imageTexture;\n"
"out uint color;\n"
"void main() {\n"
"  vec4 color1 = texture(imageTexture, fPictPos);\n"
"  float color2;\n"
"  if(fColorMask>2.5) {color2 = color1.a;}\n"
"  else if(fColorMask>1.5) {color2 = color1.b;}\n"
"  else if(fColorMask>0.5) {color2 = color1.g;}\n"
"  else {color2 = color1.r;}\n"
"  color = uint(color2*255);\n"
"}";



static const char *vertBackBlitSource = 
"#version 330\n"
"layout(location=4)in vec2 vVertPos;\n"
"out vec2 fPictPos;\n"
"void main() {\n"
"  gl_Position = vec4(vVertPos, 0.0, 1.0);\n"
"  fPictPos = (vVertPos+1.0)/2.0;\n"
"}";

static const char *fragBackBlitSource = 
"#version 330\n"
"in vec2 fPictPos;\n"
"uniform usampler1D paletteTexture;\n"
"uniform usampler2D backgroundTexture;\n"
"out vec4 color;\n"
"void main() {\n"
"  uvec4 color1 = texture(backgroundTexture, fPictPos);\n"
"  float c = float(color1.r)/255;\n"
"  uvec4 d = texture(paletteTexture, c);\n"
"  vec4 e = vec4(d)/255.0;\n"
"  color = e;\n"
"}";

static inline void SetBufferCoords(boxCoords_t viewport, boxCoords_t textureCoords, int colorMask) {
    int offset = bufferInternals.index;
    bufferInternals.index += 6;

    assert (bufferInternals.index < bufferInternals.capacity);

    bufferInternals.vertices[2*offset] = viewport.left;
    bufferInternals.vertices[2*offset+1] = viewport.bottom;
    bufferInternals.vertices[2*offset+2] = viewport.right;
    bufferInternals.vertices[2*offset+3] = viewport.bottom;
    bufferInternals.vertices[2*offset+4] = viewport.left;
    bufferInternals.vertices[2*offset+5] = viewport.top;
    bufferInternals.vertices[2*offset+6] = viewport.right;
    bufferInternals.vertices[2*offset+7] = viewport.bottom;
    bufferInternals.vertices[2*offset+8] = viewport.right;
    bufferInternals.vertices[2*offset+9] = viewport.top;
    bufferInternals.vertices[2*offset+10] = viewport.left;
    bufferInternals.vertices[2*offset+11] = viewport.top;

    bufferInternals.texCoords[2*offset] = textureCoords.left;
    bufferInternals.texCoords[2*offset+1] = textureCoords.bottom;
    bufferInternals.texCoords[2*offset+2] = textureCoords.right;
    bufferInternals.texCoords[2*offset+3] = textureCoords.bottom;
    bufferInternals.texCoords[2*offset+4] = textureCoords.left;
    bufferInternals.texCoords[2*offset+5] = textureCoords.top;
    bufferInternals.texCoords[2*offset+6] = textureCoords.right;
    bufferInternals.texCoords[2*offset+7] = textureCoords.bottom;
    bufferInternals.texCoords[2*offset+8] = textureCoords.right;
    bufferInternals.texCoords[2*offset+9] = textureCoords.top;
    bufferInternals.texCoords[2*offset+10] = textureCoords.left;
    bufferInternals.texCoords[2*offset+11] = textureCoords.top;

    bufferInternals.colorMasks[offset] = colorMask;
    bufferInternals.colorMasks[offset+1] = colorMask;
    bufferInternals.colorMasks[offset+2] = colorMask;
    bufferInternals.colorMasks[offset+3] = colorMask;
    bufferInternals.colorMasks[offset+4] = colorMask;
    bufferInternals.colorMasks[offset+5] = colorMask;
}
 
static int SelectClosestPixelIndex(unsigned char red, unsigned char green, unsigned char blue) {
    unsigned char diffR = red - gamepal[0].red;
    unsigned char diffG = green - gamepal[0].green;
    unsigned char diffB = blue - gamepal[0].blue;

    if (diffR < 0) {
        diffR = -diffR;
    }
    if (diffG < 0) {
        diffG = -diffG;
    }
    if (diffB < 0) {
        diffB = -diffB;
    }
    int cummulative = diffR + diffG + diffB;
    
    int index = 0;

    for (int i = 1; i < 255; ++i) {
        diffR = red - gamepal[i].red;
        diffG = green - gamepal[i].green;
        diffB = blue - gamepal[i].blue;
        if (diffR < 0) {
            diffR = -diffR;
        }
        if (diffG < 0) {
            diffG = -diffG;
        }
        if (diffB < 0) {
            diffB = -diffB;
        }
        int newCummulative = diffR + diffG + diffB;
        if (newCummulative < cummulative) {
            index = i;
            cummulative = newCummulative;
        }
    }

    return index;
}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
 
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

    switch (action)
    {

        // check for keypresses
        case GLFW_PRESS:
        {
            if (key == GLFW_KEY_ESCAPE) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }

            if(key == GLFW_KEY_SCROLL_LOCK || key == GLFW_KEY_F12)
            {
                GrabInput = !GrabInput;
                if (GrabInput) {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                } else {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }
                return;
            }

            LastScan = key;
            if(Keyboard[sc_Alt])
            {
                if(LastScan = GLFW_KEY_F4)
                    Quit(NULL);
            }

            if(LastScan == GLFW_KEY_KP_ENTER) LastScan = GLFW_KEY_ENTER;
            else if(LastScan == GLFW_KEY_RIGHT_SHIFT) LastScan = GLFW_KEY_LEFT_SHIFT;
            else if(LastScan == GLFW_KEY_RIGHT_ALT) LastScan = GLFW_KEY_LEFT_ALT;
            else if(LastScan == GLFW_KEY_RIGHT_CONTROL) LastScan = GLFW_KEY_LEFT_CONTROL;
            else
            {
                if((mods & GLFW_MOD_NUM_LOCK) == 0)
                {
                    switch(LastScan)
                    {
                        case GLFW_KEY_KP_2: LastScan = GLFW_KEY_DOWN; break;
                        case GLFW_KEY_KP_4: LastScan = GLFW_KEY_LEFT; break;
                        case GLFW_KEY_KP_6: LastScan = GLFW_KEY_RIGHT; break;
                        case GLFW_KEY_KP_8: LastScan = GLFW_KEY_UP; break;
                    }
                }
            }

            int sym = LastScan;
            if(sym >= 'a' && sym <= 'z')
                sym -= 32;  // convert to uppercase

            if(mods & (GLFW_MOD_SHIFT | GLFW_MOD_CAPS_LOCK))
            {
                if(sym < lengthof(ShiftNames) && ShiftNames[sym])
                    LastASCII = ShiftNames[sym];
            }
            else
            {
                if(sym < lengthof(ASCIINames) && ASCIINames[sym])
                    LastASCII = ASCIINames[sym];
            }

			if (LastScan<GLFW_KEY_I){
			}

			if(LastScan<GLFW_KEY_LAST){
                Keyboard[LastScan] = 1;
			}
            if(LastScan == GLFW_KEY_PAUSE)
                Paused = true;
            break;
        }

        case GLFW_RELEASE:
        {
            
            if(key == GLFW_KEY_KP_ENTER) key = GLFW_KEY_ENTER;
            else if(LastScan == GLFW_KEY_RIGHT_SHIFT) LastScan = GLFW_KEY_LEFT_SHIFT;
            else if(LastScan == GLFW_KEY_RIGHT_ALT) LastScan = GLFW_KEY_LEFT_ALT;
            else if(LastScan == GLFW_KEY_RIGHT_CONTROL) LastScan = GLFW_KEY_LEFT_CONTROL;
            else
            {
                if((mods & GLFW_MOD_NUM_LOCK) == 0)
                {
                    switch(key)
                    {
                        case GLFW_KEY_KP_2: LastScan = GLFW_KEY_DOWN; break;
                        case GLFW_KEY_KP_4: LastScan = GLFW_KEY_LEFT; break;
                        case GLFW_KEY_KP_6: LastScan = GLFW_KEY_RIGHT; break;
                        case GLFW_KEY_KP_8: LastScan = GLFW_KEY_UP; break;
                    }
                }
            }

			if(key<GLFW_KEY_LAST){
                Keyboard[key] = 0;
			}
            break;
        }
    }


}

static void resize_callback(GLFWwindow* window, int width, int height) {
    int hr = height * 320;
    int wr = width * 240;

    WindowCoords.width = width;
    WindowCoords.height = height;

    if (hr > wr) {
        int diff = (hr - wr) / (320*2);
        WindowCoords.viewportX = 0;
        WindowCoords.viewportY = diff;
        WindowCoords.viewportWidth = wr/240;
        WindowCoords.viewportHeight = wr/320;
    } else {
        int diff = (wr - hr) / (240*2);
        WindowCoords.viewportX = diff;
        WindowCoords.viewportY = 0;
        WindowCoords.viewportWidth = hr/240;
        WindowCoords.viewportHeight = hr/320;
    }
    glViewport(WindowCoords.viewportX,WindowCoords.viewportY, WindowCoords.viewportWidth, WindowCoords.viewportHeight);
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
 
    WindowCoords.viewportX = 0;
    WindowCoords.viewportY = 0;
    WindowCoords.viewportWidth = 640;
    WindowCoords.viewportHeight = 480;
    WindowCoords.width = 640;
    WindowCoords.height = 480;

    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowSizeCallback(window, resize_callback);
    glfwMakeContextCurrent(window);
    gladLoadGL();

    glfwSwapInterval(1);
 
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    GLint maxTextureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    if (maxTextureSize < 2048) {
        Quit("Max texture size is unsufficient");
    }
    DuPackInit(2048, 128*10, 200);

    /* setup background texture */
    glGenTextures(1, &backgroundTexture);

    uint8_t *tempDataa = (uint8_t*)malloc(320*240);
    memset(tempDataa, 0x12, 320*240);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, 320, 240, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, tempDataa);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    free(tempDataa);


    glGenFramebuffers(1, &vgaFramebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, vgaFramebuffer);
    glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, backgroundTexture, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    /* end of setup background texture */
    
    glGenVertexArrays(1, &vboRects);
    glBindVertexArray(vboRects);


    vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 320 * 3 * sizeof(GLfloat) * 2, NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    paletteCoordBuffer;
    glGenBuffers(1, &paletteCoordBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, paletteCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, 320 * 3 * sizeof(GLfloat) * 2, NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    paletteMaskBuffer;
    glGenBuffers(1, &paletteMaskBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, paletteMaskBuffer);
    glBufferData(GL_ARRAY_BUFFER, 320 * 3 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(3);

    {
        //tmp background buffer, eventually to fix with geometry shader
        GLuint tmpBackgrnd;
        float bckgrndData[] = {-1.0f, -1.0f, 1.0f, -1.f, -1.0f, 1.0f,
                                -1.0f, 1.0f, 1.0f, -1.f, 1.0f, 1.0f};
        glGenBuffers(1, &tmpBackgrnd);
        glBindBuffer(GL_ARRAY_BUFFER, tmpBackgrnd);

        glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), bckgrndData, GL_STATIC_DRAW);
        glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(4);
    }

    bufferInternals.gpuCapacity = 320 * 3;
    bufferInternals.capacity = 320 * 3;
 
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
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, DuPackGetTextureDimension(), DuPackGetTextureDimension(), 0, GL_RGBA, GL_UNSIGNED_BYTE, DuPackGetPalettizedTexture());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    {
        // create background shader
        const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertBackgroundSource, NULL);
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
        glShaderSource(fragment_shader, 1, &fragBackgroundSource, NULL);
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

        programBackground = glCreateProgram();
        glAttachShader(programBackground, vertex_shader);
        glAttachShader(programBackground, fragment_shader);
        glLinkProgram(programBackground);

            {
            GLint isLinked = 0;
            glGetProgramiv(programBackground, GL_LINK_STATUS, &isLinked);
            if (isLinked == GL_FALSE)
            {
                GLint maxLength = 0;
                glGetProgramiv(programBackground, GL_INFO_LOG_LENGTH, &maxLength);

                // The maxLength includes the NULL character
                GLchar *infoLog = (GLchar *)malloc(maxLength+1);
                
                glGetProgramInfoLog(programBackground, maxLength, &maxLength, infoLog);
                printf("prog:%s", infoLog);
                free(infoLog);
            }
        }

        glUseProgram(programBackground);
        const GLint imageTexture_location = glGetUniformLocation(programBackground, "imageTexture");
        if (imageTexture_location >= 0) {
            glUniform1i(imageTexture_location, 1);
        }

    }

    {
        // create blit shader
        const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertBackBlitSource, NULL);
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
        glShaderSource(fragment_shader, 1, &fragBackBlitSource, NULL);
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

        programBlit = glCreateProgram();
        glAttachShader(programBlit, vertex_shader);
        glAttachShader(programBlit, fragment_shader);
        glLinkProgram(programBlit);

            {
            GLint isLinked = 0;
            glGetProgramiv(programBlit, GL_LINK_STATUS, &isLinked);
            if (isLinked == GL_FALSE)
            {
                GLint maxLength = 0;
                glGetProgramiv(programBlit, GL_INFO_LOG_LENGTH, &maxLength);

                // The maxLength includes the NULL character
                GLchar *infoLog = (GLchar *)malloc(maxLength+1);
                
                glGetProgramInfoLog(programBlit, maxLength, &maxLength, infoLog);
                printf("prog:%s", infoLog);
                free(infoLog);
            }
        }

        glUseProgram(programBlit);

        const GLint paletteTexture_location = glGetUniformLocation(programBlit, "paletteTexture");
        if (paletteTexture_location >= 0) {
            glUniform1i(paletteTexture_location, 0);
        }
        const GLint imageTexture_location = glGetUniformLocation(programBlit, "backgroundTexture");
        if (imageTexture_location >= 0) {
            glUniform1i(imageTexture_location, 2);
        }

    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    const float ratio = width / (float) height;

    glViewport(0, 0, width, height);

    return 0;
}


static void GlfwDrawStuff(void) {

    if (bufferInternals.index > 0) {
        assert(bufferInternals.index < bufferInternals.gpuCapacity);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, 2 * bufferInternals.index * sizeof(float), bufferInternals.vertices, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, paletteCoordBuffer);
        glBufferData(GL_ARRAY_BUFFER, 2 * bufferInternals.index * sizeof(float), bufferInternals.texCoords, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, paletteMaskBuffer);
        glBufferData(GL_ARRAY_BUFFER, bufferInternals.index * sizeof(float), bufferInternals.colorMasks, GL_DYNAMIC_DRAW);
        
        glViewport(0, 0, 320, 240);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, vgaFramebuffer);
        glUseProgram(programBackground);
        glDrawArrays(GL_TRIANGLES, 0, bufferInternals.index);
    } 

    glViewport(WindowCoords.viewportX,WindowCoords.viewportY, WindowCoords.viewportWidth, WindowCoords.viewportHeight);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    //glUseProgram(program);
    glUseProgram(programBlit);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glfwSwapBuffers(window);

    bufferInternals.index = 0;
    
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

void SetVGAMode(unsigned *scrWidth, unsigned *scrHeight, 
                unsigned *sclFactor) {
    *scrWidth = 320; //WindowCoords.viewportWidth;
    *scrHeight = 200; //WindowCoords.viewportHeight;
    *sclFactor = 1;
}

void LoadLatchMemory (void) {
    int i;
    int start = LATCHPICS_LUMP_START;
    int end = LATCHPICS_LUMP_END;

    int w = 8*8;
    int h = ((NUMTILE8 + 7) / 8) * 8;

    CA_CacheGrChunk (STARTTILE8);
    uint8_t *data = (uint8_t *)malloc(w*h);
    uint8_t *subData = (uint8_t *)malloc(8*8);
    uint8_t *src = (uint8_t *)GetGrSegs(STARTTILE8);
    for (i=0;i<NUMTILE8;i++){
        int rrow = (i / 8) * 8;
        int rcol = (i % 8) * 8;
        ShufflePicColumns(subData, src + i*64, 8, 8);
        for (int row = 0; row < 8; ++row) {
            for (int col = 0; col < 8; ++col) {

                data[(rrow + row) * w + rcol + col] = subData[row*8+col];
            }
        }

    }
    FILE *fp = fopen("data2.raw", "wb");
    fwrite(data, w*h, 1, fp);
    fclose(fp);

    //memcpy(data, grsegs[STARTTILE8], w*h);
    latchpics[0] = DuPackAddTexture(w, h, data);
    free(data);
//     src = grsegs[STARTTILE8];

//     for (i=0;i<NUMTILE8;i++)
//     {                  source w h dest x y
//         VL_MemToLatch (src, 8, 8, (void*)surf, (i & 7) * 8, (i >> 3) * 8);
            // int pitch =  GetSurfacePitch(destSurface);
            // byte *dest = (byte *) GetSurfacePixels(destSurface) + y * pitch + x;
            // for(int ysrc = 0; ysrc < height; ysrc++)
            // {
            //     for(int xsrc = 0; xsrc < width; xsrc++)
            //     {
            //         dest[ysrc * pitch + xsrc] = source[(ysrc * (width >> 2) + (xsrc >> 2))
            //             + (xsrc & 3) * (width >> 2) * height];
            //     }
            // }
//         src += 64;
//     }
    ClearGrSegs (STARTTILE8);
     
    for (i=start;i<=end;i++)
    {
        w = pictable[i-STARTPICS].width;
        h = pictable[i-STARTPICS].height;

        CA_CacheGrChunk (i);
        uint8_t *data = (uint8_t *)malloc(w*h);
        
        ShufflePicColumns(data, (uint8_t*)GetGrSegs(i), w, h);
        
        latchpics[2+i-start] = DuPackAddTexture(w, h, data);

        free(data);
        ClearGrSegs(i);
    }

    glActiveTexture(GL_TEXTURE1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, DuPackGetTextureDimension(), DuPackGetTextureDimension(), 0, GL_RGBA, GL_UNSIGNED_BYTE, DuPackGetPalettizedTexture());
    
}

/* This function should swap buffers */
int SubFizzleFade (int x1, int y1,
                       unsigned width, unsigned height, 
                       unsigned frames, int abortable,
                       int rndbits_y, int rndmask){
    GlfwDrawStuff();
    return 0;
}

void GetJoystickDelta(int *dx, int *dy) {
    ;
}

void GetJoystickFineDelta(int *dx, int *dy) {
    ;
}

int GetJoystickButtons(void) {
    return 0;
}

int IsJoystickPresent(void) {
    return 0;
}

extern void ProcessEvents(void);
void WaitAndProcessEvents(void) {
    if (window) {
        glfwWaitEvents();
        glfwPollEvents();
        GlfwDrawStuff();
        if (glfwWindowShouldClose(window)) {
            exit(0);
        }
    }
}

void ProcessEvents(void) {
    if (window) {
        glfwPollEvents();
        GlfwDrawStuff();
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

    ShufflePicColumns(pixels, source, width, height);

    int index = DuPackAddTexture(width, height, pixels);
    
    glActiveTexture(GL_TEXTURE1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, DuPackGetTextureDimension(), DuPackGetTextureDimension(), 0, GL_RGBA, GL_UNSIGNED_BYTE, DuPackGetPalettizedTexture());
    
    free(pixels);

    boxCoords_t colorCoords;
    int colorMask;
    DuPackGetTextureCoords(index, &colorCoords.left, &colorCoords.right, &colorCoords.bottom, &colorCoords.top, &colorMask);

    boxCoords_t viewportCoords = {2.f * (destx/ 320.f) - 1.f,
                                  2.f * (destx + width)/ 320.f - 1.f,
                                  2.f * (200.f-(desty + height))/ 200.f - 1.f,
                                  2.f * (200.f-desty)/ 200.f - 1.f,};
                            
    SetBufferCoords(viewportCoords, colorCoords, colorMask);
}

void VL_Plot (int x, int y, int color)
{
    uint8_t byte = color;
    glActiveTexture(GL_TEXTURE2);
    glTexSubImage2D(GL_TEXTURE_2D, 0, x,y, 1, 1, GL_RED, GL_UNSIGNED_BYTE, &byte);
}


unsigned char VL_GetPixel (int x, int y)
{
    unsigned char pixels[4];
    GlfwDrawStuff();
    glFinish();
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    return SelectClosestPixelIndex(pixels[0], pixels[1], pixels[2]);

}

void VL_Hlin (unsigned x, unsigned y, unsigned width, int color)
{
    boxCoords_t viewportCoords = {(x - 160.f) / 160.0f,
                            (x+width - 160.0f) / 160.0f,
                            (100.0f - y) / 100.0f,
                            (100.0f - y - 1.0f) / 100.0f};
    
    boxCoords_t colorCoords;
    int colorMask;
    DuPackGetColorCoords(color, &colorCoords.left, &colorCoords.right, &colorCoords.bottom, &colorCoords.top, &colorMask);
    SetBufferCoords(viewportCoords, colorCoords, colorMask);
}

void VL_Vlin (int x, int y, int height, int color)
{
    boxCoords_t viewportCoords = {(x - 160.f) / 160.0f,
                            (x+1.f - 160.0f) / 160.0f,
                            (100.0f - y) / 100.0f,
                            (100.0f - y - height) / 100.0f};
    
    boxCoords_t colorCoords;
    int colorMask;
    DuPackGetColorCoords(color, &colorCoords.left, &colorCoords.right, &colorCoords.bottom, &colorCoords.top, &colorMask);
    SetBufferCoords(viewportCoords, colorCoords, colorMask);
}


void VL_BarScaledCoord (int scx, int scy, int scwidth, int scheight, int color)
{
    boxCoords_t viewportCoords = {(scx - 160.f) / 160.0f,
                            (scx + scwidth - 160.0f) / 160.0f,
                            (100.0f - scy) / 100.0f,
                            (100.0f - scy - scheight) / 100.0f};
    
    boxCoords_t colorCoords;
    int colorMask;
    DuPackGetColorCoords(color, &colorCoords.left, &colorCoords.right, &colorCoords.bottom, &colorCoords.top, &colorMask);
    SetBufferCoords(viewportCoords, colorCoords, colorMask);
}


void VL_MemToScreenScaledCoord (unsigned char *source, int origwidth, int origheight, int srcx, int srcy,
                                int destx, int desty, int width, int height)
{
    ;
}


void VL_LatchToScreenScaledCoord(int which, int xsrc, int ysrc,
    int width, int height, int scxdest, int scydest)
{
    int index = latchpics[which];


    boxCoords_t colorCoords;
    int colorMask;
    DuPackGetTextureCoords(index, &colorCoords.left, &colorCoords.right, &colorCoords.bottom, &colorCoords.top, &colorMask);
    
    int l,r,u,d;
    DuPackGetTextureIntCoords(index, &l, &r, &d, &u);

    boxCoords_t viewportCoords = {2.f * ((scxdest)/ 320.f) - 1.f,
                                  2.f * (scxdest + r - l)/ 320.f - 1.f,
                                  2.f * (200.f-(scydest + u - d))/ 200.f - 1.f,
                                  2.f * (200.f-scydest)/ 200.f - 1.f,};

    SetBufferCoords(viewportCoords, colorCoords, colorMask);

}

void LatchDrawPic (unsigned x, unsigned y, unsigned picnum)
{
    int which = 2+picnum-LATCHPICS_LUMP_START;
    VL_LatchToScreenScaledCoord(which,0,0,0,0, scaleFactor*x*8,scaleFactor*y);
}

void LatchDrawPicScaledCoord (unsigned scx, unsigned scy, unsigned picnum)
{
    int which = 2+picnum-LATCHPICS_LUMP_START;
    VL_LatchToScreenScaledCoord(which,0,0,0,0,scx*8,scy);
}

static void VL_ScreenToScreen (void *source, void *dest)
{
    GlfwDrawStuff();
}

/* this function should swap buffers*/
void VH_UpdateScreen()
{
    GlfwDrawStuff();
    
}

void    ThreeDRefresh (void)
{
    ;
}

void VWB_DrawPropString(const char* string, int *px, int py, int fontnumber)
{
    // fontstruct  *font;
    int         width, step, height;
    byte        *source, *dest;
    byte        ch;

    // byte *vbuf = VL_LockSurface(GetCurSurface());

    fontstruct  *font = (fontstruct *) GetGrSegs(STARTFONT+fontnumber);
    height = font->height;
    // dest = vbuf + scaleFactor * (py * curPitch + px);

    while ((ch = (byte)*string++)!=0)
    {
        width = step = font->width[ch];
        source = ((byte *)font)+font->location[ch];
        // byte *pixels = (byte*)malloc(width*height);

        // memcpy(pixels, source, width*height);
        // ShufflePicColumns(pixels, source, width, height);
        // int index = DuPackAddTexture(width, height, pixels);
        // free(pixels);

        while (width--)
        {
    //         for(int i=0;i<height;i++)
    //         {
    //             if(source[i*step])
    //             {
    //                 for(unsigned sy=0; sy<scaleFactor; sy++)
    //                     for(unsigned sx=0; sx<scaleFactor; sx++)
    //                         dest[(scaleFactor*i+sy)*curPitch+sx]=fontcolor;
    //             }
    //         }

    //         source++;
    //         px++;
    //         dest+=scaleFactor;
        }
    }
    glActiveTexture(GL_TEXTURE1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, DuPackGetTextureDimension(), DuPackGetTextureDimension(), 0, GL_RGBA, GL_UNSIGNED_BYTE, DuPackGetPalettizedTexture());
    

}

void BlitPictureToScreen(unsigned char *pic) {
    VL_MemToScreenScaledCoord(pic, 320, 200, 0, 0);
    GlfwDrawStuff();
}