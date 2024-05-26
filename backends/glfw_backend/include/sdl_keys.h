#ifndef __SDL_WOLF_KEYS_H__
#define __SDL_WOLF_KEYS_H__

#ifndef KEYS_LOADED
#define KEYS_LOADED

#define GLFW_DLL
#include <GLFW/glfw3.h>

typedef int     ScanCode;

#define sc_None         0
#define sc_Bad          0xff
#define sc_Return       GLFW_KEY_ENTER
#define sc_Enter        sc_Return
#define sc_Escape       GLFW_KEY_ESCAPE
#define sc_Space        GLFW_KEY_SPACE
#define sc_BackSpace    GLFW_KEY_BACKSPACE
#define sc_Tab          GLFW_KEY_TAB
#define sc_Alt          GLFW_KEY_LEFT_ALT
#define sc_Control      GLFW_KEY_LEFT_CONTROL
#define sc_CapsLock     GLFW_KEY_CAPSLOCK
#define sc_LShift       GLFW_KEY_LEFT_SHIFT
#define sc_RShift       GLFW_KEY_RIGHT_SHIFT
#define sc_UpArrow      GLFW_KEY_UP
#define sc_DownArrow    GLFW_KEY_DOWN
#define sc_LeftArrow    GLFW_KEY_LEFT
#define sc_RightArrow   GLFW_KEY_RIGHT
#define sc_Insert       GLFW_KEY_INSERT
#define sc_Delete       GLFW_KEY_DELETE
#define sc_Home         GLFW_KEY_HOME
#define sc_End          GLFW_KEY_END
#define sc_PgUp         GLFW_KEY_PAGE_UP
#define sc_PgDn         GLFW_KEY_PAGE_DOWN
#define sc_Keypad5      GLFW_KEY_KP_5
#define sc_F1           GLFW_KEY_F1
#define sc_F2           GLFW_KEY_F2
#define sc_F3           GLFW_KEY_F3
#define sc_F4           GLFW_KEY_F4
#define sc_F5           GLFW_KEY_F5
#define sc_F6           GLFW_KEY_F6
#define sc_F7           GLFW_KEY_F7
#define sc_F8           GLFW_KEY_F8
#define sc_F9           GLFW_KEY_F9
#define sc_F10          GLFW_KEY_F10
#define sc_F11          GLFW_KEY_F11
#define sc_F12          GLFW_KEY_F12

#define sc_ScrollLock       GLFW_KEY_SCROLL_LOCK
#define sc_PrintScreen      GLFW_KEY_PRINT_SCREEN

#define sc_1            GLFW_KEY_1
#define sc_2            GLFW_KEY_2
#define sc_3            GLFW_KEY_3
#define sc_4            GLFW_KEY_4
#define sc_5            GLFW_KEY_5
#define sc_6            GLFW_KEY_6
#define sc_7            GLFW_KEY_7
#define sc_8            GLFW_KEY_8
#define sc_9            GLFW_KEY_9
#define sc_0            GLFW_KEY_0

#define sc_A            GLFW_KEY_A
#define sc_B            GLFW_KEY_B
#define sc_C            GLFW_KEY_C
#define sc_D            GLFW_KEY_D
#define sc_E            GLFW_KEY_E
#define sc_F            GLFW_KEY_F
#define sc_G            GLFW_KEY_G
#define sc_H            GLFW_KEY_H
#define sc_I            GLFW_KEY_I
#define sc_J            GLFW_KEY_J
#define sc_K            GLFW_KEY_K
#define sc_L            GLFW_KEY_L
#define sc_M            GLFW_KEY_M
#define sc_N            GLFW_KEY_N
#define sc_O            GLFW_KEY_O
#define sc_P            GLFW_KEY_P
#define sc_Q            GLFW_KEY_Q
#define sc_R            GLFW_KEY_R
#define sc_S            GLFW_KEY_S
#define sc_T            GLFW_KEY_T
#define sc_U            GLFW_KEY_U
#define sc_V            GLFW_KEY_V
#define sc_W            GLFW_KEY_W
#define sc_X            GLFW_KEY_X
#define sc_Y            GLFW_KEY_Y
#define sc_Z            GLFW_KEY_Z

#define key_None        0
#define sc_Last         GLFW_KEY_LAST

const char *GetScanName (ScanCode scan);

#endif 

#endif