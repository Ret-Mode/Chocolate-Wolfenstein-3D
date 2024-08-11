// WL_ACT2.C

#include <stdio.h>
#include <math.h>
#include "wl_def.h"
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

extern  statetype s_hrocket;
extern  statetype s_hsmoke1;
extern  statetype s_hsmoke2;
extern  statetype s_hsmoke3;
extern  statetype s_hsmoke4;
extern  statetype s_hboom2;
extern  statetype s_hboom3;

void A_Smoke (objtype *ob);

statetype s_hrocket             = {true,SPR_HROCKET_1,3,(statefunc)T_Projectile,(statefunc)A_Smoke,&s_hrocket};
statetype s_hsmoke1             = {false,SPR_HSMOKE_1,3,NULL,NULL,&s_hsmoke2};
statetype s_hsmoke2             = {false,SPR_HSMOKE_2,3,NULL,NULL,&s_hsmoke3};
statetype s_hsmoke3             = {false,SPR_HSMOKE_3,3,NULL,NULL,&s_hsmoke4};
statetype s_hsmoke4             = {false,SPR_HSMOKE_4,3,NULL,NULL,NULL};

statetype s_hboom1              = {false,SPR_HBOOM_1,6,NULL,NULL,&s_hboom2};
statetype s_hboom2              = {false,SPR_HBOOM_2,6,NULL,NULL,&s_hboom3};
statetype s_hboom3              = {false,SPR_HBOOM_3,6,NULL,NULL,NULL};

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
    if (ob->obclass == hrocketobj)
        newobj->state = &s_hsmoke1;
    else

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
         // actually the whole method is never reached in shareware 1.0
        if (ob->obclass == rocketobj)
        {
            PlaySoundLocActor(MISSILEHITSND,ob);
            ob->state = &s_boom1;
        }

        else if (ob->obclass == hrocketobj)
        {
            PlaySoundLocActor(MISSILEHITSND,ob);
            ob->state = &s_hboom1;
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

    if ((mapon==18 || mapon==19) && !US_RndT())
    {
        switch(ob->obclass)
        {
            case mutantobj:
            case guardobj:
            case officerobj:
            case ssobj:
            case dogobj:
                PlaySoundLocActor(DEATHSCREAM6SND,ob);
                return;
        }
    }


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

            PlaySoundLocActor(sounds[US_RndT()%8],ob);

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
        case spectreobj:
            SD_PlaySound(GHOSTFADESND);
            break;
        case angelobj:
            SD_PlaySound(ANGELDEATHSND);
            break;
        case transobj:
            SD_PlaySound(TRANSDEATHSND);
            break;
        case uberobj:
            SD_PlaySound(UBERDEATHSND);
            break;
        case willobj:
            SD_PlaySound(WILHELMDEATHSND);
            break;
        case deathobj:
            SD_PlaySound(KNIGHTDEATHSND);
            break;

    }
}


/*
=============================================================================

                                SPEAR ACTORS

=============================================================================
*/

void T_Launch (objtype *ob);
void T_Will (objtype *ob);

extern  statetype s_angelshoot1;
extern  statetype s_deathshoot1;
extern  statetype s_spark1;

//
// trans
//
extern  statetype s_transstand;

extern  statetype s_transchase1;
extern  statetype s_transchase1s;
extern  statetype s_transchase2;
extern  statetype s_transchase3;
extern  statetype s_transchase3s;
extern  statetype s_transchase4;

extern  statetype s_transdie0;
extern  statetype s_transdie01;
extern  statetype s_transdie1;
extern  statetype s_transdie2;
extern  statetype s_transdie3;
extern  statetype s_transdie4;

extern  statetype s_transshoot1;
extern  statetype s_transshoot2;
extern  statetype s_transshoot3;
extern  statetype s_transshoot4;
extern  statetype s_transshoot5;
extern  statetype s_transshoot6;
extern  statetype s_transshoot7;
extern  statetype s_transshoot8;


statetype s_transstand          = {false,SPR_TRANS_W1,0,(statefunc)T_Stand,NULL,&s_transstand};

