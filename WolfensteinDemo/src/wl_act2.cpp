// WL_ACT2.C

#include <stdio.h>
#include <math.h>
#include "wl_def.h"
#include "sdl_graphics.h"
#pragma hdrstop

/*
=============================================================================

                               LOCAL CONSTANTS

=============================================================================
*/

#define PROJECTILESIZE  0xc000l

#define BJRUNSPEED      2048
#define BJJUMPSPEED     680


/*
=============================================================================

                              GLOBAL VARIABLES

=============================================================================
*/



/*
=============================================================================

                              LOCAL VARIABLES

=============================================================================
*/


dirtype dirtable[9] = {northwest,north,northeast,west,nodir,east,
    southwest,south,southeast};

short starthitpoints[4][NUMENEMIES] =
//
// BABY MODE
//
{
    {
        25,   // guards
        50,   // officer
        100,  // SS
        1,    // dogs
        850,  // Hans
        850,  // Schabbs
        200,  // fake hitler
        800,  // mecha hitler
        45,   // mutants
        25,   // ghosts
        25,   // ghosts
        25,   // ghosts
        25,   // ghosts

        850,  // Gretel
        850,  // Gift
        850,  // Fat
        5,    // en_spectre,
        1450, // en_angel,
        850,  // en_trans,
        1050, // en_uber,
        950,  // en_will,
        1250  // en_death
    },
    //
    // DON'T HURT ME MODE
    //
    {
        25,   // guards
        50,   // officer
        100,  // SS
        1,    // dogs
        950,  // Hans
        950,  // Schabbs
        300,  // fake hitler
        950,  // mecha hitler
        55,   // mutants
        25,   // ghosts
        25,   // ghosts
        25,   // ghosts
        25,   // ghosts

        950,  // Gretel
        950,  // Gift
        950,  // Fat
        10,   // en_spectre,
        1550, // en_angel,
        950,  // en_trans,
        1150, // en_uber,
        1050, // en_will,
        1350  // en_death
    },
    //
    // BRING 'EM ON MODE
    //
    {
        25,   // guards
        50,   // officer
        100,  // SS
        1,    // dogs

        1050, // Hans
        1550, // Schabbs
        400,  // fake hitler
        1050, // mecha hitler

        55,   // mutants
        25,   // ghosts
        25,   // ghosts
        25,   // ghosts
        25,   // ghosts

        1050, // Gretel
        1050, // Gift
        1050, // Fat
        15,   // en_spectre,
        1650, // en_angel,
        1050, // en_trans,
        1250, // en_uber,
        1150, // en_will,
        1450  // en_death
    },
    //
    // DEATH INCARNATE MODE
    //
    {
        25,   // guards
        50,   // officer
        100,  // SS
        1,    // dogs

        1200, // Hans
        2400, // Schabbs
        500,  // fake hitler
        1200, // mecha hitler

        65,   // mutants
        25,   // ghosts
        25,   // ghosts
        25,   // ghosts
        25,   // ghosts

        1200, // Gretel
        1200, // Gift
        1200, // Fat
        25,   // en_spectre,
        2000, // en_angel,
        1200, // en_trans,
        1400, // en_uber,
        1300, // en_will,
        1600  // en_death
    }
};

void    A_StartDeathCam (objtype *ob);


void    T_Path (objtype *ob);
void    T_Shoot (objtype *ob);
void    T_Bite (objtype *ob);
void    T_DogChase (objtype *ob);
void    T_Chase (objtype *ob);
void    T_Projectile (objtype *ob);
void    T_Stand (objtype *ob);

void A_DeathScream (objtype *ob);

extern  statetype s_rocket;
extern  statetype s_smoke1;
extern  statetype s_smoke2;
extern  statetype s_smoke3;
extern  statetype s_smoke4;
extern  statetype s_boom2;
extern  statetype s_boom3;

void A_Smoke (objtype *ob);

statetype s_rocket              = {true,SPR_ROCKET_1,3,(statefunc)T_Projectile,(statefunc)A_Smoke,&s_rocket};
statetype s_smoke1              = {false,SPR_SMOKE_1,3,NULL,NULL,&s_smoke2};
statetype s_smoke2              = {false,SPR_SMOKE_2,3,NULL,NULL,&s_smoke3};
statetype s_smoke3              = {false,SPR_SMOKE_3,3,NULL,NULL,&s_smoke4};
statetype s_smoke4              = {false,SPR_SMOKE_4,3,NULL,NULL,NULL};

statetype s_boom1               = {false,SPR_BOOM_1,6,NULL,NULL,&s_boom2};
statetype s_boom2               = {false,SPR_BOOM_2,6,NULL,NULL,&s_boom3};
statetype s_boom3               = {false,SPR_BOOM_3,6,NULL,NULL,NULL};

void    T_Schabb (objtype *ob);
void    T_SchabbThrow (objtype *ob);
void    T_Fake (objtype *ob);
void    T_FakeFire (objtype *ob);
void    T_Ghosts (objtype *ob);

void A_Slurpie (objtype *ob);
void A_HitlerMorph (objtype *ob);
void A_MechaSound (objtype *ob);

/*
=================
=
= A_Smoke
=
=================
*/

void A_Smoke (objtype *ob)
{
    GetNewActor ();
    newobj->state = &s_smoke1;
    newobj->ticcount = 6;

    newobj->tilex = ob->tilex;
    newobj->tiley = ob->tiley;
    newobj->x = ob->x;
    newobj->y = ob->y;
    newobj->obclass = inertobj;
    newobj->active = ac_yes;

    newobj->flags = FL_NEVERMARK;
}


/*
===================
=
= ProjectileTryMove
=
= returns true if move ok
===================
*/

#define PROJSIZE        0x2000

boolean ProjectileTryMove (objtype *ob)
{
    int      xl,yl,xh,yh,x,y;
    objtype *check;

    xl = (ob->x-PROJSIZE) >> TILESHIFT;
    yl = (ob->y-PROJSIZE) >> TILESHIFT;

    xh = (ob->x+PROJSIZE) >> TILESHIFT;
    yh = (ob->y+PROJSIZE) >> TILESHIFT;

    //
    // check for solid walls
    //
    for (y=yl;y<=yh;y++)
        for (x=xl;x<=xh;x++)
        {
            check = actorat[x][y];
            if (check && !ISPOINTER(check))
                return false;
        }

        return true;
}



/*
=================
=
= T_Projectile
=
=================
*/

void T_Projectile (objtype *ob)
{
    int32_t deltax,deltay;
    int     damage;
    int32_t speed;

    speed = (int32_t)ob->speed*tics;

    deltax = FixedMul(speed,costable[ob->angle]);
    deltay = -FixedMul(speed,sintable[ob->angle]);

    if (deltax>0x10000l)
        deltax = 0x10000l;
    if (deltay>0x10000l)
        deltay = 0x10000l;

    ob->x += deltax;
    ob->y += deltay;

    deltax = LABS(ob->x - player->x);
    deltay = LABS(ob->y - player->y);

    if (!ProjectileTryMove (ob))
    {
        if (ob->obclass == rocketobj)
        {
            PlaySoundLocActor(MISSILEHITSND,ob);
            ob->state = &s_boom1;
        }
        else

            ob->state = NULL;               // mark for removal

        return;
    }

    if (deltax < PROJECTILESIZE && deltay < PROJECTILESIZE)
    {       // hit the player
        switch (ob->obclass)
        {
        case needleobj:
            damage = (US_RndT() >>3) + 20;
            break;
        case rocketobj:
        case hrocketobj:
        case sparkobj:
            damage = (US_RndT() >>3) + 30;
            break;
        case fireobj:
            damage = (US_RndT() >>3);
            break;
        }

        TakeDamage (damage,ob);
        ob->state = NULL;               // mark for removal
        return;
    }

    ob->tilex = (short)(ob->x >> TILESHIFT);
    ob->tiley = (short)(ob->y >> TILESHIFT);
}


/*
=============================================================================

GUARD

=============================================================================
*/

//
// guards
//

extern  statetype s_grdstand;

extern  statetype s_grdpath1;
extern  statetype s_grdpath1s;
extern  statetype s_grdpath2;
extern  statetype s_grdpath3;
extern  statetype s_grdpath3s;
extern  statetype s_grdpath4;

extern  statetype s_grdpain;
extern  statetype s_grdpain1;

extern  statetype s_grdgiveup;

extern  statetype s_grdshoot1;
extern  statetype s_grdshoot2;
extern  statetype s_grdshoot3;
extern  statetype s_grdshoot4;

extern  statetype s_grdchase1;
extern  statetype s_grdchase1s;
extern  statetype s_grdchase2;
extern  statetype s_grdchase3;
extern  statetype s_grdchase3s;
extern  statetype s_grdchase4;

extern  statetype s_grddie1;
extern  statetype s_grddie1d;
extern  statetype s_grddie2;
extern  statetype s_grddie3;
extern  statetype s_grddie4;

statetype s_grdstand            = {true,SPR_GRD_S_1,0,(statefunc)T_Stand,NULL,&s_grdstand};

statetype s_grdpath1            = {true,SPR_GRD_W1_1,20,(statefunc)T_Path,NULL,&s_grdpath1s};
statetype s_grdpath1s           = {true,SPR_GRD_W1_1,5,NULL,NULL,&s_grdpath2};
statetype s_grdpath2            = {true,SPR_GRD_W2_1,15,(statefunc)T_Path,NULL,&s_grdpath3};
statetype s_grdpath3            = {true,SPR_GRD_W3_1,20,(statefunc)T_Path,NULL,&s_grdpath3s};
statetype s_grdpath3s           = {true,SPR_GRD_W3_1,5,NULL,NULL,&s_grdpath4};
statetype s_grdpath4            = {true,SPR_GRD_W4_1,15,(statefunc)T_Path,NULL,&s_grdpath1};

statetype s_grdpain             = {2,SPR_GRD_PAIN_1,10,NULL,NULL,&s_grdchase1};
statetype s_grdpain1            = {2,SPR_GRD_PAIN_2,10,NULL,NULL,&s_grdchase1};

statetype s_grdshoot1           = {false,SPR_GRD_SHOOT1,20,NULL,NULL,&s_grdshoot2};
statetype s_grdshoot2           = {false,SPR_GRD_SHOOT2,20,NULL,(statefunc)T_Shoot,&s_grdshoot3};
statetype s_grdshoot3           = {false,SPR_GRD_SHOOT3,20,NULL,NULL,&s_grdchase1};

statetype s_grdchase1           = {true,SPR_GRD_W1_1,10,(statefunc)T_Chase,NULL,&s_grdchase1s};
statetype s_grdchase1s          = {true,SPR_GRD_W1_1,3,NULL,NULL,&s_grdchase2};
statetype s_grdchase2           = {true,SPR_GRD_W2_1,8,(statefunc)T_Chase,NULL,&s_grdchase3};
statetype s_grdchase3           = {true,SPR_GRD_W3_1,10,(statefunc)T_Chase,NULL,&s_grdchase3s};
statetype s_grdchase3s          = {true,SPR_GRD_W3_1,3,NULL,NULL,&s_grdchase4};
statetype s_grdchase4           = {true,SPR_GRD_W4_1,8,(statefunc)T_Chase,NULL,&s_grdchase1};

