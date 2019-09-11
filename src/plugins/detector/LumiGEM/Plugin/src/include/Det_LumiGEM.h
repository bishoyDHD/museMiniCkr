#ifndef __DET_LUMIGEM__
#define __DET_LUMIGEM__

#include "TObject.h"
#include "Plugin.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "lumigemtree.h"
#include "musetdc1190tree.h"
#include "museadctree.h"
//#include "VETOtree.h"

#include <iostream>
#include "TGTab.h"
#include <vector>
#include<muserawtree.h>


#include "cLumiGEMBinaryBlobRead.h"
#include "cLumiGEMAsciiRead.h"
#include "cGEM.h"
#include "cAPV.h"
#include "cHitFinder.h"
#include "Tab_LumiGEM.h"

class TEvePointSet;



extern const char GEMname[GEM_NUM][8];
extern const char xystr[3][8];
extern const char goodbad[2][8];


class cmodeinfo{
 public:
  int x_event,y_event,x,y,maxx,maxy,maxchx,maxchy;
};


class cmodeinfo_apv{
 public:
  int x_event_apv1,y_event_apv1,x_apv1,y_apv1,maxx_apv1,maxy_apv1,maxchx_apv1,maxchy_apv1;
  int x_event_apv2,y_event_apv2,x_apv2,y_apv2,maxx_apv2,maxy_apv2,maxchx_apv2,maxchy_apv2;
};

class Det_LumiGEM : public Plugin
{
 private:
 //VETOraw *vetoraw;

  TEvePointSet * LHits;

bool preproc;
std::vector< std::vector<double> > cmcthx; // New Hit finder routines
std::vector< std::vector<double> > cmcthy; // New Hit finder routines

 public:

//std::vector< std::vector<double> > cmcthx; // New Hit finder routines
//std::vector< std::vector<double> > cmcthy; // New Hit finder routines
////////////////////////////////
   // New Hit finder routines
      std::vector<unsigned int> localMax(std::vector<double> dat, double thresh);
      std::vector<unsigned int> localMax(std::vector<double> dat, std::vector<double> thresh);
      double meanHit(unsigned int ind, std::vector<double> dat, LumiGEMhit *hit);
      unsigned int maxind(std::vector<double> const &dat, std::vector<bool> const &mask);
///////////////////////REad ASCII files ///////////////////////////////////

//cLumiGEMBinaryBlobRead *blobreader;
 // histograms for raw data cooking:
 // TH1I          *hx;
  // TH1I          *rmshisto[2][4];
  TH2D          *hm0, *hm1, *hm2, *hm3, *hm4, *hm5;
  TH1D *LMhitmapX[GEM_NUM], *LMhitmapXw[GEM_NUM], *LMhitmapY[GEM_NUM], *LMhitmapYw[GEM_NUM];
  TH1D *clustercount[GEM_NUM];
  TH2D *clust_cosmic[GEM_NUM];
  // TH1D *oneclustereve[2][4];
  //////////////////////////////

   //  double cmode_mean_x[2][GEM_NUM],cmode_mean_y[2][4];
   double cmode_mean_x_apv1[GEM_NUM],cmode_mean_y_apv1[GEM_NUM],cmode_mean_x_apv2[GEM_NUM],cmode_mean_y_apv2[GEM_NUM];
 double getavgpedx[GEM_NUM][250],getavgpedy[GEM_NUM][250],avg_ped_x[GEM_NUM][250],avg_ped_y[GEM_NUM][250];
 double getavgonlypedx[GEM_NUM][250],getavgonlypedy[GEM_NUM][250],avg_pedonly_x[GEM_NUM][250],avg_pedonly_y[GEM_NUM][250];
 double xmean[GEM_NUM][250],ymean[GEM_NUM][250],xrms[GEM_NUM][250],yrms[GEM_NUM][250],bkgmean_x[GEM_NUM][250],bkgmean_y[GEM_NUM][250],bkgrms_x[GEM_NUM][250],bkgrms_y[GEM_NUM][250];

  int           *xdat,    *ydat;
  int           *xdatraw, *ydatraw;
  int           *tmpdat;
  //
  int apvx1,apvy1;

  // data quality checking configuration:
  int valAPVsyncDataRangeLow, valAPVsyncDataRangeHigh;
  int valAPVoutOfSyncSampleThreshold, valAPVoutOfSyncEventThreshold;
  int valAPVzeroSamplesEventThreshold;
  int APVoutOfSyncCNT, APVzeroSamplesCNT;
  //
  // hit and cluster finder configuration:
  double valLMGaussFilterSigma;
  int    valLMMinimumAmplitude, valLMMinimumDistance, valLMMaximumNLM, valCFMinimumAmplitude;
  double valCFSigmaMin, valCFSigmaMax;
  unsigned int triggerpatternOI;
  int    valmake1dhits;
  int    valrepairbrokenstrips;

