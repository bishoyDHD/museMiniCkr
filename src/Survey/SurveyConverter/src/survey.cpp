/* ----------------------------------------------------------------- */
/*  survey.c                                                         */
/*                                                                   */
/*    OLYMPUS Feb'2011 & Jul'2011 survey data analysis.              */
/*                                                                   */
/*  A.Kisselev, PNPI, St.Petersburg, Russia.                         */
/*    e-mail: kisselev@hermes.desy.de                                */
/* ----------------------------------------------------------------- */

#include <assert.h>
#define _GEANT4_OFF_
#include <string>
#include <sc150.h>

#include <survey.h>
#include "TMinuit.h"


int PARNUM=0;
//
// --> allow some configurability; how about comman-line options like 
// in servey.c for HERMES?;
//

//#define _WITH_MWPC_
//#define _WITH_SC_
//#define _WITH_GEM_
//#define _WITH_BAR_
#define _WITH_WC_
//#define _WITH_SYMB_
//#define _WITH_TOF_
//#define _WITH_SC_CHAMBER_

//#define _SINGLE_OBJECT_ "TOF_LF"
//#define _SINGLE_OBJECT_ "WC_RM"

t_oly_object *_oly_objects[] = {
#ifdef _WITH_TOF_
    &tof_lf, &tof_lm, &tof_lb, &tof_rf, &tof_rm, &tof_rb, 
#endif
#ifdef _WITH_BAR_
    &bar_l, &bar_r,
#endif 
#ifdef _WITH_SC_
    &wc2sc_l, &wc2sc_r,
#endif 
#ifdef _WITH_MWPC_
    &lm_l1, &lm_l2, &lm_l3, &lm_r1, &lm_r2, &lm_r3,
#endif 
#ifdef _WITH_GEM_
    &gem_l1, &gem_l2, &gem_l3, &gem_r1, &gem_r2, &gem_r3,
#endif 
#ifdef _WITH_WC_
    &wc_li, &wc_ri, &wc_lm, &wc_rm, &wc_lo, &wc_ro,
#endif
#ifdef _WITH_SYMB_
    &symb_l, &symb_r,
#endif
#ifdef _WITH_SC_CHAMBER_
    &sc_chamber,
#endif
};
#define _OLY_OBJECT_NUM_ (sizeof(_oly_objects)/sizeof(_oly_objects[0]))


TMinuit minuit;
//t_minuit minuit;

int survey_ndf;
double survey_fcn;

char XYZ[3] = {'X', 'Y', 'Z'};

// Yes, prefer array rather than linked list;
static unsigned oly_variable_num;
static t_oly_variable **oly_variables;

inline double sqr(double x) {return x*x;}

/* ================================================================= */

static void recalculate_geometry( void )
{
    int obj, pt, xyz, iq;

    // Loop through all objects;
    for(obj=0; obj<_OLY_OBJECT_NUM_; obj++)
    {
	t_oly_object *object = _oly_objects[obj];

	if (!object->active) continue;

	memset(&object->local_to_survey, 0x00, sizeof(t_3d_transformation));

	// Recalculate the complete 3D transformation;
	assert(!calculate_rotation_matrix(&object->local_to_lumi));
	assert(!mul_t_t(&object->local_to_survey, &object->lumi_to_doris, 
			&object->local_to_lumi));
	
	for(pt=0; pt<object->_actual_point_num; pt++)
	{
	    t_oly_point *point = object->points + pt;

	    // Calculate local coordinates taking possible extra shifts into account;
	    for(xyz=_X_; xyz<=_Z_; xyz++)
	    {
		t_oly_coord *coord = point->coord + xyz;

		point->_local[xyz] = coord->xx;

		for(iq=0; iq<_EXTRA_VAIABLE_NUM_MAX_; iq++)
		{
		    t_oly_extra *extra = coord->extra + iq;

		    if (extra->var) point->_local[xyz] += extra->cff*extra->var->value;
		} /*for iq*/
	    } /*for xyz*/

	    assert(!direct_3d_point_transform(point->global, &object->local_to_survey, 
					      point->_local));
	} /*for pt*/
    } /*for obj*/
} /* recalculate_geometry */

