/* ------------------------------------------------------------------------------------ */
/*  wc.c                                                                                */
/*                                                                                      */
/*    OLYMPUS drift chambers.                                                           */
/*                                                                                      */
/*  A.Kisselev, PNPI, St.Petersburg, Russia.                                            */
/*    e-mail: kisselev@hermes.desy.de                                                   */
/* ------------------------------------------------------------------------------------ */

#include <cstdlib>

#include <survey.h>

/* ---- WC geometry defintions -------------------------------------------------------- */

// Looks like ~200um is good enough;
#define _WC_MATCHING_ACCURACY_XY_    ( 0.20)
#define _WC_MATCHING_ACCURACY_ZZ_    ( 0.20)

// Well, optical target ball centers are 12.74mm above the WC flat surfaces; 
// and one also has to account side surface slopes of 99.46 (upstream) 
// and 144.29 (downstream) degrees (see e-mail from J.Kelsey 2011/08/05);
// the actual direction cosines are given by Jim 2011/10/12;
#define _DX_07_436_                 ( 0.584 * 12.74)
#define _DX_12_576_                 ( 0.986 * 12.74)
#define _DZ_02_094_                 ( 0.164 * 12.74)
#define _DZ_10_345_                 ( 0.812 * 12.74)
// Similar values for top (bottom) sides;
#define _DX_00_1091_                ( 0.108 * 12.74)
#define _DY_00_9296_                ( 0.924 * 12.74)
#define _DZ_00_3519_                ( 0.367 * 12.74)

static t_oly_variable wc_ri_fy        = {"WC_RI_FY",  "[mm]", 0, 0.0};
static t_oly_variable wc_ri_fz        = {"WC_RI_FZ",  "[mm]", 0, 0.0};

static t_oly_variable wc_ro_fx        = {"WC_RO_FX",  "[mm]", 0, 0.0};
static t_oly_variable wc_ro_fy        = {"WC_RO_FY",  "[mm]", 0, 0.0};
static t_oly_variable wc_ro_fz        = {"WC_RO_FZ",  "[mm]", 0, 0.0};

static t_oly_variable wc_lo_fy        = {"WC_LO_FY",  "[mm]", 0, 0.0};

t_oly_object wc_li = {
    "WC_LI", 1, NULL,  _73_54_DEGREES_, {"olympus/Driftkammer-gross-IST.txt"},
    _WC_MATCHING_ACCURACY_XY_, _WC_MATCHING_ACCURACY_ZZ_, _6PAR_, {},  800., {},
    // Yes, upstream end points are missing for this chamber;
    {{"LCDIIT", {{ -1629.775 - _DX_07_436_},  {  88.9}, {110.053 - _DZ_10_345_}}},
     {"LCDIIM", {{ -1497.301 - _DX_07_436_},  {   0.0}, { 14.826 - _DZ_10_345_}}},
     {"LCDIIB", {{ -1629.775 - _DX_07_436_},  { -88.9}, {110.053 - _DZ_10_345_}}},

     {"LCTIDI", {{ -1409.926 - _DX_00_1091_}, { 132.076 + _DY_00_9296_}, { 23.628 - _DZ_00_3519_}}},
     {"LCTIDO", {{ -1417.023 - _DX_00_1091_}, { 192.544 + _DY_00_9296_}, {190.571 - _DZ_00_3519_}}},
     {"LCTIUI", {{   -98.130 - _DX_00_1091_}, { 286.036 + _DY_00_9296_}, { 23.628 - _DZ_00_3519_}}},
     {"LCTIUO", {{  -105.227 - _DX_00_1091_}, { 346.504 + _DY_00_9296_}, {190.571 - _DZ_00_3519_}}},

     {"LCBIDI", {{ -1409.926 - _DX_00_1091_}, {-132.076 - _DY_00_9296_}, { 23.628 - _DZ_00_3519_}}},
     {"LCBIDO", {{ -1417.023 - _DX_00_1091_}, {-192.544 - _DY_00_9296_}, {190.571 - _DZ_00_3519_}}},
     {"LCBIUI", {{   -98.130 - _DX_00_1091_}, {-286.036 - _DY_00_9296_}, { 23.628 - _DZ_00_3519_}}},
     {"LCBIUO", {{  -105.227 - _DX_00_1091_}, {-346.504 - _DY_00_9296_}, {190.571 - _DZ_00_3519_}}},
    }};

