#ifndef __GENBASE_H_
#define __GENBASE_H_


//forward declaration to make root happy. gsl has this as an anonymous struct (gsl_(q)rng), which can not be forward declared. so we have to do some magick(tm).

struct qrng;
struct rng;


#include "generatortree.h"
#include "TVector3.h"
#include "TRotation.h"


// This is the definition of a Generator base class

class GeneratorBase
{
 protected:
  int nHaltonDim;
  TVector3 beamCenter;
  TVector3 beamDirection;
  double targetDensity;
  double beamEnergy;
  int beamCharge; //(-1 for electrons, +1 for positrons)
  qrng *quasiRandom;
  rng *random;
  double *qrndNumbers;
  bool useQuasi;
  TRotation beamRotation;
 public:
  

  GeneratorBase(int num_halton_dim, unsigned int skip,unsigned long int seed); //how many dimensions, how many events to skip (for halton), and the seed for 
  ~GeneratorBase();
    

  virtual double getRandom(); //gets the next random number from the Mersenne Twister implementation.

  virtual void useQuasiRandom(bool);

  virtual int generate(GeneratorEvent *eventinfo); // the "typical" function.
  virtual int generateEvent(GeneratorEvent *eventinfo); //eventinfo is supplied from the outside (to write to trees directly). Return : positive: Number of tracks. Negative: Error.  This should be overloaded by the real generators.
  virtual void generateVertex(GeneratorEvent *eventinfo); //helper function to generate a new vertex position.

  virtual void setBeamEnergy(double en);
  virtual void setBeamCharge(int q);  
  virtual void setBeamOrigin(TVector3 origin);
  virtual void setBeamDirection(TVector3 dir);
  virtual void setTargetDensity(double maxdens); //to scale energy loss
  static double getMass(std::string particle);
  virtual void handleBeamGeometry(GeneratorEvent *eventinfo); // handles beam position and slope

};

#endif