statetype s_grddie1             = {false,SPR_GRD_DIE_1,15,NULL,(statefunc)A_DeathScream,&s_grddie2};
statetype s_grddie2             = {false,SPR_GRD_DIE_2,15,NULL,NULL,&s_grddie3};
statetype s_grddie3             = {false,SPR_GRD_DIE_3,15,NULL,NULL,&s_grddie4};
statetype s_grddie4             = {false,SPR_GRD_DEAD,0,NULL,NULL,&s_grddie4};


//
// ghosts
//
extern  statetype s_blinkychase1;
extern  statetype s_blinkychase2;
extern  statetype s_inkychase1;
extern  statetype s_inkychase2;
extern  statetype s_pinkychase1;
extern  statetype s_pinkychase2;
extern  statetype s_clydechase1;
extern  statetype s_clydechase2;

statetype s_blinkychase1        = {false,SPR_BLINKY_W1,10,(statefunc)T_Ghosts,NULL,&s_blinkychase2};
statetype s_blinkychase2        = {false,SPR_BLINKY_W2,10,(statefunc)T_Ghosts,NULL,&s_blinkychase1};

statetype s_inkychase1          = {false,SPR_INKY_W1,10,(statefunc)T_Ghosts,NULL,&s_inkychase2};
statetype s_inkychase2          = {false,SPR_INKY_W2,10,(statefunc)T_Ghosts,NULL,&s_inkychase1};

statetype s_pinkychase1         = {false,SPR_PINKY_W1,10,(statefunc)T_Ghosts,NULL,&s_pinkychase2};
statetype s_pinkychase2         = {false,SPR_PINKY_W2,10,(statefunc)T_Ghosts,NULL,&s_pinkychase1};

statetype s_clydechase1         = {false,SPR_CLYDE_W1,10,(statefunc)T_Ghosts,NULL,&s_clydechase2};
statetype s_clydechase2         = {false,SPR_CLYDE_W2,10,(statefunc)T_Ghosts,NULL,&s_clydechase1};


//
// dogs
//

extern  statetype s_dogpath1;
extern  statetype s_dogpath1s;
extern  statetype s_dogpath2;
extern  statetype s_dogpath3;
extern  statetype s_dogpath3s;
extern  statetype s_dogpath4;

extern  statetype s_dogjump1;
extern  statetype s_dogjump2;
extern  statetype s_dogjump3;
extern  statetype s_dogjump4;
extern  statetype s_dogjump5;

extern  statetype s_dogchase1;
extern  statetype s_dogchase1s;
extern  statetype s_dogchase2;
extern  statetype s_dogchase3;
extern  statetype s_dogchase3s;
extern  statetype s_dogchase4;

extern  statetype s_dogdie1;
extern  statetype s_dogdie1d;
extern  statetype s_dogdie2;
extern  statetype s_dogdie3;
extern  statetype s_dogdead;

statetype s_dogpath1            = {true,SPR_DOG_W1_1,20,(statefunc)T_Path,NULL,&s_dogpath1s};
statetype s_dogpath1s           = {true,SPR_DOG_W1_1,5,NULL,NULL,&s_dogpath2};
statetype s_dogpath2            = {true,SPR_DOG_W2_1,15,(statefunc)T_Path,NULL,&s_dogpath3};
statetype s_dogpath3            = {true,SPR_DOG_W3_1,20,(statefunc)T_Path,NULL,&s_dogpath3s};
statetype s_dogpath3s           = {true,SPR_DOG_W3_1,5,NULL,NULL,&s_dogpath4};
statetype s_dogpath4            = {true,SPR_DOG_W4_1,15,(statefunc)T_Path,NULL,&s_dogpath1};

statetype s_dogjump1            = {false,SPR_DOG_JUMP1,10,NULL,NULL,&s_dogjump2};
statetype s_dogjump2            = {false,SPR_DOG_JUMP2,10,NULL,(statefunc)T_Bite,&s_dogjump3};
statetype s_dogjump3            = {false,SPR_DOG_JUMP3,10,NULL,NULL,&s_dogjump4};
statetype s_dogjump4            = {false,SPR_DOG_JUMP1,10,NULL,NULL,&s_dogjump5};
statetype s_dogjump5            = {false,SPR_DOG_W1_1,10,NULL,NULL,&s_dogchase1};

statetype s_dogchase1           = {true,SPR_DOG_W1_1,10,(statefunc)T_DogChase,NULL,&s_dogchase1s};
statetype s_dogchase1s          = {true,SPR_DOG_W1_1,3,NULL,NULL,&s_dogchase2};
statetype s_dogchase2           = {true,SPR_DOG_W2_1,8,(statefunc)T_DogChase,NULL,&s_dogchase3};
statetype s_dogchase3           = {true,SPR_DOG_W3_1,10,(statefunc)T_DogChase,NULL,&s_dogchase3s};
statetype s_dogchase3s          = {true,SPR_DOG_W3_1,3,NULL,NULL,&s_dogchase4};
statetype s_dogchase4           = {true,SPR_DOG_W4_1,8,(statefunc)T_DogChase,NULL,&s_dogchase1};

statetype s_dogdie1             = {false,SPR_DOG_DIE_1,15,NULL,(statefunc)A_DeathScream,&s_dogdie2};
statetype s_dogdie2             = {false,SPR_DOG_DIE_2,15,NULL,NULL,&s_dogdie3};
statetype s_dogdie3             = {false,SPR_DOG_DIE_3,15,NULL,NULL,&s_dogdead};
statetype s_dogdead             = {false,SPR_DOG_DEAD,15,NULL,NULL,&s_dogdead};


//
// officers
//

extern  statetype s_ofcstand;

extern  statetype s_ofcpath1;
extern  statetype s_ofcpath1s;
extern  statetype s_ofcpath2;
extern  statetype s_ofcpath3;
extern  statetype s_ofcpath3s;
extern  statetype s_ofcpath4;

extern  statetype s_ofcpain;
extern  statetype s_ofcpain1;

extern  statetype s_ofcgiveup;

extern  statetype s_ofcshoot1;
extern  statetype s_ofcshoot2;
extern  statetype s_ofcshoot3;
extern  statetype s_ofcshoot4;

extern  statetype s_ofcchase1;
extern  statetype s_ofcchase1s;
extern  statetype s_ofcchase2;
extern  statetype s_ofcchase3;
extern  statetype s_ofcchase3s;
extern  statetype s_ofcchase4;

extern  statetype s_ofcdie1;
extern  statetype s_ofcdie2;
extern  statetype s_ofcdie3;
extern  statetype s_ofcdie4;
extern  statetype s_ofcdie5;

statetype s_ofcstand            = {true,SPR_OFC_S_1,0,(statefunc)T_Stand,NULL,&s_ofcstand};

statetype s_ofcpath1            = {true,SPR_OFC_W1_1,20,(statefunc)T_Path,NULL,&s_ofcpath1s};
statetype s_ofcpath1s           = {true,SPR_OFC_W1_1,5,NULL,NULL,&s_ofcpath2};
statetype s_ofcpath2            = {true,SPR_OFC_W2_1,15,(statefunc)T_Path,NULL,&s_ofcpath3};
statetype s_ofcpath3            = {true,SPR_OFC_W3_1,20,(statefunc)T_Path,NULL,&s_ofcpath3s};
statetype s_ofcpath3s           = {true,SPR_OFC_W3_1,5,NULL,NULL,&s_ofcpath4};
statetype s_ofcpath4            = {true,SPR_OFC_W4_1,15,(statefunc)T_Path,NULL,&s_ofcpath1};

statetype s_ofcpain             = {2,SPR_OFC_PAIN_1,10,NULL,NULL,&s_ofcchase1};
statetype s_ofcpain1            = {2,SPR_OFC_PAIN_2,10,NULL,NULL,&s_ofcchase1};

statetype s_ofcshoot1           = {false,SPR_OFC_SHOOT1,6,NULL,NULL,&s_ofcshoot2};
statetype s_ofcshoot2           = {false,SPR_OFC_SHOOT2,20,NULL,(statefunc)T_Shoot,&s_ofcshoot3};
statetype s_ofcshoot3           = {false,SPR_OFC_SHOOT3,10,NULL,NULL,&s_ofcchase1};

statetype s_ofcchase1           = {true,SPR_OFC_W1_1,10,(statefunc)T_Chase,NULL,&s_ofcchase1s};
statetype s_ofcchase1s          = {true,SPR_OFC_W1_1,3,NULL,NULL,&s_ofcchase2};
statetype s_ofcchase2           = {true,SPR_OFC_W2_1,8,(statefunc)T_Chase,NULL,&s_ofcchase3};
statetype s_ofcchase3           = {true,SPR_OFC_W3_1,10,(statefunc)T_Chase,NULL,&s_ofcchase3s};
statetype s_ofcchase3s          = {true,SPR_OFC_W3_1,3,NULL,NULL,&s_ofcchase4};
statetype s_ofcchase4           = {true,SPR_OFC_W4_1,8,(statefunc)T_Chase,NULL,&s_ofcchase1};

statetype s_ofcdie1             = {false,SPR_OFC_DIE_1,11,NULL,(statefunc)A_DeathScream,&s_ofcdie2};
statetype s_ofcdie2             = {false,SPR_OFC_DIE_2,11,NULL,NULL,&s_ofcdie3};
statetype s_ofcdie3             = {false,SPR_OFC_DIE_3,11,NULL,NULL,&s_ofcdie4};
statetype s_ofcdie4             = {false,SPR_OFC_DIE_4,11,NULL,NULL,&s_ofcdie5};
statetype s_ofcdie5             = {false,SPR_OFC_DEAD,0,NULL,NULL,&s_ofcdie5};


//
// mutant
//

extern  statetype s_mutstand;

extern  statetype s_mutpath1;
extern  statetype s_mutpath1s;
extern  statetype s_mutpath2;
extern  statetype s_mutpath3;
extern  statetype s_mutpath3s;
extern  statetype s_mutpath4;

extern  statetype s_mutpain;
extern  statetype s_mutpain1;

extern  statetype s_mutgiveup;

extern  statetype s_mutshoot1;
extern  statetype s_mutshoot2;
extern  statetype s_mutshoot3;
extern  statetype s_mutshoot4;

extern  statetype s_mutchase1;
extern  statetype s_mutchase1s;
extern  statetype s_mutchase2;
extern  statetype s_mutchase3;
extern  statetype s_mutchase3s;
extern  statetype s_mutchase4;

extern  statetype s_mutdie1;
extern  statetype s_mutdie2;
extern  statetype s_mutdie3;
extern  statetype s_mutdie4;
extern  statetype s_mutdie5;

statetype s_mutstand            = {true,SPR_MUT_S_1,0,(statefunc)T_Stand,NULL,&s_mutstand};

statetype s_mutpath1            = {true,SPR_MUT_W1_1,20,(statefunc)T_Path,NULL,&s_mutpath1s};
statetype s_mutpath1s           = {true,SPR_MUT_W1_1,5,NULL,NULL,&s_mutpath2};
statetype s_mutpath2            = {true,SPR_MUT_W2_1,15,(statefunc)T_Path,NULL,&s_mutpath3};
statetype s_mutpath3            = {true,SPR_MUT_W3_1,20,(statefunc)T_Path,NULL,&s_mutpath3s};
statetype s_mutpath3s           = {true,SPR_MUT_W3_1,5,NULL,NULL,&s_mutpath4};
statetype s_mutpath4            = {true,SPR_MUT_W4_1,15,(statefunc)T_Path,NULL,&s_mutpath1};

