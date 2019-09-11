#ifndef __TAB_LUMIGEM__
#define __TAB_LUMIGEM__

#include "TObject.h"
#include "Plugin.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "lumigemtree.h"
#include <iostream>
#include "TCanvas.h"
#include "TGButton.h"
#include "TGNumberEntry.h"
#include "TGComboBox.h"
#include "TGProgressBar.h"
#include "RQ_OBJECT.h"

#include<muserawtree.h>


#include "cLumiGEMBinaryBlobRead.h"
#include "cGEM.h"
#include "cAPV.h"
#include "cHitFinder.h"
//#include "Det_LumiGEM.h"

class Det_LumiGEM;

class Tab_LumiGEM //: public Plugin
{
  //RQ_OBJECT("Tab_LumiGEM");
 public:
  Tab_LumiGEM();
  virtual ~Tab_LumiGEM();
  //
  Long_t InitGUI(Det_LumiGEM *plugin, TGCompositeFrame *tgframe);
  Long_t defineHistograms();
  Long_t ProcessSClumi();
  Long_t ProcessTOFs();
  Long_t ProcessSiPMs();
  Long_t CheckTriggerPattern();
  Long_t DrawRawData();
  Long_t SimpleTracking();
  Long_t showTraces();
  Long_t PrepareHitMaps();
  Long_t DrawHitMaps();
  Long_t FillHitMaps();
  //
  void SetTriggerPatternOI();
  //
double cmode_mean_x_apv1[3],cmode_mean_y_apv1[3];
double cmode_mean_x_apv2[3],cmode_mean_y_apv2[3];
int avg_ped_x[3][250],avg_ped_y[3][250];

 private:
  Det_LumiGEM      *lumiplugin;
  TGCompositeFrame *myframe;
  TCanvas          *canvas;
  TH1I *rawhistoX[3], *rawhistoY[3];
  TH1I *hitmapX[3],   *hitmapY[3], *hitmap2d[3];
  TH1I *clusterampl[3], *clustercharge[3];
  TH2D *tofhitmap, *tofadcmap[2], *tofqsummap, *tofposmap;
  TH2D *tofmtimemap, *tofqsummtimemap[36];
  TH2D *T1hitmap, *T2hitmap, *T1hitmapTUT, *T2hitmapTUT;
  TH1D *MIGEMresX, *MIGEMresY;
  TH2D *T1TUTeffmap, *T2TUTeffmap;
  TH1D *TOFtrackhisto;
  TH1D *SiPMTDChisto[4], *SiPMTOFhisto[2];
  TH2D *SiPMQDCscattraw[2], *SiPMQDCscatttrk[2];
  int   T1T2acceptcnt, TUTtriggercnt;
  FILE *triggerfile;
  //
  // GUI:
  TGCheckButton    *CBGraphicsOutput, *CBFastProcessing, *CB1dClusters;
  TGNumberEntry    *NEVerbosityLevel;
  TGCheckButton    *CBStopOnGoodEvent, *CBSkipOtherEvents;
  TGNumberEntry    *NETriggerPatternOI, *NETriggerOICounter, *NETriggerUnderTest, *NEGEMidOI;
  TGHProgressBar   *PBTrigEffBar;
  TGCheckButton    *CBRearTOFs;
  TGNumberEntry    *NEReadTOFtof, *NEReadTOFedep;
  TGNumberEntry    *NETOFmulti;
  TGCheckButton    *CBGEMtrackcandidate;
  TGCheckButton    *CBTOFmon[36];
  TGTextButton     *TBtest;
  TGComboBox       *CBvsPlotX, *CBvsPlotY;
  TGCheckButton    *CBfreerunCTLB[16], *CBprescaledCTLB[16];
  TGTextEntry      *TEFreePattHex, *TEPrescPattHex;

  Pixel_t           red, green;

  void Set1dClusterOnOff();
  void DoTest() {printf("test!\n");};

  bool fillhistosTab, protoncandidate;

  int verbose;
  int HistogramsDefined;

  ClassDef(Tab_LumiGEM,1);
};

#endif
