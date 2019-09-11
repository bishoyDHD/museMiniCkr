#ifndef __MCGENISOTROPIC__
#define __MCGENISOTROPIC__

#include "TObject.h"
#include "MCGenPlugin.h"
#include "TTree.h"
#include <iostream>
#include "generators.h"

class TH1D;

class MCGenIsotropic:public MCGenPlugin
{
 private:
  GeneratorIsotropic *gen;

  // Histograms
  TH1D * scatAngleHist;
  TH1D * thetaHist;
  TH1D * phiHist;

 public:
  MCGenIsotropic(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p);
  virtual ~MCGenIsotropic();
  // add funtions with return value Long_t here:
  
  Long_t genStartup();
  Long_t genProcess();
  Long_t genFinalize();

  // Functions for setting generator parameters
  Long_t setThetaRange(double tMin, double tMax);
  Long_t setPhiRange(double pRange);
  Long_t setParticle(std::string p);
  Long_t setMomentum(double mom);
   
  virtual Long_t cmdline(char * cmd);

  ClassDef(MCGenIsotropic,1);
    };

#endif