statetype s_transchase1         = {false,SPR_TRANS_W1,10,(statefunc)T_Chase,NULL,&s_transchase1s};
statetype s_transchase1s        = {false,SPR_TRANS_W1,3,NULL,NULL,&s_transchase2};
statetype s_transchase2         = {false,SPR_TRANS_W2,8,(statefunc)T_Chase,NULL,&s_transchase3};
statetype s_transchase3         = {false,SPR_TRANS_W3,10,(statefunc)T_Chase,NULL,&s_transchase3s};
statetype s_transchase3s        = {false,SPR_TRANS_W3,3,NULL,NULL,&s_transchase4};
statetype s_transchase4         = {false,SPR_TRANS_W4,8,(statefunc)T_Chase,NULL,&s_transchase1};

statetype s_transdie0           = {false,SPR_TRANS_W1,1,NULL,(statefunc)A_DeathScream,&s_transdie01};
statetype s_transdie01          = {false,SPR_TRANS_W1,1,NULL,NULL,&s_transdie1};
statetype s_transdie1           = {false,SPR_TRANS_DIE1,15,NULL,NULL,&s_transdie2};
statetype s_transdie2           = {false,SPR_TRANS_DIE2,15,NULL,NULL,&s_transdie3};
statetype s_transdie3           = {false,SPR_TRANS_DIE3,15,NULL,NULL,&s_transdie4};
statetype s_transdie4           = {false,SPR_TRANS_DEAD,0,NULL,NULL,&s_transdie4};

statetype s_transshoot1         = {false,SPR_TRANS_SHOOT1,30,NULL,NULL,&s_transshoot2};
statetype s_transshoot2         = {false,SPR_TRANS_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_transshoot3};
statetype s_transshoot3         = {false,SPR_TRANS_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_transshoot4};
statetype s_transshoot4         = {false,SPR_TRANS_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_transshoot5};
statetype s_transshoot5         = {false,SPR_TRANS_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_transshoot6};
statetype s_transshoot6         = {false,SPR_TRANS_SHOOT2,10,NULL,(statefunc)T_Shoot,&s_transshoot7};
statetype s_transshoot7         = {false,SPR_TRANS_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_transshoot8};
statetype s_transshoot8         = {false,SPR_TRANS_SHOOT1,10,NULL,NULL,&s_transchase1};


/*
===============
=
= SpawnTrans
=
===============
*/

void SpawnTrans (int tilex, int tiley)
{
    //        word *map;
    //        word tile;

    if (SoundBlasterPresent && DigiMode != sds_Off)
        s_transdie01.tictime = 105;

    SpawnNewObj (tilex,tiley,&s_transstand);
    newobj->obclass = transobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_trans];
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
    if (!loadedgame)
        gamestate.killtotal++;
}


//
// uber
//
void T_UShoot (objtype *ob);

extern  statetype s_uberstand;

extern  statetype s_uberchase1;
extern  statetype s_uberchase1s;
extern  statetype s_uberchase2;
extern  statetype s_uberchase3;
extern  statetype s_uberchase3s;
extern  statetype s_uberchase4;

extern  statetype s_uberdie0;
extern  statetype s_uberdie01;
extern  statetype s_uberdie1;
extern  statetype s_uberdie2;
extern  statetype s_uberdie3;
extern  statetype s_uberdie4;
extern  statetype s_uberdie5;

extern  statetype s_ubershoot1;
extern  statetype s_ubershoot2;
extern  statetype s_ubershoot3;
extern  statetype s_ubershoot4;
extern  statetype s_ubershoot5;
extern  statetype s_ubershoot6;
extern  statetype s_ubershoot7;


statetype s_uberstand           = {false,SPR_UBER_W1,0,(statefunc)T_Stand,NULL,&s_uberstand};

statetype s_uberchase1          = {false,SPR_UBER_W1,10,(statefunc)T_Chase,NULL,&s_uberchase1s};
statetype s_uberchase1s         = {false,SPR_UBER_W1,3,NULL,NULL,&s_uberchase2};
statetype s_uberchase2          = {false,SPR_UBER_W2,8,(statefunc)T_Chase,NULL,&s_uberchase3};
statetype s_uberchase3          = {false,SPR_UBER_W3,10,(statefunc)T_Chase,NULL,&s_uberchase3s};
statetype s_uberchase3s         = {false,SPR_UBER_W3,3,NULL,NULL,&s_uberchase4};
statetype s_uberchase4          = {false,SPR_UBER_W4,8,(statefunc)T_Chase,NULL,&s_uberchase1};