statetype s_mutpain             = {2,SPR_MUT_PAIN_1,10,NULL,NULL,&s_mutchase1};
statetype s_mutpain1            = {2,SPR_MUT_PAIN_2,10,NULL,NULL,&s_mutchase1};

statetype s_mutshoot1           = {false,SPR_MUT_SHOOT1,6,NULL,(statefunc)T_Shoot,&s_mutshoot2};
statetype s_mutshoot2           = {false,SPR_MUT_SHOOT2,20,NULL,NULL,&s_mutshoot3};
statetype s_mutshoot3           = {false,SPR_MUT_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_mutshoot4};
statetype s_mutshoot4           = {false,SPR_MUT_SHOOT4,20,NULL,NULL,&s_mutchase1};

statetype s_mutchase1           = {true,SPR_MUT_W1_1,10,(statefunc)T_Chase,NULL,&s_mutchase1s};
statetype s_mutchase1s          = {true,SPR_MUT_W1_1,3,NULL,NULL,&s_mutchase2};
statetype s_mutchase2           = {true,SPR_MUT_W2_1,8,(statefunc)T_Chase,NULL,&s_mutchase3};
statetype s_mutchase3           = {true,SPR_MUT_W3_1,10,(statefunc)T_Chase,NULL,&s_mutchase3s};
statetype s_mutchase3s          = {true,SPR_MUT_W3_1,3,NULL,NULL,&s_mutchase4};
statetype s_mutchase4           = {true,SPR_MUT_W4_1,8,(statefunc)T_Chase,NULL,&s_mutchase1};

statetype s_mutdie1             = {false,SPR_MUT_DIE_1,7,NULL,(statefunc)A_DeathScream,&s_mutdie2};
statetype s_mutdie2             = {false,SPR_MUT_DIE_2,7,NULL,NULL,&s_mutdie3};
statetype s_mutdie3             = {false,SPR_MUT_DIE_3,7,NULL,NULL,&s_mutdie4};
statetype s_mutdie4             = {false,SPR_MUT_DIE_4,7,NULL,NULL,&s_mutdie5};
statetype s_mutdie5             = {false,SPR_MUT_DEAD,0,NULL,NULL,&s_mutdie5};


//
// SS
//

extern  statetype s_ssstand;

extern  statetype s_sspath1;
extern  statetype s_sspath1s;
extern  statetype s_sspath2;
extern  statetype s_sspath3;
extern  statetype s_sspath3s;
extern  statetype s_sspath4;

extern  statetype s_sspain;
extern  statetype s_sspain1;

extern  statetype s_ssshoot1;
extern  statetype s_ssshoot2;
extern  statetype s_ssshoot3;
extern  statetype s_ssshoot4;
extern  statetype s_ssshoot5;
extern  statetype s_ssshoot6;
extern  statetype s_ssshoot7;
extern  statetype s_ssshoot8;
extern  statetype s_ssshoot9;

extern  statetype s_sschase1;
extern  statetype s_sschase1s;
extern  statetype s_sschase2;
extern  statetype s_sschase3;
extern  statetype s_sschase3s;
extern  statetype s_sschase4;

extern  statetype s_ssdie1;
extern  statetype s_ssdie2;
extern  statetype s_ssdie3;
extern  statetype s_ssdie4;

statetype s_ssstand             = {true,SPR_SS_S_1,0,(statefunc)T_Stand,NULL,&s_ssstand};

statetype s_sspath1             = {true,SPR_SS_W1_1,20,(statefunc)T_Path,NULL,&s_sspath1s};
statetype s_sspath1s            = {true,SPR_SS_W1_1,5,NULL,NULL,&s_sspath2};
statetype s_sspath2             = {true,SPR_SS_W2_1,15,(statefunc)T_Path,NULL,&s_sspath3};
statetype s_sspath3             = {true,SPR_SS_W3_1,20,(statefunc)T_Path,NULL,&s_sspath3s};
statetype s_sspath3s            = {true,SPR_SS_W3_1,5,NULL,NULL,&s_sspath4};
statetype s_sspath4             = {true,SPR_SS_W4_1,15,(statefunc)T_Path,NULL,&s_sspath1};

statetype s_sspain              = {2,SPR_SS_PAIN_1,10,NULL,NULL,&s_sschase1};
statetype s_sspain1             = {2,SPR_SS_PAIN_2,10,NULL,NULL,&s_sschase1};

statetype s_ssshoot1            = {false,SPR_SS_SHOOT1,20,NULL,NULL,&s_ssshoot2};
statetype s_ssshoot2            = {false,SPR_SS_SHOOT2,20,NULL,(statefunc)T_Shoot,&s_ssshoot3};
statetype s_ssshoot3            = {false,SPR_SS_SHOOT3,10,NULL,NULL,&s_ssshoot4};
statetype s_ssshoot4            = {false,SPR_SS_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_ssshoot5};
statetype s_ssshoot5            = {false,SPR_SS_SHOOT3,10,NULL,NULL,&s_ssshoot6};
statetype s_ssshoot6            = {false,SPR_SS_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_ssshoot7};
statetype s_ssshoot7            = {false,SPR_SS_SHOOT3,10,NULL,NULL,&s_ssshoot8};
statetype s_ssshoot8            = {false,SPR_SS_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_ssshoot9};
statetype s_ssshoot9            = {false,SPR_SS_SHOOT3,10,NULL,NULL,&s_sschase1};

statetype s_sschase1            = {true,SPR_SS_W1_1,10,(statefunc)T_Chase,NULL,&s_sschase1s};
statetype s_sschase1s           = {true,SPR_SS_W1_1,3,NULL,NULL,&s_sschase2};
statetype s_sschase2            = {true,SPR_SS_W2_1,8,(statefunc)T_Chase,NULL,&s_sschase3};
statetype s_sschase3            = {true,SPR_SS_W3_1,10,(statefunc)T_Chase,NULL,&s_sschase3s};
statetype s_sschase3s           = {true,SPR_SS_W3_1,3,NULL,NULL,&s_sschase4};
statetype s_sschase4            = {true,SPR_SS_W4_1,8,(statefunc)T_Chase,NULL,&s_sschase1};

statetype s_ssdie1              = {false,SPR_SS_DIE_1,15,NULL,(statefunc)A_DeathScream,&s_ssdie2};
statetype s_ssdie2              = {false,SPR_SS_DIE_2,15,NULL,NULL,&s_ssdie3};
statetype s_ssdie3              = {false,SPR_SS_DIE_3,15,NULL,NULL,&s_ssdie4};
statetype s_ssdie4              = {false,SPR_SS_DEAD,0,NULL,NULL,&s_ssdie4};



//
// hans
//
extern  statetype s_bossstand;

extern  statetype s_bosschase1;
extern  statetype s_bosschase1s;
extern  statetype s_bosschase2;
extern  statetype s_bosschase3;
extern  statetype s_bosschase3s;
extern  statetype s_bosschase4;

extern  statetype s_bossdie1;
extern  statetype s_bossdie2;
extern  statetype s_bossdie3;
extern  statetype s_bossdie4;

extern  statetype s_bossshoot1;
extern  statetype s_bossshoot2;
extern  statetype s_bossshoot3;
extern  statetype s_bossshoot4;
extern  statetype s_bossshoot5;
extern  statetype s_bossshoot6;
extern  statetype s_bossshoot7;
extern  statetype s_bossshoot8;


statetype s_bossstand           = {false,SPR_BOSS_W1,0,(statefunc)T_Stand,NULL,&s_bossstand};

statetype s_bosschase1          = {false,SPR_BOSS_W1,10,(statefunc)T_Chase,NULL,&s_bosschase1s};
statetype s_bosschase1s         = {false,SPR_BOSS_W1,3,NULL,NULL,&s_bosschase2};
statetype s_bosschase2          = {false,SPR_BOSS_W2,8,(statefunc)T_Chase,NULL,&s_bosschase3};
statetype s_bosschase3          = {false,SPR_BOSS_W3,10,(statefunc)T_Chase,NULL,&s_bosschase3s};
statetype s_bosschase3s         = {false,SPR_BOSS_W3,3,NULL,NULL,&s_bosschase4};
statetype s_bosschase4          = {false,SPR_BOSS_W4,8,(statefunc)T_Chase,NULL,&s_bosschase1};

statetype s_bossdie1            = {false,SPR_BOSS_DIE1,15,NULL,(statefunc)A_DeathScream,&s_bossdie2};
statetype s_bossdie2            = {false,SPR_BOSS_DIE2,15,NULL,NULL,&s_bossdie3};
statetype s_bossdie3            = {false,SPR_BOSS_DIE3,15,NULL,NULL,&s_bossdie4};
statetype s_bossdie4            = {false,SPR_BOSS_DEAD,0,NULL,NULL,&s_bossdie4};

statetype s_bossshoot1          = {false,SPR_BOSS_SHOOT1,30,NULL,NULL,&s_bossshoot2};
statetype s_bossshoot2          = {false,SPR_BOSS_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_bossshoot3};
statetype s_bossshoot3          = {false,SPR_BOSS_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_bossshoot4};
statetype s_bossshoot4          = {false,SPR_BOSS_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_bossshoot5};
statetype s_bossshoot5          = {false,SPR_BOSS_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_bossshoot6};
statetype s_bossshoot6          = {false,SPR_BOSS_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_bossshoot7};
statetype s_bossshoot7          = {false,SPR_BOSS_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_bossshoot8};
statetype s_bossshoot8          = {false,SPR_BOSS_SHOOT1,10,NULL,NULL,&s_bosschase1};


//
// gretel
//
extern  statetype s_gretelstand;

extern  statetype s_gretelchase1;
extern  statetype s_gretelchase1s;
extern  statetype s_gretelchase2;
extern  statetype s_gretelchase3;
extern  statetype s_gretelchase3s;
extern  statetype s_gretelchase4;

extern  statetype s_greteldie1;
extern  statetype s_greteldie2;
extern  statetype s_greteldie3;
extern  statetype s_greteldie4;

extern  statetype s_gretelshoot1;
extern  statetype s_gretelshoot2;
extern  statetype s_gretelshoot3;
extern  statetype s_gretelshoot4;
extern  statetype s_gretelshoot5;
extern  statetype s_gretelshoot6;
extern  statetype s_gretelshoot7;
extern  statetype s_gretelshoot8;


statetype s_gretelstand         = {false,SPR_GRETEL_W1,0,(statefunc)T_Stand,NULL,&s_gretelstand};

statetype s_gretelchase1        = {false,SPR_GRETEL_W1,10,(statefunc)T_Chase,NULL,&s_gretelchase1s};
statetype s_gretelchase1s       = {false,SPR_GRETEL_W1,3,NULL,NULL,&s_gretelchase2};
statetype s_gretelchase2        = {false,SPR_GRETEL_W2,8,(statefunc)T_Chase,NULL,&s_gretelchase3};
statetype s_gretelchase3        = {false,SPR_GRETEL_W3,10,(statefunc)T_Chase,NULL,&s_gretelchase3s};
statetype s_gretelchase3s       = {false,SPR_GRETEL_W3,3,NULL,NULL,&s_gretelchase4};
statetype s_gretelchase4        = {false,SPR_GRETEL_W4,8,(statefunc)T_Chase,NULL,&s_gretelchase1};

statetype s_greteldie1          = {false,SPR_GRETEL_DIE1,15,NULL,(statefunc)A_DeathScream,&s_greteldie2};
statetype s_greteldie2          = {false,SPR_GRETEL_DIE2,15,NULL,NULL,&s_greteldie3};
statetype s_greteldie3          = {false,SPR_GRETEL_DIE3,15,NULL,NULL,&s_greteldie4};
statetype s_greteldie4          = {false,SPR_GRETEL_DEAD,0,NULL,NULL,&s_greteldie4};

