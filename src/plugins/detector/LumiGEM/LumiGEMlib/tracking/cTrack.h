#include "CLHEP/Vector/ThreeVector.h"

using namespace CLHEP;

class cTrack
{
 public:
  cTrack(int n=0);
  ~cTrack();
  int GetNPoints();
  int SetPoint(int n, double x, double y, double z);
  int SetPointCoor(int n, int coor, double x);
  int AddPoint(double x, double y, double z);
  int RemovePoint(int n);
  int GetPoint(int n, double *x, double *y, double *z);
  double GetPointX(int n);
  double GetPointY(int n);
  double GetPointZ(int n);
  double GetPointCoor(int n, int coor);
  int    CalculateTrajectory(int options);
  double DistanceInXYPlane(double x, double y, double z,
				double *dx=NULL, double *dy=NULL);
  double Zvertex(double *distance=NULL);
  int    PrintPoints();
  void   PrintVector(Hep3Vector*);
  enum TrajOptions
  {
    nofield = 0,
    homogeneousBfield,
    fieldmap
  };
  //int DrawTrajectory(TPad *pad);
 protected:
 private:
  int nalloc, nnum;        //! Number of allocated vectors
                           //!   that are currently allocated
                           //!   (nalloc), or in use (nnum):
                           //!   nnum<=nalloc
  Hep3Vector **v;          //! Array of 3-vectors to store
                           //!   points to construct track from
  Hep3Vector *t_r0, *t_dr; //! Start vector and direction
                           //!   for 2-point tracks
  int t_type;              //! Track type
};
