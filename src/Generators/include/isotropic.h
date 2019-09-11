#ifndef __ISOTROPIC_H_
#define __ISOTROPIC_H_


#include "base.h"


class GeneratorIsotropic:public GeneratorBase
{
 private:
  std::string particle;
  double cosThetaMin,cosThetaDelta;
  double phiRange; 
  double weight;
  double mass;
  double momentum;
  void recalcWeight();
 public:
 GeneratorIsotropic( int skip,unsigned long int seed):GeneratorBase(2,skip,seed){}; //we need 2 parameters for the isotropic generation.
  virtual int generateEvent(GeneratorEvent *eventinfo); //eventinfo is supplied from the outside (to write to trees directly). Return : positive: Number of tracks. Negative: Error.  This should be overloaded by the real generators.
  void setParticle(std::string name);
  void setMomentum(double mom);
  void setThetaRange(double thetamin,double thetamax); //in degrees
  void setPhiRange(double phirange);  // in degrees, phi range will be  0+-range and 180+-range.

  
};

#endif