/* ================================================================= */

static void survey_minuit_function(int *npar, double grad[], 
  double *fval, double xval[], int *iflag, void *ptr)
{
    int obj, iq, pt, xyz;

    // Assign back internal variables based on the MINUIT input for this step; 
    //JCB    assign_minuit_parameters(&minuit, *npar, xval);



    survey_ndf = -*npar; *fval = 0.;

    // After all th eparameters are assigned recalculate the whole geometry;
    recalculate_geometry();

    // Loop through all objects;
    for(obj=0; obj<_OLY_OBJECT_NUM_; obj++)
    {
	t_oly_object *object = _oly_objects[obj];

	if (!object->active) continue;
	
	for(pt=0; pt<object->_actual_point_num; pt++)
	{
	    t_3d_vector to, diff;
	    t_oly_point *point = object->points + pt;

	    if (point->dummy_point_flag) continue;

	    // Prefer to move survey points to the object local coordinate
	    // system; just because XY and ZZ errors can be different there, rather
	    // than in survey system (where errors are for now just considered 0.0);
	    assert(!inversed_3d_point_transform(to, &object->local_to_survey, point->survey));

	    sub_v_v(diff, point->_local, to);

	    // Loop through all the projections; assume diagonal errors;
	    for(xyz=_X_; xyz<=_Z_; xyz++)
		*fval += sqr(diff[xyz])/sqr(xyz == _Z_ ? object->matching_accuracy_zz : 
					    object->matching_accuracy_xy);

	    survey_ndf += 3;
	} /*for pt*/
    } /*for obj*/
} /* survey_minuit_function */

/* ================================================================= */

static int declare_oly_variable(t_oly_variable *var)
{
    int iq;

    // Check that was not defined yet;
    for(iq=0; iq<oly_variable_num; iq++)
	if (var == oly_variables[iq]) 
	    return 0;

    // Append variable to the declared list;
    oly_variable_num++;
    oly_variables = (t_oly_variable **)realloc(oly_variables, oly_variable_num*sizeof(t_oly_variable*));
    assert(oly_variables);

    oly_variables[oly_variable_num-1] = var;

    minuit.DefineParameter(PARNUM++,, var->name, &var->value, &var->err, 0,0);
    //JCB    assert(declare_minuit_parameter(&minuit, var->name, &var->value, &var->err, 
    //				    0, 1., 0., 0.));
    return 0;
} /* declare_oly_variable */

/* ----------------------------------------------------------------- */

static t_oly_point *get_point_by_name(char *name)
{
    int obj, pt;

    // Just carelessly loop through all points and find matching name;
    for(obj=0; obj<_OLY_OBJECT_NUM_; obj++)
    {
	t_oly_object *object = _oly_objects[obj];

	if (!object->active) continue;
    
	// Search for the matching survey point name;
	for(pt=0; pt<object->_actual_point_num; pt++)
	{
	    t_oly_point *point = object->points + pt;

	    if (!strcmp(name, point->name)) return point;
	} /*for pt*/
    } /*fpr obj*/

    return NULL;
} /* get_point_by_name */

/* ----------------------------------------------------------------- */

static int print_global_point_coordinates(char *name)
{
    int xyz;
    t_oly_point *point = get_point_by_name(name);

    if (!point) return -1;
		
    printf(" --> %s\n", name);
    for(xyz=_X_; xyz<=_Z_; xyz++)
	printf("         %c = %8.2f [mm]\n", XYZ[xyz], point->global[xyz]);

    return 0;
} /* print_global_point_coordinates */

/* ----------------------------------------------------------------- */

static int print_global_point_distance(char *from, char *to)
{
    t_3d_vector diff;
    t_oly_point *pfrom = get_point_by_name(from), *pto = get_point_by_name(to);

    if (!pfrom || !pto) return -1;

    sub_v_v(diff, pto->global, pfrom->global);

    printf(" %s --> %s:%8.2f [mm] \n", from, to, len_v(diff));

    return 0;
} /* print_global_point_distance */

