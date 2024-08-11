// WL_INTER.C

#include "wl_def.h"
#pragma hdrstop

LRstruct LevelRatios[LRpack];
int32_t lastBreathTime = 0;

void Write (int x, int y, const char *string);

//==========================================================================

/*
==================
=
= CLearSplitVWB
=
==================
*/

void
ClearSplitVWB (void)
{
    WindowX = 0;
    WindowY = 0;
    WindowW = 320;
    WindowH = 160;
}


//==========================================================================


//==========================================================================

/*
==================
=
= Victory
=
==================
*/

void
Victory (void)
{
}


//==========================================================================

/*
==================
=
= PG13
=
==================
*/

void
PG13 (void)
{
    VW_FadeOut ();
    VWB_Bar (0, 0, 320, 200, 0x82);     // background

    CA_CacheGrChunk (PG13PIC);
    VWB_DrawPic (216, 110, PG13PIC);
    VW_UpdateScreen ();

    UNCACHEGRCHUNK (PG13PIC);

    VW_FadeIn ();
    IN_UserInput (TickBase * 7);

    VW_FadeOut ();
}



//==========================================================================

void
Write (int x, int y, const char *string)
{
    static const int alpha[] = { L_NUM0PIC, L_NUM1PIC, L_NUM2PIC, L_NUM3PIC, L_NUM4PIC, L_NUM5PIC,
        L_NUM6PIC, L_NUM7PIC, L_NUM8PIC, L_NUM9PIC, L_COLONPIC, 0, 0, 0, 0, 0, 0, L_APIC, L_BPIC,
        L_CPIC, L_DPIC, L_EPIC, L_FPIC, L_GPIC, L_HPIC, L_IPIC, L_JPIC, L_KPIC,
        L_LPIC, L_MPIC, L_NPIC, L_OPIC, L_PPIC, L_QPIC, L_RPIC, L_SPIC, L_TPIC,
        L_UPIC, L_VPIC, L_WPIC, L_XPIC, L_YPIC, L_ZPIC
    };

    int i, ox, nx, ny, len = (int) strlen(string);
    char ch;

    ox = nx = x * 8;
    ny = y * 8;
    for (i = 0; i < len; i++)
    {
        if (string[i] == '\n')
        {
            nx = ox;
            ny += 16;
        }
        else
        {
            ch = string[i];
            if (ch >= 'a')
                ch -= ('a' - 'A');
            ch -= '0';

            switch (string[i])
            {
                case '!':
                    VWB_DrawPic (nx, ny, L_EXPOINTPIC);
                    nx += 8;
                    continue;

                case '\'':
                    VWB_DrawPic (nx, ny, L_APOSTROPHEPIC);
                    nx += 8;
                    continue;

                case ' ':
                    break;

                case 0x3a:     // ':'
                    VWB_DrawPic (nx, ny, L_COLONPIC);
                    nx += 8;
                    continue;

                case '%':
                    VWB_DrawPic (nx, ny, L_PERCENTPIC);
                    break;

                default:
                    VWB_DrawPic (nx, ny, alpha[ch]);
            }
            nx += 16;
        }
    }
}


//
// Breathe Mr. BJ!!!
//
void
BJ_Breathe (void)
{
    static int which = 0, max = 10;
    int pics[2] = { L_GUYPIC, L_GUY2PIC };

    SDL_Delay(5);

    if ((int32_t) GetTimeCount () - lastBreathTime > max)
    {
        which ^= 1;
        VWB_DrawPic (0, 16, pics[which]);
        VW_UpdateScreen ();
        lastBreathTime = GetTimeCount();
        max = 35;
    }
}



/*
==================
=
= LevelCompleted
=
= Entered with the screen faded out
= Still in split screen mode with the status bar
=
= Exit with the screen faded out
=
==================
*/

