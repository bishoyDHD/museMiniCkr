/* ----------------------------------------------------------------- */
/*  tof.c                                                            */
/*                                                                   */
/*    OLYMPUS ToF scintillators.                                     */
/*                                                                   */
/*  A.Kisselev, PNPI, St.Petersburg, Russia.                         */
/*    e-mail: kisselev@hermes.desy.de                                */
/* ----------------------------------------------------------------- */

#include <cstdlib>

#include <survey.h>

/* ---- ToF detector geometry definitions -------------------------- */

// It looks like all the paddles of a given group are pretty much 
// "in plane", but spacing is an issue; forward (short) wall has 
// definitely better flatness;
#define _TOF_MATCHING_ACCURACY_SMALL_XY_  ( 2.50)
#define _TOF_MATCHING_ACCURACY_SMALL_ZZ_  ( 0.20)
#define _TOF_MATCHING_ACCURACY_LARGE_XY_  ( 1.50)
#define _TOF_MATCHING_ACCURACY_LARGE_ZZ_  ( 0.50)

#define _BB_LENGTH_SMALL_DEFAULT_       (1290.00)
#define _BB_WIDTH_SMALL_DEFAULT_        ( 165.00)
#define _BB_STEP_SMALL_DEFAULT_         ( 165.00)

#define _BB_LENGTH_LARGE_DEFAULT_       (1900.00)
#define _BB_WIDTH_LARGE_DEFAULT_        ( 250.00)
#define _BB_STEP_LARGE_DEFAULT_         ( 265.00)

// Offset of backbone survey points with respect to the scintillator middle in transverse
// direction; in fact the major source of systematics; 
//#define _BB_TO_SC_OFFSET_SMALL_         ( 114.00)
//#define _BB_TO_SC_OFFSET_LARGE_         ( 121.00)
// Measured by hand 2011/11/09 -> thickness is identical for small and large plates; 
// 30mm offset is due to the adapter used by M.Noak; 105mm from backbone outer surface
// to the wrapper scintillator; 13mm is half wrapped plate thickness (which is ~28mm)
// offset by 1mm in order to account thin lead plate in front of the actual scintillator
// volume (NB: scintillator itself is 1" thick);
#define _BB_TO_SC_OFFSET_SMALL_         (  30.00 + 105.00 + 13.00)
#define _BB_TO_SC_OFFSET_LARGE_         _BB_TO_SC_OFFSET_SMALL_

// Intrinsic paddle widths;
#define _SC_WIDTH_SMALL_                ( 178.00)
#define _SC_WIDTH_LARGE_                ( 262.00)
// Scintillator thickness;
#define _SC_THICKNESS_                  (  25.40)

static t_oly_variable bb_length_small = {"BB_LN_SML", "[mm]", "ToF backbone height (small)", _BB_LENGTH_SMALL_DEFAULT_, 0};
static t_oly_variable bb_width_small  = {"BB_WD_SML", "[mm]", "ToF backbone width  (small)", _BB_WIDTH_SMALL_DEFAULT_,  0};
static t_oly_variable bb_step_small   = {"BB_ST_SML", "[mm]", "ToF 'granularity'   (small)", _BB_STEP_SMALL_DEFAULT_,   0};

static t_oly_variable bb_length_large = {"BB_LN_BIG", "[mm]", "ToF backbone height (large)", _BB_LENGTH_LARGE_DEFAULT_};
static t_oly_variable bb_width_large  = {"BB_WD_BIG", "[mm]", "ToF backbone width  (large)", _BB_WIDTH_LARGE_DEFAULT_};
static t_oly_variable bb_step_large   = {"BB_ST_BIG", "[mm]", "ToF 'granularity'   (large)", _BB_STEP_LARGE_DEFAULT_};

static t_oly_variable fx              = {"FX",        "[mm]", 0, 0.0};
static t_oly_variable fy              = {"FY",        "[mm]", 0, 0.0};
static t_oly_variable fz              = {"FZ",        "[mm]", 0, 0.0};