statetype s_uberdie0            = {false,SPR_UBER_W1,1,NULL,(statefunc)A_DeathScream,&s_uberdie01};
statetype s_uberdie01           = {false,SPR_UBER_W1,1,NULL,NULL,&s_uberdie1};
statetype s_uberdie1            = {false,SPR_UBER_DIE1,15,NULL,NULL,&s_uberdie2};
statetype s_uberdie2            = {false,SPR_UBER_DIE2,15,NULL,NULL,&s_uberdie3};
statetype s_uberdie3            = {false,SPR_UBER_DIE3,15,NULL,NULL,&s_uberdie4};
statetype s_uberdie4            = {false,SPR_UBER_DIE4,15,NULL,NULL,&s_uberdie5};
statetype s_uberdie5            = {false,SPR_UBER_DEAD,0,NULL,NULL,&s_uberdie5};

statetype s_ubershoot1          = {false,SPR_UBER_SHOOT1,30,NULL,NULL,&s_ubershoot2};
statetype s_ubershoot2          = {false,SPR_UBER_SHOOT2,12,NULL,(statefunc)T_UShoot,&s_ubershoot3};
statetype s_ubershoot3          = {false,SPR_UBER_SHOOT3,12,NULL,(statefunc)T_UShoot,&s_ubershoot4};
statetype s_ubershoot4          = {false,SPR_UBER_SHOOT4,12,NULL,(statefunc)T_UShoot,&s_ubershoot5};
statetype s_ubershoot5          = {false,SPR_UBER_SHOOT3,12,NULL,(statefunc)T_UShoot,&s_ubershoot6};
statetype s_ubershoot6          = {false,SPR_UBER_SHOOT2,12,NULL,(statefunc)T_UShoot,&s_ubershoot7};
statetype s_ubershoot7          = {false,SPR_UBER_SHOOT1,12,NULL,NULL,&s_uberchase1};


/*
===============
=
= SpawnUber
=
===============
*/

void SpawnUber (int tilex, int tiley)
{
    if (SoundBlasterPresent && DigiMode != sds_Off)
        s_uberdie01.tictime = 70;

    SpawnNewObj (tilex,tiley,&s_uberstand);
    newobj->obclass = uberobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_uber];
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
    if (!loadedgame)
        gamestate.killtotal++;
}


/*
===============
=
= T_UShoot
=
===============
*/

void T_UShoot (objtype *ob)
{
    int     dx,dy,dist;

    T_Shoot (ob);

    dx = abs(ob->tilex - player->tilex);
    dy = abs(ob->tiley - player->tiley);
    dist = dx>dy ? dx : dy;
    if (dist <= 1)
        TakeDamage (10,ob);
}


//
// will
//
extern  statetype s_willstand;

extern  statetype s_willchase1;
extern  statetype s_willchase1s;
extern  statetype s_willchase2;
extern  statetype s_willchase3;
extern  statetype s_willchase3s;
extern  statetype s_willchase4;

extern  statetype s_willdie1;
extern  statetype s_willdie2;
extern  statetype s_willdie3;
extern  statetype s_willdie4;
extern  statetype s_willdie5;
extern  statetype s_willdie6;

extern  statetype s_willshoot1;
extern  statetype s_willshoot2;
extern  statetype s_willshoot3;
extern  statetype s_willshoot4;
extern  statetype s_willshoot5;
extern  statetype s_willshoot6;


statetype s_willstand           = {false,SPR_WILL_W1,0,(statefunc)T_Stand,NULL,&s_willstand};

