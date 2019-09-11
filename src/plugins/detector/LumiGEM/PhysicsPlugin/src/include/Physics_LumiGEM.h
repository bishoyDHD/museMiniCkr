#ifndef __PHYSICS_LUMIGEM__
#define __PHYSICS_LUMIGEM__

#include "TObject.h"
#include "Plugin.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "lumigemtree.h"
//#include "toftree.h"
//#include "slowctrl.h"
//#include "sclumi.h"
#include <iostream>
#include "TGTab.h"

#include<muserawtree.h>


class Physics_LumiGEM : public Plugin
{
  //
  // trigger and eventid:
  int lumitriggercounter[2];
  //
  // slowcontrol data that we might need:
  /*  slowctrl::manager *SCManager;
  slowctrl::datum *SC_BeamCurrent, *SC_EDipole, *SC_ToroidCurrentSet, *SC_ToroidCurrentMeas;
  slowctrl::datum *SC_TargetFlowSet, *SC_TargetFlowMeas;
  SCLumiInfo    *SClumi;
  */
  //
  // cooked raw data:
  LumiGEM *lumigem;
  //  ToF     *tof;
  MRTEventInfo *eventinfo;
  MRTRunInfo   *runinfo;
  double   runstarttime, runstoptime;
  //
  // luminosity estimate:
  int lumitrackcnt[2], protoncnt[2];
  //
  // result file:
  FILE *resultfile;

  Long_t SimpleTracking(int verbose=0);
  TH2I *T1hitmap, *T2hitmap;
  int   T1T2acceptcnt, SiPMtriggercnt;


 public:
  Physics_LumiGEM(TTree *in, TTree *out, TFile *inf_, TFile *outf_, TObject *p);
  virtual ~Physics_LumiGEM();
  //
  // for cooking of track/cluster data:
  Long_t defineHistograms();
  Long_t startup();
  Long_t process();
  Long_t done();
  Long_t cmdline(char *cmd);

  ClassDef(Physics_LumiGEM,1);
};

#endif
