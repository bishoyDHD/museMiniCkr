#include "cTrack.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"

//!
//! Track Class Constructor
cTrack::cTrack(int n)
{
  v = NULL;
  // should we already allocate memory for some points?
  if (n>0)
    {
      v = new Hep3Vector*[n];
      for (int i=0; i<n; i++)
	v[i] = new Hep3Vector();
      nalloc=n;
      nnum=n;
    }
  else
    {
      nalloc=0;
      nnum=0;
    };
  // trajectory:
  t_type = 0;
  t_r0   = NULL;
  t_dr   = NULL;
}
//!
//! Track Class Destructor
cTrack::~cTrack()
{
  if (v)
    {
      // do something...
    };
}
//!
//! Returns the number of stored points.
int cTrack::GetNPoints()
{
  return nnum;
}
//!
//! Set the coordinate coor of point number n to x.
//! Returns 0 on success, 1 if n or coor out of range.
int cTrack::SetPointCoor(int n, int coor, double x)
{
  if ((n<0)||(n>=nnum))
    return 1;
  if ((coor<0)||(coor>2))
    return 1;
  switch (coor)
    {
    case 0:
      v[n]->setX(x); break;
    case 1:
      v[n]->setY(x); break;
    case 2:
      v[n]->setZ(x); break;
    };
  return 0;
}
//! Set coordinates of point number n to (x, y, z).
//! Returns 0 on success, 1 if n out of range.
int cTrack::SetPoint(int n, double x, double y, double z)
{
  if ((n<0)||(n>=nnum))
    return 1;
  v[n]->set(x, y, z);
  //printf(" cTrack::SetPoint: %d: %3.1lf %3.1lf %3.1lf\n", n, x, y, z);
  return 0;
}
//!
//! Add another point with coordinates (x, y, z)
//! to the list of points.
//! The number of stored points increases from n
//! to n+1.
//! Returns the new number of points in the list
//! on success or 0 in case memory couldn't be
//! allocated.
int cTrack::AddPoint(double x, double y, double z)
{
  Hep3Vector **vaux = new Hep3Vector*[nnum+1];
  for (int n=0; n<nnum; n++)
    vaux[n] = v[n];
  vaux[nnum] = new Hep3Vector(x, y, z);
  delete[] v;
  v = vaux;
  nalloc++;
  nnum++;
  return 0;
}
//!
//! Remove point n from the list.
//! The number of stored points is decreased
//! from n to n-1.
int cTrack::RemovePoint(int n)
{
  delete v[n];
  for (int i=n+1; i<nnum; i++)
    v[i-1] = v[i];
  v[nnum]=NULL;
  nnum--;
  return 0;
}
//!
//! Return the x coordinate of point number n.
//! Returns nan in case n is out of range.
double cTrack::GetPointX(int n)
{
  if ((n<0)||(n>=nnum))
    return nan("");
  return v[n]->getX();
}
//!
//! Return the y coordinate of point number n.
//! Returns nan in case n is out of range.
double cTrack::GetPointY(int n)
{
  if ((n<0)||(n>=nnum))
    return nan("");
  return v[n]->getY();
}
//!
//! Return the z coordinate of point number n.
//! Returns nan in case n is out of range.
double cTrack::GetPointZ(int n)
{
  if ((n<0)||(n>=nnum))
    return nan("");
  return v[n]->getZ();
}
//!
//! Return the coordinate coor of point number n.
//! Returns nan in case n or coor is out of range.
double cTrack::GetPointCoor(int n, int coor)
{
  if ((n<0)||(n>=nnum))
    return nan("");
  if ((coor<0)||(coor>2))
    return nan("");
  switch (coor)
    {
    case 0:
      return v[n]->getX();
    case 1:
      return v[n]->getY();
    case 2:
      return v[n]->getZ();
    };
  return nan("");
}
//!
//! Calculate trajectory or track.
int cTrack::CalculateTrajectory(int options)
{
  switch (nnum)
    {
    case 0: // without points there's no trajectory...
      return 1;
    case 1: // one point only is insufficient...
      return 1;
    case 2: // trajectory from two space points:
      // calculate unit vector pointing from v[0] towards v[1]:
      if (t_r0) delete t_r0;
      if (t_dr) delete t_dr;
      t_r0 = new Hep3Vector(*v[0]);
      t_dr = new Hep3Vector(*v[0]);
      *t_dr = *t_dr-*v[1];
      //printf("length = %lf\n", t_dr->mag());
      //printf("direction: %lf %lf %lf\n",
      //	     t_dr->getX(), t_dr->getY(), t_dr->getZ());
      *t_dr=t_dr->unit();
      t_type = 2;
      return 0;
    case 3: // calculate curved trajectory:
      
      return 1;
    };
  return 2;
}
//!
//! Calculate the distance of a point for the track
//! for a given z coordinate. Useful to determine the
//! spatial resolution of a detector element that lies
//! in a XY plane.
double cTrack::DistanceInXYPlane(double x, double y, double z,
				double *dx, double *dy)
{
  switch (t_type)
    {
    case 2:
      double z0 = t_r0->getZ();
      double dz = t_dr->getZ();
      double t  = (z-z0)/dz;
      //printf("t=%lf\n", t);
      // vectors to the points p=(x,y,z) and r=traj(z=0):
      //Hep3Vector *p = new Hep3Vector(x, y, z);
      //Hep3Vector *r = new Hep3Vector(*t_r0);
      Hep3Vector p(x, y, z);
      Hep3Vector r(*t_r0);

      // calculate vector to point on trajectory at z: r'=t*t_dr+r
      //printf("calc traj coords at z\n");
      r += (*t_dr*t);
      //printf(" track is at %3.2lf %3.2lf %3.2lf\n", r.getX(), r.getY(), r.getZ());
      // p-r: p'=p-r
      //printf("subtracting\n");
      p-=r;
      double distx  = fabs(p.x());
      double disty  = fabs(p.y());
      double distz  = p.z();
      double distxy = sqrt(pow(distx,2.)+pow(disty,2.));
      //printf(" distance = %lf ; %lf ; %lf = %lf\n",
      //     distx, disty, distz, distxy);
      if (dx) *dx = distx;
      if (dy) *dy = disty;
      return distxy;
    };
  return 0;
}
//!
//! Calculate z coordinate of point of trajectory with
//! minimum distance to z axis.
double cTrack::Zvertex(double *distance)
{
  double v0z  = t_r0->getZ();
  double v1z  = t_dr->getZ();
  double v0v1 = t_r0->dot(*t_dr);
  double v1v1 = t_dr->dot(*t_dr);
  double t = 
    ( -v0z + 1./v1z*v0v1 ) /
    (  v1z - 1./v1z*v1v1 );
  if (distance)
    {
      Hep3Vector v_zmin((*t_r0) + (*t_dr*t));
      *distance = sqrt(pow(v_zmin.getX(),2.)+pow(v_zmin.getY(),2.));
    };
  return v0z + t*v1z;
}
//!
//! Print a list of coordinates of all stored points to stdout.
int cTrack::PrintPoints()
{
  printf("\n   #\t  x\t\t  y\t\t  z\n");
  printf("======================================================\n");
  for (int i=0; i<nnum; i++)
    printf(" %3d\t%e\t%e\t%e\n", i, v[i]->getY(), v[i]->getY(), v[i]->getZ());
  printf("\n");
  return nnum;
}
//!
//! Print the components of a 3-vector to stdout.
void cTrack::PrintVector(Hep3Vector *vector)
{
  printf("%lf %lf %lf",
	 vector->getX(), vector->getY(), vector->getZ());
}
//! Draw the trajectory (if calculated yet)
//! in the pad.
// int DrawTrajectory(TPad *pad)
// {
//   return 0;
// }
