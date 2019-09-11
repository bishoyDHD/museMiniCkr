#include <MCGenBorn.h>

#include<iostream>
#include<cmath>
#include "TH2.h"

MCGenBorn::MCGenBorn(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p):MCGenPlugin(in,out,inf_,outf_,p)
{
  gen=new GeneratorBorn(getSkip(),getSeed());
  generator = gen;
  gen->setThetaRange(10.,100.);
  gen->setPhiRange(20.);
};

MCGenBorn::~MCGenBorn()
{
  if (gen) delete gen;
};

Long_t MCGenBorn::cmdline(char *cmd)
{
  //add cmdline hanling here

  return 0; // 0 = all ok
};


extern "C"{
Plugin *factory(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p)
{
  return (Plugin *) new MCGenBorn(in,out,inf_,outf_,p);
}
}

Long_t MCGenBorn::genStartup()
{
  scatAngleHist = dH2("MC/Gen/born/angle_v_angle","Lepton and Proton Scattering Angles;Lepton Angle (Degrees);Proton Angle (Degrees);Expected Counts",
		      180,0.,180.,90,0.,90.);

  return 0;
}

Long_t MCGenBorn::genProcess()
{
  gen->generate(ge);
  scatAngleHist->Fill(ge->particles[0].momentum.Angle(ge->lepton_prescatter.momentum.Vect())*180./M_PI,
		      ge->particles[1].momentum.Angle(ge->lepton_prescatter.momentum.Vect())*180./M_PI,
		      ge->weight);
  return Plugin::ok;
}

Long_t MCGenBorn::genFinalize()
{
  return 0;
}

Long_t MCGenBorn::setThetaRange(double tMin, double tMax)
{
  gen->setThetaRange(tMin,tMax);
  return 0;
}

Long_t MCGenBorn::setPhiRange(double pRange)
{
  gen->setPhiRange(pRange);
  return 0;
}

ClassImp(MCGenBorn);

