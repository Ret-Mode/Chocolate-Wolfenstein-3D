//
//  crt.c
//  Chocolate Wolfenstein 3D
//
//  Created by fabien sanglard on 2014-08-26.
//
//

#include "crt.h"

#ifdef _WIN32
#include <WTypes.h>
#include <gl\GL.h>
#include "SDL.h"
#elif __linux__
#include <GL/gl.h>
#include "SDL/SDL.h"
#else
#include <OpenGL/gl.h>
#include "SDL/SDL.h"
#endif

static int width;
static int height;

static GLuint crtTexture;

static unsigned char coloredFrameBuffer[320*200*3];
void ConvertPaletteToRGB(unsigned char *pixelPointer, int width, int height);

void CRT_Init(int _width){
    width  = _width;
    height = _width * 3.0/4.0;
    
    //Alloc the OpenGL texture where the screen will be uploaded each frame.
    glGenTextures(1, &crtTexture);
    glBindTexture(GL_TEXTURE_2D, crtTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(
                 GL_TEXTURE_2D,         // target
                 0,                     // level, 0 = base, no minimap,
                 GL_RGB,                // internalformat
                 320,                   // width
                 200,                   // height
                 0,                     // border, always 0 in OpenGL ES
                 GL_RGB,                // format
                 GL_UNSIGNED_BYTE,      // type
                 0
                 );
    
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
    
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
 
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapBuffers();
}

#include "id_vl.h"
void CRT_DAC(void){
    
    //Convert palette based framebuffer to RGB for OpenGL
    byte* pixelPointer = coloredFrameBuffer;
    ConvertPaletteToRGB(pixelPointer, 320, 200);    

    //Upload texture
    glBindTexture(GL_TEXTURE_2D, crtTexture);
    glTexSubImage2D(GL_TEXTURE_2D,
                    0,
                    0,
                    0,
                    320,
                    200,
                    GL_RGB,
                    GL_UNSIGNED_BYTE,
                    coloredFrameBuffer);
    
    //Draw a quad with the texture
    glBegin(GL_QUADS);
        glTexCoord2f(0, 1); glVertex3i(0,0,0);
        glTexCoord2f(0, 0); glVertex3i(0,height,0);
        glTexCoord2f(1, 0); glVertex3i(width,height,0);
        glTexCoord2f(1, 1); glVertex3i(width,0,0);
    glEnd();
    
    
    //Flip buffer
    SDL_GL_SwapBuffers();
	
	Uint8 *keystate = SDL_GetKeyState(NULL);
	static int wasPressed = 0;
	if ( keystate[SDLK_i] ){
		if (!wasPressed){
			wasPressed = 1;
			CRT_Screenshot();
		}
	}
	else
		wasPressed = 0;	
}

void CRT_Screenshot(void){

  const char* filename = "screenshot.tga" ;

  printf("Screenshot.\n");

  //This prevents the images getting padded 
 // when the width multiplied by 3 is not a multiple of 4
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
 
  int nSize = width*height*3;
  // First let's create our buffer, 3 channels per Pixel
  char* dataBuffer = (char*)malloc(nSize*sizeof(char));
 
  if (!dataBuffer) return;
 
   // Let's fetch them from the backbuffer	
   // We request the pixels in GL_BGR format
  	#define GL_BGR                            0x80E0
   glReadPixels((GLint)0, (GLint)0,(GLint)width, (GLint)height, GL_BGR, GL_UNSIGNED_BYTE, dataBuffer);
 
   //Now the file creation
   FILE *filePtr = fopen(filename, "wb");
   if (!filePtr) return;
 
 
   unsigned char TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};
   unsigned char header[6] = { (unsigned char)(width%256),
                                (unsigned char)(width/256),
                                (unsigned char)(height%256),
                                (unsigned char)(height/256),24,0};

   // We write the headers
   fwrite(TGAheader,	sizeof(unsigned char),	12,	filePtr);
   fwrite(header,	sizeof(unsigned char),	6,	filePtr);
   // And finally our image data
   fwrite(dataBuffer,	sizeof(GLubyte),	nSize,	filePtr);
   fclose(filePtr);
}