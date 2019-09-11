/* ------------------------------------------------------------------------------------ */
/*  ep-lumi.c                                                                           */
/*                                                                                      */
/*    OLYMPUS 12-degree Lumi monitor.                                                   */
/*                                                                                      */
/*  A.Kisselev, PNPI, St.Petersburg, Russia.                                            */
/*    e-mail: kisselev@hermes.desy.de                                                   */
/* ------------------------------------------------------------------------------------ */

#include <cstdlib>

#include <survey.h>

/* ---- MWPC/GEM support bars geometry defintions ------------------------------------- */

#define _BAR_MATCHING_ACCURACY_XY_   ( 0.25)
#define _BAR_MATCHING_ACCURACY_ZZ_   ( 0.25)

#define _BAR_V_SIZE_DEFAULT_         (157.0)
#define _BAR_L_SIZE_DEFAULT_         (860.0)

// Well, should be offset as a whole by +/-140mm wrt the 12-degree axis;
#define _BAR_T_OFFSET_               (140.0)

// These parameters one can tune based on symmetry considerations;
static t_oly_variable bar_v_size = {"BAR_V_SIZE", "[mm]", "12-degree Lumi bar 'length'", _BAR_V_SIZE_DEFAULT_};
static t_oly_variable bar_l_size = {"BAR_L_SIZE", "[mm]", "12-degree Lumi bar 'height'", _BAR_L_SIZE_DEFAULT_};

t_oly_object bar_l = {
    "BAR_L",  1, NULL, _12_00_DEGREES_, {"olympus/GEM-IST.txt"},
    _BAR_MATCHING_ACCURACY_XY_, _BAR_MATCHING_ACCURACY_ZZ_, _6PAR_, {}, 2400., {}, 
    {{"LGEMREF.1", {  { _BAR_T_OFFSET_}, {0., {{&bar_v_size,  0.5}}}, {0., {{&bar_l_size,  0.5}}}}},
     {"LGEMREF.2", {  { _BAR_T_OFFSET_}, {0., {{&bar_v_size, -0.5}}}, {0., {{&bar_l_size,  0.5}}}}},
     {"LGEMREF.3", {  { _BAR_T_OFFSET_}, {0., {{&bar_v_size,  0.5}}}, {0., {{&bar_l_size, -0.5}}}}},
     {"LGEMREF.4", {  { _BAR_T_OFFSET_}, {0., {{&bar_v_size, -0.5}}}, {0., {{&bar_l_size, -0.5}}}}}
    }};

t_oly_object bar_r = {
    "BAR_R",  1, NULL, -_12_00_DEGREES_, {"olympus/GEM-IST.txt"},
    _BAR_MATCHING_ACCURACY_XY_, _BAR_MATCHING_ACCURACY_ZZ_, _6PAR_, {}, 2400., {}, 
    {{"RGEMREF.1", {  {-_BAR_T_OFFSET_}, {0., {{&bar_v_size,  0.5}}}, {0., {{&bar_l_size, -0.5}}}}},
     {"RGEMREF.2", {  {-_BAR_T_OFFSET_}, {0., {{&bar_v_size, -0.5}}}, {0., {{&bar_l_size, -0.5}}}}},
     {"RGEMREF.3", {  {-_BAR_T_OFFSET_}, {0., {{&bar_v_size,  0.5}}}, {0., {{&bar_l_size,  0.5}}}}},
     {"RGEMREF.4", {  {-_BAR_T_OFFSET_}, {0., {{&bar_v_size, -0.5}}}, {0., {{&bar_l_size,  0.5}}}}}
    }};

/* ---- MWPC geometry defintions ------------------------------------------------------ */

// Chamber package thickness (without plastic layers for now!); 
// --> measure de-facto value later, perhaps separately for all chambers!;
#define _MWPC_THICKNESS_             (49.50)

// Screw flat surface height wrt the chamber surface; must be groove 
// depth (-4mm) + 5mm + 10mm (see alignment screw drawing); 
#define _SCREW_HEIGHT_               (11.00)