statetype s_gretelshoot1        = {false,SPR_GRETEL_SHOOT1,30,NULL,NULL,&s_gretelshoot2};
statetype s_gretelshoot2        = {false,SPR_GRETEL_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_gretelshoot3};
statetype s_gretelshoot3        = {false,SPR_GRETEL_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_gretelshoot4};
statetype s_gretelshoot4        = {false,SPR_GRETEL_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_gretelshoot5};
statetype s_gretelshoot5        = {false,SPR_GRETEL_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_gretelshoot6};
statetype s_gretelshoot6        = {false,SPR_GRETEL_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_gretelshoot7};
statetype s_gretelshoot7        = {false,SPR_GRETEL_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_gretelshoot8};
statetype s_gretelshoot8        = {false,SPR_GRETEL_SHOOT1,10,NULL,NULL,&s_gretelchase1};



/*
===============
=
= SpawnStand
=
===============
*/

void SpawnStand (enemy_t which, int tilex, int tiley, int dir)
{
    word *map;
    word tile;

    switch (which)
    {
        case en_guard:
            SpawnNewObj (tilex,tiley,&s_grdstand);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_officer:
            SpawnNewObj (tilex,tiley,&s_ofcstand);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_mutant:
            SpawnNewObj (tilex,tiley,&s_mutstand);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_ss:
            SpawnNewObj (tilex,tiley,&s_ssstand);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;
    }


    map = mapsegs[0]+(tiley<<mapshift)+tilex;
    tile = *map;
    if (tile == AMBUSHTILE)
    {
        tilemap[tilex][tiley] = 0;

        if (*(map+1) >= AREATILE)
            tile = *(map+1);
        if (*(map-mapwidth) >= AREATILE)
            tile = *(map-mapwidth);
        if (*(map+mapwidth) >= AREATILE)
            tile = *(map+mapwidth);
        if ( *(map-1) >= AREATILE)
            tile = *(map-1);

        *map = tile;
        newobj->areanumber = tile-AREATILE;

        newobj->flags |= FL_AMBUSH;
    }

    newobj->obclass = (classtype)(guardobj + which);
    newobj->hitpoints = starthitpoints[gamestate.difficulty][which];
    newobj->dir = (dirtype)(dir * 2);
    newobj->flags |= FL_SHOOTABLE;
}



/*
===============
=
= SpawnDeadGuard
=
===============
*/

void SpawnDeadGuard (int tilex, int tiley)
{
    SpawnNewObj (tilex,tiley,&s_grddie4);
    DEMOIF_SDL
    {
        newobj->flags |= FL_NONMARK;    // walk through moving enemy fix
    }
    newobj->obclass = inertobj;
}



/*
===============
=
= SpawnBoss
=
===============
*/

void SpawnBoss (int tilex, int tiley)
{
    SpawnNewObj (tilex,tiley,&s_bossstand);
    newobj->speed = SPDPATROL;

    newobj->obclass = bossobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_boss];
    newobj->dir = nodir;
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
    if (!loadedgame)
        gamestate.killtotal++;
}

/*
===============
=
= SpawnGretel
=
===============
*/

void SpawnGretel (int tilex, int tiley)
{
    SpawnNewObj (tilex,tiley,&s_gretelstand);
    newobj->speed = SPDPATROL;

    newobj->obclass = gretelobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_gretel];
    newobj->dir = nodir;
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
    if (!loadedgame)
        gamestate.killtotal++;
}

/*
===============
=
= SpawnPatrol
=
===============
*/

void SpawnPatrol (enemy_t which, int tilex, int tiley, int dir)
{
    switch (which)
    {
        case en_guard:
            SpawnNewObj (tilex,tiley,&s_grdpath1);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_officer:
            SpawnNewObj (tilex,tiley,&s_ofcpath1);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_ss:
            SpawnNewObj (tilex,tiley,&s_sspath1);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_mutant:
            SpawnNewObj (tilex,tiley,&s_mutpath1);
            newobj->speed = SPDPATROL;
            if (!loadedgame)
                gamestate.killtotal++;
            break;

        case en_dog:
            SpawnNewObj (tilex,tiley,&s_dogpath1);
            newobj->speed = SPDDOG;
            if (!loadedgame)
                gamestate.killtotal++;
            break;
    }

    newobj->obclass = (classtype)(guardobj+which);
    newobj->dir = (dirtype)(dir*2);
    newobj->hitpoints = starthitpoints[gamestate.difficulty][which];
    newobj->distance = TILEGLOBAL;
    newobj->flags |= FL_SHOOTABLE;
    newobj->active = ac_yes;

    actorat[newobj->tilex][newobj->tiley] = NULL;           // don't use original spot

    switch (dir)
    {
        case 0:
            newobj->tilex++;
            break;
        case 1:
            newobj->tiley--;
            break;
        case 2:
            newobj->tilex--;
            break;
        case 3:
            newobj->tiley++;
            break;
    }

    actorat[newobj->tilex][newobj->tiley] = newobj;
}



/*
==================
=
= A_DeathScream
=
==================
*/

void A_DeathScream (objtype *ob)
{

    switch (ob->obclass)
    {
        case mutantobj:
            PlaySoundLocActor(AHHHGSND,ob);
            break;

        case guardobj:
        {
            int sounds[9]={ DEATHSCREAM1SND,
                DEATHSCREAM2SND,
                DEATHSCREAM3SND,
                DEATHSCREAM4SND,
                DEATHSCREAM5SND,
                DEATHSCREAM7SND,
                DEATHSCREAM8SND,
                DEATHSCREAM9SND

            };

            PlaySoundLocActor(sounds[US_RndT()%2],ob);

            break;
        }
        case officerobj:
            PlaySoundLocActor(NEINSOVASSND,ob);
            break;
        case ssobj:
            PlaySoundLocActor(LEBENSND,ob); // JAB
            break;
        case dogobj:
            PlaySoundLocActor(DOGDEATHSND,ob);      // JAB
            break;
        case bossobj:
            SD_PlaySound(MUTTISND);                         // JAB
            break;
        case schabbobj:
            SD_PlaySound(MEINGOTTSND);
            break;
        case fakeobj:
            SD_PlaySound(HITLERHASND);
            break;
        case mechahitlerobj:
            SD_PlaySound(SCHEISTSND);
            break;
        case realhitlerobj:
            SD_PlaySound(EVASND);
            break;
        case gretelobj:
            SD_PlaySound(MEINSND);
            break;
        case giftobj:
            SD_PlaySound(DONNERSND);
            break;
        case fatobj:
            SD_PlaySound(ROSESND);
            break;

    }
}


/*
=============================================================================

                            SCHABBS / GIFT / FAT

=============================================================================
*/


/*
===============
=
= SpawnGhosts
=
===============
*/

void SpawnGhosts (int which, int tilex, int tiley)
{
    switch(which)
    {
        case en_blinky:
            SpawnNewObj (tilex,tiley,&s_blinkychase1);
            break;
        case en_clyde:
            SpawnNewObj (tilex,tiley,&s_clydechase1);
            break;
        case en_pinky:
            SpawnNewObj (tilex,tiley,&s_pinkychase1);
            break;
        case en_inky:
            SpawnNewObj (tilex,tiley,&s_inkychase1);
            break;
    }

    newobj->obclass = ghostobj;
    newobj->speed = SPDDOG;

    newobj->dir = east;
    newobj->flags |= FL_AMBUSH;
    if (!loadedgame)
    {
        gamestate.killtotal++;
        gamestate.killcount++;
    }
}



void    T_Gift (objtype *ob);
void    T_GiftThrow (objtype *ob);

void    T_Fat (objtype *ob);
void    T_FatThrow (objtype *ob);

//
// schabb
//
extern  statetype s_schabbstand;

extern  statetype s_schabbchase1;
extern  statetype s_schabbchase1s;
extern  statetype s_schabbchase2;
extern  statetype s_schabbchase3;
extern  statetype s_schabbchase3s;
extern  statetype s_schabbchase4;

extern  statetype s_schabbdie1;
extern  statetype s_schabbdie2;
extern  statetype s_schabbdie3;
extern  statetype s_schabbdie4;
extern  statetype s_schabbdie5;
extern  statetype s_schabbdie6;

extern  statetype s_schabbshoot1;
extern  statetype s_schabbshoot2;

extern  statetype s_needle1;
extern  statetype s_needle2;
extern  statetype s_needle3;
extern  statetype s_needle4;

extern  statetype s_schabbdeathcam;


statetype s_schabbstand         = {false,SPR_SCHABB_W1,0,(statefunc)T_Stand,NULL,&s_schabbstand};

statetype s_schabbchase1        = {false,SPR_SCHABB_W1,10,(statefunc)T_Schabb,NULL,&s_schabbchase1s};
statetype s_schabbchase1s       = {false,SPR_SCHABB_W1,3,NULL,NULL,&s_schabbchase2};
statetype s_schabbchase2        = {false,SPR_SCHABB_W2,8,(statefunc)T_Schabb,NULL,&s_schabbchase3};
statetype s_schabbchase3        = {false,SPR_SCHABB_W3,10,(statefunc)T_Schabb,NULL,&s_schabbchase3s};
statetype s_schabbchase3s       = {false,SPR_SCHABB_W3,3,NULL,NULL,&s_schabbchase4};
statetype s_schabbchase4        = {false,SPR_SCHABB_W4,8,(statefunc)T_Schabb,NULL,&s_schabbchase1};

statetype s_schabbdeathcam      = {false,SPR_SCHABB_W1,1,NULL,NULL,&s_schabbdie1};

statetype s_schabbdie1          = {false,SPR_SCHABB_W1,10,NULL,(statefunc)A_DeathScream,&s_schabbdie2};
statetype s_schabbdie2          = {false,SPR_SCHABB_W1,10,NULL,NULL,&s_schabbdie3};
statetype s_schabbdie3          = {false,SPR_SCHABB_DIE1,10,NULL,NULL,&s_schabbdie4};
statetype s_schabbdie4          = {false,SPR_SCHABB_DIE2,10,NULL,NULL,&s_schabbdie5};
statetype s_schabbdie5          = {false,SPR_SCHABB_DIE3,10,NULL,NULL,&s_schabbdie6};
statetype s_schabbdie6          = {false,SPR_SCHABB_DEAD,20,NULL,(statefunc)A_StartDeathCam,&s_schabbdie6};

statetype s_schabbshoot1        = {false,SPR_SCHABB_SHOOT1,30,NULL,NULL,&s_schabbshoot2};
statetype s_schabbshoot2        = {false,SPR_SCHABB_SHOOT2,10,NULL,(statefunc)T_SchabbThrow,&s_schabbchase1};

statetype s_needle1             = {false,SPR_HYPO1,6,(statefunc)T_Projectile,NULL,&s_needle2};
statetype s_needle2             = {false,SPR_HYPO2,6,(statefunc)T_Projectile,NULL,&s_needle3};
statetype s_needle3             = {false,SPR_HYPO3,6,(statefunc)T_Projectile,NULL,&s_needle4};
statetype s_needle4             = {false,SPR_HYPO4,6,(statefunc)T_Projectile,NULL,&s_needle1};


//
// gift
//
extern  statetype s_giftstand;