t_oly_object wc_ri = {
    "WC_RI", 1, NULL, -_73_54_DEGREES_, {"olympus/Driftkammer-gross-IST.txt"},
    _WC_MATCHING_ACCURACY_XY_, _WC_MATCHING_ACCURACY_ZZ_, _6PAR_, {},  800., {},
    {{"RCUIIT", {{  -18.338 - _DX_12_576_},  { 266.7}, {110.053 - _DZ_02_094_}}}, 
     {"RCUIIM", {{   -4.175 - _DX_12_576_},  {   0.0}, { 25.055 - _DZ_02_094_}}},
     {"RCUIIB", {{  -18.338 - _DX_12_576_},  {-266.7}, {110.053 - _DZ_02_094_}}},
     {"RCDIIT", {{ 1629.775 + _DX_07_436_},  {  88.9}, {110.053 - _DZ_10_345_}}},
     {"RCDIIM", {{ 1497.301 + _DX_07_436_},  {   0.0}, { 14.826 - _DZ_10_345_}}},
     {"RCDIIB", {{ 1629.775 + _DX_07_436_},  { -88.9}, {110.053 - _DZ_10_345_}}},

     {"RCTIDI", {{ 1409.926 + _DX_00_1091_}, { 132.076 + _DY_00_9296_}, { 23.628 - _DZ_00_3519_}}},
     {"RCTIDO", {{ 1417.023 + _DX_00_1091_}, { 192.544 + _DY_00_9296_}, {190.571 - _DZ_00_3519_}}},
     {"RCTIUI", {{   98.130 + _DX_00_1091_}, { 286.036 + _DY_00_9296_}, { 23.628 - _DZ_00_3519_}}},
     {"RCTIUO", {{  105.227 + _DX_00_1091_}, { 346.504 + _DY_00_9296_}, {190.571 - _DZ_00_3519_}}},

     {"RCBIDI", {{ 1409.926 + _DX_00_1091_}, {-132.076 - _DY_00_9296_}, { 23.628 - _DZ_00_3519_}}},
     {"RCBIDO", {{ 1417.023 + _DX_00_1091_}, {-192.544 - _DY_00_9296_}, {190.571 - _DZ_00_3519_}}},
     // Yes, YZ-coordinates of these two neighboring points seem to be a bit  
     // screwed up in a similar way; do not mind to introduce 2 extra parameters;
     {"RCBIUI", {{   98.130 + _DX_00_1091_}, {-286.036 - _DY_00_9296_, {{&wc_ri_fy, 1.}}}, 
		 { 23.628 - _DZ_00_3519_, {{&wc_ri_fz, 1.}}}}},
     {"RCBIUO", {{  105.227 + _DX_00_1091_}, {-346.504 - _DY_00_9296_, {{&wc_ri_fy, 1.}}}, 
		 {190.571 - _DZ_00_3519_, {{&wc_ri_fz, 1.}}}}},
    }};


t_oly_object wc_lm = {
    "WC_LM", 1, NULL,  _73_54_DEGREES_, {"olympus/Driftkammer-gross-IST.txt"},
    _WC_MATCHING_ACCURACY_XY_, _WC_MATCHING_ACCURACY_ZZ_, _6PAR_, {}, 1200., {},
    // Only 3 downstream marks available;
    {{"LCDMIT", {{-2270.259 - _DX_07_436_},  { 152.4},                  {160.500 - _DZ_10_345_}}},
     {"LCDMIM", {{-2067.606 - _DX_07_436_},  {   0.0},                  { 14.826 - _DZ_10_345_}}},
     {"LCDMIB", {{-2270.259 - _DX_07_436_},  {-152.4},                  {160.500 - _DZ_10_345_}}},

     // Only 3 marks available for the top side;
     {"LCTMDI", {{-1992.814 - _DX_00_1091_}, { 220.679 + _DY_00_9296_}, { 34.763 - _DZ_00_3519_}}},
     {"LCTMDO", {{-2004.582 - _DX_00_1091_}, { 320.947 + _DY_00_9296_}, {303.279 - _DZ_00_3519_}}},
     {"LCTMUO", {{  -87.342 - _DX_00_1091_}, { 545.965 + _DY_00_9296_}, {303.279 - _DZ_00_3519_}}},

     {"LCBMDI", {{-1992.814 - _DX_00_1091_}, {-220.679 - _DY_00_9296_}, { 34.763 - _DZ_00_3519_}}},
     {"LCBMDO", {{-2004.582 - _DX_00_1091_}, {-320.947 - _DY_00_9296_}, {303.279 - _DZ_00_3519_}}},
     {"LCBMUI", {{  -75.574 - _DX_00_1091_}, {-445.698 - _DY_00_9296_}, { 34.763 - _DZ_00_3519_}}},
     {"LCBMUO", {{  -87.342 - _DX_00_1091_}, {-545.965 - _DY_00_9296_}, {303.279 - _DZ_00_3519_}}},
    }};

