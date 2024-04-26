//
//  ID Engine
//  ID_IN.c - Input Manager
//  v1.0d1
//  By Jason Blochowiak
//

//
//  This module handles dealing with the various input devices
//
//  Depends on: Memory Mgr (for demo recording), Sound Mgr (for timing stuff),
//              User Mgr (for command line parms)
//
//  Globals:
//      LastScan - The keyboard scan code of the last key pressed
//      LastASCII - The ASCII value of the last key pressed
//  DEBUG - there are more globals
//

#include "wl_def.h"


/*
=============================================================================

                    GLOBAL VARIABLES

=============================================================================
*/


//
// configuration variables
//
boolean MousePresent;
boolean forcegrabmouse;


//  Global variables
volatile boolean    Keyboard[sc_Last];
volatile boolean    Paused;
volatile char       LastASCII;
volatile ScanCode   LastScan;

//KeyboardDef   KbdDefs = {0x1d,0x38,0x47,0x48,0x49,0x4b,0x4d,0x4f,0x50,0x51};
static KeyboardDef KbdDefs = {
    sc_Control,             // button0
    sc_Alt,                 // button1
    sc_Home,                // upleft
    sc_UpArrow,             // up
    sc_PgUp,                // upright
    sc_LeftArrow,           // left
    sc_RightArrow,          // right
    sc_End,                 // downleft
    sc_DownArrow,           // down
    sc_PgDn                 // downright
};


int JoyNumButtons;


/*
=============================================================================

                    LOCAL VARIABLES

=============================================================================
*/



static  boolean     IN_Started;

static  Direction   DirTable[] =        // Quick lookup for total direction
{
    dir_NorthWest,  dir_North,  dir_NorthEast,
    dir_West,       dir_None,   dir_East,
    dir_SouthWest,  dir_South,  dir_SouthEast
};


///////////////////////////////////////////////////////////////////////////
//
//  INL_GetMouseButtons() - Gets the status of the mouse buttons from the
//      mouse driver
//
///////////////////////////////////////////////////////////////////////////
static int
INL_GetMouseButtons(void)
{
    return GetMouseButtons();
}

///////////////////////////////////////////////////////////////////////////
//
//  IN_GetJoyDelta() - Returns the relative movement of the specified
//      joystick (from +/-127)
//
///////////////////////////////////////////////////////////////////////////
void IN_GetJoyDelta(int *dx,int *dy)
{
    GetJoystickDelta(dx, dy);
}

///////////////////////////////////////////////////////////////////////////
//
//  IN_GetJoyFineDelta() - Returns the relative movement of the specified
//      joystick without dividing the results by 256 (from +/-127)
//
///////////////////////////////////////////////////////////////////////////
void IN_GetJoyFineDelta(int *dx, int *dy)
{
    GetJoystickFineDelta(dx, dy);
}

/*
===================
=
= IN_JoyButtons
=
===================
*/

int IN_JoyButtons()
{
    return GetJoystickButtons();
}

boolean IN_JoyPresent()
{
    return IsJoystickPresent();
}



void IN_WaitAndProcessEvents(void)
{
    WaitAndProcessEvents();

}

void IN_ProcessEvents(void)
{
    ProcessEvents();
}


///////////////////////////////////////////////////////////////////////////
//
//  IN_Startup() - Starts up the Input Mgr
//
///////////////////////////////////////////////////////////////////////////
void
IN_Startup(void)
{
    if (IN_Started)
        return;

    IN_ClearKeysDown();
    JoystickStartup();

    // I didn't find a way to ask libSDL whether a mouse is present, yet...

    MousePresent = true;
    IN_Started = true;
}

///////////////////////////////////////////////////////////////////////////
//
//  IN_Shutdown() - Shuts down the Input Mgr
//
///////////////////////////////////////////////////////////////////////////
void
IN_Shutdown(void)
{
    if (!IN_Started)
        return;

    JoystickShutdown();

    IN_Started = false;
}

///////////////////////////////////////////////////////////////////////////
//
//  IN_ClearKeysDown() - Clears the keyboard array
//
///////////////////////////////////////////////////////////////////////////
void
IN_ClearKeysDown(void)
{
    LastScan = sc_None;
    LastASCII = key_None;
    memset ((void *) Keyboard,0,sizeof(Keyboard));
}