statetype s_willchase1          = {false,SPR_WILL_W1,10,(statefunc)T_Will,NULL,&s_willchase1s};
statetype s_willchase1s         = {false,SPR_WILL_W1,3,NULL,NULL,&s_willchase2};
statetype s_willchase2          = {false,SPR_WILL_W2,8,(statefunc)T_Will,NULL,&s_willchase3};
statetype s_willchase3          = {false,SPR_WILL_W3,10,(statefunc)T_Will,NULL,&s_willchase3s};
statetype s_willchase3s         = {false,SPR_WILL_W3,3,NULL,NULL,&s_willchase4};
statetype s_willchase4          = {false,SPR_WILL_W4,8,(statefunc)T_Will,NULL,&s_willchase1};

statetype s_willdeathcam        = {false,SPR_WILL_W1,1,NULL,NULL,&s_willdie1};

statetype s_willdie1            = {false,SPR_WILL_W1,1,NULL,(statefunc)A_DeathScream,&s_willdie2};
statetype s_willdie2            = {false,SPR_WILL_W1,10,NULL,NULL,&s_willdie3};
statetype s_willdie3            = {false,SPR_WILL_DIE1,10,NULL,NULL,&s_willdie4};
statetype s_willdie4            = {false,SPR_WILL_DIE2,10,NULL,NULL,&s_willdie5};
statetype s_willdie5            = {false,SPR_WILL_DIE3,10,NULL,NULL,&s_willdie6};
statetype s_willdie6            = {false,SPR_WILL_DEAD,20,NULL,NULL,&s_willdie6};

statetype s_willshoot1          = {false,SPR_WILL_SHOOT1,30,NULL,NULL,&s_willshoot2};
statetype s_willshoot2          = {false,SPR_WILL_SHOOT2,10,NULL,(statefunc)T_Launch,&s_willshoot3};
statetype s_willshoot3          = {false,SPR_WILL_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_willshoot4};
statetype s_willshoot4          = {false,SPR_WILL_SHOOT4,10,NULL,(statefunc)T_Shoot,&s_willshoot5};
statetype s_willshoot5          = {false,SPR_WILL_SHOOT3,10,NULL,(statefunc)T_Shoot,&s_willshoot6};
statetype s_willshoot6          = {false,SPR_WILL_SHOOT4,10,NULL,(statefunc)T_Shoot,&s_willchase1};


/*
===============
=
= SpawnWill
=
===============
*/

void SpawnWill (int tilex, int tiley)
{
    if (SoundBlasterPresent && DigiMode != sds_Off)
        s_willdie2.tictime = 70;

    SpawnNewObj (tilex,tiley,&s_willstand);
    newobj->obclass = willobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_will];
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
    if (!loadedgame)
        gamestate.killtotal++;
}


/*
================
=
= T_Will
=
================
*/

void T_Will (objtype *ob)
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
            if (ob->obclass == willobj)
                NewState (ob,&s_willshoot1);
            else if (ob->obclass == angelobj)
                NewState (ob,&s_angelshoot1);
            else
                NewState (ob,&s_deathshoot1);
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


//
// death
//
extern  statetype s_deathstand;

extern  statetype s_deathchase1;
extern  statetype s_deathchase1s;
extern  statetype s_deathchase2;
extern  statetype s_deathchase3;
extern  statetype s_deathchase3s;
extern  statetype s_deathchase4;

extern  statetype s_deathdie1;
extern  statetype s_deathdie2;
extern  statetype s_deathdie3;
extern  statetype s_deathdie4;
extern  statetype s_deathdie5;
extern  statetype s_deathdie6;
extern  statetype s_deathdie7;
extern  statetype s_deathdie8;
extern  statetype s_deathdie9;

extern  statetype s_deathshoot1;
extern  statetype s_deathshoot2;
extern  statetype s_deathshoot3;
extern  statetype s_deathshoot4;
extern  statetype s_deathshoot5;


statetype s_deathstand          = {false,SPR_DEATH_W1,0,(statefunc)T_Stand,NULL,&s_deathstand};

statetype s_deathchase1         = {false,SPR_DEATH_W1,10,(statefunc)T_Will,NULL,&s_deathchase1s};
statetype s_deathchase1s        = {false,SPR_DEATH_W1,3,NULL,NULL,&s_deathchase2};
statetype s_deathchase2         = {false,SPR_DEATH_W2,8,(statefunc)T_Will,NULL,&s_deathchase3};
statetype s_deathchase3         = {false,SPR_DEATH_W3,10,(statefunc)T_Will,NULL,&s_deathchase3s};
statetype s_deathchase3s        = {false,SPR_DEATH_W3,3,NULL,NULL,&s_deathchase4};
statetype s_deathchase4         = {false,SPR_DEATH_W4,8,(statefunc)T_Will,NULL,&s_deathchase1};