/* ----------------------------------------------------------------- */

static void final_printout( void )
{
    int obj, xyz;

    // General information;
    printf("\n\n\n --> chi^2/ndf: %7.1f/%3d\n", survey_fcn, survey_ndf);

    // Global extra variables;
    if (oly_variable_num)
    {
	int iq;

	printf("\n --> extra variables ->\n\n");

	for(iq=0; iq<oly_variable_num; iq++)
	{
	    t_oly_variable *var = oly_variables[iq];

	    printf("  %12s: %7.2f +/- %6.2f %s -> %s\n", 
		   var->name, var->value, var->err, var->units, 
		   var->comment ? var->comment : ""); 
	} /*for iq*/
    }

    // Per-object information;
    printf("\n  --> 3D objects ->\n");
    for(obj=0; obj<_OLY_OBJECT_NUM_; obj++)
    {
	t_oly_object *object = _oly_objects[obj];

	if (!object->active) continue;

	printf("\n---------------------------\n");
	printf("\n   %s (%2d survey points)\n", object->name, object->true_point_num);
	printf("     -> %7.2f degree nominal installation angle (Y-rotation)\n", object->yrot_angle);
	printf("     -> estimated accuracy %6.2f mm (XY) & %6.2f mm (Z) per point\n", 
	       object->matching_accuracy_xy, object->matching_accuracy_zz);
	printf("\n     -> coordinates in Y-rotated (local) system:\n");
	for(xyz=_X_; xyz<=_Z_; xyz++)
	    printf("         %c = %8.2f +/- %6.2f [mm]\n", XYZ[xyz], object->local_to_lumi.shift[xyz], 
		   object->shift_err[xyz]);
	printf("     -> slopes in Y-rotated system:\n");
	for(xyz=_X_; xyz<=_Z_; xyz++)
	    printf("        R%c = %8.3f +/- %6.3f [degree] %s\n", XYZ[xyz], object->local_to_lumi.slope[xyz], 
		   object->slope_err[xyz], object->slope_err[xyz] ? "" : "(bound)");

	if (object->global_offset_by_hand[_X_] || object->global_offset_by_hand[_Y_] ||
	    object->global_offset_by_hand[_Z_])
	{
	  printf("\n     -> 'by hand' offsets in OLYMPUS (global) system:\n");
	  for(xyz=_X_; xyz<=_Z_; xyz++)
	    printf("         D%c = %8.2f [mm]\n", XYZ[xyz], object->global_offset_by_hand[xyz]);
	} /*if*/

	// Eventually print out object coordinates in OLYMPUS system; account possible "by hand" shift;
	printf("\n     -> object coordinates in OLYMPUS (global) system:\n");
	for(xyz=_X_; xyz<=_Z_; xyz++)
	    printf("         %c = %8.2f [mm]\n", XYZ[xyz], 
		   object->local_to_survey.shift[xyz] + object->global_offset_by_hand[xyz]);

	// In case of >1 elements print out their 3D locations as well;
	if (object->element_num > 1)
	{
	    int el;

	    printf("\n     -> element coordinates in OLYMPUS (global) system:\n");

	    for(el=0; el<object->element_num; el++)
	    {
		// Want ordering to always start "from the beam line";
		int qel = object->yrot_angle > 0. ? el : object->element_num - el - 1;
		t_3d_vector xx = {*object->step*(qel - (object->element_num-1)/2.), 0., 0.}, global;

		assert(!direct_3d_point_transform(global, &object->local_to_survey, xx));
		
		// Account possible "by hand" shift;
		add_v_v(global, global, object->global_offset_by_hand);

		for(xyz=_X_; xyz<=_Z_; xyz++)
		    printf("el#%d ->         %c = %8.2f [mm]\n", el+1, XYZ[xyz], global[xyz]);
		printf(" --> R = %8.2f [cm], theta = %8.3f [degree], alpha = %8.3f [degree]\n\n", 
		       mm2cm(sqrt(sqr(global[_X_])+sqr(global[_Z_]))), fabs(rad2deg(atan(global[_X_]/global[_Z_]))), 
		       fabs(object->yrot_angle + object->local_to_lumi.slope[_Y_]));
	    } /*for el*/
	} /*if*/

    } /*for obj*/

    printf("\n\n");
    //print_global_point_coordinates("LF#1");
    //print_global_point_coordinates("RF#1");
    print_global_point_coordinates("RB#3");
    //print_global_point_distance   ("RF#1", "LF#1");
    //print_global_point_distance   ("RF#2", "RM#1");
    //print_global_point_distance   ("RM#3", "RB#1");
    //print_global_point_distance   ("LB#2", "RB#2");
    //print_global_point_distance   ("FD1.13", "FD6.13");
    //print_global_point_distance   ("FD1.25", "FD6.25");
#if 0

    {
	t_3d_line line;
	t_oly_point *from = get_point_by_name("RM#2");
	t_oly_point *to   = get_point_by_name("RM#3");
	t_oly_point *pt   = get_point_by_name("RB#1");

	assert(from && to && pt);

	assert(!build_line_from_2_points(from->global, to->global, &line));

	printf("--> %8.2f\n", point_to_line_dist(pt->global, &line));
	
	print_global_point_coordinates("RM#2");
	print_global_point_coordinates("RM#3");
	print_global_point_coordinates("RB#1");

	{
	    t_3d_vector dv1, dv2;

	    sub_v_v(dv1, from->global, pt->global);
	    sub_v_v(dv2, to->global,   pt->global);

	    printf("dv1: %7.2f %7.2f\n", dv1[_X_], dv1[_Z_]);
	    printf("dv2: %7.2f %7.2f\n", dv2[_X_], dv2[_Z_]);
	}
    }
#endif
} /* final_printout */