t_oly_object wc_rm = {
    "WC_RM", 1, NULL, -_73_54_DEGREES_, {"olympus/Driftkammer-gross-IST.txt"},
    _WC_MATCHING_ACCURACY_XY_, _WC_MATCHING_ACCURACY_ZZ_, _6PAR_, {}, 1200., {},
    {{"RCUMIT", {{  -26.743 - _DX_12_576_},  { 381.0},                  {160.500 - _DZ_02_094_}}},
     {"RCUMIM", {{   -4.175 - _DX_12_576_},  {   0.0},                  { 25.055 - _DZ_02_094_}}},
     {"RCUMIB", {{  -26.743 - _DX_12_576_},  {-381.0},                  {160.500 - _DZ_02_094_}}},
     {"RCDMIT", {{ 2270.259 + _DX_07_436_},  { 152.4},                  {160.500 - _DZ_10_345_}}},
     {"RCDMIM", {{ 2067.606 + _DX_07_436_},  {   0.0},                  { 14.826 - _DZ_10_345_}}},
     {"RCDMIB", {{ 2270.259 + _DX_07_436_},  {-152.4},                  {160.500 - _DZ_10_345_}}},

     {"RCTMDI", {{ 1992.814 + _DX_00_1091_}, { 220.679 + _DY_00_9296_}, { 34.763 - _DZ_00_3519_}}},
     {"RCTMDO", {{ 2004.582 + _DX_00_1091_}, { 320.947 + _DY_00_9296_}, {303.279 - _DZ_00_3519_}}},
     {"RCTMUI", {{   75.574 + _DX_00_1091_}, { 445.698 + _DY_00_9296_}, { 34.763 - _DZ_00_3519_}}},
     {"RCTMUO", {{   87.342 + _DX_00_1091_}, { 545.965 + _DY_00_9296_}, {303.279 - _DZ_00_3519_}}},

     {"RCBMDI", {{ 1992.814 + _DX_00_1091_}, {-220.679 - _DY_00_9296_}, { 34.763 - _DZ_00_3519_}}},
     {"RCBMDO", {{ 2004.582 + _DX_00_1091_}, {-320.947 - _DY_00_9296_}, {303.279 - _DZ_00_3519_}}},
     {"RCBMUI", {{   75.574 + _DX_00_1091_}, {-445.698 - _DY_00_9296_}, { 34.763 - _DZ_00_3519_}}},
     {"RCBMUO", {{   87.342 + _DX_00_1091_}, {-545.965 - _DY_00_9296_}, {303.279 - _DZ_00_3519_}}},
    }};