void
LevelCompleted (void)
{
#define VBLWAIT 30
#define PAR_AMOUNT      500
#define PERCENT100AMT   10000
    typedef struct
    {
        float time;
        char timestr[6];
    } times;

    int x, i, min, sec, ratio, kr, sr, tr;
    char tempstr[10];
    int32_t bonus, timeleft = 0;
    times parTimes[] = {
        //
        // SPEAR OF DESTINY TIMES
        //
        {1.5, "01:30"},
        {3.5, "03:30"},
        {2.75, "02:45"},
        {3.5, "03:30"},
        {0, "??:??"},           // Boss 1
        {4.5, "04:30"},
        {3.25, "03:15"},
        {2.75, "02:45"},
        {4.75, "04:45"},
        {0, "??:??"},           // Boss 2
        {6.5, "06:30"},
        {4.5, "04:30"},
        {2.75, "02:45"},
        {4.5, "04:30"},
        {6, "06:00"},
        {0, "??:??"},           // Boss 3
        {6, "06:00"},
        {0, "??:??"},           // Boss 4
        {0, "??:??"},           // Secret level 1
        {0, "??:??"},           // Secret level 2

    };

    CacheLump (LEVELEND_LUMP_START, LEVELEND_LUMP_END);
    ClearSplitVWB ();           // set up for double buffering in split screen
    VWB_Bar (0, 0, 320, screenHeight / scaleFactor - STATUSLINES + 1, VIEWCOLOR);

    if (bordercol != VIEWCOLOR)
        DrawStatusBorder (VIEWCOLOR);

    StartCPMusic (ENDLEVEL_MUS);

//
// do the intermission
//
    IN_ClearKeysDown ();
    IN_StartAck ();

    VWB_DrawPic (0, 16, L_GUYPIC);
    if (mapon != 4 && mapon != 9 && mapon != 15 && mapon < 17)

    {
        Write (14, 2, "floor\ncompleted");
        Write (14, 7, STR_BONUS "     0");
        Write (16, 10, STR_TIME);
        Write (16, 12, STR_PAR);
        Write (9, 14, STR_RAT2KILL);
        Write (5, 16, STR_RAT2SECRET);
        Write (1, 18, STR_RAT2TREASURE);


        Write (26, 2, itoa (gamestate.mapon + 1, tempstr, 10));
        Write (26, 12, parTimes[gamestate.episode * 10 + mapon].timestr);

        //
        // PRINT TIME
        //
        sec = gamestate.TimeCount / 70;

        if (sec > 99 * 60)      // 99 minutes max
            sec = 99 * 60;

        if (gamestate.TimeCount < parTimes[gamestate.episode * 10 + mapon].time * 4200)
            timeleft = (int32_t) ((parTimes[gamestate.episode * 10 + mapon].time * 4200) / 70 - sec);

        min = sec / 60;
        sec %= 60;
        i = 26 * 8;

        VWB_DrawPic (i, 10 * 8, L_NUM0PIC + (min / 10));
        i += 2 * 8;
        VWB_DrawPic (i, 10 * 8, L_NUM0PIC + (min % 10));
        i += 2 * 8;
        Write (i / 8, 10, ":");
        i += 1 * 8;
        VWB_DrawPic (i, 10 * 8, L_NUM0PIC + (sec / 10));
        i += 2 * 8;
        VWB_DrawPic (i, 10 * 8, L_NUM0PIC + (sec % 10));

        VW_UpdateScreen ();
        VW_FadeIn ();


        //
        // FIGURE RATIOS OUT BEFOREHAND
        //
        kr = sr = tr = 0;
        if (gamestate.killtotal)
            kr = (gamestate.killcount * 100) / gamestate.killtotal;
        if (gamestate.secrettotal)
            sr = (gamestate.secretcount * 100) / gamestate.secrettotal;
        if (gamestate.treasuretotal)
            tr = (gamestate.treasurecount * 100) / gamestate.treasuretotal;


        //
        // PRINT TIME BONUS
        //
        bonus = timeleft * PAR_AMOUNT;
        if (bonus)
        {
            for (i = 0; i <= timeleft; i++)
            {
                ltoa ((int32_t) i * PAR_AMOUNT, tempstr, 10);
                x = 36 - (int) strlen(tempstr) * 2;
                Write (x, 7, tempstr);
                if (!(i % (PAR_AMOUNT / 10)))
                    SD_PlaySound (ENDBONUS1SND);
                if(!usedoublebuffering || !(i % (PAR_AMOUNT / 50))) VW_UpdateScreen ();
                while(SD_SoundPlaying ())
                    BJ_Breathe ();
                if (IN_CheckAck ())
                    goto done;
            }

            VW_UpdateScreen ();

            SD_PlaySound (ENDBONUS2SND);
            while (SD_SoundPlaying ())
                BJ_Breathe ();
        }

#define RATIOXX                37

        //
        // KILL RATIO
        //
        ratio = kr;
        for (i = 0; i <= ratio; i++)
        {
            itoa (i, tempstr, 10);
            x = RATIOXX - (int) strlen(tempstr) * 2;
            Write (x, 14, tempstr);
            if (!(i % 10))
                SD_PlaySound (ENDBONUS1SND);
            if(!usedoublebuffering || !(i & 1)) VW_UpdateScreen ();
            while (SD_SoundPlaying ())
                BJ_Breathe ();

            if (IN_CheckAck ())
                goto done;
        }
        if (ratio >= 100)
        {
            VW_WaitVBL (VBLWAIT);
            SD_StopSound ();
            bonus += PERCENT100AMT;
            ltoa (bonus, tempstr, 10);
            x = (RATIOXX - 1) - (int) strlen(tempstr) * 2;
            Write (x, 7, tempstr);
            VW_UpdateScreen ();
            SD_PlaySound (PERCENT100SND);
        }
        else if (!ratio)
        {
            VW_WaitVBL (VBLWAIT);
            SD_StopSound ();
            SD_PlaySound (NOBONUSSND);
        }
        else
            SD_PlaySound (ENDBONUS2SND);

        VW_UpdateScreen ();
        while (SD_SoundPlaying ())
            BJ_Breathe ();

        //
        // SECRET RATIO
        //
        ratio = sr;
        for (i = 0; i <= ratio; i++)
        {
            itoa (i, tempstr, 10);
            x = RATIOXX - (int) strlen(tempstr) * 2;
            Write (x, 16, tempstr);
            if (!(i % 10))
                SD_PlaySound (ENDBONUS1SND);
            if(!usedoublebuffering || !(i & 1)) VW_UpdateScreen ();
            while (SD_SoundPlaying ())
                BJ_Breathe ();

            if (IN_CheckAck ())
                goto done;
        }
        if (ratio >= 100)
        {
            VW_WaitVBL (VBLWAIT);
            SD_StopSound ();
            bonus += PERCENT100AMT;
            ltoa (bonus, tempstr, 10);
            x = (RATIOXX - 1) - (int) strlen(tempstr) * 2;
            Write (x, 7, tempstr);
            VW_UpdateScreen ();
            SD_PlaySound (PERCENT100SND);
        }
        else if (!ratio)
        {
            VW_WaitVBL (VBLWAIT);
            SD_StopSound ();
            SD_PlaySound (NOBONUSSND);
        }
        else
            SD_PlaySound (ENDBONUS2SND);
        VW_UpdateScreen ();
        while (SD_SoundPlaying ())
            BJ_Breathe ();

        //
        // TREASURE RATIO
        //
        ratio = tr;
        for (i = 0; i <= ratio; i++)
        {
            itoa (i, tempstr, 10);
            x = RATIOXX - (int) strlen(tempstr) * 2;
            Write (x, 18, tempstr);
            if (!(i % 10))
                SD_PlaySound (ENDBONUS1SND);
            if(!usedoublebuffering || !(i & 1)) VW_UpdateScreen ();
            while (SD_SoundPlaying ())
                BJ_Breathe ();
            if (IN_CheckAck ())
                goto done;
        }
        if (ratio >= 100)
        {
            VW_WaitVBL (VBLWAIT);
            SD_StopSound ();
            bonus += PERCENT100AMT;
            ltoa (bonus, tempstr, 10);
            x = (RATIOXX - 1) - (int) strlen(tempstr) * 2;
            Write (x, 7, tempstr);
            VW_UpdateScreen ();
            SD_PlaySound (PERCENT100SND);
        }
        else if (!ratio)
        {
            VW_WaitVBL (VBLWAIT);
            SD_StopSound ();
            SD_PlaySound (NOBONUSSND);
        }
        else
            SD_PlaySound (ENDBONUS2SND);
        VW_UpdateScreen ();
        while (SD_SoundPlaying ())
            BJ_Breathe ();


        //
        // JUMP STRAIGHT HERE IF KEY PRESSED
        //
done:   itoa (kr, tempstr, 10);
        x = RATIOXX - (int) strlen(tempstr) * 2;
        Write (x, 14, tempstr);

        itoa (sr, tempstr, 10);
        x = RATIOXX - (int) strlen(tempstr) * 2;
        Write (x, 16, tempstr);

        itoa (tr, tempstr, 10);
        x = RATIOXX - (int) strlen(tempstr) * 2;
        Write (x, 18, tempstr);

        bonus = (int32_t) timeleft *PAR_AMOUNT +
            (PERCENT100AMT * (kr >= 100)) +
            (PERCENT100AMT * (sr >= 100)) + (PERCENT100AMT * (tr >= 100));

        GivePoints (bonus);
        ltoa (bonus, tempstr, 10);
        x = 36 - (int) strlen(tempstr) * 2;
        Write (x, 7, tempstr);

        //
        // SAVE RATIO INFORMATION FOR ENDGAME
        //
        LevelRatios[mapon].kill = kr;
        LevelRatios[mapon].secret = sr;
        LevelRatios[mapon].treasure = tr;
        LevelRatios[mapon].time = min * 60 + sec;
    }
    else
    {

        Write (10, 16, "15000 bonus!");

        VW_UpdateScreen ();
        VW_FadeIn ();

        GivePoints (15000);
    }


    DrawScore ();
    VW_UpdateScreen ();

    lastBreathTime = GetTimeCount();
    IN_StartAck ();
    while (!IN_CheckAck ())
        BJ_Breathe ();

//
// done
//
    if (gamestate.mapon == 1)
    {
        SD_PlaySound (BONUS1UPSND);

        CA_CacheGrChunk (STARTFONT + 1);
        Message ("This concludes your demo\n"
                 "of Spear of Destiny! Now,\n" "go to your local software\n" "store and buy it!");
        UNCACHEGRCHUNK (STARTFONT + 1);

        IN_ClearKeysDown ();
        IN_Ack ();
    }

    VW_FadeOut ();
    DrawPlayBorder();

    UnCacheLump (LEVELEND_LUMP_START, LEVELEND_LUMP_END);
}