// Feb'2011 optical target ball center height wrt the flat nest surface; 
// I'd expect 12.70+3.00=15.70mm (MEA design value + our change adapting to 
// 10mm magnet); Martin measured it to be 15.75mm on one of the targets, fine;
// NB: well, actually can not tell now which of the 3 targets was used where; 
#define _NEST_N0_HEIGHT_             (15.75)

// Jul'2011 targets; see e-mail from M.Noak 2011/08/16; 
#define _NEST_N1_HEIGHT_             (15.71)
// Thin tail target used for MWPC#1,2 (poor quality holes);
#define _NEST_N4_HEIGHT_             (15.74)
// Nest with M6 thread, used for the chambers #1,2;
#define _NEST_N6_HEIGHT_             (17.95)

// Nominal corner screw offsets in transrevse direction 
// wrt the chamber module center;
#define _MWPC_XY_OFFSET_             (80.00)

// Check this value, please!!!!!!!;
#define _PLASTIK_THICKNESS_          ( 1.00)

// Feb'2011 definitions; apparently MWPC#2 was swapped and therefore had survey marks
// on its upstream (front) side;
#define _ZD0_OFFSET_  (  _MWPC_THICKNESS_/2. + _SCREW_HEIGHT_      + _NEST_N0_HEIGHT_)
#define _ZU0_OFFSET_  (-_ZD0_OFFSET_)
 
// Make it better later (tune or measure package thickness, plastic, etc); for
// now just tune this single value in order to obtain best chi^2;
#define _FIX_                        ( 0.50)

// Prefer to put proper sign for Upstream and Downstream target locations
// in the "final" installation;
#define _ZD1_OFFSET_  (  _MWPC_THICKNESS_/2. + _SCREW_HEIGHT_      + _NEST_N1_HEIGHT_ - _FIX_)
#define _ZD4_OFFSET_  (  _MWPC_THICKNESS_/2. + _SCREW_HEIGHT_      + _NEST_N4_HEIGHT_ - _FIX_)
#define _ZU6_OFFSET_  (-(_MWPC_THICKNESS_/2. + _PLASTIK_THICKNESS_ + _NEST_N6_HEIGHT_ - _FIX_))

// Release Z-coordinate accuracy first and tune these ~150um in 
// XY-projection (perhaps was rather ~170um in Feb'2011, who cares); 
// then find Z-accuracy which gives a reasonable chi^2 increment; 
// seems like ~700um is fine; 
#define _MWPC_MATCHING_ACCURACY_XY_  ( 0.15)
#define _MWPC_MATCHING_ACCURACY_ZZ_  ( 0.70)