t_oly_object tof_lf = {
    "TOF_LF",  4, &bb_step_small.value, _25_00_DEGREES_, {"olympus/Flugzeitdetektor-IST.txt"},
    _TOF_MATCHING_ACCURACY_SMALL_XY_, _TOF_MATCHING_ACCURACY_SMALL_ZZ_, _6PAR_, {}, 4000., {},
    {	// Plate #01;
	{"FD4.1",  {{ 0., {{&bb_step_small, -1.5}, {&bb_width_small, -0.5}}}, { 0., {{&bb_length_small, -0.5}}}, { _BB_TO_SC_OFFSET_SMALL_}}},
	{"FD4.2",  {{ 0., {{&bb_step_small, -1.5}, {&bb_width_small,  0.5}}}, { 0., {{&bb_length_small, -0.5}}}, { _BB_TO_SC_OFFSET_SMALL_}}},
	{"FD4.8",  {{ 0., {{&bb_step_small, -1.5}, {&bb_width_small, -0.5}}}, { 0., {{&bb_length_small,  0.5}}}, { _BB_TO_SC_OFFSET_SMALL_}}},
	{"FD4.10", {{ 0., {{&bb_step_small, -1.5}, {&bb_width_small,  0.5}}}, { 0., {{&bb_length_small,  0.5}}}, { _BB_TO_SC_OFFSET_SMALL_}}},

	// Plate #02;
	{"FD4.3",  {{ 0., {{&bb_step_small, -0.5}, {&bb_width_small, -0.5}}}, { 0., {{&bb_length_small, -0.5}}}, { _BB_TO_SC_OFFSET_SMALL_}}},
	{"FD4.4",  {{ 0., {{&bb_step_small, -0.5}, {&bb_width_small,  0.5}}}, { 0., {{&bb_length_small, -0.5}}}, { _BB_TO_SC_OFFSET_SMALL_}}},
	{"FD4.12", {{ 0., {{&bb_step_small, -0.5}, {&bb_width_small,  0.5}}}, { 0., {{&bb_length_small,  0.5}}}, { _BB_TO_SC_OFFSET_SMALL_}}},
	{"FD4.11", {{ 0., {{&bb_step_small, -0.5}, {&bb_width_small,  0.0}}}, { 0., {{&bb_length_small,  0.5}}}, { _BB_TO_SC_OFFSET_SMALL_}}},

	// Plate #03;
	{"FD4.5",  {{ 0., {{&bb_step_small,  0.5}, {&bb_width_small, -0.5}}}, { 0., {{&bb_length_small, -0.5}}}, { _BB_TO_SC_OFFSET_SMALL_}}},
	{"FD4.6",  {{ 0., {{&bb_step_small,  0.5}, {&bb_width_small,  0.5}}}, { 0., {{&bb_length_small, -0.5}}}, { _BB_TO_SC_OFFSET_SMALL_}}},
	{"FD4.13", {{ 0., {{&bb_step_small,  0.5}, {&bb_width_small,  0.0}}}, { 0., {{&bb_length_small,  0.5}}}, { _BB_TO_SC_OFFSET_SMALL_}}},

	// Plate #04;
	{"FD4.7",  {{ 0., {{&bb_step_small,  1.5}, {&bb_width_small, -0.5}}}, { 0., {{&bb_length_small, -0.5}}}, { _BB_TO_SC_OFFSET_SMALL_}}},
	{"FD4.15", {{ 0., {{&bb_step_small,  1.5}, {&bb_width_small, -0.5}}}, { 0., {{&bb_length_small,  0.5}}}, { _BB_TO_SC_OFFSET_SMALL_}}},

	{"LF#1",   {{-_SC_WIDTH_SMALL_/2., {{&bb_step_small, -1.5}}},         { 0.},                             {-_SC_THICKNESS_/2.}}, 1},
    }};