//==========================================================================


/*
=================
=
= PreloadGraphics
=
= Fill the cache up
=
=================
*/

boolean
PreloadUpdate (unsigned current, unsigned total)
{
    unsigned w = WindowW - scaleFactor * 10;

    VWB_BarScaledCoord (WindowX + scaleFactor * 5, WindowY + WindowH - scaleFactor * 3,
        w, scaleFactor * 2, BLACK);
    w = ((int32_t) w * current) / total;
    if (w)
    {
        VWB_BarScaledCoord (WindowX + scaleFactor * 5, WindowY + WindowH - scaleFactor * 3,
            w, scaleFactor * 2, 0x37);       //SECONDCOLOR);
        VWB_BarScaledCoord (WindowX + scaleFactor * 5, WindowY + WindowH - scaleFactor * 3,
            w - scaleFactor * 1, scaleFactor * 1, 0x32);

    }
    VW_UpdateScreen ();
//      if (LastScan == sc_Escape)
//      {
//              IN_ClearKeysDown();
//              return(true);
//      }
//      else
    return (false);
}

void
PreloadGraphics (void)
{
    DrawLevel ();
    ClearSplitVWB ();           // set up for double buffering in split screen

    VWB_BarScaledCoord (0, 0, screenWidth, screenHeight - scaleFactor * (STATUSLINES - 1), bordercol);
    LatchDrawPicScaledCoord ((screenWidth-scaleFactor*224)/16,
        (screenHeight-scaleFactor*(STATUSLINES+48))/2, GETPSYCHEDPIC);

    WindowX = (screenWidth - scaleFactor*224)/2;
    WindowY = (screenHeight - scaleFactor*(STATUSLINES+48))/2;
    WindowW = scaleFactor * 28 * 8;
    WindowH = scaleFactor * 48;

    VW_UpdateScreen ();
    VW_FadeIn ();

//      PM_Preload (PreloadUpdate);
    PreloadUpdate (10, 10);
    IN_UserInput (70);
    VW_FadeOut ();

    DrawPlayBorder ();
    VW_UpdateScreen ();
}