t_oly_object wc_lo = {
    "WC_LO", 1, NULL,  _73_54_DEGREES_, {"olympus/Driftkammer-gross-IST.txt"},
    _WC_MATCHING_ACCURACY_XY_, _WC_MATCHING_ACCURACY_ZZ_, _6PAR_, {}, 1500., {},
    // Ok, only top & bottom sides were surveyed for this chamber;
    {{"LCTODI", {{-2595.277 - _DX_00_1091_}, { 332.828 + _DY_00_9296_, {{&wc_lo_fy, .5}}}, { 17.721 - _DZ_00_3519_}}},
     {"LCTODO", {{-2604.199 - _DX_00_1091_}, { 408.842 + _DY_00_9296_, {{&wc_lo_fy, .5}}}, {199.086 - _DZ_00_3519_}}},
     {"LCTOUI", {{ -173.500 - _DX_00_1091_}, { 617.061 + _DY_00_9296_, {{&wc_lo_fy, .5}}}, { 17.721 - _DZ_00_3519_}}},
     {"LCTOUO", {{ -182.421 - _DX_00_1091_}, { 693.075 + _DY_00_9296_, {{&wc_lo_fy, .5}}}, {199.086 - _DZ_00_3519_}}},

     {"LCBODI", {{-2595.277 - _DX_00_1091_}, {-332.828 - _DY_00_9296_, {{&wc_lo_fy,-.5}}}, { 17.721 - _DZ_00_3519_}}},
     {"LCBODO", {{-2604.199 - _DX_00_1091_}, {-408.842 - _DY_00_9296_, {{&wc_lo_fy,-.5}}}, {199.086 - _DZ_00_3519_}}},
     {"LCBOUI", {{ -173.500 - _DX_00_1091_}, {-617.061 - _DY_00_9296_, {{&wc_lo_fy,-.5}}}, { 17.721 - _DZ_00_3519_}}},
     {"LCBOUO", {{ -182.421 - _DX_00_1091_}, {-693.075 - _DY_00_9296_, {{&wc_lo_fy,-.5}}}, {199.086 - _DZ_00_3519_}}},
    }};

t_oly_object wc_ro = {
    "WC_RO", 1, NULL, -_73_54_DEGREES_, {"olympus/Driftkammer-gross-IST.txt"},
    _WC_MATCHING_ACCURACY_XY_, _WC_MATCHING_ACCURACY_ZZ_, _6PAR_, {}, 1500., {},
    {{"RCUOIT", {{  -18.309 - _DX_12_576_, {{&wc_ro_fx, 1.}}}, {  533.4}, {109.880 - _DZ_02_094_, {{&wc_ro_fz, 1.}}}}},
     {"RCUOIM", {{   -4.175 - _DX_12_576_, {{&wc_ro_fx, 1.}}}, {    0.0}, { 25.055 - _DZ_02_094_, {{&wc_ro_fz, 1.}}}}},
     {"RCUOIB", {{  -18.309 - _DX_12_576_, {{&wc_ro_fx, 1.}}}, { -533.4}, {109.880 - _DZ_02_094_, {{&wc_ro_fz, 1.}}}}},

     {"RCTODI", {{ 2595.277 + _DX_00_1091_}, { 332.828 + _DY_00_9296_, {{&wc_ro_fy, .5}}}, { 17.721 - _DZ_00_3519_}}},
     {"RCTODO", {{ 2604.199 + _DX_00_1091_}, { 408.842 + _DY_00_9296_, {{&wc_ro_fy, .5}}}, {199.086 - _DZ_00_3519_}}},
     {"RCTOUI", {{  173.500 + _DX_00_1091_}, { 617.061 + _DY_00_9296_, {{&wc_ro_fy, .5}}}, { 17.721 - _DZ_00_3519_}}},
     {"RCTOUO", {{  182.421 + _DX_00_1091_}, { 693.075 + _DY_00_9296_, {{&wc_ro_fy, .5}}}, {199.086 - _DZ_00_3519_}}},

     {"RCBODI", {{ 2595.277 + _DX_00_1091_}, {-332.828 - _DY_00_9296_, {{&wc_ro_fy,-.5}}}, { 17.721 - _DZ_00_3519_}}},
     {"RCBODO", {{ 2604.199 + _DX_00_1091_}, {-408.842 - _DY_00_9296_, {{&wc_ro_fy,-.5}}}, {199.086 - _DZ_00_3519_}}},
     {"RCBOUI", {{  173.500 + _DX_00_1091_}, {-617.061 - _DY_00_9296_, {{&wc_ro_fy,-.5}}}, { 17.721 - _DZ_00_3519_}}},
     {"RCBOUO", {{  182.421 + _DX_00_1091_}, {-693.075 - _DY_00_9296_, {{&wc_ro_fy,-.5}}}, {199.086 - _DZ_00_3519_}}},
    }};

/* ------------------------------------------------------------------------------------ */