extern  statetype s_giftchase1;
extern  statetype s_giftchase1s;
extern  statetype s_giftchase2;
extern  statetype s_giftchase3;
extern  statetype s_giftchase3s;
extern  statetype s_giftchase4;

extern  statetype s_giftdie1;
extern  statetype s_giftdie2;
extern  statetype s_giftdie3;
extern  statetype s_giftdie4;
extern  statetype s_giftdie5;
extern  statetype s_giftdie6;

extern  statetype s_giftshoot1;
extern  statetype s_giftshoot2;

extern  statetype s_needle1;
extern  statetype s_needle2;
extern  statetype s_needle3;
extern  statetype s_needle4;

extern  statetype s_giftdeathcam;

extern  statetype s_boom1;
extern  statetype s_boom2;
extern  statetype s_boom3;


statetype s_giftstand           = {false,SPR_GIFT_W1,0,(statefunc)T_Stand,NULL,&s_giftstand};

statetype s_giftchase1          = {false,SPR_GIFT_W1,10,(statefunc)T_Gift,NULL,&s_giftchase1s};
statetype s_giftchase1s         = {false,SPR_GIFT_W1,3,NULL,NULL,&s_giftchase2};
statetype s_giftchase2          = {false,SPR_GIFT_W2,8,(statefunc)T_Gift,NULL,&s_giftchase3};
statetype s_giftchase3          = {false,SPR_GIFT_W3,10,(statefunc)T_Gift,NULL,&s_giftchase3s};
statetype s_giftchase3s         = {false,SPR_GIFT_W3,3,NULL,NULL,&s_giftchase4};
statetype s_giftchase4          = {false,SPR_GIFT_W4,8,(statefunc)T_Gift,NULL,&s_giftchase1};

statetype s_giftdeathcam        = {false,SPR_GIFT_W1,1,NULL,NULL,&s_giftdie1};

statetype s_giftdie1            = {false,SPR_GIFT_W1,1,NULL,(statefunc)A_DeathScream,&s_giftdie2};
statetype s_giftdie2            = {false,SPR_GIFT_W1,10,NULL,NULL,&s_giftdie3};
statetype s_giftdie3            = {false,SPR_GIFT_DIE1,10,NULL,NULL,&s_giftdie4};
statetype s_giftdie4            = {false,SPR_GIFT_DIE2,10,NULL,NULL,&s_giftdie5};
statetype s_giftdie5            = {false,SPR_GIFT_DIE3,10,NULL,NULL,&s_giftdie6};
statetype s_giftdie6            = {false,SPR_GIFT_DEAD,20,NULL,(statefunc)A_StartDeathCam,&s_giftdie6};

statetype s_giftshoot1          = {false,SPR_GIFT_SHOOT1,30,NULL,NULL,&s_giftshoot2};
statetype s_giftshoot2          = {false,SPR_GIFT_SHOOT2,10,NULL,(statefunc)T_GiftThrow,&s_giftchase1};


//
// fat
//
extern  statetype s_fatstand;

extern  statetype s_fatchase1;
extern  statetype s_fatchase1s;
extern  statetype s_fatchase2;
extern  statetype s_fatchase3;
extern  statetype s_fatchase3s;
extern  statetype s_fatchase4;

extern  statetype s_fatdie1;
extern  statetype s_fatdie2;
extern  statetype s_fatdie3;
extern  statetype s_fatdie4;
extern  statetype s_fatdie5;
extern  statetype s_fatdie6;

extern  statetype s_fatshoot1;
extern  statetype s_fatshoot2;
extern  statetype s_fatshoot3;
extern  statetype s_fatshoot4;
extern  statetype s_fatshoot5;
extern  statetype s_fatshoot6;

extern  statetype s_needle1;
extern  statetype s_needle2;
extern  statetype s_needle3;
extern  statetype s_needle4;

extern  statetype s_fatdeathcam;


statetype s_fatstand            = {false,SPR_FAT_W1,0,(statefunc)T_Stand,NULL,&s_fatstand};

statetype s_fatchase1           = {false,SPR_FAT_W1,10,(statefunc)T_Fat,NULL,&s_fatchase1s};
statetype s_fatchase1s          = {false,SPR_FAT_W1,3,NULL,NULL,&s_fatchase2};
statetype s_fatchase2           = {false,SPR_FAT_W2,8,(statefunc)T_Fat,NULL,&s_fatchase3};
statetype s_fatchase3           = {false,SPR_FAT_W3,10,(statefunc)T_Fat,NULL,&s_fatchase3s};
statetype s_fatchase3s          = {false,SPR_FAT_W3,3,NULL,NULL,&s_fatchase4};
statetype s_fatchase4           = {false,SPR_FAT_W4,8,(statefunc)T_Fat,NULL,&s_fatchase1};

statetype s_fatdeathcam         = {false,SPR_FAT_W1,1,NULL,NULL,&s_fatdie1};

statetype s_fatdie1             = {false,SPR_FAT_W1,1,NULL,(statefunc)A_DeathScream,&s_fatdie2};
statetype s_fatdie2             = {false,SPR_FAT_W1,10,NULL,NULL,&s_fatdie3};
statetype s_fatdie3             = {false,SPR_FAT_DIE1,10,NULL,NULL,&s_fatdie4};
statetype s_fatdie4             = {false,SPR_FAT_DIE2,10,NULL,NULL,&s_fatdie5};
statetype s_fatdie5             = {false,SPR_FAT_DIE3,10,NULL,NULL,&s_fatdie6};
statetype s_fatdie6             = {false,SPR_FAT_DEAD,20,NULL,(statefunc)A_StartDeathCam,&s_fatdie6};

statetype s_fatshoot1           = {false,SPR_FAT_SHOOT1,30,NULL,NULL,&s_fatshoot2};
statetype s_fatshoot2           = {false,SPR_FAT_SHOOT2,10,NULL,(statefunc)T_GiftThrow,&s_fatshoot3};
statetype s_fatshoot3           = {false,SPR_FAT_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_fatshoot4};
statetype s_fatshoot4           = {false,SPR_FAT_SHOOT4,10,NULL,(statefunc)T_Shoot,&s_fatshoot5};
statetype s_fatshoot5           = {false,SPR_FAT_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_fatshoot6};
statetype s_fatshoot6           = {false,SPR_FAT_SHOOT4,10,NULL,(statefunc)T_Shoot,&s_fatchase1};


/*
===============
=
= SpawnSchabbs
=
===============
*/

void SpawnSchabbs (int tilex, int tiley)
{
    if (DigiMode != sds_Off)
        s_schabbdie2.tictime = 140;
    else
        s_schabbdie2.tictime = 5;

    SpawnNewObj (tilex,tiley,&s_schabbstand);
    newobj->speed = SPDPATROL;

    newobj->obclass = schabbobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_schabbs];
    newobj->dir = nodir;
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
    if (!loadedgame)
        gamestate.killtotal++;
}


/*
===============
=
= SpawnGift
=
===============
*/

void SpawnGift (int tilex, int tiley)
{
    if (DigiMode != sds_Off)
        s_giftdie2.tictime = 140;
    else
        s_giftdie2.tictime = 5;

    SpawnNewObj (tilex,tiley,&s_giftstand);
    newobj->speed = SPDPATROL;

    newobj->obclass = giftobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_gift];
    newobj->dir = nodir;
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
    if (!loadedgame)
        gamestate.killtotal++;
}


/*
===============
=
= SpawnFat
=
===============
*/

void SpawnFat (int tilex, int tiley)
{
    if (DigiMode != sds_Off)
        s_fatdie2.tictime = 140;
    else
        s_fatdie2.tictime = 5;

    SpawnNewObj (tilex,tiley,&s_fatstand);
    newobj->speed = SPDPATROL;

    newobj->obclass = fatobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_fat];
    newobj->dir = nodir;
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
    if (!loadedgame)
        gamestate.killtotal++;
}


/*
=================
=
= T_SchabbThrow
=
=================
*/

void T_SchabbThrow (objtype *ob)
{
    int32_t deltax,deltay;
    float   angle;
    int     iangle;

    deltax = player->x - ob->x;
    deltay = ob->y - player->y;
    angle = (float) atan2((float) deltay, (float) deltax);
    if (angle<0)
        angle = (float) (M_PI*2+angle);
    iangle = (int) (angle/(M_PI*2)*ANGLES);

    GetNewActor ();
    newobj->state = &s_needle1;
    newobj->ticcount = 1;

    newobj->tilex = ob->tilex;
    newobj->tiley = ob->tiley;
    newobj->x = ob->x;
    newobj->y = ob->y;
    newobj->obclass = needleobj;
    newobj->dir = nodir;
    newobj->angle = iangle;
    newobj->speed = 0x2000l;

    newobj->flags = FL_NEVERMARK;
    newobj->active = ac_yes;

    PlaySoundLocActor (SCHABBSTHROWSND,newobj);
}

/*
=================
=
= T_GiftThrow
=
=================
*/

void T_GiftThrow (objtype *ob)
{
    int32_t deltax,deltay;
    float   angle;
    int     iangle;

    deltax = player->x - ob->x;
    deltay = ob->y - player->y;
    angle = (float) atan2((float) deltay, (float) deltax);
    if (angle<0)
        angle = (float) (M_PI*2+angle);
    iangle = (int) (angle/(M_PI*2)*ANGLES);

    GetNewActor ();
    newobj->state = &s_rocket;
    newobj->ticcount = 1;

    newobj->tilex = ob->tilex;
    newobj->tiley = ob->tiley;
    newobj->x = ob->x;
    newobj->y = ob->y;
    newobj->obclass = rocketobj;
    newobj->dir = nodir;
    newobj->angle = iangle;
    newobj->speed = 0x2000l;
    newobj->flags = FL_NEVERMARK;
    newobj->active = ac_yes;

    PlaySoundLocActor (MISSILEFIRESND,newobj);

}


/*
=================
=
= T_Schabb
=
=================
*/

void T_Schabb (objtype *ob)
{
    int32_t move;
    int     dx,dy,dist;
    boolean dodge;

    dodge = false;
    dx = abs(ob->tilex - player->tilex);
    dy = abs(ob->tiley - player->tiley);
    dist = dx>dy ? dx : dy;

    if (CheckLine(ob))                                              // got a shot at player?
    {
        ob->hidden = false;
        if ( (unsigned) US_RndT() < (tics<<3) && objfreelist)
        {
            //
            // go into attack frame
            //
            NewState (ob,&s_schabbshoot1);
            return;
        }
        dodge = true;
    }
    else
        ob->hidden = true;

    if (ob->dir == nodir)
    {
        if (dodge)
            SelectDodgeDir (ob);
        else
            SelectChaseDir (ob);
        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }

    move = ob->speed*tics;

    while (move)
    {
        if (ob->distance < 0)
        {
            //
            // waiting for a door to open
            //
            OpenDoor (-ob->distance-1);
            if (doorobjlist[-ob->distance-1].action != dr_open)
                return;
            ob->distance = TILEGLOBAL;      // go ahead, the door is now open
            TryWalk(ob);
        }

        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }

        //
        // reached goal tile, so select another one
        //

        //
        // fix position to account for round off during moving
        //
        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;

        move -= ob->distance;

        if (dist <4)
            SelectRunDir (ob);
        else if (dodge)
            SelectDodgeDir (ob);
        else
            SelectChaseDir (ob);

        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }
}


/*
=================
=
= T_Gift
=
=================
*/

