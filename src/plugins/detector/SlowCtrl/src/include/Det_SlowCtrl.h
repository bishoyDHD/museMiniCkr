#ifndef __DET_SLOWCTRL__
#define __DET_SLOWCTRL__

#include "TObject.h"
#include "Plugin.h"
#include "TTree.h"
#include <iostream>
#include "muserawtree.h"
#include "slowctrl.h"

class Det_SlowCtrl:public Plugin
{
 private:
  slowctrl::manager manager;
  MRTIndex2Name *index2name;
  MRTSlowData   *data,*dataout;
  int nextevent;
  void internal_process();
  int * lastEventResult;
 public:
  Det_SlowCtrl(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p);
  virtual ~Det_SlowCtrl();
  // add funtions with return value Long_t here:
  Long_t init();
  Long_t process();


  virtual Long_t cmdline(char * cmd);

  ClassDef(Det_SlowCtrl,1);
};

#endif
