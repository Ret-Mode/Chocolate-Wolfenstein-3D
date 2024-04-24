#ifndef __EMPTY_KEY_LAYOUT_H__
#define __EMPTY_KEY_LAYOUT_H__

typedef int     ScanCode;

#define sc_None         0
#define sc_Bad          0xff
#define sc_Return       sc_Bad
#define sc_Enter        sc_Bad
#define sc_Escape       sc_Bad
#define sc_Space        sc_Bad
#define sc_BackSpace    sc_Bad
#define sc_Tab          sc_Bad
#define sc_Alt          sc_Bad
#define sc_Control      sc_Bad
#define sc_CapsLock     sc_Bad
#define sc_LShift       sc_Bad
#define sc_RShift       sc_Bad
#define sc_UpArrow      sc_Bad
#define sc_DownArrow    sc_Bad
#define sc_LeftArrow    sc_Bad
#define sc_RightArrow   sc_Bad
#define sc_Insert       sc_Bad
#define sc_Delete       sc_Bad
#define sc_Home         sc_Bad
#define sc_End          sc_Bad
#define sc_PgUp         sc_Bad
#define sc_PgDn         sc_Bad
#define sc_Keypad5      sc_Bad
#define sc_F1           sc_Bad
#define sc_F2           sc_Bad
#define sc_F3           sc_Bad
#define sc_F4           sc_Bad
#define sc_F5           sc_Bad
#define sc_F6           sc_Bad
#define sc_F7           sc_Bad
#define sc_F8           sc_Bad
#define sc_F9           sc_Bad
#define sc_F10          sc_Bad
#define sc_F11          sc_Bad
#define sc_F12          sc_Bad

#define sc_ScrollLock       sc_Bad
#define sc_PrintScreen      sc_Bad

#define sc_1            sc_Bad
#define sc_2            sc_Bad
#define sc_3            sc_Bad
#define sc_4            sc_Bad
#define sc_5            sc_Bad
#define sc_6            sc_Bad
#define sc_7            sc_Bad
#define sc_8            sc_Bad
#define sc_9            sc_Bad
#define sc_0            sc_Bad

#define sc_A            sc_Bad
#define sc_B            sc_Bad
#define sc_C            sc_Bad
#define sc_D            sc_Bad
#define sc_E            sc_Bad
#define sc_F            sc_Bad
#define sc_G            sc_Bad
#define sc_H            sc_Bad
#define sc_I            sc_Bad
#define sc_J            sc_Bad
#define sc_K            sc_Bad
#define sc_L            sc_Bad
#define sc_M            sc_Bad
#define sc_N            sc_Bad
#define sc_O            sc_Bad
#define sc_P            sc_Bad
#define sc_Q            sc_Bad
#define sc_R            sc_Bad
#define sc_S            sc_Bad
#define sc_T            sc_Bad
#define sc_U            sc_Bad
#define sc_V            sc_Bad
#define sc_W            sc_Bad
#define sc_X            sc_Bad
#define sc_Y            sc_Bad
#define sc_Z            sc_Bad

#define key_None        0
#define sc_Last         sc_Bad

#define KEYS_LOADED

#define GetScanName(value) "?"

#endif