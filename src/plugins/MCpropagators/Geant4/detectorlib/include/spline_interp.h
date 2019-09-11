//
// Header containing the necessary spline interpolation classes for the magnetic
// field interpolation routines (also included some base class material in case
// we want to try other interpolation schemes)
//
// Created March 18, 2013
//
// Brian S. Henderson (bhender1@mit.edu)
//
// Based on the cubic spline routines developed in Chapter 3 of Numerical
// Recipes, Third Edition
//

#include <vector>
#include <cmath>

// Base structure for interpolation schemes (should we want to add more later)
struct ibase
{
	// Various integers needed
	int n, mm, jsav, cor, dj;

	// Data point storage
	const double *xx, *yy;
	
	// Constructor for interpolating on table of x and y (each length m)
	ibase(std::vector<double> &x, const double *y, int m) : n(x.size()), mm(m), jsav(0), cor(0), xx(&x[0]), yy(y)
	{
		dj = min(1,(int)pow((double)n,0.25));
	}

	// Alternate constructer
	ibase(const std::vector<double> &x, const double *y, int m) : n(x.size()), mm(m), jsav(0), cor(0), xx(&x[0]), yy(y)
	{
		dj = min(1,(int)pow((double)n,0.25));
	}

	// User output function
	double interp(double x)
	{
		// Call function to decide the grid location
		int jlo = cor ? hunt(x) : locate(x);
		return rawinterp(jlo,x);
	}

	// Grid location functions (uncorrelated and correlated)
	int locate (const double x);
	int hunt(const double x);

	// Virtual placeholder for the interpolation provided by the derived spline class
	double virtual rawinterp(int jlo, double x) = 0;

};

// Base grid location function
int ibase::locate(const double x)
{
	int ju, jm, jl; // Temporary indices

	// Check to make sure the provided data is sane
	if (n<2 || mm<2 || mm > n) throw("\n\nData provided to locate is poorly sized!\n\n");

	// Check to see if the data is in ascending order (true if yes)
	bool ascnd = (xx[n-1] >= xx[0]);

	// Initialize the upper and lower limits
	jl = 0; ju = n-1;

	// Bisect to find the right index
	while (ju-jl > 1)
	{
		// Compute the midpoint
		jm = (ju+jl) >> 1;

		// Adjust the right index
		if ((x >= xx[jm]) ==  ascnd) jl = jm;
		else ju = jm;
	}

	// See if it is worth using the hunt function next time
	cor = (abs((double) (jl-jsav)) > dj) ? 0 : 1;
	
	// Save the most recent lower bound index
	jsav = jl;

	// Return the index
	return max(0,min(n-mm,jl-((mm-2)>>1)));
	
};

// A function that speeds up the grid bisection when subsequent points are close
// together
int ibase::hunt(const double x)
{
	// Load the saved lower bound into the current lower bound
	int jl = jsav;

	// Declare the various integers needed
	int jm, ju, inc = 1;

	// Check to see if the data is in ascending order (true if yes)
	bool ascnd = (xx[n-1] >= xx[0]);

	// Check to see if the given indices are bad, if so skip to bijection
	if (jl<0 || jl>(n-1)) {jl = 0; ju = n-1;}
	// Else, do the hunt scan
	else
	{
		// Hunt upward in indices if ascending order
		if ((x >= xx[jl]) == ascnd)
		{
			for(;;)
			{
				// Set the upper index by the increment
				ju = jl + inc;
				
				// Check to make sure you haven't run off the list
				if (ju >= n-1) {ju = n-1; break;}
				// See if you have found the right index range
				else if ((x < xx[ju]) == ascnd) break;
				// If not, up the increment by a factor of two and loop
				else
				{
					jl = ju; inc += inc;
				}
			}
		}
		// Hunt downward if not ascending order
		else
		{
			// Reset the upper bound
			ju = jl;
			
			// Do the hunt down in the same fashion as above
			for(;;)
			{
				// Set the lower index by the increment
				jl = ju - inc;
				
				// Check to make sure you haven't run off the list
				if (jl <= 0) {jl = 0; break;}
				// See if you have found the right index range
				else if ((x >= xx[jl]) == ascnd) break;
				// If not, up the increment by a factor of two and loop
				else
				{
					ju = jl; inc += inc;
				}
			}
		}
	}

	// Now finish by bisecting
	while (ju-jl > 1)
	{
		// Compute the midpoint
		jm = (ju+jl) >> 1;

		// Adjust the right index
		if ((x >= xx[jm]) ==  ascnd) jl = jm;
		else ju = jm;
	}

	// See if it is worth using the hunt function next time
	cor = (abs((double) (jl-jsav)) > dj) ? 0 : 1;
	
	// Save the most recent lower bound index
	jsav = jl;

	// Return the index
	return max(0,min(n-mm,jl-((mm-2)>>1)));

};

