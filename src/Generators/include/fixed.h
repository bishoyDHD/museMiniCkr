#ifndef __FIXED_H_
#define __FIXED_H_


#include "base.h"


class GeneratorFixed:public GeneratorBase
{
 private:
  std::string particle;
  double theta;
  double phi; 
  double mass;
  double momentum;
 public:
 GeneratorFixed( int skip,unsigned long int seed):GeneratorBase(0,skip,seed){}; //we don't need any random numbers for a fixed generator
  virtual int generateEvent(GeneratorEvent *eventinfo); //eventinfo is supplied from the outside (to write to trees directly). Return : positive: Number of tracks. Negative: Error.  This should be overloaded by the real generators.
  void setParticle(std::string name);
  void setMomentum(double mom);
  void setTheta(double t); //in degrees
  void setPhi(double p);  // in degrees
};

#endif
