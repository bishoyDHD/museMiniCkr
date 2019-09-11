/* ----------------------------------------------------------------- */
/*  survey.h                                                         */
/*                                                                   */
/*    OLYMPUS survey code include file.                              */ 
/*                                                                   */
/*  A.Kisselev, PNPI, St.Petersburg, Russia.                         */
/*    e-mail: kisselev@hermes.desy.de                                */
/* ----------------------------------------------------------------- */
#include <3d.h>

#ifndef _OLY_SURVEY_
#define _OLY_SURVEY_

// Few thing were different in Feb'2011 survey; uncomment if want to 
// go back reanalyze that data (2x MWPCs only);
//#define _DORIS_TEST_RUN_FEB_2011_

// OLYMPUS nominal IP (target cell center) is upstream of the survey
// measurements coordinate system by this number; well, it was so for
// Feb'2011 measurements (DORIS test run) done by W.Benecke; M.Noak 
// presents data in the IP-shifted coordinate system --> ;
#ifdef _DORIS_TEST_RUN_FEB_2011_
#define _OLYMPUS_IP_OFFSET_          (729.9)
#else
#define _OLYMPUS_IP_OFFSET_          (  0.0)
#endif

// ep-elastic monitor installation angle; 
#define _12_00_DEGREES_              (12.00)

// ToF forward wall installation angle;
#define _25_00_DEGREES_              (25.00)
// ToF middle wall installation angle;
#define _64_00_DEGREES_              (64.00)

// WC installation angle; assume that TDR value is correct;
#define _73_54_DEGREES_              (73.54)

typedef struct {
    // This is basically for MINUIT declaration and printout;
    char *name, *units, *comment;

    // Assume step ~1mm, always; 
    double value;

    // If this is set to '1', default value will be used;
    int fixed;


    // Never change parameter order above this line!;


    // Error will be determined by MINUIT;
    double err;
} t_oly_variable;

// Do not need more for now;
#define _EXTRA_VAIABLE_NUM_MAX_ 3

typedef struct {
    t_oly_variable *var;

    double cff;
} t_oly_extra;

typedef struct {
    // Nominal offset perhaps corrected by extra variable value with 
    // an extra coefficient;
    double xx;

    t_oly_extra extra[_EXTRA_VAIABLE_NUM_MAX_];
} t_oly_coord;

typedef struct {
    // Point name;
    char *name;

    // Design coordinate definition;
    t_oly_coord coord[3];

    // If '1', this point is used just to get its coordinates in the global
    // OLYMPUS system, not to match against any survey data;
    int dummy_point_flag;


    // Never change parameter order above this line!;


    // Back door pointer to the mother object;
    void *object;

    // Point 3D coordinates in object local system (which is the 
    // "natural" system with Z along the 12 degree line and Y upwards);
    t_3d_vector _local, global;

    // Point 3D coordinates in OLYMPUS machine (IP-bound) coordinate 
    // system; Z upwards, X along the beam line;
    t_3d_vector survey;

    // Survey entry exists for this point;
    int imported;
} t_oly_point;

// If ever need more, change this number and recompile;
#define _POINTS_PER_OBJECT_MAX_      100

// For the GEMs do not want to allow RX/RY rotations; they can be 
// specified as extra parameters though;
#define _3PAR_                         0
#define _4PAR_                         1
#define _5PPAR_                        2
#define _5MPAR_                        3
#define _6PAR_                         4

// Max number of M.Noak's files with survey point measurements per 
// object; well, in fact typically just 1;
#define _ASCII_FILES_PER_OBJECT_MAX_  10

typedef struct {
    // Object name;
    char *name;

    // Number of X-equidistant elements in this object (useful for scintillator
    // walls, in order to print out 3D locations in a way Doug's Monte-Carlo 
    // expects this input;
    int element_num;
    // Step variable may be different for different objects;
    double *step;

    // Is +/-12 degrees for the Lumi monitor;
    double yrot_angle;

    // Input files with the survey point measurements;
    char *ascii[_ASCII_FILES_PER_OBJECT_MAX_];

    // Expected accuracy in XY- and Z-directions;
    double matching_accuracy_xy, matching_accuracy_zz;

    // Either _6PAR_ or whatever other _*PAR_;
    int mode;
    // May want to fix RX & RY for _4PAR_ case; or perhaps all 
    // 3 rotations in _3PAR_ case;
    double rxyz[3];

    // Expected distance from the IP along the installation angle line; 
    // make MINUIT life a bit easier;
    double expected_distance_to_target;

  // May want to shift object as a whole in OLYMPUS system (as it was
  // needed for the rare ToF walls which were surveyed and then shifted
  // by ~30mm along th ebeam line; really do not want to screw up any 
  // internal calculations by these offsets (so the enter only for 
  // GLOBAL coordinate printout);
  t_3d_vector global_offset_by_hand;

    // Object points;
    t_oly_point points[_POINTS_PER_OBJECT_MAX_];


    // Never change parameter order above this line!;


    // Only a fraction of objects may be activated;
    int active;

    // The actual number of declared points;
    int _actual_point_num, dummy_point_num, true_point_num;

    // A 3D transformation which moves object from its local coordinate 
    // system into the respective +/-12-degree Lumi one;
    t_3d_transformation local_to_lumi;

    // A 3D transformation which is just a +/-12 degree Y-rotation; just
    // assume that it can be independently assigned for every object;
    t_3d_transformation lumi_to_doris, local_to_survey;
    // MINUIT diagonal errors;
    t_3d_vector shift_err, slope_err;
} t_oly_object;

extern t_oly_object tof_lf, tof_lm, tof_lb, tof_rf, tof_rm, tof_rb;
extern t_oly_object bar_l, bar_r;
extern t_oly_object lm_l1, lm_l2, lm_l3, lm_r1, lm_r2, lm_r3;
extern t_oly_object gem_l1, gem_l2, gem_l3, gem_r1, gem_r2, gem_r3;
extern t_oly_object wc_li, wc_ri, wc_lm, wc_rm, wc_lo, wc_ro, wc2sc_l, wc2sc_r;
extern t_oly_object symb_l, symb_r, sc_chamber;

#endif
