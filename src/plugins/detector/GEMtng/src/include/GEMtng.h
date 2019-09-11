#ifndef __GEMTNG__
#define __GEMTNG__

#include "TObject.h"
#include "Plugin.h"
#include "TTree.h"
#include <iostream>
#include<muserawtree.h>
#include "TH2.h"

class telescope;

class GEMtng:public Plugin
{
 private:

  MRTBinaryBlob *LUMI[2];
  telescope *telescopes[2];
  TH2D *h_xcorrelation,*h_ycorrelation;

 public:
  GEMtng(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p);
  virtual ~GEMtng();
  // add funtions with return value Long_t here:

  Long_t dH_allRaw();
  Long_t dH_planeRaw();
  Long_t dH_planeProcessed();
  Long_t dH_planeSum(); 
  Long_t dH_planeHits();
  Long_t dH_stackHits();
  Long_t dH_dxdy();


  
  Long_t startup();
  Long_t process();
  Long_t finalize();

  virtual Long_t cmdline(char * cmd);

  ClassDef(GEMtng,1);
    };

#endif
