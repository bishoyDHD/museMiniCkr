#include <MCGenIsotropic.h>

#include<iostream>
#include<cmath>
#include "TH1.h"


MCGenIsotropic::MCGenIsotropic(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p):MCGenPlugin(in,out,inf_,outf_,p)
{
  gen=new GeneratorIsotropic(getSkip(),getSeed());
  generator = gen;
  gen->setParticle("e-");
  gen->setMomentum(2000);// 2GeV
  gen->setThetaRange(10,110);
  gen->setPhiRange(20); 
};


MCGenIsotropic::~MCGenIsotropic()
{
  if (gen) delete gen;
};

Long_t MCGenIsotropic::genStartup()
{
  scatAngleHist = dH1("MC/Gen/isotropic/scatteringAngle","Scattering Angle;Theta (Degrees);Expected Counts",180,0.,180.);
  thetaHist = dH1("MC/Gen/isotropic/theta","Theta in Lab Frame;Theta (Degrees);Expected Counts",180,0.,180.);
  phiHist = dH1("MC/Gen/isotropic/phi","Phi in Lab Frame;Phi (Degrees);Expected Counts",360,-90.,270.);

  return 0;
}


Long_t MCGenIsotropic::genProcess()
{
  gen->generate(ge);
  // Fill our histograms
  scatAngleHist->Fill(ge->particles[0].momentum.Angle(ge->lepton_prescatter.momentum.Vect())*180./M_PI,ge->weight);
  thetaHist->Fill(ge->particles[0].momentum.Theta()*180./M_PI,ge->weight);
  phiHist->Fill(fmod((ge->particles[0].momentum.Phi()*180./M_PI)+450.,360.)-90.,ge->weight);

  return Plugin::ok;
}


Long_t MCGenIsotropic::genFinalize()
{
  // nothing special
  return 0;
}

Long_t MCGenIsotropic::setThetaRange(double tMin, double tMax)
{
  gen->setThetaRange(tMin,tMax);
  return 0;
}

Long_t MCGenIsotropic::setPhiRange(double pRange)
{
  gen->setPhiRange(pRange);
  return 0;
}

Long_t MCGenIsotropic::setParticle(std::string p)
{
  gen->setParticle(p);
  return 0;
}

Long_t MCGenIsotropic::setMomentum(double mom)
{
  gen->setMomentum(mom);
  return 0;
}

Long_t MCGenIsotropic::cmdline(char *cmd)
{
  //add cmdline hanling here

  return 0; // 0 = all ok
};

extern "C"{
Plugin *factory(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p)
{
  return (Plugin *) new MCGenIsotropic(in,out,inf_,outf_,p);
}
}


ClassImp(MCGenIsotropic);