//==========================================================================

/*
==================
=
= DrawHighScores
=
==================
*/

void
DrawHighScores (void)
{
    char buffer[16];
    word i, w, h;
    HighScore *s;
    CacheLump (BACKDROP_LUMP_START, BACKDROP_LUMP_END);
    ClearMScreen ();
    DrawStripes (10);
    UnCacheLump (BACKDROP_LUMP_START, BACKDROP_LUMP_END);

    CacheLump (HIGHSCORES_LUMP_START, HIGHSCORES_LUMP_END);
    CA_CacheGrChunk (STARTFONT + 1);
    VWB_DrawPic (0, 0, HIGHSCORESPIC);

    fontnumber = 1;

    SETFONTCOLOR (HIGHLIGHT, 0x29);


    for (i = 0, s = Scores; i < MaxScores; i++, s++)
    {
        PrintY = 76 + (16 * i);

        //
        // name
        //
        PrintX = 16;

        US_Print (s->name);

        //
        // level
        //
        itoa (s->completed, buffer, 10);
        USL_MeasureString (buffer, &w, &h);
        PrintX = 194 - w;
        if (s->completed == 21)
            VWB_DrawPic (PrintX + 8, PrintY - 1, C_WONSPEARPIC);
        else

            US_Print (buffer);

        //
        // score
        //
        itoa (s->score, buffer, 10);
        USL_MeasureString (buffer, &w, &h);
        PrintX = 292 - w;

        US_Print (buffer);
    }

    VW_UpdateScreen ();

    UnCacheLump (HIGHSCORES_LUMP_START, HIGHSCORES_LUMP_END);
    fontnumber = 0;

}

