/* ------------------------------------------------------------------------------------ */
/*  sc-chamber.c                                                                        */
/*                                                                                      */
/*    OLYMPUS scattering chamber.                                                       */
/*                                                                                      */
/*  A.Kisselev, PNPI, St.Petersburg, Russia.                                            */
/*    e-mail: kisselev@hermes.desy.de                                                   */
/* ------------------------------------------------------------------------------------ */

#include <cstdlib>

#include <survey.h>

/* ---- scattering chamber geometry defintions ---------------------------------------- */

//
//  Well, assume that optical target ball centers are 12.74mm above the flat surfaces;
//  --> this needs to be verified!!!;
// 
#define _12_740_                 (12.74)
// Times sin(3.12) and cos(3.12);
#define _00_693_                 (_12_740_ * 0.0544)
#define _12_721_                 (_12_740_ * 0.9985)

// Yes, no way better than this;
#define _SC_MATCHING_ACCURACY_XY_   ( 0.25)
#define _SC_MATCHING_ACCURACY_ZZ_   ( 0.25)

static t_oly_variable fx              = {"FX",        "[mm]", 0, 0.0};
static t_oly_variable fy              = {"FY",        "[mm]", 0, 0.0};
static t_oly_variable fz              = {"FZ",        "[mm]", 0, 0.0};

t_oly_object sc_chamber = {
    "SC_CHAMBER",  1, NULL, 0.0, {"olympus/targetcell-IST.txt"},
    _SC_MATCHING_ACCURACY_XY_, _SC_MATCHING_ACCURACY_ZZ_, _6PAR_, {}, 0., {},
    {{"1", {  { 0.0},             { 127.000+_12_740_}, { -599.200}}},
     // Original file from Jim has Z=515.102 for this Hall#4 which seems to be wrong; 
     // also X-coordinate is a bit too off from 0.0mm;
     {"2", {  { 0.0},             { 127.000+_12_740_}, {  515.102, {{&fz, 1.}}}}},
     {"3", {  { 87.086+_12_721_}, { 100.000},          {_00_693_}}},
     {"4", {  { 87.086+_12_721_}, {-100.000},          {_00_693_}}},
     {"5", {  { 0.0},             { 100.000},          { -650.000 -_12_740_}}},
     // Y-coordinate seems to be off by ~1mm;
     {"6", {  { 0.0},             {-100.000, {{&fy, 1.}}},          { -650.000 -_12_740_}}},
    }};

#if 0
1		-599.231	-0.0245	139.7477
2		499.4596	0.0096	139.6565
3		0.2005	100.3527	99.6978
4		0.3628	100.3299	-99.9138
5		-662.8696	-0.083	99.6768
6		-663.3624	0.1208	-99.1854
#endif

/* ------------------------------------------------------------------------------------ */