void T_Gift (objtype *ob)
{
    int32_t move;
    int     dx,dy,dist;
    boolean dodge;

    dodge = false;
    dx = abs(ob->tilex - player->tilex);
    dy = abs(ob->tiley - player->tiley);
    dist = dx>dy ? dx : dy;

    if (CheckLine(ob))                                              // got a shot at player?
    {
        ob->hidden = false;
        if ( (unsigned) US_RndT() < (tics<<3) && objfreelist)
        {
            //
            // go into attack frame
            //
            NewState (ob,&s_giftshoot1);
            return;
        }
        dodge = true;
    }
    else
        ob->hidden = true;

    if (ob->dir == nodir)
    {
        if (dodge)
            SelectDodgeDir (ob);
        else
            SelectChaseDir (ob);
        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }

    move = ob->speed*tics;

    while (move)
    {
        if (ob->distance < 0)
        {
            //
            // waiting for a door to open
            //
            OpenDoor (-ob->distance-1);
            if (doorobjlist[-ob->distance-1].action != dr_open)
                return;
            ob->distance = TILEGLOBAL;      // go ahead, the door is now open
            TryWalk(ob);
        }

        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }

        //
        // reached goal tile, so select another one
        //

        //
        // fix position to account for round off during moving
        //
        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;

        move -= ob->distance;

        if (dist <4)
            SelectRunDir (ob);
        else if (dodge)
            SelectDodgeDir (ob);
        else
            SelectChaseDir (ob);

        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }
}


/*
=================
=
= T_Fat
=
=================
*/

void T_Fat (objtype *ob)
{
    int32_t move;
    int     dx,dy,dist;
    boolean dodge;

    dodge = false;
    dx = abs(ob->tilex - player->tilex);
    dy = abs(ob->tiley - player->tiley);
    dist = dx>dy ? dx : dy;

    if (CheckLine(ob))                                              // got a shot at player?
    {
        ob->hidden = false;
        if ( (unsigned) US_RndT() < (tics<<3) && objfreelist)
        {
            //
            // go into attack frame
            //
            NewState (ob,&s_fatshoot1);
            return;
        }
        dodge = true;
    }
    else
        ob->hidden = true;

    if (ob->dir == nodir)
    {
        if (dodge)
            SelectDodgeDir (ob);
        else
            SelectChaseDir (ob);
        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }

    move = ob->speed*tics;

    while (move)
    {
        if (ob->distance < 0)
        {
            //
            // waiting for a door to open
            //
            OpenDoor (-ob->distance-1);
            if (doorobjlist[-ob->distance-1].action != dr_open)
                return;
            ob->distance = TILEGLOBAL;      // go ahead, the door is now open
            TryWalk(ob);
        }

        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }

        //
        // reached goal tile, so select another one
        //

        //
        // fix position to account for round off during moving
        //
        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;

        move -= ob->distance;

        if (dist <4)
            SelectRunDir (ob);
        else if (dodge)
            SelectDodgeDir (ob);
        else
            SelectChaseDir (ob);

        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }
}


/*
=============================================================================

                                    HITLERS

=============================================================================
*/


//
// fake
//
extern  statetype s_fakestand;

extern  statetype s_fakechase1;
extern  statetype s_fakechase1s;
extern  statetype s_fakechase2;
extern  statetype s_fakechase3;
extern  statetype s_fakechase3s;
extern  statetype s_fakechase4;

extern  statetype s_fakedie1;
extern  statetype s_fakedie2;
extern  statetype s_fakedie3;
extern  statetype s_fakedie4;
extern  statetype s_fakedie5;
extern  statetype s_fakedie6;

extern  statetype s_fakeshoot1;
extern  statetype s_fakeshoot2;
extern  statetype s_fakeshoot3;
extern  statetype s_fakeshoot4;
extern  statetype s_fakeshoot5;
extern  statetype s_fakeshoot6;
extern  statetype s_fakeshoot7;
extern  statetype s_fakeshoot8;
extern  statetype s_fakeshoot9;

extern  statetype s_fire1;
extern  statetype s_fire2;

statetype s_fakestand           = {false,SPR_FAKE_W1,0,(statefunc)T_Stand,NULL,&s_fakestand};

statetype s_fakechase1          = {false,SPR_FAKE_W1,10,(statefunc)T_Fake,NULL,&s_fakechase1s};
statetype s_fakechase1s         = {false,SPR_FAKE_W1,3,NULL,NULL,&s_fakechase2};
statetype s_fakechase2          = {false,SPR_FAKE_W2,8,(statefunc)T_Fake,NULL,&s_fakechase3};
statetype s_fakechase3          = {false,SPR_FAKE_W3,10,(statefunc)T_Fake,NULL,&s_fakechase3s};
statetype s_fakechase3s         = {false,SPR_FAKE_W3,3,NULL,NULL,&s_fakechase4};
statetype s_fakechase4          = {false,SPR_FAKE_W4,8,(statefunc)T_Fake,NULL,&s_fakechase1};

statetype s_fakedie1            = {false,SPR_FAKE_DIE1,10,NULL,(statefunc)A_DeathScream,&s_fakedie2};
statetype s_fakedie2            = {false,SPR_FAKE_DIE2,10,NULL,NULL,&s_fakedie3};
statetype s_fakedie3            = {false,SPR_FAKE_DIE3,10,NULL,NULL,&s_fakedie4};
statetype s_fakedie4            = {false,SPR_FAKE_DIE4,10,NULL,NULL,&s_fakedie5};
statetype s_fakedie5            = {false,SPR_FAKE_DIE5,10,NULL,NULL,&s_fakedie6};
statetype s_fakedie6            = {false,SPR_FAKE_DEAD,0,NULL,NULL,&s_fakedie6};

statetype s_fakeshoot1          = {false,SPR_FAKE_SHOOT,8,NULL,(statefunc)T_FakeFire,&s_fakeshoot2};
statetype s_fakeshoot2          = {false,SPR_FAKE_SHOOT,8,NULL,(statefunc)T_FakeFire,&s_fakeshoot3};
statetype s_fakeshoot3          = {false,SPR_FAKE_SHOOT,8,NULL,(statefunc)T_FakeFire,&s_fakeshoot4};
statetype s_fakeshoot4          = {false,SPR_FAKE_SHOOT,8,NULL,(statefunc)T_FakeFire,&s_fakeshoot5};
statetype s_fakeshoot5          = {false,SPR_FAKE_SHOOT,8,NULL,(statefunc)T_FakeFire,&s_fakeshoot6};
statetype s_fakeshoot6          = {false,SPR_FAKE_SHOOT,8,NULL,(statefunc)T_FakeFire,&s_fakeshoot7};
statetype s_fakeshoot7          = {false,SPR_FAKE_SHOOT,8,NULL,(statefunc)T_FakeFire,&s_fakeshoot8};
statetype s_fakeshoot8          = {false,SPR_FAKE_SHOOT,8,NULL,(statefunc)T_FakeFire,&s_fakeshoot9};
statetype s_fakeshoot9          = {false,SPR_FAKE_SHOOT,8,NULL,NULL,&s_fakechase1};

statetype s_fire1               = {false,SPR_FIRE1,6,NULL,(statefunc)T_Projectile,&s_fire2};
statetype s_fire2               = {false,SPR_FIRE2,6,NULL,(statefunc)T_Projectile,&s_fire1};

//
// hitler
//
extern  statetype s_mechachase1;
extern  statetype s_mechachase1s;
extern  statetype s_mechachase2;
extern  statetype s_mechachase3;
extern  statetype s_mechachase3s;
extern  statetype s_mechachase4;

extern  statetype s_mechadie1;
extern  statetype s_mechadie2;
extern  statetype s_mechadie3;
extern  statetype s_mechadie4;

extern  statetype s_mechashoot1;
extern  statetype s_mechashoot2;
extern  statetype s_mechashoot3;
extern  statetype s_mechashoot4;
extern  statetype s_mechashoot5;
extern  statetype s_mechashoot6;


extern  statetype s_hitlerchase1;
extern  statetype s_hitlerchase1s;
extern  statetype s_hitlerchase2;
extern  statetype s_hitlerchase3;
extern  statetype s_hitlerchase3s;
extern  statetype s_hitlerchase4;

extern  statetype s_hitlerdie1;
extern  statetype s_hitlerdie2;
extern  statetype s_hitlerdie3;
extern  statetype s_hitlerdie4;
extern  statetype s_hitlerdie5;
extern  statetype s_hitlerdie6;
extern  statetype s_hitlerdie7;
extern  statetype s_hitlerdie8;
extern  statetype s_hitlerdie9;
extern  statetype s_hitlerdie10;

extern  statetype s_hitlershoot1;
extern  statetype s_hitlershoot2;
extern  statetype s_hitlershoot3;
extern  statetype s_hitlershoot4;
extern  statetype s_hitlershoot5;
extern  statetype s_hitlershoot6;

extern  statetype s_hitlerdeathcam;

statetype s_mechastand          = {false,SPR_MECHA_W1,0,(statefunc)T_Stand,NULL,&s_mechastand};

statetype s_mechachase1         = {false,SPR_MECHA_W1,10,(statefunc)T_Chase,(statefunc)A_MechaSound,&s_mechachase1s};
statetype s_mechachase1s        = {false,SPR_MECHA_W1,6,NULL,NULL,&s_mechachase2};
statetype s_mechachase2         = {false,SPR_MECHA_W2,8,(statefunc)T_Chase,NULL,&s_mechachase3};
statetype s_mechachase3         = {false,SPR_MECHA_W3,10,(statefunc)T_Chase,(statefunc)A_MechaSound,&s_mechachase3s};
statetype s_mechachase3s        = {false,SPR_MECHA_W3,6,NULL,NULL,&s_mechachase4};
statetype s_mechachase4         = {false,SPR_MECHA_W4,8,(statefunc)T_Chase,NULL,&s_mechachase1};

statetype s_mechadie1           = {false,SPR_MECHA_DIE1,10,NULL,(statefunc)A_DeathScream,&s_mechadie2};
statetype s_mechadie2           = {false,SPR_MECHA_DIE2,10,NULL,NULL,&s_mechadie3};
statetype s_mechadie3           = {false,SPR_MECHA_DIE3,10,NULL,(statefunc)A_HitlerMorph,&s_mechadie4};
statetype s_mechadie4           = {false,SPR_MECHA_DEAD,0,NULL,NULL,&s_mechadie4};

statetype s_mechashoot1         = {false,SPR_MECHA_SHOOT1,30,NULL,NULL,&s_mechashoot2};
statetype s_mechashoot2         = {false,SPR_MECHA_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_mechashoot3};
statetype s_mechashoot3         = {false,SPR_MECHA_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_mechashoot4};
statetype s_mechashoot4         = {false,SPR_MECHA_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_mechashoot5};
statetype s_mechashoot5         = {false,SPR_MECHA_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_mechashoot6};
statetype s_mechashoot6         = {false,SPR_MECHA_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_mechachase1};


statetype s_hitlerchase1        = {false,SPR_HITLER_W1,6,(statefunc)T_Chase,NULL,&s_hitlerchase1s};
statetype s_hitlerchase1s       = {false,SPR_HITLER_W1,4,NULL,NULL,&s_hitlerchase2};
statetype s_hitlerchase2        = {false,SPR_HITLER_W2,2,(statefunc)T_Chase,NULL,&s_hitlerchase3};
statetype s_hitlerchase3        = {false,SPR_HITLER_W3,6,(statefunc)T_Chase,NULL,&s_hitlerchase3s};
statetype s_hitlerchase3s       = {false,SPR_HITLER_W3,4,NULL,NULL,&s_hitlerchase4};
statetype s_hitlerchase4        = {false,SPR_HITLER_W4,2,(statefunc)T_Chase,NULL,&s_hitlerchase1};