//===========================================================================


/*
=======================
=
= CheckHighScore
=
=======================
*/

void
CheckHighScore (int32_t score, word other)
{
    word i, j;
    int n;
    HighScore myscore;

    strcpy (myscore.name, "");
    myscore.score = score;
    myscore.episode = gamestate.episode;
    myscore.completed = other;

    for (i = 0, n = -1; i < MaxScores; i++)
    {
        if ((myscore.score > Scores[i].score)
            || ((myscore.score == Scores[i].score) && (myscore.completed > Scores[i].completed)))
        {
            for (j = MaxScores; --j > i;)
                Scores[j] = Scores[j - 1];
            Scores[i] = myscore;
            n = i;
            break;
        }
    }
    StartCPMusic (XAWARD_MUS);

    DrawHighScores ();

    VW_FadeIn ();

    if (n != -1)
    {
        //
        // got a high score
        //
        PrintY = 76 + (16 * n);
        PrintX = 16;
        fontnumber = 1;
        VWB_Bar (PrintX - 2, PrintY - 2, 145, 15, 0x9c);
        VW_UpdateScreen ();
        backcolor = 0x9c;
        fontcolor = 15;
        US_LineInput (PrintX, PrintY, Scores[n].name, 0, true, MaxHighName, 130);

    }
    else
    {
        IN_ClearKeysDown ();
        IN_UserInput (500);
    }

}

//===========================================================================
