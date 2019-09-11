/* ------------------------------------------------------------------------------------ */
/*  ee-lumi.c                                                                           */
/*                                                                                      */
/*    OLYMPUS Bhabha/Moeller Lumi monitor.                                              */
/*                                                                                      */
/*  A.Kisselev, PNPI, St.Petersburg, Russia.                                            */
/*    e-mail: kisselev@hermes.desy.de                                                   */
/* ------------------------------------------------------------------------------------ */

#include <cstdlib>

#include <survey.h>

/* ---- Bhabha/Moeller Lumi monitor geometry defintions ------------------------------- */

// Perhaps something like ~270um in fact;
#define _SYMB_MATCHING_ACCURACY_XY_  ( 0.30)
#define _SYMB_MATCHING_ACCURACY_ZZ_  ( 0.30)

t_oly_object symb_l = {
    "SYMB_L",  1, NULL, 0., {"olympus/MoellerdetektorBox-IST.txt"},
    _SYMB_MATCHING_ACCURACY_XY_, _SYMB_MATCHING_ACCURACY_ZZ_, _6PAR_, {}, 3000., {},
    {{"1.1", {{   183.450}, {-173.059}, { 761.104}}},
     {"1.2", {{   380.008}, {-173.059}, { 915.586}}},  
     {"1.3", {{   493.544}, {-173.059}, { 317.999}}},
     {"1.4", {{   264.480}, {-173.059}, {  52.754}}},
     {"1.5", {{    60.000}, { 141.941}, {   0.000}}},
     {"1.6", {{   -60.000}, { 141.941}, {   0.000}}}
    }};

t_oly_object symb_r = {
    "SYMB_R", 1, NULL, 0., {"olympus/MoellerdetektorBox-IST.txt"},
    _SYMB_MATCHING_ACCURACY_XY_, _SYMB_MATCHING_ACCURACY_ZZ_, _6PAR_, {}, 3000., {},
    {{"2.1", {{  -183.450}, {-173.059}, { 761.104}}},
     {"2.2", {{  -380.008}, {-173.059}, { 915.586}}},  
     {"2.3", {{  -493.544}, {-173.059}, { 317.999}}},
     {"2.4", {{  -264.480}, {-173.059}, {  52.754}}},
     {"2.5", {{   -60.000}, { 141.941}, {   0.000}}},
     {"2.6", {{    60.000}, { 141.941}, {   0.000}}}
    }};

/* ------------------------------------------------------------------------------------ */
