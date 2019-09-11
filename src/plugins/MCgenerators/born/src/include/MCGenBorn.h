#ifndef __MCGENBORN__
#define __MCGENBORN__

#include "TObject.h"
#include "MCGenPlugin.h"
#include "TTree.h"
#include <iostream>
#include "generators.h"

class TH2D;

class MCGenBorn:public MCGenPlugin
{
 private:
  GeneratorBorn * gen;

  // Histograms
  TH2D * scatAngleHist;

 public:
  MCGenBorn(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p);
  virtual ~MCGenBorn();
  Long_t genStartup();
  Long_t genProcess();
  Long_t genFinalize();
  
  // Functions for setting angular ranges
  Long_t setThetaRange(double tMin, double tMax);
  Long_t setPhiRange(double pRange);

  virtual Long_t cmdline(char * cmd);

  ClassDef(MCGenBorn,1);
    };

#endif