  //defines the cluster size
  int csize;
  //
  // histograms for raw data cooking:
  TH1I          *hx;
  TH1D          *rmshisto[GEM_NUM];
  TH2D          *H_XvsX[GEM_NUM][GEM_NUM];
  TH2D          *H_YvsY[GEM_NUM][GEM_NUM];
  TH2D          *H_YvsX[GEM_NUM][GEM_NUM];
  TH2D          *H_XvsY[GEM_NUM][GEM_NUM];
  TH2D          *h2thresholdx[GEM_NUM];
  TH2D          *h2thresholdy[GEM_NUM];
  TH1D          *h1thresholdx[GEM_NUM];
  TH1D          *h1thresholdy[GEM_NUM];

  TH2D          *H_XvsX_cut[GEM_NUM][GEM_NUM];
  TH2D          *H_YvsY_cut[GEM_NUM][GEM_NUM];
  TH2D          *H_hitmap2d[GEM_NUM];
  TH2D          *hm2d[GEM_NUM];

  TH1D *osci[24];

  TH1D *totadc_xped[GEM_NUM][250];
  TH1D *totadc_yped[GEM_NUM][250];
  TH2D *totadc_yped_eve[GEM_NUM][250];
  TH2D *totadc_yped_strip[GEM_NUM];
  TH2D *totadc_xped_strip[GEM_NUM];
  TH2D *avg_yped_strip[GEM_NUM];
  TH2D *avg_xped_strip[GEM_NUM];
  TH2D *xcm_ped[GEM_NUM];
  TH2D *ycm_ped[GEM_NUM];
  //TH2D *GEM0X_bin2bin_ped,*GEM1X_bin2bin_ped,*GEM2X_bin2bin_ped,*GEM3X_bin2bin_ped;
  //TH2D *GEM0Y_bin2bin_ped,*GEM1Y_bin2bin_ped,*GEM2Y_bin2bin_ped,*GEM3Y_bin2bin_ped;
    TH2D *GEM0X_bin2bin_ped,*GEM1X_bin2bin_ped,*GEM2X_bin2bin_ped;
    TH2D *GEM0Y_bin2bin_ped,*GEM1Y_bin2bin_ped,*GEM2Y_bin2bin_ped;
  //
  // histograms for explora:
  TH1I          *rmshisto_explora;
  TH2I          *chargesharing_explora;
  TH2I          *clustermultiplicity_explora;
  TH2I          *amplitudes_explora;
  TH2I          *hitmap_explora[GEM_NUM];

  TH2D *h2maxxADCb4cm[GEM_NUM],*h2maxyADCb4cm[GEM_NUM],*h2maxxADCcm[GEM_NUM],*h2maxyADCcm[GEM_NUM],*h2maxxADCcmped[GEM_NUM],*h2maxyADCcmped[GEM_NUM];
  //  TH2D *h2maxxADCcm[GEM_NUM],*h2maxyADCcm[GEM_NUM];
  TH2D *h2adcspec_x[GEM_NUM],*h2adcspec_y[GEM_NUM],*h2adcspec_nopk_x[GEM_NUM],*h2adcspec_nopk_y[GEM_NUM],*h2evespec_nopk_x[GEM_NUM],*h2evespec_nopk_y[GEM_NUM];
  TH2D *bckrms_nopk_x[GEM_NUM],*bckrms_nopk_y[GEM_NUM];
  TH2D *h2adcspec_cmode_x[GEM_NUM],*h2adcspec_cmode_y[GEM_NUM],*h2adcspec_cmode_nopk_x[GEM_NUM],*h2adcspec_cmode_nopk_y[GEM_NUM];
  TH2D *h2adcspec_cm_pedsub_x[GEM_NUM],*h2adcspec_cm_pedsub_y[GEM_NUM],*h2adcspec_cm_pedsub_nopk_x[GEM_NUM],*h2adcspec_cm_pedsub_nopk_y[GEM_NUM];
  TH2D *h2adcspec_cm_pedsub_peak_x[GEM_NUM],*h2adcspec_cm_pedsub_peak_y[GEM_NUM];

  TH2D *h2adcspec_pedsub_nopk_x[GEM_NUM],*h2adcspec_pedsub_nopk_y[GEM_NUM];
  TH1D *h1rawadc_x[GEM_NUM],*h1rawadc_y[GEM_NUM];