statetype s_deathdeathcam       = {false,SPR_DEATH_W1,1,NULL,NULL,&s_deathdie1};

statetype s_deathdie1           = {false,SPR_DEATH_W1,1,NULL,(statefunc)A_DeathScream,&s_deathdie2};
statetype s_deathdie2           = {false,SPR_DEATH_W1,10,NULL,NULL,&s_deathdie3};
statetype s_deathdie3           = {false,SPR_DEATH_DIE1,10,NULL,NULL,&s_deathdie4};
statetype s_deathdie4           = {false,SPR_DEATH_DIE2,10,NULL,NULL,&s_deathdie5};
statetype s_deathdie5           = {false,SPR_DEATH_DIE3,10,NULL,NULL,&s_deathdie6};
statetype s_deathdie6           = {false,SPR_DEATH_DIE4,10,NULL,NULL,&s_deathdie7};
statetype s_deathdie7           = {false,SPR_DEATH_DIE5,10,NULL,NULL,&s_deathdie8};
statetype s_deathdie8           = {false,SPR_DEATH_DIE6,10,NULL,NULL,&s_deathdie9};
statetype s_deathdie9           = {false,SPR_DEATH_DEAD,0,NULL,NULL,&s_deathdie9};

statetype s_deathshoot1         = {false,SPR_DEATH_SHOOT1,30,NULL,NULL,&s_deathshoot2};
statetype s_deathshoot2         = {false,SPR_DEATH_SHOOT2,10,NULL,(statefunc)T_Launch,&s_deathshoot3};
statetype s_deathshoot3         = {false,SPR_DEATH_SHOOT4,10,NULL,(statefunc)T_Shoot,&s_deathshoot4};
statetype s_deathshoot4         = {false,SPR_DEATH_SHOOT3,10,NULL,(statefunc)T_Launch,&s_deathshoot5};
statetype s_deathshoot5         = {false,SPR_DEATH_SHOOT4,10,NULL,(statefunc)T_Shoot,&s_deathchase1};


/*
===============
=
= SpawnDeath
=
===============
*/

void SpawnDeath (int tilex, int tiley)
{
    if (SoundBlasterPresent && DigiMode != sds_Off)
        s_deathdie2.tictime = 105;

    SpawnNewObj (tilex,tiley,&s_deathstand);
    newobj->obclass = deathobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_death];
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
    if (!loadedgame)
        gamestate.killtotal++;
}

/*
===============
=
= T_Launch
=
===============
*/

void T_Launch (objtype *ob)
{
    int32_t deltax,deltay;
    float   angle;
    int     iangle;

    deltax = player->x - ob->x;
    deltay = ob->y - player->y;
    angle = (float) atan2 ((float) deltay, (float) deltax);
    if (angle<0)
        angle = (float) (M_PI*2+angle);
    iangle = (int) (angle/(M_PI*2)*ANGLES);
    if (ob->obclass == deathobj)
    {
        T_Shoot (ob);
        if (ob->state == &s_deathshoot2)
        {
            iangle-=4;
            if (iangle<0)
                iangle+=ANGLES;
        }
        else
        {
            iangle+=4;
            if (iangle>=ANGLES)
                iangle-=ANGLES;
        }
    }

    GetNewActor ();
    newobj->state = &s_rocket;
    newobj->ticcount = 1;

    newobj->tilex = ob->tilex;
    newobj->tiley = ob->tiley;
    newobj->x = ob->x;
    newobj->y = ob->y;
    newobj->obclass = rocketobj;
    switch(ob->obclass)
    {
        case deathobj:
            newobj->state = &s_hrocket;
            newobj->obclass = hrocketobj;
            PlaySoundLocActor (KNIGHTMISSILESND,newobj);
            break;
        case angelobj:
            newobj->state = &s_spark1;
            newobj->obclass = sparkobj;
            PlaySoundLocActor (ANGELFIRESND,newobj);
            break;
        default:
            PlaySoundLocActor (MISSILEFIRESND,newobj);
    }

    newobj->dir = nodir;
    newobj->angle = iangle;
    newobj->speed = 0x2000l;
    newobj->flags = FL_NEVERMARK;
    newobj->active = ac_yes;
}