t_oly_object tof_rf = {
    "TOF_RF",  4, &bb_step_small.value, -_25_00_DEGREES_, {"olympus/Flugzeitdetektor-IST.txt"},
    _TOF_MATCHING_ACCURACY_SMALL_XY_, _TOF_MATCHING_ACCURACY_SMALL_ZZ_, _6PAR_, {}, 4000., {},
    {	// Plate #01;
	{"FD3.1",  {{ 0., {{&bb_step_small,  1.5}, {&bb_width_small,  0.5}}}, { 0., {{&bb_length_small, -0.5}}}, { _BB_TO_SC_OFFSET_SMALL_}}},
	{"FD3.2",  {{ 0., {{&bb_step_small,  1.5}, {&bb_width_small, -0.5}}}, { 0., {{&bb_length_small, -0.5}}}, { _BB_TO_SC_OFFSET_SMALL_}}},
	//{"FD3.9",  {{ 0., {{&bb_step_small,  1.5}, {&bb_width_small,  0.5}}}, { 0., {{&bb_length_small,  0.5}}}, { _BB_TO_SC_OFFSET_SMALL_, {{&fix, 1.}}}}},

	// Plate #02;
	{"FD3.3",  {{ 0., {{&bb_step_small,  0.5}, {&bb_width_small,  0.5}}}, { 0., {{&bb_length_small, -0.5}}}, { _BB_TO_SC_OFFSET_SMALL_}}},
	{"FD3.4",  {{ 0., {{&bb_step_small,  0.5}, {&bb_width_small, -0.5}}}, { 0., {{&bb_length_small, -0.5}}}, { _BB_TO_SC_OFFSET_SMALL_}}},
	//{"FD3.11", {{ 0., {{&bb_step_small,  0.5}, {&bb_width_small, -0.5}}}, { 0., {{&bb_length_small,  0.5}}}, { _BB_TO_SC_OFFSET_SMALL_, {{&fix, 1.}}}}},

	// Plate #03;
	{"FD3.5",  {{ 0., {{&bb_step_small, -0.5}, {&bb_width_small,  0.5}}}, { 0., {{&bb_length_small, -0.5}}}, { _BB_TO_SC_OFFSET_SMALL_}}},
	{"FD3.6",  {{ 0., {{&bb_step_small, -0.5}, {&bb_width_small, -0.5}}}, { 0., {{&bb_length_small, -0.5}}}, { _BB_TO_SC_OFFSET_SMALL_}}},
	//{"FD3.12", {{ 0., {{&bb_step_small, -0.5}, {&bb_width_small,  0.5}}}, { 0., {{&bb_length_small,  0.5}}}, { _BB_TO_SC_OFFSET_SMALL_}}},
	//{"FD3.13", {{ 0., {{&bb_step_small, -0.5}, {&bb_width_small,  0.0}}}, { 0., {{&bb_length_small,  0.5}}}, { _BB_TO_SC_OFFSET_SMALL_}}},
	{"FD3.14", {{ 0., {{&bb_step_small, -0.5}, {&bb_width_small, -0.5}}}, { 0., {{&bb_length_small,  0.5}}}, { _BB_TO_SC_OFFSET_SMALL_}}},

	// Plate #04;
	{"FD3.7",  {{ 0., {{&bb_step_small, -1.5}, {&bb_width_small,  0.5}}}, { 0., {{&bb_length_small, -0.5}}}, { _BB_TO_SC_OFFSET_SMALL_}}},
	{"FD3.15", {{ 0., {{&bb_step_small, -1.5}, {&bb_width_small,  0.5}}}, { 0., {{&bb_length_small,  0.5}}}, { _BB_TO_SC_OFFSET_SMALL_}}},

	{"RF#1",   {{ _SC_WIDTH_SMALL_/2., {{&bb_step_small,  1.5}}},         { 0.},                             {-_SC_THICKNESS_/2.}}, 1},
	{"RF#2",   {{-_SC_WIDTH_SMALL_/2., {{&bb_step_small, -1.5}}},         { 0.},                             {-_SC_THICKNESS_/2.}}, 1},
    }};