  TH2D *h2adcspec_base_x[GEM_NUM],*h2adcspec_base_y[GEM_NUM],*h2adcspec_base_nopk_x[GEM_NUM],*h2adcspec_base_nopk_y[GEM_NUM];
  TH2D *h2hitmaps[GEM_NUM],*h2amplmapsx[GEM_NUM],*h2amplmapsy[GEM_NUM],*normamplmapsx[GEM_NUM],*normamplmapsy[GEM_NUM],*H_amplmap2d[GEM_NUM];
TH2D *deadchmap[GEM_NUM];
  //  TH2D *normamplmapsx[GEM_NUM],*normamplmapsy[GEM_NUM];

  TH1D *H_hitampx[GEM_NUM],*H_hitampy[GEM_NUM],*totentries[GEM_NUM];

  TH1D *h1cmode_after_x[GEM_NUM],*h1cmode_after_y[GEM_NUM];
  // TH1D *h1adcspec_cmode_x[GEM_NUM],*h1adcspec_cmode_y[GEM_NUM];
  TH1D *h1adcspec_cmode_x_apv1[GEM_NUM],*h1adcspec_cmode_y_apv1[GEM_NUM],*h1adcspec_cmode_x_apv2[GEM_NUM],*h1adcspec_cmode_y_apv2[GEM_NUM];
  TH1D *h1adcspec_x[GEM_NUM],*h1adcspec_y[GEM_NUM];

  int nx,ny;
  //

  cGEM          *GEM[GEM_NUM];
  cHitFinder    *hitfinder;
  //cLumiGEMDataRead *datareader;
  cLumiGEMBinaryBlobRead *blobreader;

  // branches on input tree:
  MRTBinaryBlob *LUMI;
  MRTEventInfo  *eventinfo;
  MRTRunInfo    *runinfo;
  // branch on output tree:
  LumiGEM       *gemo;
  std::vector <LumiGEMhit> hitlist; // to store 1d hits if 2d hits should appear in gemo!
  int StringToValue(const char *needle, const char haystack[][8]);
  void ShowBasicConfig();
  //
  // trigger and eventid:
  int lumitriggercounter, lumibankcounter;
  //
  double         beamcharge;
  double         runstarttime, runstoptime;
  double         thiseventtime, lasteventtime;
  TH1D          *histo_estlumi;
  double         estlumicounter;
  double         nextsctime;
  //
  // Detector status:
  bool           ProcessDetectorFlagOnly;
  char           WriteDetectorFlagFilename[512];
  unsigned int   DetectorStatus;


  //stuff for inner loop
  bool zeroFLAG, outofsyncFLAG;
  int xclustercnt[GEM_NUM], yclustercnt[GEM_NUM];

  Long_t common_process();
  bool common_process_inner(int i);

  bool doFindClusters;
  void findClusters(int i);
  bool allSpecHist;
  bool someSpecHist;

 //////////////////////////////////////////////////////////////////

 public:
  Det_LumiGEM(TTree *in, TTree *out, TFile *inf_, TFile *outf_, TObject *p);
  virtual ~Det_LumiGEM();
  //
  // general configuration:
  int    verbose;
  Long_t SetVerbosity(int verb) { verbose = verb; return 0; };
  Long_t SetTriggerPattern(int id, unsigned int triggerpattern) { triggerpatternOI = triggerpattern; return 0; };
  Long_t setDetectorFlagOnly(bool trueorfalse=true) { ProcessDetectorFlagOnly = trueorfalse; return 0; };
  Long_t SetDetectorFlagFilename(const char *fn) { sprintf(WriteDetectorFlagFilename, "%s", fn); return 0; };
  //
  // configuration of APV/GEM mapping:
  Long_t AddAPVtoGEM(int lAPVnum, const char *gem, const char *xy, int skipfirst);
  Long_t AddAPVtoGEMright(int id, int lAPVnum, const char *gem, const char *xy, int skipfirst);
  Long_t AddAPVtoGEMleft(int id, int lAPVnum, const char *gem, const char *xy, int skipfirst);
  Long_t ModifyAPVright(int id, int lAPVnum, const char *chlist, int size, cAPV::eSampleModification m);
  Long_t ModifyAPVleft(int id, int lAPVnum, const char *chlist, int size, cAPV::eSampleModification m);
  Long_t SwapGEMaxis(int id, const char *sector, const char *gem, const char *xy);