statetype s_hitlerdeathcam      = {false,SPR_HITLER_W1,10,NULL,NULL,&s_hitlerdie1};

statetype s_hitlerdie1          = {false,SPR_HITLER_W1,1,NULL,(statefunc)A_DeathScream,&s_hitlerdie2};
statetype s_hitlerdie2          = {false,SPR_HITLER_W1,10,NULL,NULL,&s_hitlerdie3};
statetype s_hitlerdie3          = {false,SPR_HITLER_DIE1,10,NULL,(statefunc)A_Slurpie,&s_hitlerdie4};
statetype s_hitlerdie4          = {false,SPR_HITLER_DIE2,10,NULL,NULL,&s_hitlerdie5};
statetype s_hitlerdie5          = {false,SPR_HITLER_DIE3,10,NULL,NULL,&s_hitlerdie6};
statetype s_hitlerdie6          = {false,SPR_HITLER_DIE4,10,NULL,NULL,&s_hitlerdie7};
statetype s_hitlerdie7          = {false,SPR_HITLER_DIE5,10,NULL,NULL,&s_hitlerdie8};
statetype s_hitlerdie8          = {false,SPR_HITLER_DIE6,10,NULL,NULL,&s_hitlerdie9};
statetype s_hitlerdie9          = {false,SPR_HITLER_DIE7,10,NULL,NULL,&s_hitlerdie10};
statetype s_hitlerdie10         = {false,SPR_HITLER_DEAD,20,NULL,(statefunc)A_StartDeathCam,&s_hitlerdie10};

statetype s_hitlershoot1        = {false,SPR_HITLER_SHOOT1,30,NULL,NULL,&s_hitlershoot2};
statetype s_hitlershoot2        = {false,SPR_HITLER_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_hitlershoot3};
statetype s_hitlershoot3        = {false,SPR_HITLER_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_hitlershoot4};
statetype s_hitlershoot4        = {false,SPR_HITLER_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_hitlershoot5};
statetype s_hitlershoot5        = {false,SPR_HITLER_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_hitlershoot6};
statetype s_hitlershoot6        = {false,SPR_HITLER_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_hitlerchase1};



/*
===============
=
= SpawnFakeHitler
=
===============
*/

void SpawnFakeHitler (int tilex, int tiley)
{
    if (DigiMode != sds_Off)
        s_hitlerdie2.tictime = 140;
    else
        s_hitlerdie2.tictime = 5;

    SpawnNewObj (tilex,tiley,&s_fakestand);
    newobj->speed = SPDPATROL;

    newobj->obclass = fakeobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_fake];
    newobj->dir = nodir;
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
    if (!loadedgame)
        gamestate.killtotal++;
}


/*
===============
=
= SpawnHitler
=
===============
*/

void SpawnHitler (int tilex, int tiley)
{
    if (DigiMode != sds_Off)
        s_hitlerdie2.tictime = 140;
    else
        s_hitlerdie2.tictime = 5;


    SpawnNewObj (tilex,tiley,&s_mechastand);
    newobj->speed = SPDPATROL;

    newobj->obclass = mechahitlerobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_hitler];
    newobj->dir = nodir;
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
    if (!loadedgame)
        gamestate.killtotal++;
}


/*
===============
=
= A_HitlerMorph
=
===============
*/

void A_HitlerMorph (objtype *ob)
{
    short hitpoints[4]={500,700,800,900};

    SpawnNewObj (ob->tilex,ob->tiley,&s_hitlerchase1);
    newobj->speed = SPDPATROL*5;

    newobj->x = ob->x;
    newobj->y = ob->y;

    newobj->distance = ob->distance;
    newobj->dir = ob->dir;
    newobj->flags = ob->flags | FL_SHOOTABLE;
    newobj->flags &= ~FL_NONMARK;   // hitler stuck with nodir fix

    newobj->obclass = realhitlerobj;
    newobj->hitpoints = hitpoints[gamestate.difficulty];
}


////////////////////////////////////////////////////////
//
// A_MechaSound
// A_Slurpie
//
////////////////////////////////////////////////////////
void A_MechaSound (objtype *ob)
{
    if (areabyplayer[ob->areanumber])
        PlaySoundLocActor (MECHSTEPSND,ob);
}

void A_Slurpie (objtype *)
{
    SD_PlaySound(SLURPIESND);
}

/*
=================
=
= T_FakeFire
=
=================
*/

void T_FakeFire (objtype *ob)
{
    int32_t deltax,deltay;
    float   angle;
    int     iangle;

    if (!objfreelist)       // stop shooting if over MAXACTORS
    {
        NewState (ob,&s_fakechase1);
        return;
    }

    deltax = player->x - ob->x;
    deltay = ob->y - player->y;
    angle = (float) atan2((float) deltay, (float) deltax);
    if (angle<0)
        angle = (float)(M_PI*2+angle);
    iangle = (int) (angle/(M_PI*2)*ANGLES);

    GetNewActor ();
    newobj->state = &s_fire1;
    newobj->ticcount = 1;

    newobj->tilex = ob->tilex;
    newobj->tiley = ob->tiley;
    newobj->x = ob->x;
    newobj->y = ob->y;
    newobj->dir = nodir;
    newobj->angle = iangle;
    newobj->obclass = fireobj;
    newobj->speed = 0x1200l;
    newobj->flags = FL_NEVERMARK;
    newobj->active = ac_yes;

    PlaySoundLocActor (FLAMETHROWERSND,newobj);
}



/*
=================
=
= T_Fake
=
=================
*/

void T_Fake (objtype *ob)
{
    int32_t move;

    if (CheckLine(ob))                      // got a shot at player?
    {
        ob->hidden = false;
        if ( (unsigned) US_RndT() < (tics<<1) && objfreelist)
        {
            //
            // go into attack frame
            //
            NewState (ob,&s_fakeshoot1);
            return;
        }
    }
    else
        ob->hidden = true;

    if (ob->dir == nodir)
    {
        SelectDodgeDir (ob);
        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }

    move = ob->speed*tics;

    while (move)
    {
        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }

        //
        // reached goal tile, so select another one
        //

        //
        // fix position to account for round off during moving
        //
        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;

        move -= ob->distance;

        SelectDodgeDir (ob);

        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }
}


/*
============================================================================

STAND

============================================================================
*/


/*
===============
=
= T_Stand
=
===============
*/

void T_Stand (objtype *ob)
{
    SightPlayer (ob);
}


/*
============================================================================

CHASE

============================================================================
*/

/*
=================
=
= T_Chase
=
=================
*/

void T_Chase (objtype *ob)
{
    int32_t move,target;
    int     dx,dy,dist,chance;
    boolean dodge;

    if (gamestate.victoryflag)
        return;

    dodge = false;
    if (CheckLine(ob))      // got a shot at player?
    {
        ob->hidden = false;
        dx = abs(ob->tilex - player->tilex);
        dy = abs(ob->tiley - player->tiley);
        dist = dx>dy ? dx : dy;

#ifdef PLAYDEMOLIKEORIGINAL
        if(DEMOCOND_ORIG)
        {
            if(!dist || (dist == 1 && ob->distance < 0x4000))
                chance = 300;
            else
                chance = (tics<<4)/dist;
        }
        else
#endif
        {
            if (dist)
                chance = (tics<<4)/dist;
            else
                chance = 300;

            if (dist == 1)
            {
                target = abs(ob->x - player->x);
                if (target < 0x14000l)
                {
                    target = abs(ob->y - player->y);
                    if (target < 0x14000l)
                        chance = 300;
                }
            }
        }

        if ( US_RndT()<chance)
        {
            //
            // go into attack frame
            //
            switch (ob->obclass)
            {
                case guardobj:
                    NewState (ob,&s_grdshoot1);
                    break;
                case officerobj:
                    NewState (ob,&s_ofcshoot1);
                    break;
                case mutantobj:
                    NewState (ob,&s_mutshoot1);
                    break;
                case ssobj:
                    NewState (ob,&s_ssshoot1);
                    break;
                case bossobj:
                    NewState (ob,&s_bossshoot1);
                    break;
                case gretelobj:
                    NewState (ob,&s_gretelshoot1);
                    break;
                case mechahitlerobj:
                    NewState (ob,&s_mechashoot1);
                    break;
                case realhitlerobj:
                    NewState (ob,&s_hitlershoot1);
                    break;
            }
            return;
        }
        dodge = true;
    }
    else
        ob->hidden = true;

    if (ob->dir == nodir)
    {
        if (dodge)
            SelectDodgeDir (ob);
        else
            SelectChaseDir (ob);
        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }

    move = ob->speed*tics;

    while (move)
    {
        if (ob->distance < 0)
        {
            //
            // waiting for a door to open
            //
            OpenDoor (-ob->distance-1);
            if (doorobjlist[-ob->distance-1].action != dr_open)
                return;
            ob->distance = TILEGLOBAL;      // go ahead, the door is now open
            DEMOIF_SDL
            {
                TryWalk(ob);
            }
        }

        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }

        //
        // reached goal tile, so select another one
        //

        //
        // fix position to account for round off during moving
        //
        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;

        move -= ob->distance;

        if (dodge)
            SelectDodgeDir (ob);
        else
            SelectChaseDir (ob);

        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }
}


/*
=================
=
= T_Ghosts
=
=================
*/

void T_Ghosts (objtype *ob)
{
    int32_t move;

    if (ob->dir == nodir)
    {
        SelectChaseDir (ob);
        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }

    move = ob->speed*tics;

    while (move)
    {
        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }

        //
        // reached goal tile, so select another one
        //

        //
        // fix position to account for round off during moving
        //
        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;

        move -= ob->distance;

        SelectChaseDir (ob);

        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }
}

/*
=================
=
= T_DogChase
=
=================
*/

void T_DogChase (objtype *ob)
{
    int32_t    move;
    int32_t    dx,dy;


    if (ob->dir == nodir)
    {
        SelectDodgeDir (ob);
        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }

    move = ob->speed*tics;

    while (move)
    {
        //
        // check for byte range
        //
        dx = player->x - ob->x;
        if (dx<0)
            dx = -dx;
        dx -= move;
        if (dx <= MINACTORDIST)
        {
            dy = player->y - ob->y;
            if (dy<0)
                dy = -dy;
            dy -= move;
            if (dy <= MINACTORDIST)
            {
                NewState (ob,&s_dogjump1);
                return;
            }
        }

        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }

        //
        // reached goal tile, so select another one
        //

        //
        // fix position to account for round off during moving
        //
        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;

        move -= ob->distance;

        SelectDodgeDir (ob);

        if (ob->dir == nodir)
            return;                                                 // object is blocked in
    }
}



/*
============================================================================

                                    PATH

============================================================================
*/


/*
===============
=
= SelectPathDir
=
===============
*/

void SelectPathDir (objtype *ob)
{
    unsigned spot;

    spot = MAPSPOT(ob->tilex,ob->tiley,1)-ICONARROWS;

    if (spot<8)
    {
        // new direction
        ob->dir = (dirtype)(spot);
    }

    ob->distance = TILEGLOBAL;

    if (!TryWalk (ob))
        ob->dir = nodir;
}