/* ----------------------------------------------------------------- */

static void crap( void )
{
    t_3d_vector v13 = {  98.130, 286.036,  23.628};
    t_3d_vector v14 = {1409.926, 132.076,  23.628};
    t_3d_vector v16 = {1417.023, 192.544, 190.571};
    t_3d_vector v17 = {  75.574, 445.698,  34.763};
    t_3d_vector v18 = {1992.814, 220.679,  34.763};
    t_3d_vector v20 = {2004.582, 320.947, 303.279};
    t_3d_vector v21 = { 173.500, 617.061,  17.721};
    t_3d_vector v22 = {2595.277, 332.828,  17.721};
    t_3d_vector v24 = {2604.199, 408.842, 199.086};
    t_3d_vector q1, q2, qv;

    sub_v_v(q1, v13, v14);
    sub_v_v(q2, v16, v14);
    //sub_v_v(q1, v17, v18);
    //sub_v_v(q2, v20, v18);
    //sub_v_v(q1, v21, v22);
    //sub_v_v(q2, v24, v22);

    vmul_v_v(qv, q1, q2);
    normalize_v(qv);
    printf("%f %f %f\n", qv[0], qv[1], qv[2]);
    exit(0);
} 

/* ----------------------------------------------------------------- */

int argn;
char *argp[100];
int split_string(char *b)
{
  argn=0;
  argp[0]=b;
  for (int i=0;i<strlen(b);i++)
      if (b[i]==' ')
	{
	  argn++;
	  argp[argn]=&b[i+1];
	  b[i]=0;
	}
  return 0;
}

