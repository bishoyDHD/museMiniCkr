#ifndef __MCGENFIXED__
#define __MCGENFIXED__

#include "TObject.h"
#include "MCGenPlugin.h"
#include "TTree.h"
#include <iostream>
#include "generators.h"

class MCGenFixed:public MCGenPlugin
{
 private:
  GeneratorFixed * gen;

 public:
  MCGenFixed(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p);
  virtual ~MCGenFixed();
  
  Long_t genStartup();
  Long_t genProcess();
  Long_t genFinalize();

  // Functions for setting angular ranges
  Long_t setParticle(std::string p);
  Long_t setTheta(double theta);
  Long_t setPhi(double phi);
  Long_t setMomentum(double mom);

  virtual Long_t cmdline(char * cmd);

  ClassDef(MCGenFixed,1);
    };

#endif