t_oly_object tof_lm = {
    "TOF_LM",  5, &bb_step_large.value, _64_00_DEGREES_, {"olympus/Flugzeitdetektor-IST.txt"},
    _TOF_MATCHING_ACCURACY_LARGE_XY_, _TOF_MATCHING_ACCURACY_LARGE_ZZ_, _6PAR_, {}, 3500., {},
    {	 // Plate #05;
	{"FD5.11", {{ 0., {{&bb_step_large, -2.0}, {&bb_width_large, -0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD5.1",  {{ 0., {{&bb_step_large, -2.0}, {&bb_width_large, -0.5}}}, { 0., {{&bb_length_large, -0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD5.2",  {{ 0., {{&bb_step_large, -2.0}, {&bb_width_large,  0.5}}}, { 0., {{&bb_length_large, -0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	
	// Plate #06;
	{"FD5.14", {{ 0., {{&bb_step_large, -1.0}, {&bb_width_large,  0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD5.4",  {{ 0., {{&bb_step_large, -1.0}, {&bb_width_large,  0.5}}}, { 0., {{&bb_length_large, -0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD5.3",  {{ 0., {{&bb_step_large, -1.0}, {&bb_width_large, -0.5}}}, { 0., {{&bb_length_large, -0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},

	// Plate #07;
	{"FD5.16", {{ 0., {{&bb_step_large,  0.0}, {&bb_width_large,  0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD5.15", {{ 0., {{&bb_step_large,  0.0}, {&bb_width_large, -0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD5.5",  {{ 0., {{&bb_step_large,  0.0}, {&bb_width_large, -0.5}}}, { 0., {{&bb_length_large, -0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	
	// Plate #08;
	{"FD5.17", {{ 0., {{&bb_step_large,  1.0}, {&bb_width_large, -0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD5.18", {{ 0., {{&bb_step_large,  1.0}, {&bb_width_large,  0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD5.8",  {{ 0., {{&bb_step_large,  1.0}, {&bb_width_large,  0.5}}}, { 0., {{&bb_length_large, -0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	
	// Plate #09;
	{"FD5.19", {{ 0., {{&bb_step_large,  2.0}, {&bb_width_large, -0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD5.20", {{ 0., {{&bb_step_large,  2.0}, {&bb_width_large,  0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD5.9",  {{ 0., {{&bb_step_large,  2.0}, {&bb_width_large, -0.5}}}, { 0., {{&bb_length_large, -0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
    }};

t_oly_object tof_rm = {
    "TOF_RM",  5, &bb_step_large.value, -_64_00_DEGREES_, {"olympus/Flugzeitdetektor-IST.txt"},
    _TOF_MATCHING_ACCURACY_LARGE_XY_, _TOF_MATCHING_ACCURACY_LARGE_ZZ_, _6PAR_, {}, 3500., {},
    {	 // Plate #05;
	{"FD2.11", {{ 0., {{&bb_step_large,  2.0}, {&bb_width_large,  0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD2.12", {{ 0., {{&bb_step_large,  2.0}, {&bb_width_large, -0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD2.1",  {{ 0., {{&bb_step_large,  2.0}, {&bb_width_large,  0.5}}}, { 0., {{&bb_length_large, -0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD2.2",  {{ 0., {{&bb_step_large,  2.0}, {&bb_width_large, -0.5}}}, { 0., {{&bb_length_large, -0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},

	// Plate #06;
	{"FD2.13", {{ 0., {{&bb_step_large,  1.0}, {&bb_width_large,  0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD2.4",  {{ 0., {{&bb_step_large,  1.0}, {&bb_width_large, -0.5}}}, { 0., {{&bb_length_large, -0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD2.3",  {{ 0., {{&bb_step_large,  1.0}, {&bb_width_large,  0.5}}}, { 0., {{&bb_length_large, -0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},

	// Plate #07; bb_step_large-part could be omitted here of course;
	{"FD2.5",  {{ 0., {{&bb_step_large,  0.0}, {&bb_width_large,  0.5}}}, { 0., {{&bb_length_large, -0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD2.6",  {{ 0., {{&bb_step_large,  0.0}, {&bb_width_large, -0.5}}}, { 0., {{&bb_length_large, -0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD2.16", {{ 0., {{&bb_step_large,  0.0}, {&bb_width_large, -0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},

	// Plate #08;
	{"FD2.17", {{ 0., {{&bb_step_large, -1.0}, {&bb_width_large,  0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD2.18", {{ 0., {{&bb_step_large, -1.0}, {&bb_width_large, -0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD2.7",  {{ 0., {{&bb_step_large, -1.0}, {&bb_width_large,  0.5}}}, { 0., {{&bb_length_large, -0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}}, 

	// Plate #09;
	{"FD2.10", {{ 0., {{&bb_step_large, -2.0}, {&bb_width_large, -0.5}}}, { 0., {{&bb_length_large, -0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD2.19", {{ 0., {{&bb_step_large, -2.0}, {&bb_width_large,  0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}}, 
	{"FD2.20", {{ 0., {{&bb_step_large, -2.0}, {&bb_width_large, -0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}}, 

	{"RM#1",   {{ _SC_WIDTH_LARGE_/2., {{&bb_step_large,  1.0}}},         { 0.},                             {-_SC_THICKNESS_/2.}}, 1},
	{"RM#2",   {{ _SC_WIDTH_LARGE_/2., {{&bb_step_large, -2.0}}},         { 0.},                             {-_SC_THICKNESS_/2.}}, 1},
	{"RM#3",   {{-_SC_WIDTH_LARGE_/2., {{&bb_step_large, -2.0}}},         { 0.},                             {-_SC_THICKNESS_/2.}}, 1},
    }};

t_oly_object tof_lb = {
    "TOF_LB",  9, &bb_step_large.value, 90., {"olympus/Flugzeitdetektor-IST.txt"},
    _TOF_MATCHING_ACCURACY_LARGE_XY_, _TOF_MATCHING_ACCURACY_LARGE_ZZ_, _6PAR_, {}, 2600., {0., 0., 31.},
    {	// Plate #10; -> outlier!;
        //{"FD6.42", {{ 0., {{&bb_step_large, -4.0}, {&bb_width_large,  0.5}, {&lx10, 1.}}}, { 0., {{&bb_length_large,  0.5}}}, 
	//  { _BB_TO_SC_OFFSET_LARGE_}}},
        //{"FD6.41", {{ 0., {{&bb_step_large, -4.0}, {&bb_width_large,  0.5}, {&lx10, 1.}}}, { 0., {{&bb_length_large, -0.5}}}, 
	//{ _BB_TO_SC_OFFSET_LARGE_}}},
 
	// Plate #11 -> 10 as of 2011/09/29 -> need to check once again;
	{"FD6.2",  {{ 0., {{&bb_step_large, -4.0}, {&bb_width_large,  0.5}}}, { 0., {{&bb_length_large, -0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD6.1",  {{ 0., {{&bb_step_large, -4.0}, {&bb_width_large, -0.5}}}, { 0., {{&bb_length_large, -0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD6.18", {{ 0., {{&bb_step_large, -4.0}, {&bb_width_large,  0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD6.17", {{ 0., {{&bb_step_large, -4.0}, {&bb_width_large, -0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},

	// Plate #12 -> 11;
	{"FD6.20", {{ 0., {{&bb_step_large, -3.0}, {&bb_width_large,  0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD6.19", {{ 0., {{&bb_step_large, -3.0}, {&bb_width_large, -0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD6.3",  {{ 0., {{&bb_step_large, -3.0}, {&bb_width_large, -0.5}}}, { 0., {{&bb_length_large, -0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},

	// Plate #13 -> 12;
	{"FD6.21", {{ 0., {{&bb_step_large, -2.0}, {&bb_width_large, -0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},

	// Plate #14 -> 13;
	{"FD6.8",  {{ 0., {{&bb_step_large, -1.0}, {&bb_width_large,  0.5}}}, { 0., {{&bb_length_large, -0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD6.24", {{ 0., {{&bb_step_large, -1.0}, {&bb_width_large,  0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},

	// Plate #15 -> 14;
	{"FD6.9",  {{ 0., {{&bb_step_large,  0.0}, {&bb_width_large, -0.5}}}, { 0., {{&bb_length_large, -0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD6.10", {{ 0., {{&bb_step_large,  0.0}, {&bb_width_large,  0.5}}}, { 0., {{&bb_length_large, -0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD6.25", {{ 0., {{&bb_step_large,  0.0}, {&bb_width_large, -0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD6.26", {{ 0., {{&bb_step_large,  0.0}, {&bb_width_large,  0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},

	// Plate #16 -> 15;
	{"FD6.12", {{ 0., {{&bb_step_large,  1.0}, {&bb_width_large,  0.5}}}, { 0., {{&bb_length_large, -0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD6.11", {{ 0., {{&bb_step_large,  1.0}, {&bb_width_large, -0.5}}}, { 0., {{&bb_length_large, -0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD6.27", {{ 0., {{&bb_step_large,  1.0}, {&bb_width_large, -0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},

	// Plate #17 -> 16;
	{"FD6.13", {{ 0., {{&bb_step_large,  2.0}, {&bb_width_large, -0.5}}}, { 0., {{&bb_length_large, -0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},

	// Plate #18; -> outlier!;
	//{"FD6.16", {{ 0., {{&bb_step_large,  4.0}, {&bb_width_large,  0.5}, {&fx, 1.}}}, { 0., {{&bb_length_large, -0.5}, {&fy, 1.}}}, 
	//    { _BB_TO_SC_OFFSET_LARGE_, {{&fz, 1.}}}}},
	//{"FD6.32", {{ 0., {{&bb_step_large,  4.0}, {&bb_width_large,  0.5}, {&fx, 1.}}}, { 0., {{&bb_length_large,  0.5}, {&fy, 1.}}}, 
	//    { _BB_TO_SC_OFFSET_LARGE_, {{&fz, 1.}}}}},

	{"LB#2",   {{ _SC_WIDTH_LARGE_/2., {{&bb_step_large,  4.0}}},         { 0.},                             {-_SC_THICKNESS_/2.}}, 1},
    }};

t_oly_object tof_rb = {
    "TOF_RB",  9, &bb_step_large.value, -90., {"olympus/Flugzeitdetektor-IST.txt"},
    _TOF_MATCHING_ACCURACY_LARGE_XY_, _TOF_MATCHING_ACCURACY_LARGE_ZZ_, _6PAR_, {}, 2600., {0., 0., 30.},
    {	// Plate #10; -> outlier!;
        //{"FD1.42", {{ 0., {{&bb_step_large,  4.0}, {&bb_width_large, -0.5}, {&fix, 1.}}}, { 0., {{&bb_length_large,  0.5}}}, 
	//    { _BB_TO_SC_OFFSET_LARGE_}}},
        //{"FD1.41", {{ 0., {{&bb_step_large,  4.0}, {&bb_width_large, -0.5}, {&fix, 1.}}}, { 0., {{&bb_length_large, -0.5}}}, 
	//    { _BB_TO_SC_OFFSET_LARGE_}}},
	
	// Plate #11 -> 10 as of 2011/09/29 -> need to check once again; 
	{"FD1.1",  {{ 0., {{&bb_step_large,  4.0}, {&bb_width_large,  0.5}}}, { 0., {{&bb_length_large, -0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD1.18", {{ 0., {{&bb_step_large,  4.0}, {&bb_width_large, -0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD1.17", {{ 0., {{&bb_step_large,  4.0}, {&bb_width_large,  0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},

	// Plate #12 -> 11;
	{"FD1.19", {{ 0., {{&bb_step_large,  3.0}, {&bb_width_large,  0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},

	// Plate #13 -> 12;
	{"FD1.22", {{ 0., {{&bb_step_large,  2.0}, {&bb_width_large, -0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD1.6",  {{ 0., {{&bb_step_large,  2.0}, {&bb_width_large, -0.5}}}, { 0., {{&bb_length_large, -0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},

	// Plate #14 -> 13;
	{"FD1.23", {{ 0., {{&bb_step_large,  1.0}, {&bb_width_large,  0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD1.24", {{ 0., {{&bb_step_large,  1.0}, {&bb_width_large, -0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD1.7",  {{ 0., {{&bb_step_large,  1.0}, {&bb_width_large,  0.5}}}, { 0., {{&bb_length_large, -0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},

	// Plate #15 -> 14;
	{"FD1.25", {{ 0., {{&bb_step_large,  0.0}, {&bb_width_large,  0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},

	// Plate #16 -> 15;
	{"FD1.12", {{ 0., {{&bb_step_large, -1.0}, {&bb_width_large, -0.5}}}, { 0., {{&bb_length_large, -0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD1.28", {{ 0., {{&bb_step_large, -1.0}, {&bb_width_large, -0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},

	// Plate #17 -> 16;
	{"FD1.13", {{ 0., {{&bb_step_large, -2.0}, {&bb_width_large,  0.5}}}, { 0., {{&bb_length_large, -0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},
	{"FD1.29", {{ 0., {{&bb_step_large, -2.0}, {&bb_width_large,  0.5}}}, { 0., {{&bb_length_large,  0.5}}}, { _BB_TO_SC_OFFSET_LARGE_}}},

	// Plate #18; -> outlier!;
	//{"FD1.16", {{ 0., {{&bb_step_large, -4.0}, {&bb_width_large, -0.5}, {&fx, 1.}}}, 
	//    { 0., {{&bb_length_large, -0.5}, {&fy, 1.}}}, 
	//    { _BB_TO_SC_OFFSET_LARGE_, {{&fz, 1.}}}}},
	//{"FD1.32", {{ 0., {{&bb_step_large, -4.0}, {&bb_width_large, -0.5}, {&fx, 1.}}}, 
	//  { 0., {{&bb_length_large,  0.5}, {&fy, 1.}}}, 
	//  { _BB_TO_SC_OFFSET_LARGE_, {{&fz, 1.}}}}},
	//{"FD1.43", {{ 0., {{&bb_step_large, -5.0}, {&bb_width_large, -0.5}, {&fx, 1.}}}, 
	//  { 0., {{&bb_length_large,  0.5}, {&fy, 1.}}}, 
	//  { _BB_TO_SC_OFFSET_LARGE_, {{&fz, 1.}}}}},

	{"RB#1",   {{ _SC_WIDTH_LARGE_/2., {{&bb_step_large,  4.0}}},         { 0.},                             {-_SC_THICKNESS_/2.}}, 1},
	{"RB#2",   {{-_SC_WIDTH_LARGE_/2., {{&bb_step_large, -4.0}}},         { 0.},                             {-_SC_THICKNESS_/2.}}, 1},
	// Z-coordinate should match RFUOM; differs by ~30mm in fact; better than 1m, right?;
	{"RB#3",   {{-_SC_WIDTH_LARGE_/2.-133., {{&bb_step_large, -4.0}}},    { 0.},                             {-_SC_THICKNESS_/2.}}, 1},
    }};


#if 0
FD3.8           3975.8538       -1323.712       678.574
FD3.9           3975.9872       -1324.486       678.5535    #01 + 

FD3.11          3839.4394       -1642.0952      681.0039    #02 +

FD3.12          3835.4678       -1651.4458      678.8337    #03 + ?
FD3.13          3799.7364       -1734.8015      676.8362    #03 - (middle)
FD3.14          3763.4099       -1818.954       675.8786    #03 + ?

FD3.15          3759.8115       -1827.2332      674.5165    #04 + ?

FD2.11          3758.8465       -2119.7453      936.8707    #05 +
FD2.12          3537.4882       -2229.327       940.0178    #05 +

FD2.13          3522.6226       -2236.7093      940.9366    #06 +

FD2.16          3062.5547       -2465.034       939.4157    #07 +

FD2.17          3046.6178       -2473.502       939.5203    #08 +
FD2.18          2823.4273       -2583.8623      939.6732    #08 +

FD2.19          2809.5469       -2591.1726      938.2849    #09 +
FD2.20          2586.3345       -2701.6891      940.2679    #09 +

FD1.42		2473.2083	-2653.7563	943.2874  #10 -

FD1.18		2195.3606	-2655.2841	943.5413  #11 +
FD1.17		2443.7373	-2655.4586	943.2975  #11 +

FD1.19		2178.7419	-2654.9265	942.0939  #12 +

FD1.22		1664.5373	-2654.9001	944.0221  #13 +

FD1.24		1397.7601	-2654.5679	945.1293  #14 +
FD1.23		1645.9367	-2654.7377	944.7599  #14 +

FD1.25		1380.6691	-2654.8781	943.8916  #15 + ?

FD1.28		867.2065	-2654.2734	943.7535  #16 +

FD1.29		851.2522	-2653.6644	943.8469  #17 +

FD1.32		333.5153	-2650.9731	937.2857  #18 -

FD1.43		102.0803	-2652.0913	935.6112


FD3.1           3971.0076       -1325.0198      -615.3009   #01 +
FD3.2           3906.2362       -1477.2061      -614.226    #01 +

FD3.3           3899.841        -1493.4204      -614.2466   #02 +
FD3.4           3834.3835       -1646.0128      -612.8476   #02 +

FD3.5           3828.6188       -1661.5119      -617.1413   #03 +
FD3.6           3762.7641       -1815.3627      -615.8951   #03 +

FD3.7           3756.4583       -1831.0076      -619.4587   #04 + ?

FD2.1           3754.6265       -2121.6048      -965.6891   #05 +
FD2.2           3531.6042       -2232.2516      -964.5564   #05 +

FD2.3           3516.7429       -2238.6789      -963.3317   #06 +
FD2.4           3293.7059       -2349.0078      -962.8138   #06 +

FD2.5           3279.0169       -2356.1938      -961.57     #07 +
FD2.6           3054.9324       -2466.5127      -960.33     #07 +

FD2.7           3040.1076       -2473.6897      -960.612    #08 +

FD2.10          2580.5036       -2700.6692      -961.2806   #09 +

FD1.41		2473.0621	-2653.6205	-957.4135 #10 -

FD1.1		2442.9762	-2654.6361	-957.9455 #11 +

FD1.6		1663.2175	-2655.0958	-957.1216 #13 +

FD1.7		1646.4405	-2655.556	-956.902  #14 +

FD1.12		869.6855	-2656.2043	-958.4033 #16 +

FD1.13		853.0528	-2656.4561	-958.1532 #17 +

FD1.16		336.063	        -2653.8582	-948.7367 #18 -

FD1.44		101.6918	-2655.8488	-949.6582
#endif


#if 0
FD4.8		3978.919	1337.581	681.7621  #01 +
FD4.9		3944.7928	1414.5479	681.274   
FD4.10		3908.4023	1498.7598	681.2081  #01 +

FD4.11		3870.9094	1583.0475	683.7854  #02 + (middle)
FD4.12		3834.9023	1665.6245	685.2691  #02 + 

FD4.13		3798.1401	1748.3852	682.7617  #03 + (middle)
FD4.15		3758.2304	1840.0415	682.3468  #04 +


FD5.11          3747.0398       2131.2249       943.9242    #05 +

FD5.14          3283.4203       2356.2342       944.4885    #06 +

FD5.15          3271.3013       2361.9195       940.9408    #07 +
FD5.16          3047.8449       2470.9673       943.3846    #07 +

FD5.17          3034.0549       2477.1855       942.942     #08 +
FD5.18          2808.7301       2586.7344       941.6087    #08 +

FD5.19          2793.9855       2593.9385       941.3531    #09 +
FD5.20          2569.6954       2701.9404       940.6727    #09 +

FD6.42		2465.3108	2656.5402	943.2855  #10 -

FD6.18		2184.5153	2654.0097	945.0015  #11 +
FD6.17		2434.1249	2653.7701	943.238   #11 +

FD6.20		1920.1273	2654.3862	943.959   #12 +
FD6.19		2168.155	2654.2424	945.1782  #12 +

FD6.21		1904.9091	2654.42	        944.2623  #13 +

FD6.24		1391.4642	2654.3057	946.843   #14 +

FD6.26		1125.5032	2654.0234	945.1947  #15 +
FD6.25		1376.1009	2654.4348	946.9502  #15 +

FD6.27		1109.7513	2654.5548	945.0359  #16 +

FD6.32		328.2378	2649.2643	933.7488  #18 -

FD6.43		95.1393	        2652.4037	930.7275

FD4.1		3976.1926	1339.2902	-613.53   #01 +
FD4.2		3909.5949	1490.315	-613.5891 #01 +

FD4.3		3902.6082	1506.0925	-612.7464 #02 +
FD4.4		3835.7438	1657.5474	-613.478  #02 +

FD4.5		3829.1377	1673.1877	-614.0512 #03 +
FD4.6		3762.441	1824.8133	-612.5228 #03 +

FD4.7		3755.0693	1839.8966	-611.0798 #04 +


FD5.10          2570.5304       2708.8736       -961.2805

FD5.1           3750.5088       2135.2668       -957.0979   #05 +
FD5.2           3525.0649       2244.7163       -958.4255   #05 +

FD5.3           3510.867        2252.15         -957.8287   #06 +
FD5.4           3286.3757       2361.5483       -960.6839   #06 +

FD5.5           3271.6137       2368.3015       -960.7203   #07 +

FD5.8           2810.4559       2592.6034       -958.9622   #08 +

FD5.9           2795.6865       2599.4995       -959.9894   #09 + 

FD6.41		2469.6253	2659.5955	-957.5366 #10 -

FD6.2		2188.3399	2657.6113	-959.4813 #11 +
FD6.1		2438.951	2656.9994	-957.4242 #11 +

FD6.3		2172.6377	2657.2258	-957.481  #12 +

FD6.8		1396.1392	2656.8258	-958.6709 #14 +

FD6.10		1131.2271	2655.7352	-958.5106 #15 +
FD6.9		1380.5154	2656.0808	-957.9162   #15 +

FD6.12		866.0196	2654.0376	-959.3267 #16 +
FD6.11		1116.129	2655.2201	-957.054  #16 +

FD6.13		850.5948	2654.0831	-959.8357 #17 +

FD6.16		331.8769	2648.9073	-952.2942 #18 -

FD6.44		100.3998	2652.1911	-956.0307
#endif