  //Long_t SkipAPVChannelsright(int id, int lAPVnum, const char *channelstoskip, int size);
  Long_t SkipAPVChannelsright(int id, int lAPVnum, const char *channelstoskip, int size);
  //
  // configuration of data quality checking:
  Long_t SetAPVsyncDataRange(int id, int low, int high)
  { valAPVsyncDataRangeLow=low; valAPVsyncDataRangeHigh=high; return 0; };
  Long_t SetAPVoutOfSyncSampleThreshold(int id, int val)  { valAPVoutOfSyncSampleThreshold=val;  return 0; };
  Long_t SetAPVoutOfSyncEventThreshold(int id, int val)   { valAPVoutOfSyncEventThreshold=val;   return 0; };
  Long_t SetAPVzeroSamplesEventThreshold(int id, int val) { valAPVzeroSamplesEventThreshold=val; return 0; };
  //
  // configuration of hit and cluster finder:
  Long_t SetLMGaussFilterSigma(int id, double val) { valLMGaussFilterSigma = val; return 0; };
  Long_t SetLMMininumAmplitude(int id, int val)    { valLMMinimumAmplitude = val; return 0; };
  Long_t SetLMMinimumDistance(int id, int val)     { valLMMinimumDistance  = val; return 0; };
  Long_t SetLMMaximumNLM(int id, int val)          { valLMMaximumNLM       = val; return 0; };
  Long_t SetCFMinimumAmplitude(int id, int val)    { valCFMinimumAmplitude = val; return 0; };
  Long_t SetCFSigmaRange(int id, double min, double max) { valCFSigmaMin = min; valCFSigmaMax = max; return 0; };
  Long_t SetMake1dClusters(int id, int val)        { valmake1dhits = val; return 0; };
  Long_t SetRepairBrokenStrips(int id, int val)    { valrepairbrokenstrips = val; return 0; };
  //


  //configuration for pedestal

  Long_t SetPerChannelPedestal(int id, int ,int ,int,int , int );

  // mask the dead channels
 Long_t Maskdeadchannels(int id, int ,int,int,int);

 // Do APV channel-2-channel gain calibration
  Long_t Channel_2_channel_gain_calib(int id ,int ,int, int, double);
 

  // configuration of SC channels to be watched:
  Long_t watchSCchannel(int id, const char *name, int sector, double low, double high);
  Long_t watchSCHVchannel(int id, const char *name, int sector, double setpoint, double tolerance);
  //
  // initialization routines:
  void InitSC();               // initialize slow control interface
  void InitLumiBranches_in();  // initialize lumi data branches in input tree
  void InitLumiBranches_out(); // initialize lumi data branches in output tree
  void InitRMShistos();        // initialize RMS histos (data quality control)
  void InitHitFinder();        // initialize hit and cluster finder
  //

  // for cooking of raw data:
  Long_t defineHistograms();
  Long_t defineHistogramsSpec();
  Long_t defineHistogramsSpecAll();
  Long_t startup();
  Long_t process();
  Long_t test();
  Long_t finaltest();

  Long_t showTraces();
  Long_t done();
  Long_t cmdline(char *cmd);
  //
  // For the Event Display
  Long_t startup_vis();
  bool vis;

  /////////////All background processes (cmode, pedestal, etc...), dead chanel masking, and APV gain calibration constants related ....////////////////////////

  bool use_gainmatch,dead_ch_masking;
  bool doSubtractped,doSubtractcmode,getmeancmode,doSubtractbaseline,onlybin2binsub,doSubtractonlyped,doAPVcalib;

  //cmodeinfo findCMode(int t,int i);
  //findCMode appears to be an old dead function
  cmodeinfo_apv findCMode_apv(int i);

  void subtractCMode(int i,cmodeinfo_apv &);
  void subtractBin2Bin(int i);
 void subtractonlyBin2Bin(int i);
void dobaselinesub(int i);

  int deadchannels[250*2*GEM_NUM*2]; /// Read the dead channels from the init file
  double CalibConsts[250*2*GEM_NUM*2];/// Read the APV gain calibration coefficients from the init file

 Long_t histos_cm_ped();
Long_t process_cm_ped();
Long_t done_cm_ped();
 void cleanup_cm_ped();

  // for pedestal:
 int perchannelped[250*2*GEM_NUM*2]; /// if you read the pedestals from init file
  Long_t startup_findPedestal();
  Long_t dumpPedestals();
 Long_t dump_background_sub();
 Long_t startup_background_sub();
 // for common mode:
  Long_t startup_cmode();
  Long_t getmean_cmode();
  
  // for only pedestal: (without Cmode subtraction)
  Long_t startup_onlyPedestal();
  Long_t dumponlyPedestals();

// APV channel-to-channel calibration
 void callib_apvchannels(int i);
Long_t done_APVcalib();
Long_t startup_APVcalib();

//////////////////////////////////////////////////////////////////////////////////////////////////////

  // common routines for all plugin configurations:
  int    CheckRange(int n, int *dat, int low, int high);
  int    ReadTelescopeData();
  void   cleanup();
  //
  // stuff for the Visual Cooker (visco):
  TGCompositeFrame *viscotab;
  Tab_LumiGEM      *LumiGUI;

  //vectors related to common-mode noise and bin-2-bin pedestal subtractions

  ClassDef(Det_LumiGEM,1);
};

#endif