// Base 1D spline interpolation structure
struct spline1D : ibase
{
	std::vector<double> y2; // Vector of the second derivatives of the data
	
	// Constructor for 1D interpolation
	spline1D(std::vector<double> &xv, std::vector<double> &yv) : ibase(xv,&yv[0],2), y2(xv.size())
	{
		sety2(&xv[0],&yv[0]);
	}

	// Constructors for 2D interpolation
	spline1D(std::vector<double> &xv, const double *yv) : ibase(xv,yv,2), y2(xv.size())
	{
		sety2(&xv[0],yv);
	}

	// Constructor for 1D interpolation
	spline1D(const std::vector<double> &xv, std::vector<double> &yv) : ibase(xv,&yv[0],2), y2(xv.size())
	{
		sety2(&xv[0],&yv[0]);
	}

	// Function to compute and store the second derivatives of the data
	void sety2(const double *xv, const double *yv);

	// Function that does the interpolation
	double rawinterp(int jl, double xv);

};

// Function to compute the derivatives needed for the spline interpoloatoin
void spline1D::sety2(const double *xv, const double *yv)
{
	// Various doubles needed
	double p, qn, sig, un;

	// Get the size of the vectors involved
	int n = y2.size();
	
	// Temporary storage vector
	std::vector<double> u(n-1);
	
	// Set "natural" spline boundary conditions (first and second derivatives
	// zero at endpoints)
	y2[0] = 0; qn = 0;
	u[0] = 0; un = 0;
	
	// Tridiagonal second derivative algorithm (see Numerical Recipes)
	for (int i = 1; i<(n-1); i++)
	{
		sig = (xv[i]-xv[i-1])/(xv[i+1]-xv[i-1]);
		p = sig*y2[i-1]+2;
		y2[i] = (sig-1)/p;
		u[i] = (yv[i+1]-yv[i])/(xv[i+1]-xv[i]) - (yv[i]-yv[i-1])/(xv[i]-xv[i-1]);
		u[i] = (6*u[i]/(xv[i+1]-xv[i-1])-sig*u[i-1])/p;
	}

	// Fill the second derivative vector by back-substitution
	for (int k = n-2; k >= 0; k--)
	{
		y2[k] = y2[k]*y2[k+1]+u[k];
	}	
}

// 1D Spline Interpolator using standard cubic spline formula
double spline1D::rawinterp(int jl, double x)
{
	int klo = jl, khi =jl+1; // Set the bounding indices
	double y, h, b, a;
	
	// Difference along the x vector
	h = xx[khi]-xx[klo];
	if (h==0) throw("\n\nProvided points are at same x value!\n\n");

	// Fractions along intervals of desired point
	a = (xx[khi]-x)/h;
	b = (x-xx[klo])/h;

	// Cubic spline formula
	y = a*yy[klo] + b*yy[khi] + ((a*a*a-a)-y2[klo] + (b*b*b-b)*y2[khi])*h*h/6;

	// Return the interpolated value
	return y;
	
}

// 2D Spline Interpolator (bicubic)
struct spline2D
{
	int m, n; // Dimensions of data

	// Matrix for storage of output values
	const std::vector< std::vector<double> > &y;
	
	// Placeholder vectors
	const std::vector<double> &x1;
	std::vector<double> yv;

	// Vector of 1D interpolations along the rows
	std::vector<spline1D*> srp;

	// Constructor
	spline2D(std::vector<double> &x1v, std::vector<double> &x2v, std::vector< std::vector<double> > &ym) :
   	m(x1v.size()), n(x2v.size()), y(ym), yv(m), x1(x1v), srp(m)
		{
			// Yo dawg, I heard you like splines...so I put a spline in each row of your 2D data
			for (int i=0; i<m; i++) srp[i] = new spline1D(x2v,&y[i][0]);
		}

	// Destructor to take care of 1D spline pointers
	~spline2D() {for (int i=0; i<m; i++) delete srp[i];}

	// Interpolation function
	double interp(double x1p, double x2p)
	{
		// Interpolate at x2p in every row
		for (int i=0; i<m; i++) yv[i] = (*srp[i]).interp(x2p);

		// Interpolate on the interpolation
		spline1D scol(x1,yv);
		return scol.interp(x1p);
	}
	
};

// Final 3D spline
struct spline3D
{
	// Setup a 2D spline for each plane
};

// Toy function to compute values for testing of the interpolation
double toytest(double x, double y = 0)
{
	double q = x*y+x*x*y*y*y-3*x-2*y*y*y*y;
	return q;
}