// Prefer to extract Feb'2011 measurements in a separate #ifdef; does not work now I guess;
#ifdef _DORIS_TEST_RUN_FEB_2011_
    {"OLY_1", 1, NULL, -_12_00_DEGREES_, 
       _MWPC_MATCHING_ACCURACY_XY_, _MWPC_MATCHING_ACCURACY_ZZ_, _6PAR_, {}, 1500., {}, 
     {{"MWIC1.L", { _MWPC_XY_OFFSET_,                0., _ZD0_OFFSET_}}, 
      {"MWIC1.T", {-_MWPC_XY_OFFSET_,  _MWPC_XY_OFFSET_, _ZD0_OFFSET_}}, 
      {"MWIC1.B", {-_MWPC_XY_OFFSET_, -_MWPC_XY_OFFSET_, _ZD0_OFFSET_}}}}, 

    {"OLY_2", 1, NULL, -_12_00_DEGREES_, 
       _MWPC_MATCHING_ACCURACY_XY_, _MWPC_MATCHING_ACCURACY_ZZ_, _6PAR_, {}, 2000., {}, 
     {{"MWIC2.L", { _MWPC_XY_OFFSET_,                0., _ZU0_OFFSET_}}, 
      {"MWIC2.T", {-_MWPC_XY_OFFSET_,  _MWPC_XY_OFFSET_, _ZU0_OFFSET_}}, 
      {"MWIC2.B", {-_MWPC_XY_OFFSET_, -_MWPC_XY_OFFSET_, _ZU0_OFFSET_}}}
#endif

t_oly_object lm_l1 = {
    "LM_L1", 1, NULL, _12_00_DEGREES_, {"olympus/GEM-IST.txt"},
    _MWPC_MATCHING_ACCURACY_XY_, _MWPC_MATCHING_ACCURACY_ZZ_, _6PAR_, {}, 2000., {}, 
    {{"L1UM",      {{-_MWPC_XY_OFFSET_}, {               0.}, {   _ZU6_OFFSET_}}},
     {"L1UT",      {{ _MWPC_XY_OFFSET_}, { _MWPC_XY_OFFSET_}, {   _ZU6_OFFSET_}}},
     {"L1UB",      {{ _MWPC_XY_OFFSET_}, {-_MWPC_XY_OFFSET_}, {   _ZU6_OFFSET_}}},
     {"L1DM",      {{ _MWPC_XY_OFFSET_}, {               0.}, {   _ZD4_OFFSET_}}}
    }};

t_oly_object lm_l2 = {
    "LM_L2", 1, NULL, _12_00_DEGREES_, {"olympus/GEM-IST.txt"},
    _MWPC_MATCHING_ACCURACY_XY_, _MWPC_MATCHING_ACCURACY_ZZ_, _6PAR_, {}, 2300., {}, 
    {{"L2UM",      {{-_MWPC_XY_OFFSET_}, {               0.}, {   _ZU6_OFFSET_}}},
     {"L2UT",      {{ _MWPC_XY_OFFSET_}, { _MWPC_XY_OFFSET_}, {   _ZU6_OFFSET_}}},
     {"L2UB",      {{ _MWPC_XY_OFFSET_}, {-_MWPC_XY_OFFSET_}, {   _ZU6_OFFSET_}}},
     {"L2DM",      {{ _MWPC_XY_OFFSET_}, {               0.}, {   _ZD4_OFFSET_}}}
    }};

t_oly_object lm_l3 = {
    "LM_L3", 1, NULL, _12_00_DEGREES_, {"olympus/GEM-IST.txt"},
    _MWPC_MATCHING_ACCURACY_XY_, _MWPC_MATCHING_ACCURACY_ZZ_, _6PAR_, {}, 2700., {}, 
    {{"L3UM",      {{-_MWPC_XY_OFFSET_}, {               0.}, {   _ZU6_OFFSET_}}},
     {"L3UT",      {{ _MWPC_XY_OFFSET_}, { _MWPC_XY_OFFSET_}, {   _ZU6_OFFSET_}}},
     {"L3UB",      {{ _MWPC_XY_OFFSET_}, {-_MWPC_XY_OFFSET_}, {   _ZU6_OFFSET_}}},
     {"L3DM",      {{-_MWPC_XY_OFFSET_}, {               0.}, {   _ZD1_OFFSET_}}},
     {"L3DT",      {{ _MWPC_XY_OFFSET_}, { _MWPC_XY_OFFSET_}, {   _ZD1_OFFSET_}}},
     {"L3DB",      {{ _MWPC_XY_OFFSET_}, {-_MWPC_XY_OFFSET_}, {   _ZD1_OFFSET_}}}
    }};

t_oly_object lm_r1 = {
    "LM_R1",  1, NULL, -_12_00_DEGREES_, {"olympus/GEM-IST.txt"},
    _MWPC_MATCHING_ACCURACY_XY_, _MWPC_MATCHING_ACCURACY_ZZ_, _6PAR_, {}, 2000., {}, 
    {{"R1UM",      {{ _MWPC_XY_OFFSET_}, {               0.}, {   _ZU6_OFFSET_}}},
     {"R1UT",      {{-_MWPC_XY_OFFSET_}, { _MWPC_XY_OFFSET_}, {   _ZU6_OFFSET_}}},
     {"R1UB",      {{-_MWPC_XY_OFFSET_}, {-_MWPC_XY_OFFSET_}, {   _ZU6_OFFSET_}}},
     {"R1DM",      {{-_MWPC_XY_OFFSET_}, {               0.}, {   _ZD4_OFFSET_}}}
    }};

t_oly_object lm_r2 = {
    "LM_R2",  1, NULL, -_12_00_DEGREES_, {"olympus/GEM-IST.txt"},
    _MWPC_MATCHING_ACCURACY_XY_, _MWPC_MATCHING_ACCURACY_ZZ_, _6PAR_, {}, 2300., {}, 
    {{"R2UM",      {{ _MWPC_XY_OFFSET_}, {               0.}, {   _ZU6_OFFSET_}}},
     {"R2UT",      {{-_MWPC_XY_OFFSET_}, { _MWPC_XY_OFFSET_}, {   _ZU6_OFFSET_}}},
     {"R2UB",      {{-_MWPC_XY_OFFSET_}, {-_MWPC_XY_OFFSET_}, {   _ZU6_OFFSET_}}}
    }};

t_oly_object lm_r3 = {
    "LM_R3",  1, NULL, -_12_00_DEGREES_, {"olympus/GEM-IST.txt"},
    _MWPC_MATCHING_ACCURACY_XY_, _MWPC_MATCHING_ACCURACY_ZZ_, _6PAR_, {}, 2700., {},
    {{"R3UM",      {{ _MWPC_XY_OFFSET_}, {               0.}, {   _ZU6_OFFSET_}}},
     {"R3DM",      {{ _MWPC_XY_OFFSET_}, {               0.}, {   _ZD1_OFFSET_}}},
     {"R3DT",      {{-_MWPC_XY_OFFSET_}, { _MWPC_XY_OFFSET_}, {   _ZD1_OFFSET_}}},
     {"R3DB",      {{-_MWPC_XY_OFFSET_}, {-_MWPC_XY_OFFSET_}, {   _ZD1_OFFSET_}}}
    }};

/* ---- GEM geometry defintions ------------------------------------------------------- */

#define _GEM_MATCHING_ACCURACY_XY_   ( 0.06)
// Set the same as for XY-direction; perhsp not the best idea;
#define _GEM_MATCHING_ACCURACY_ZZ_   ( 0.06)

#define _70_03MM_  (70.03)
#define _00_02MM_  ( 0.02)
#define _00_04MM_  ( 0.04)
#define _69_80MM_  (69.80)

t_oly_object gem_l1 = {
    "GEM_L1",   1, NULL, _12_00_DEGREES_, {"olympus/GEM-IST.txt"},
    _GEM_MATCHING_ACCURACY_XY_, _GEM_MATCHING_ACCURACY_ZZ_, _5MPAR_, {}, 1850., {},
    {{"LGEM.5",    {{ -_00_04MM_},  { _69_80MM_},             {0.}}}, 
     {"LGEM.6",    {{ -_70_03MM_},  {-_00_02MM_},             {0.}}}, 
    }};

t_oly_object gem_l2 = {
    "GEM_L2",   1, NULL, _12_00_DEGREES_, {"olympus/GEM-IST.txt"},
    _GEM_MATCHING_ACCURACY_XY_, _GEM_MATCHING_ACCURACY_ZZ_, _5MPAR_, {}, 2150., {},
    {{"LGEM.3",    {{ -_00_04MM_},  { _69_80MM_},             {0.}}}, 
     {"LGEM.4",    {{ -_70_03MM_},  {-_00_02MM_},             {0.}}}, 
    }};

t_oly_object gem_l3 = {
    "GEM_L3",   1, NULL, _12_00_DEGREES_, {"olympus/GEM-IST.txt"},
    _GEM_MATCHING_ACCURACY_XY_, _GEM_MATCHING_ACCURACY_ZZ_, _5PPAR_, {}, 2600., {},
    {{"LGEM.1",    {{ -_00_04MM_},  {-_69_80MM_},             {0.}}}, 
     {"LGEM.2",    {{ -_70_03MM_},  { _00_02MM_},             {0.}}}, 
    }};

t_oly_object gem_r1 = {
    "GEM_R1",   1, NULL, -_12_00_DEGREES_, {"olympus/GEM-IST.txt"},
    _GEM_MATCHING_ACCURACY_XY_, _GEM_MATCHING_ACCURACY_ZZ_, _5MPAR_, {}, 1850., {},
    {{"RGEM.1",    {{  _00_04MM_},  {-_69_80MM_},             {0.}}},
     {"RGEM.2",    {{  _70_03MM_},  { _00_02MM_},             {0.}}},
    }};

t_oly_object gem_r2 = {
    "GEM_R2",   1, NULL, -_12_00_DEGREES_, {"olympus/GEM-IST.txt"},
    _GEM_MATCHING_ACCURACY_XY_, _GEM_MATCHING_ACCURACY_ZZ_, _5MPAR_, {}, 2150., {},
    {{"RGEM.3",    {{  _00_04MM_},  {-_69_80MM_},             {0.}}},
     {"RGEM.4",    {{  _70_03MM_},  { _00_02MM_},             {0.}}},
    }};

t_oly_object gem_r3 = {
    "GEM_R3",   1, NULL, -_12_00_DEGREES_, {"olympus/GEM-IST.txt"},
    _GEM_MATCHING_ACCURACY_XY_, _GEM_MATCHING_ACCURACY_ZZ_, _5PPAR_, {}, 2600., {},
    {{"RGEM.5",    {{  _00_04MM_},  { _69_80MM_},             { 0.}}},
     {"RGEM.6",    {{  _70_03MM_},  {-_00_02MM_},             { 0.}}},
    }};

/* ---- SC#0 support holes (WC#1 downstream end) -------------------------------------- */

// Just carelessly take values used for the WCs themselves; clearly the actual
// internal consistency is much better than these ~170um per projection; who cares;
#define _WC2SC_MATCHING_ACCURACY_XY_    ( 0.17)
#define _WC2SC_MATCHING_ACCURACY_ZZ_    ( 0.17)

// Well, optical target ball centers are 12.74mm above the WC flat surfaces; 
#define _12_74_                         (12.74)

//
// -> 163.149mm is just calculated out of the DX & DY coordinates of RCDIIT & RCDIIM,
//    see wc.c for details given by J.Kelsey;
//

t_oly_object wc2sc_l = {
    "WC2SC_L",  1, NULL, _12_00_DEGREES_, {"olympus/Driftkammer-gross-IST.txt"},
    _WC2SC_MATCHING_ACCURACY_XY_, _WC2SC_MATCHING_ACCURACY_ZZ_, _6PAR_, {},  1700., {},
    {{"LCDIIT", {{  -_12_74_}, {  88.9}, {    0.0}}},
     {"LCDIIM", {{  -_12_74_}, {   0.0}, { -163.149}}},
     {"LCDIIB", {{  -_12_74_}, { -88.9}, {    0.0}}}
    }};

t_oly_object wc2sc_r = {
    "WC2SC_R",  1, NULL, -_12_00_DEGREES_, {"olympus/Driftkammer-gross-IST.txt"},
    _WC2SC_MATCHING_ACCURACY_XY_, _WC2SC_MATCHING_ACCURACY_ZZ_, _6PAR_, {},  1700., {},
    {{"RCDIIT", {{   _12_74_}, {  88.9}, {    0.0}}},
     {"RCDIIM", {{   _12_74_}, {   0.0}, { -163.149}}},
     {"RCDIIB", {{   _12_74_}, { -88.9}, {    0.0}}}
    }};


/* ------------------------------------------------------------------------------------ */