//
// angel
//
void A_Relaunch (objtype *ob);
void A_Victory (objtype *ob);
void A_StartAttack (objtype *ob);
void A_Breathing (objtype *ob);

extern  statetype s_angelstand;

extern  statetype s_angelchase1;
extern  statetype s_angelchase1s;
extern  statetype s_angelchase2;
extern  statetype s_angelchase3;
extern  statetype s_angelchase3s;
extern  statetype s_angelchase4;

extern  statetype s_angeldie1;
extern  statetype s_angeldie11;
extern  statetype s_angeldie2;
extern  statetype s_angeldie3;
extern  statetype s_angeldie4;
extern  statetype s_angeldie5;
extern  statetype s_angeldie6;
extern  statetype s_angeldie7;
extern  statetype s_angeldie8;
extern  statetype s_angeldie9;

extern  statetype s_angelshoot1;
extern  statetype s_angelshoot2;
extern  statetype s_angelshoot3;
extern  statetype s_angelshoot4;
extern  statetype s_angelshoot5;
extern  statetype s_angelshoot6;

extern  statetype s_angeltired;
extern  statetype s_angeltired2;
extern  statetype s_angeltired3;
extern  statetype s_angeltired4;
extern  statetype s_angeltired5;
extern  statetype s_angeltired6;
extern  statetype s_angeltired7;

extern  statetype s_spark1;
extern  statetype s_spark2;
extern  statetype s_spark3;
extern  statetype s_spark4;


statetype s_angelstand          = {false,SPR_ANGEL_W1,0,(statefunc)T_Stand,NULL,&s_angelstand};

statetype s_angelchase1         = {false,SPR_ANGEL_W1,10,(statefunc)T_Will,NULL,&s_angelchase1s};
statetype s_angelchase1s        = {false,SPR_ANGEL_W1,3,NULL,NULL,&s_angelchase2};
statetype s_angelchase2         = {false,SPR_ANGEL_W2,8,(statefunc)T_Will,NULL,&s_angelchase3};
statetype s_angelchase3         = {false,SPR_ANGEL_W3,10,(statefunc)T_Will,NULL,&s_angelchase3s};
statetype s_angelchase3s        = {false,SPR_ANGEL_W3,3,NULL,NULL,&s_angelchase4};
statetype s_angelchase4         = {false,SPR_ANGEL_W4,8,(statefunc)T_Will,NULL,&s_angelchase1};

statetype s_angeldie1           = {false,SPR_ANGEL_W1,1,NULL,(statefunc)A_DeathScream,&s_angeldie11};
statetype s_angeldie11          = {false,SPR_ANGEL_W1,1,NULL,NULL,&s_angeldie2};
statetype s_angeldie2           = {false,SPR_ANGEL_DIE1,10,NULL,(statefunc)A_Slurpie,&s_angeldie3};
statetype s_angeldie3           = {false,SPR_ANGEL_DIE2,10,NULL,NULL,&s_angeldie4};
statetype s_angeldie4           = {false,SPR_ANGEL_DIE3,10,NULL,NULL,&s_angeldie5};
statetype s_angeldie5           = {false,SPR_ANGEL_DIE4,10,NULL,NULL,&s_angeldie6};
statetype s_angeldie6           = {false,SPR_ANGEL_DIE5,10,NULL,NULL,&s_angeldie7};
statetype s_angeldie7           = {false,SPR_ANGEL_DIE6,10,NULL,NULL,&s_angeldie8};
statetype s_angeldie8           = {false,SPR_ANGEL_DIE7,10,NULL,NULL,&s_angeldie9};
statetype s_angeldie9           = {false,SPR_ANGEL_DEAD,130,NULL,(statefunc)A_Victory,&s_angeldie9};

