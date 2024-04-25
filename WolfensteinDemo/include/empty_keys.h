#ifndef __EMPTY_KEY_LAYOUT_H__
#define __EMPTY_KEY_LAYOUT_H__

#ifndef KEYS_LOADED
#define KEYS_LOADED

typedef int     ScanCode;

#define sc_None         0
#define sc_Bad          0xff
#define sc_Return       0x01
#define sc_Enter        0x02
#define sc_Escape       0x03
#define sc_Space        0x04
#define sc_BackSpace    0x05
#define sc_Tab          0x06
#define sc_Alt          0x07
#define sc_Control      0x08
#define sc_CapsLock     0x09
#define sc_LShift       0x0A
#define sc_RShift       0x0B
#define sc_UpArrow      0x0C
#define sc_DownArrow    0x0D
#define sc_LeftArrow    0x0E
#define sc_RightArrow   0x0F
#define sc_Insert       0x10
#define sc_Delete       0x11
#define sc_Home         0x12
#define sc_End          0x13
#define sc_PgUp         0x14
#define sc_PgDn         0x15
#define sc_Keypad5      0x16
#define sc_F1           0x17
#define sc_F2           0x18
#define sc_F3           0x19
#define sc_F4           0x1A
#define sc_F5           0x1B
#define sc_F6           0x1C
#define sc_F7           0x1D
#define sc_F8           0x1E
#define sc_F9           0x1F
#define sc_F10          0x20
#define sc_F11          0x21
#define sc_F12          0x22

#define sc_ScrollLock       0x23
#define sc_PrintScreen      0x24

#define sc_1            0x25
#define sc_2            0x26
#define sc_3            0x27
#define sc_4            0x28
#define sc_5            0x29
#define sc_6            0x2A
#define sc_7            0x2B
#define sc_8            0x2C
#define sc_9            0x2D
#define sc_0            0x2E

#define sc_A            0x2F
#define sc_B            0x30
#define sc_C            0x31
#define sc_D            0x32
#define sc_E            0x33
#define sc_F            0x34
#define sc_G            0x35
#define sc_H            0x36
#define sc_I            0x37
#define sc_J            0x38
#define sc_K            0x39
#define sc_L            0x3A
#define sc_M            0x3B
#define sc_N            0x3C
#define sc_O            0x3D
#define sc_P            0x3E
#define sc_Q            0x3F
#define sc_R            0x40
#define sc_S            0x41
#define sc_T            0x42
#define sc_U            0x43
#define sc_V            0x44
#define sc_W            0x45
#define sc_X            0x46
#define sc_Y            0x47
#define sc_Z            0x48

#define key_None        0x49
#define sc_Last         0x50

#define GetScanName(value) "?"

#endif

#endif