int main ( void )
{
    int err, obj, qt, pt, xyz, active_object_num = 0;
    char name[128];

    //redirect_fortran_output("/dev/null", "/dev/null");

    //crap();

    for(obj=0; obj<_OLY_OBJECT_NUM_; obj++)
    {
	t_oly_object *object = _oly_objects[obj];

#ifdef _SINGLE_OBJECT_
	if (strcmp(object->name, _SINGLE_OBJECT_)) continue;
#endif
	object->active = 1;
	active_object_num++;
	
	// Calculate Lumi->survey coordinate system transformation; 
	create_1d_rotation_matrix(object->yrot_angle, _Y_, object->lumi_to_doris.rotation);
	object->lumi_to_doris.rotation_defined_flag = 1;

	// Figure out number of survey points; 
	for(pt=0; pt<_POINTS_PER_OBJECT_MAX_; pt++)
	{
	    t_oly_point *point = object->points + pt;

	    if (!point->name) break;

	    point->object = object;

	    // Eventually should check for overlapping names through ALL objects?; hmm;
	    for(qt=0; qt<pt; qt++)
		if (!strcmp(point->name, object->points[qt].name))
		{
		    printf("Point name '%s' encountered twice!\n", point->name); 
		    return -1;
		} /*for qt ..if*/

	    object->_actual_point_num++;
	    if (point->dummy_point_flag) 
		object->dummy_point_num++;
	    else
		object->true_point_num++;
	} /*for pt*/

	{
	    int iq;

	    // Loop through all the ASCII input files for this object;
	    for(iq=0; iq<_ASCII_FILES_PER_OBJECT_MAX_; iq++)
	    {
		if (!object->ascii[iq]) break;

		{
		    char buffer[STRING_LEN_MAX];
		    FILE *ff = fopen(object->ascii[iq], "r");

		    if (!ff)
		    {
			printf("Failed to open '%s' file with survey data!\n", 
			       object->ascii[iq]);
			return -1;
		    } /*if*/

		    while (fgets(buffer, STRING_LEN_MAX, ff))
		    {
			// Replace tabs by spaces, otherwise split_string() fails;
			{
			    int ip, len = strlen(buffer);
			    
			    for(ip=0; ip<len; ip++)
				if (buffer[ip] == '\t') buffer[ip] = ' ';
			} 

			// Strings of interest are name + XYZ coordinates;
			if (split_string(buffer) || argn != 4) continue;
			//printf("%d: %s\n", argn, argp[0]);

			// Search for the matching survey point name;
			for(pt=0; pt<object->_actual_point_num; pt++)
			{
			    t_oly_point *point = object->points + pt;

			    if (!strcmp(point->name, argp[0]))
			    {
				// Swap Benecke's coordinates right here; do not mind to do 
				// this in invariant way (via rotations), just re-assign;
				point->survey[_X_] =  atof(argp[2]);
				point->survey[_Y_] =  atof(argp[3]);
				// Yes, account for the Z-shift right here;
				point->survey[_Z_] =  atof(argp[1]) + _OLYMPUS_IP_OFFSET_;

				//printf("  --> match!\n");
				point->imported = 1;

				break;
			    } /*if*/
			} /*for pt*/
		    } /*while*/

		    fclose(ff);
		}
	    } /*for iq*/
	}

	// Check that all points have their survey measurements;
	for(pt=0; pt<object->_actual_point_num; pt++)
	{
	    t_oly_point *point = object->points + pt;

	    if (!point->dummy_point_flag && !point->imported)
	    {
		printf("Point '%s' has no survey measurement!\n", point->name);
		return -1;
	    } /*if*/
	} /*for pt*/

	// Otherwise clean positioning is impossible; 
	if ((object->mode == _6PAR_ &&   object->true_point_num < 3) ||
	    ((object->mode == _4PAR_ || object->mode == _5PPAR_ || object->mode == _5MPAR_) 
	     && object->true_point_num < 2) ||
	    (object->mode == _3PAR_ &&  !object->true_point_num))
	{
	    printf("Only '%d' true points declared for object '%s'!\n", 
		   object->true_point_num, object->name);
	    return -1;
	} /*if*/

	object->local_to_lumi.slope_defined_flag = 1;

	// Other parameters may stay with starting values 0.0;
	object->local_to_lumi.shift[_Z_] = object->expected_distance_to_target;

	// Declare object 3D offsets and shifts;
	for(xyz=_X_; xyz<=_Z_; xyz++)
	{
	    snprintf(name, 128-1, "%s--%c", object->name, XYZ[xyz]);
	    minuit.DefineParameter(PARNUM++,name,object->local_to_lumi.shift + xyz,   object->shift_err + xyz, 0, 0);
	    //JCB    assert(declare_minuit_parameter(&minuit, name, object->local_to_lumi.shift + xyz, 
	    //					    object->shift_err + xyz, 0, 1., 0., 0.));
	} /*for xyz*/

	if (object->mode != _3PAR_)
	{
	    int from;
	    static double pmul = 1., nmul = -1.;

	    switch (object->mode)
	    {
		case _6PAR_:
		    from = _X_; break;
		case _5PPAR_:
		  printf("Not implemented\n");
		  exit(-1);
		  //JCB		    assert(declare_minuit_link(&minuit, object->local_to_lumi.slope + _X_,
		  //			       object->local_to_lumi.slope + _Y_, &pmul, _OP_MUL_));
		    from = _Y_; break;
		case _5MPAR_:
		  printf("Not implemented\n");
		  exit(-1);
		  //JCB assert(declare_minuit_link(&minuit, object->local_to_lumi.slope + _X_,
		  //			       object->local_to_lumi.slope + _Y_, &nmul, _OP_MUL_));
		    from = _Y_; break;
		case _4PAR_:
		    from = _Z_; break;
		default:
		    assert(0);
	    } /*switch*/
	    
	    for(xyz=from; xyz<=_Z_; xyz++)
	    {
		snprintf(name, 128-1, "%s-R%c", object->name, XYZ[xyz]);
		minuit.DefineParameter(PARNUM++, name, object->local_to_lumi.slope + xyz, object->slope_err + xyz, -10, 10);
		//JCB		assert(declare_minuit_parameter(&minuit, name, object->local_to_lumi.slope + xyz, 
		//				object->slope_err + xyz, 0, 1., -10., 10.));
	    } /*if..for xyz*/
	} /*if*/

	// Would not hurt in any mode;
	for(xyz=_X_; xyz<=_Z_; xyz++)
	    object->local_to_lumi.slope[xyz] = object->rxyz[xyz];

	// Declare point correction variables;
	for(pt=0; pt<object->_actual_point_num; pt++)
	{
	    int iq;
	    t_oly_point *point = object->points + pt;
	    
	    if (point->dummy_point_flag) continue;

	    for(xyz=_X_; xyz<=_Z_; xyz++)
	    {
		t_oly_coord *coord = point->coord + xyz;

		for(iq=0; iq<_EXTRA_VAIABLE_NUM_MAX_; iq++)
		{
		    t_oly_extra *extra = coord->extra + iq;

		    if (extra->var && !extra->var->fixed && 
			declare_oly_variable(extra->var))
		    {
			printf("Failed to declare variable '%s'\n", extra->var->name);
			return -1;
		    } /*if*/
		} /*for iq*/
	    } /*for xyz*/
	} /*for pt*/

    } /*for obj*/

    // Well, at least one object, please;
    assert(active_object_num);

    //JCB    assert(!minuit_init(&minuit));



    {
	int idummy;
	double dummy;

	// Run MIGRAD pass;
	//JCB	MNCOMD(survey_minuit_function, "MIGRAD 5000 .1", err, 0);
	
	// Assign the parameters at minimum; 
	//JCB	get_minuit_best_parameters(&minuit);
	//JCB save_minuit_parameters(&minuit, NULL);
	
	// Get chi^2 value at minimum;
	//JCB MNSTAT(survey_fcn, dummy, dummy, idummy, idummy, idummy);

	// Run HESSE pass;
	//JCB MNCOMD(survey_minuit_function, "HESSE", err, 0);

	// Restore parameters at minimum;
	//JCB restore_minuit_parameters(&minuit, NULL);

	// Store MINUIT errors in internal variables; 
	//JCB get_minuit_errors(&minuit);
    }

    // Calculate the geometry with final parameter values;
    recalculate_geometry();

    // Produce final printout;
    final_printout();

    exit(0);
} /* main */

/* ================================================================= */