statetype s_angelshoot1         = {false,SPR_ANGEL_SHOOT1,10,NULL,(statefunc)A_StartAttack,&s_angelshoot2};
statetype s_angelshoot2         = {false,SPR_ANGEL_SHOOT2,20,NULL,(statefunc)T_Launch,&s_angelshoot3};
statetype s_angelshoot3         = {false,SPR_ANGEL_SHOOT1,10,NULL,(statefunc)A_Relaunch,&s_angelshoot2};

statetype s_angeltired          = {false,SPR_ANGEL_TIRED1,40,NULL,(statefunc)A_Breathing,&s_angeltired2};
statetype s_angeltired2         = {false,SPR_ANGEL_TIRED2,40,NULL,NULL,&s_angeltired3};
statetype s_angeltired3         = {false,SPR_ANGEL_TIRED1,40,NULL,(statefunc)A_Breathing,&s_angeltired4};
statetype s_angeltired4         = {false,SPR_ANGEL_TIRED2,40,NULL,NULL,&s_angeltired5};
statetype s_angeltired5         = {false,SPR_ANGEL_TIRED1,40,NULL,(statefunc)A_Breathing,&s_angeltired6};
statetype s_angeltired6         = {false,SPR_ANGEL_TIRED2,40,NULL,NULL,&s_angeltired7};
statetype s_angeltired7         = {false,SPR_ANGEL_TIRED1,40,NULL,(statefunc)A_Breathing,&s_angelchase1};

statetype s_spark1              = {false,SPR_SPARK1,6,(statefunc)T_Projectile,NULL,&s_spark2};
statetype s_spark2              = {false,SPR_SPARK2,6,(statefunc)T_Projectile,NULL,&s_spark3};
statetype s_spark3              = {false,SPR_SPARK3,6,(statefunc)T_Projectile,NULL,&s_spark4};
statetype s_spark4              = {false,SPR_SPARK4,6,(statefunc)T_Projectile,NULL,&s_spark1};


void A_Slurpie (objtype *)
{
    SD_PlaySound(SLURPIESND);
}

void A_Breathing (objtype *)
{
    SD_PlaySound(ANGELTIREDSND);
}

/*
===============
=
= SpawnAngel
=
===============
*/

void SpawnAngel (int tilex, int tiley)
{
    if (SoundBlasterPresent && DigiMode != sds_Off)
        s_angeldie11.tictime = 105;

    SpawnNewObj (tilex,tiley,&s_angelstand);
    newobj->obclass = angelobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_angel];
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH;
    if (!loadedgame)
        gamestate.killtotal++;
}


/*
=================
=
= A_Victory
=
=================
*/

void A_Victory (objtype *)
{
    playstate = ex_victorious;
}


/*
=================
=
= A_StartAttack
=
=================
*/

void A_StartAttack (objtype *ob)
{
    ob->temp1 = 0;
}


/*
=================
=
= A_Relaunch
=
=================
*/

void A_Relaunch (objtype *ob)
{
    if (++ob->temp1 == 3)
    {
        NewState (ob,&s_angeltired);
        return;
    }

    if (US_RndT()&1)
    {
        NewState (ob,&s_angelchase1);
        return;
    }
}




//
// spectre
//
void T_SpectreWait (objtype *ob);
void A_Dormant (objtype *ob);

extern  statetype s_spectrewait1;
extern  statetype s_spectrewait2;
extern  statetype s_spectrewait3;
extern  statetype s_spectrewait4;

extern  statetype s_spectrechase1;
extern  statetype s_spectrechase2;
extern  statetype s_spectrechase3;
extern  statetype s_spectrechase4;

extern  statetype s_spectredie1;
extern  statetype s_spectredie2;
extern  statetype s_spectredie3;
extern  statetype s_spectredie4;

extern  statetype s_spectrewake;

statetype s_spectrewait1        = {false,SPR_SPECTRE_W1,10,(statefunc)T_Stand,NULL,&s_spectrewait2};
statetype s_spectrewait2        = {false,SPR_SPECTRE_W2,10,(statefunc)T_Stand,NULL,&s_spectrewait3};
statetype s_spectrewait3        = {false,SPR_SPECTRE_W3,10,(statefunc)T_Stand,NULL,&s_spectrewait4};
statetype s_spectrewait4        = {false,SPR_SPECTRE_W4,10,(statefunc)T_Stand,NULL,&s_spectrewait1};

