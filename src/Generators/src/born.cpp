#include "born.h"

GeneratorBorn::GeneratorBorn(int skip,unsigned long int seed):GeneratorBase(2,skip,seed)
{
  mP = getMass("proton");
  me = getMass("e-");
}

void GeneratorBorn::setThetaRange(double thetamin,double thetamax)
{
  cosThetaMin=cos(thetamin*M_PI/180);
  cosThetaDelta=cos(thetamax*M_PI/180)-cosThetaMin;
  recalcWeight();
}

void GeneratorBorn::setPhiRange(double range)
{
  phiRange=range*M_PI/180;
  recalcWeight();
}

void GeneratorBorn::recalcWeight()
{
  weight=-phiRange*4*cosThetaDelta; //This is the part of 4Pi 
}

int GeneratorBorn::generateEvent(GeneratorEvent *ev)
{
  //build theta:
  double theta=acos(cosThetaMin+cosThetaDelta*qrndNumbers[0]);
  double r=qrndNumbers[1];
  
//build phi
  int side=0;
  if (r>=0.5)
    {
      side=M_PI;
      r-=0.5;
    }
  double phi=side+phiRange*4*(r-0.25);
  double ePrime = leptonEnergy(theta);
  double eMom = sqrt(ePrime*ePrime - me*me);
  ev->weight*=weight*crossSection(theta,ePrime);

  GeneratorParticle e,p;
  e.particle=ev->lepton_prescatter.particle;
  e.momentum.SetXYZM(eMom*sin(theta)*cos(phi),eMom*sin(theta)*sin(phi),eMom*cos(theta),me);
  ev->particles.push_back(e);

  p.particle="proton";
  p.momentum.SetXYZM(0.,0.,0.,mP);
  p.momentum+=ev->lepton_prescatter.momentum-e.momentum;
  ev->particles.push_back(p);
  return 1;
}

double GeneratorBorn::crossSection(double theta, double ePrime) // in units of cm^2 / sr
{
  const double constantFactor = 2. * 0.007297 * 0.007297 * 3.8937966E-30; //2.0*alpha*alpha*(cm*MeV)^2
  const double mu = 2.7928456; 
  const double qSq = 2.0*beamEnergy*ePrime*(1.0-cos(theta));
  double gE =  (1.0 + qSq/7.1E5);
  gE = 1./(gE*gE);
  const double gM = mu * gE;
  const double tau = qSq / (4.*mP*mP);
  const double epsilon = 1.0 / (1.0 + 2.0*(1.0+tau)*tan(theta/2.0)*tan(theta/2.0));
  const double csMott = constantFactor*ePrime*ePrime*ePrime*(1+cos(theta))/(qSq*qSq*beamEnergy);
  return csMott* (epsilon * gE * gE + tau * gM * gM ) / (epsilon * (1.+tau) );
}

double GeneratorBorn::leptonEnergy(double theta)
{
  return beamEnergy * mP / (mP + beamEnergy*(1. - cos(theta)));
}
