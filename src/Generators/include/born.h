#ifndef __BORN_H_
#define __BORN_H_


#include "base.h"


class GeneratorBorn:public GeneratorBase
{
 private:
  int leptonCharge; // +1 is for positrons, -1 is for electrons.  
  double cosThetaMin,cosThetaDelta; // Lepton angle range
  double phiRange; 
  double weight;
  void recalcWeight();
  double mP,me;
 public:
 GeneratorBorn(int skip,unsigned long int seed);
  virtual int generateEvent(GeneratorEvent *eventinfo); //eventinfo is supplied from the outside (to write to trees directly). Return : positive: Number of tracks. Negative: Error.  This should be overloaded by the real generators.
  void setThetaRange(double thetamin,double thetamax); //in degrees
  void setPhiRange(double phirange);  // in degrees, phi range will be  0+-range and 180+-range.
  double leptonEnergy(double theta);
  double crossSection(double theta, double ePrime); // in units of cm^2 / sr
};

#endif