///////////////////////////////////////////////////////////////////////////
//
//  IN_ReadControl() - Reads the device associated with the specified
//      player and fills in the control info struct
//
///////////////////////////////////////////////////////////////////////////
void
IN_ReadControl(int player,ControlInfo *info)
{
    word        buttons;
    int         dx,dy;
    Motion      mx,my;

    dx = dy = 0;
    mx = my = motion_None;
    buttons = 0;

    IN_ProcessEvents();

    if (Keyboard[KbdDefs.upleft])
        mx = motion_Left,my = motion_Up;
    else if (Keyboard[KbdDefs.upright])
        mx = motion_Right,my = motion_Up;
    else if (Keyboard[KbdDefs.downleft])
        mx = motion_Left,my = motion_Down;
    else if (Keyboard[KbdDefs.downright])
        mx = motion_Right,my = motion_Down;

    if (Keyboard[KbdDefs.up])
        my = motion_Up;
    else if (Keyboard[KbdDefs.down])
        my = motion_Down;

    if (Keyboard[KbdDefs.left])
        mx = motion_Left;
    else if (Keyboard[KbdDefs.right])
        mx = motion_Right;

    if (Keyboard[KbdDefs.button0])
        buttons += 1 << 0;
    if (Keyboard[KbdDefs.button1])
        buttons += 1 << 1;

    dx = mx * 127;
    dy = my * 127;

    info->x = dx;
    info->xaxis = mx;
    info->y = dy;
    info->yaxis = my;
    info->button0 = (buttons & (1 << 0)) != 0;
    info->button1 = (buttons & (1 << 1)) != 0;
    info->button2 = (buttons & (1 << 2)) != 0;
    info->button3 = (buttons & (1 << 3)) != 0;
    info->dir = DirTable[((my + 1) * 3) + (mx + 1)];
}

///////////////////////////////////////////////////////////////////////////
//
//  IN_Ack() - waits for a button or key press.  If a button is down, upon
// calling, it must be released for it to be recognized
//
///////////////////////////////////////////////////////////////////////////

boolean btnstate[NUMBUTTONS];

void IN_StartAck(void)
{
    IN_ProcessEvents();
//
// get initial state of everything
//
    IN_ClearKeysDown();
    memset(btnstate, 0, sizeof(btnstate));

    int buttons = IN_JoyButtons() << 4;

    if(MousePresent)
        buttons |= IN_MouseButtons();

    for(int i = 0; i < NUMBUTTONS; i++, buttons >>= 1)
        if(buttons & 1)
            btnstate[i] = true;
}


boolean IN_CheckAck (void)
{
    IN_ProcessEvents();
//
// see if something has been pressed
//
    if(LastScan)
        return true;

    int buttons = IN_JoyButtons() << 4;

    if(MousePresent)
        buttons |= IN_MouseButtons();

    for(int i = 0; i < NUMBUTTONS; i++, buttons >>= 1)
    {
        if(buttons & 1)
        {
            if(!btnstate[i])
            {
                // Wait until button has been released
                do
                {
                    IN_WaitAndProcessEvents();
                    buttons = IN_JoyButtons() << 4;

                    if(MousePresent)
                        buttons |= IN_MouseButtons();
                }
                while(buttons & (1 << i));

                return true;
            }
        }
        else
            btnstate[i] = false;
    }

    return false;
}


void IN_Ack (void)
{
    IN_StartAck ();

    do
    {
        IN_WaitAndProcessEvents();
    }
    while(!IN_CheckAck ());
}


///////////////////////////////////////////////////////////////////////////
//
//  IN_UserInput() - Waits for the specified delay time (in ticks) or the
//      user pressing a key or a mouse button. If the clear flag is set, it
//      then either clears the key or waits for the user to let the mouse
//      button up.
//
///////////////////////////////////////////////////////////////////////////
boolean IN_UserInput(longword delay)
{
    longword    lasttime;

    lasttime = GetTimeCount();
    IN_StartAck ();
    do
    {
        IN_ProcessEvents();
        if (IN_CheckAck())
            return true;
        DelayMilliseconds(5);
    } while (GetTimeCount() - lasttime < delay);
    return(false);
}

//===========================================================================

/*
===================
=
= IN_MouseButtons
=
===================
*/
int IN_MouseButtons (void)
{
    if (MousePresent)
        return INL_GetMouseButtons();
    else
        return 0;
}

bool IN_IsInputGrabbed()
{
    return IsInputGrabbed();
}

void IN_CenterMouse()
{
    CenterMouse(screenWidth, screenHeight);
}