statetype s_spectrechase1       = {false,SPR_SPECTRE_W1,10,(statefunc)T_Ghosts,NULL,&s_spectrechase2};
statetype s_spectrechase2       = {false,SPR_SPECTRE_W2,10,(statefunc)T_Ghosts,NULL,&s_spectrechase3};
statetype s_spectrechase3       = {false,SPR_SPECTRE_W3,10,(statefunc)T_Ghosts,NULL,&s_spectrechase4};
statetype s_spectrechase4       = {false,SPR_SPECTRE_W4,10,(statefunc)T_Ghosts,NULL,&s_spectrechase1};

statetype s_spectredie1         = {false,SPR_SPECTRE_F1,10,NULL,NULL,&s_spectredie2};
statetype s_spectredie2         = {false,SPR_SPECTRE_F2,10,NULL,NULL,&s_spectredie3};
statetype s_spectredie3         = {false,SPR_SPECTRE_F3,10,NULL,NULL,&s_spectredie4};
statetype s_spectredie4         = {false,SPR_SPECTRE_F4,300,NULL,NULL,&s_spectrewake};
statetype s_spectrewake         = {false,SPR_SPECTRE_F4,10,NULL,(statefunc)A_Dormant,&s_spectrewake};

/*
===============
=
= SpawnSpectre
=
===============
*/

void SpawnSpectre (int tilex, int tiley)
{
    SpawnNewObj (tilex,tiley,&s_spectrewait1);
    newobj->obclass = spectreobj;
    newobj->hitpoints = starthitpoints[gamestate.difficulty][en_spectre];
    newobj->flags |= FL_SHOOTABLE|FL_AMBUSH|FL_BONUS; // |FL_NEVERMARK|FL_NONMARK;
    if (!loadedgame)
        gamestate.killtotal++;
}


/*
===============
=
= A_Dormant
=
===============
*/

void A_Dormant (objtype *ob)
{
    int32_t     deltax,deltay;
    int         xl,xh,yl,yh;
    int         x,y;
    uintptr_t   tile;

    deltax = ob->x - player->x;
    if (deltax < -MINACTORDIST || deltax > MINACTORDIST)
        goto moveok;
    deltay = ob->y - player->y;
    if (deltay < -MINACTORDIST || deltay > MINACTORDIST)
        goto moveok;

    return;
moveok:

    xl = (ob->x-MINDIST) >> TILESHIFT;
    xh = (ob->x+MINDIST) >> TILESHIFT;
    yl = (ob->y-MINDIST) >> TILESHIFT;
    yh = (ob->y+MINDIST) >> TILESHIFT;

    for (y=yl ; y<=yh ; y++)
        for (x=xl ; x<=xh ; x++)
        {
            tile = (uintptr_t)actorat[x][y];
            if (!tile)
                continue;
            if (!ISPOINTER(tile))
                return;
            if (((objtype *)tile)->flags&FL_SHOOTABLE)
                return;
        }

        ob->flags |= FL_AMBUSH | FL_SHOOTABLE;
        ob->flags &= ~FL_ATTACKMODE;
        ob->flags &= ~FL_NONMARK;      // stuck bugfix 1
        ob->dir = nodir;
        NewState (ob,&s_spectrewait1);
}




/*
=============================================================================

                            SCHABBS / GIFT / FAT

=============================================================================
*/


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

        if(DEMOCOND_ORIG)
        {
            if(!dist || (dist == 1 && ob->distance < 0x4000))
                chance = 300;
            else
                chance = (tics<<4)/dist;
        }
        else

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
                case angelobj:
                    NewState (ob,&s_angelshoot1);
                    break;
                case transobj:
                    NewState (ob,&s_transshoot1);
                    break;
                case uberobj:
                    NewState (ob,&s_ubershoot1);
                    break;
                case willobj:
                    NewState (ob,&s_willshoot1);
                    break;
                case deathobj:
                    NewState (ob,&s_deathshoot1);
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
