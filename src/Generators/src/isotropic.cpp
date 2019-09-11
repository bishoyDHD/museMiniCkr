#include "isotropic.h"



void GeneratorIsotropic::setParticle(std::string name)
{
  particle=name;
  mass=getMass(name);
}

void GeneratorIsotropic::setThetaRange(double thetamin,double thetamax)
{
  cosThetaMin=cos(thetamin*M_PI/180);
  cosThetaDelta=cos(thetamax*M_PI/180)-cosThetaMin;
  recalcWeight();
}

void GeneratorIsotropic::setPhiRange(double range)
{
  phiRange=range*M_PI/180;
  recalcWeight();
}

void GeneratorIsotropic::recalcWeight()
{
  weight=-phiRange*4*cosThetaDelta; //This is the part of 4Pi 
}


void GeneratorIsotropic::setMomentum(double mom)
{
  momentum=mom;
}

int GeneratorIsotropic::generateEvent(GeneratorEvent *ev)
{
  //build theta:
  double theta=acos(cosThetaMin+cosThetaDelta*qrndNumbers[0]);
  double r=qrndNumbers[1];
  
  //build phi:
  double phi=0.;
  if (r>=0.5)
    {
      phi=M_PI;
      r-=0.5;
    }
  phi+=phiRange*4*(r-0.25);
  ev->weight*=weight;

  GeneratorParticle p;
  p.particle=particle;
  p.momentum.SetXYZM(momentum*sin(theta)*cos(phi),momentum*sin(theta)*sin(phi),momentum*cos(theta),mass);
  ev->particles.push_back(p);
  return 1;
}