/*
===============
=
= T_Path
=
===============
*/

void T_Path (objtype *ob)
{
    int32_t    move;

    if (SightPlayer (ob))
        return;

    if (ob->dir == nodir)
    {
        SelectPathDir (ob);
        if (ob->dir == nodir)
            return;                                 // all movement is blocked
    }


    move = ob->speed*tics;

    while (move)
    {
        if (ob->distance < 0)
        {
            //
            // waiting for a door to open
            //
            OpenDoor (-ob->distance-1);
            if (doorobjlist[-ob->distance-1].action != dr_open)
                return;
            ob->distance = TILEGLOBAL;      // go ahead, the door is now open
            DEMOIF_SDL
            {
                TryWalk(ob);
            }
        }

        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }

        if (ob->tilex>MAPSIZE || ob->tiley>MAPSIZE)
        {
            sprintf (str, "T_Path hit a wall at %u,%u, dir %u",
                ob->tilex,ob->tiley,ob->dir);
            Quit (str);
        }

        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;
        move -= ob->distance;

        SelectPathDir (ob);

        if (ob->dir == nodir)
            return;                                 // all movement is blocked
    }
}


/*
=============================================================================

                                    FIGHT

=============================================================================
*/


/*
===============
=
= T_Shoot
=
= Try to damage the player, based on skill level and player's speed
=
===============
*/

void T_Shoot (objtype *ob)
{
    int     dx,dy,dist;
    int     hitchance,damage;

    hitchance = 128;

    if (!areabyplayer[ob->areanumber])
        return;

    if (CheckLine (ob))                    // player is not behind a wall
    {
        dx = abs(ob->tilex - player->tilex);
        dy = abs(ob->tiley - player->tiley);
        dist = dx>dy ? dx:dy;

        if (ob->obclass == ssobj || ob->obclass == bossobj)
            dist = dist*2/3;                                        // ss are better shots

        if (thrustspeed >= RUNSPEED)
        {
            if (ob->flags&FL_VISABLE)
                hitchance = 160-dist*16;                // player can see to dodge
            else
                hitchance = 160-dist*8;
        }
        else
        {
            if (ob->flags&FL_VISABLE)
                hitchance = 256-dist*16;                // player can see to dodge
            else
                hitchance = 256-dist*8;
        }

        // see if the shot was a hit

        if (US_RndT()<hitchance)
        {
            if (dist<2)
                damage = US_RndT()>>2;
            else if (dist<4)
                damage = US_RndT()>>3;
            else
                damage = US_RndT()>>4;

            TakeDamage (damage,ob);
        }
    }

    switch(ob->obclass)
    {
        case ssobj:
            PlaySoundLocActor(SSFIRESND,ob);
            break;
        case giftobj:
        case fatobj:
            PlaySoundLocActor(MISSILEFIRESND,ob);
            break;
        case mechahitlerobj:
        case realhitlerobj:
        case bossobj:
            PlaySoundLocActor(BOSSFIRESND,ob);
            break;
        case schabbobj:
            PlaySoundLocActor(SCHABBSTHROWSND,ob);
            break;
        case fakeobj:
            PlaySoundLocActor(FLAMETHROWERSND,ob);
            break;
        default:
            PlaySoundLocActor(NAZIFIRESND,ob);
    }
}


/*
===============
=
= T_Bite
=
===============
*/

void T_Bite (objtype *ob)
{
    int32_t    dx,dy;

    PlaySoundLocActor(DOGATTACKSND,ob);     // JAB

    dx = player->x - ob->x;
    if (dx<0)
        dx = -dx;
    dx -= TILEGLOBAL;
    if (dx <= MINACTORDIST)
    {
        dy = player->y - ob->y;
        if (dy<0)
            dy = -dy;
        dy -= TILEGLOBAL;
        if (dy <= MINACTORDIST)
        {
            if (US_RndT()<180)
            {
                TakeDamage (US_RndT()>>4,ob);
                return;
            }
        }
    }
}


/*
============================================================================

                                    BJ VICTORY

============================================================================
*/


//
// BJ victory
//

void T_BJRun (objtype *ob);
void T_BJJump (objtype *ob);
void T_BJDone (objtype *ob);
void T_BJYell (objtype *ob);

void T_DeathCam (objtype *ob);

extern  statetype s_bjrun1;
extern  statetype s_bjrun1s;
extern  statetype s_bjrun2;
extern  statetype s_bjrun3;
extern  statetype s_bjrun3s;
extern  statetype s_bjrun4;

extern  statetype s_bjjump1;
extern  statetype s_bjjump2;
extern  statetype s_bjjump3;
extern  statetype s_bjjump4;


statetype s_bjrun1              = {false,SPR_BJ_W1,12,(statefunc)T_BJRun,NULL,&s_bjrun1s};
statetype s_bjrun1s             = {false,SPR_BJ_W1,3, NULL,NULL,&s_bjrun2};
statetype s_bjrun2              = {false,SPR_BJ_W2,8,(statefunc)T_BJRun,NULL,&s_bjrun3};
statetype s_bjrun3              = {false,SPR_BJ_W3,12,(statefunc)T_BJRun,NULL,&s_bjrun3s};
statetype s_bjrun3s             = {false,SPR_BJ_W3,3, NULL,NULL,&s_bjrun4};
statetype s_bjrun4              = {false,SPR_BJ_W4,8,(statefunc)T_BJRun,NULL,&s_bjrun1};


statetype s_bjjump1             = {false,SPR_BJ_JUMP1,14,(statefunc)T_BJJump,NULL,&s_bjjump2};
statetype s_bjjump2             = {false,SPR_BJ_JUMP2,14,(statefunc)T_BJJump,(statefunc)T_BJYell,&s_bjjump3};
statetype s_bjjump3             = {false,SPR_BJ_JUMP3,14,(statefunc)T_BJJump,NULL,&s_bjjump4};
statetype s_bjjump4             = {false,SPR_BJ_JUMP4,300,NULL,(statefunc)T_BJDone,&s_bjjump4};


statetype s_deathcam            = {false,0,0,NULL,NULL,NULL};


/*
===============
=
= SpawnBJVictory
=
===============
*/

void SpawnBJVictory (void)
{
    SpawnNewObj (player->tilex,player->tiley+1,&s_bjrun1);
    newobj->x = player->x;
    newobj->y = player->y;
    newobj->obclass = bjobj;
    newobj->dir = north;
    newobj->temp1 = 6;                      // tiles to run forward
}



/*
===============
=
= T_BJRun
=
===============
*/

void T_BJRun (objtype *ob)
{
    int32_t    move;

    move = BJRUNSPEED*tics;

    while (move)
    {
        if (move < ob->distance)
        {
            MoveObj (ob,move);
            break;
        }


        ob->x = ((int32_t)ob->tilex<<TILESHIFT)+TILEGLOBAL/2;
        ob->y = ((int32_t)ob->tiley<<TILESHIFT)+TILEGLOBAL/2;
        move -= ob->distance;

        SelectPathDir (ob);

        if ( !(--ob->temp1) )
        {
            NewState (ob,&s_bjjump1);
            return;
        }
    }
}


/*
===============
=
= T_BJJump
=
===============
*/

void T_BJJump (objtype *ob)
{
    int32_t    move;

    move = BJJUMPSPEED*tics;
    MoveObj (ob,move);
}


/*
===============
=
= T_BJYell
=
===============
*/

void T_BJYell (objtype *ob)
{
    PlaySoundLocActor(YEAHSND,ob);  // JAB
}


/*
===============
=
= T_BJDone
=
===============
*/

void T_BJDone (objtype *)
{
    playstate = ex_victorious;                              // exit castle tile
}



//===========================================================================


/*
===============
=
= CheckPosition
=
===============
*/

boolean CheckPosition (objtype *ob)
{
    int     x,y,xl,yl,xh,yh;
    objtype *check;

    xl = (ob->x-PLAYERSIZE) >> TILESHIFT;
    yl = (ob->y-PLAYERSIZE) >> TILESHIFT;

    xh = (ob->x+PLAYERSIZE) >> TILESHIFT;
    yh = (ob->y+PLAYERSIZE) >> TILESHIFT;

    //
    // check for solid walls
    //
    for (y=yl;y<=yh;y++)
    {
        for (x=xl;x<=xh;x++)
        {
            check = actorat[x][y];
            if (check && !ISPOINTER(check))
                return false;
        }
    }

    return true;
}


/*
===============
=
= A_StartDeathCam
=
===============
*/

void    A_StartDeathCam (objtype *ob)
{
    int32_t dx,dy;
    float   fangle;
    int32_t xmove,ymove;
    int32_t dist;

    FinishPaletteShifts ();

    DelayMilliseconds (100*8);

    if (gamestate.victoryflag)
    {
        playstate = ex_victorious;                              // exit castle tile
        return;
    }

    if(usedoublebuffering) VH_UpdateScreen();

    gamestate.victoryflag = true;
    unsigned fadeheight = viewsize != 21 ? screenHeight-scaleFactor*STATUSLINES : screenHeight;
    VL_BarScaledCoord (0, 0, screenWidth, fadeheight, bordercol);
    FizzleFade(0, 0, screenWidth, fadeheight, 70, false);

    if (bordercol != VIEWCOLOR)
    {
        CA_CacheGrChunk (STARTFONT+1);
        GetFontData()->fontnumber = 1;
        //SetFontNumber(1);
        SETFONTCOLOR(15,bordercol);
        PrintX = 68; PrintY = 45;
        US_Print (STR_SEEAGAIN);
        ClearGrSegs(STARTFONT+1);
    }
    else
    {
        CacheLump(LEVELEND_LUMP_START,LEVELEND_LUMP_END);
        Write(0,7,STR_SEEAGAIN);

    }

    VW_UpdateScreen ();
    if(usedoublebuffering) VH_UpdateScreen();

    IN_UserInput(300);

    //
    // line angle up exactly
    //
    NewState (player,&s_deathcam);

    player->x = gamestate.killx;
    player->y = gamestate.killy;

    dx = ob->x - player->x;
    dy = player->y - ob->y;

    fangle = (float) atan2((float) dy, (float) dx);          // returns -pi to pi
    if (fangle<0)
        fangle = (float) (M_PI*2+fangle);

    player->angle = (short) (fangle/(M_PI*2)*ANGLES);

    //
    // try to position as close as possible without being in a wall
    //
    dist = 0x14000l;
    do
    {
        xmove = FixedMul(dist,costable[player->angle]);
        ymove = -FixedMul(dist,sintable[player->angle]);

        player->x = ob->x - xmove;
        player->y = ob->y - ymove;
        dist += 0x1000;

    } while (!CheckPosition (player));
    plux = (word)(player->x >> UNSIGNEDSHIFT);                      // scale to fit in unsigned
    pluy = (word)(player->y >> UNSIGNEDSHIFT);
    player->tilex = (word)(player->x >> TILESHIFT);         // scale to tile values
    player->tiley = (word)(player->y >> TILESHIFT);

    //
    // go back to the game
    //

    DrawPlayBorder ();

    fizzlein = true;

    switch (ob->obclass)
    {
        case schabbobj:
            NewState (ob,&s_schabbdeathcam);
            break;
        case realhitlerobj:
            NewState (ob,&s_hitlerdeathcam);
            break;
        case giftobj:
            NewState (ob,&s_giftdeathcam);
            break;
        case fatobj:
            NewState (ob,&s_fatdeathcam);
            break;
    }
}


