#include <Det_LumiGEM.h>
#include "TStyle.h"

#include<iostream>
#include<cmath>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <string>
#include<stdio.h> 

// Event display files
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TProfile.h"

// Root
#include <math.h>
#include "TF1.h"
#include "TGraph.h"
#include "TH1.h"
#include "TMath.h"

#include "TAxis.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TCanvas.h"
#include "TApplication.h"
#include "TStyle.h"
#include "TPad.h"
#include "TROOT.h"
#include "TColor.h"
#include "TFrame.h"
#include "TVirtualPad.h"

using namespace std;

Det_LumiGEM::Det_LumiGEM(TTree *in, TTree *out, TFile *inf_, TFile *outf_, TObject *p)
  : Plugin(in, out, inf_, outf_, p)
{
  cout << "LumiGEM: Plugin constructor called" << endl;

  verbose=0;

  // generate six cGEM objects to build two telescopes:
    for (int i=0; i<GEM_NUM; i++)// 6 GEMs per telescope
      {
	GEM[i] = new cGEM(4); // our GEMs have 4 APVs each
	rmshisto[i] = NULL;
      };

  // some hard-coded defaults for the configuration:
  valLMGaussFilterSigma = 1.5;
  valLMMinimumAmplitude =  25;
  valLMMinimumDistance  =   5;
  valLMMaximumNLM       =  10;
  valCFMinimumAmplitude = 100;
  valCFSigmaMin         = 0.4;
  valCFSigmaMax         = 1.5;
  valmake1dhits         =   0;
  valrepairbrokenstrips =   1;
  // triggerpatternOI      = 0x4;
  valAPVsyncDataRangeLow      = -65535;
  valAPVsyncDataRangeHigh     = +65535;
  valAPVoutOfSyncSampleThreshold   = 0;
  valAPVoutOfSyncEventThreshold    = 1;
  valAPVzeroSamplesEventThreshold  = 1;

  //Cluster size
  //variable created by Ethan to remove hard coded constants everywhere
  //Originally 2, changed to make clusters a bit smoother
  csize = 3;
  // Not event display mode by default
  vis = false;
  // trigger pattern of interest: by default we're interested in anything:
  triggerpatternOI = 0xff;

  // slowcontrol lumi info not yet connected:
  //  SClumi = NULL;
  // eventinfo branch not yet connected:
  // eventinfo = NULL;

  // Detector status undefined at the beginning:
  ProcessDetectorFlagOnly = false;
  DetectorStatus = LumiGEM::unknown;
  WriteDetectorFlagFilename[0]=0;

  LumiGUI = new Tab_LumiGEM();

  //unused
  // for (int i=0;i<250*GEM_NUM*2*2;i++)
  //   perchannelped[i]=0;

  someSpecHist=false;
  allSpecHist=false;
}

Det_LumiGEM::~Det_LumiGEM()
{
}

Long_t Det_LumiGEM::defineHistograms()
{
  printf(" Det_LumiGEM::defineHistograms() called\n");
  InitRMShistos();
  // x vs x and y vs y histograms (mapping check):
  int nbins_vs=64;
    for (int i=0; i<GEM_NUM; i++)
      {
	//	H_hitmap2d[t][i] = dH2(Form("MUSEtele/%s/%s_hitmap",leftright[t], GEMname[i]),
	//		       Form("%s %s GEM hitmap", leftright[t], GEMname[i]),
	//		       250, -0.5, 249.5, 250, -0.5, 249.5);
	H_hitmap2d[i] = NULL;
	H_hitmap2d[i] = dH2(Form("MUSEtele/%s_hitmap", GEMname[i]),
			    Form("%s GEM hitmap", GEMname[i]),
		            100, -50.0, 50.0, 100, -50.0, 50.0);

	//	hm2d[t][i] = dH2(Form("MUSEtele/%s/%s_gemhitmap",leftright[t], GEMname[i]),
	//		       Form("%s %s GEM hitmap", leftright[t], GEMname[i]),
	//		       400,-60,60,400,-60,60);

	for (int j=0; j<GEM_NUM; j++)
	  {
	    H_XvsX[i][j]=NULL;
	    H_YvsY[i][j]=NULL;
	    if (i==j) continue;
	    H_XvsX[i][j] = dH2(Form("MUSEtele/%sx_vs_%sx", GEMname[i], GEMname[j]),
				  Form("%s x vs. %s x", GEMname[i], GEMname[j]),
				  nbins_vs, -0.5, 249.5, nbins_vs, -0.5, 249.5);
	    printf("    [%d][%d] %p\n", i, j, H_XvsX[i][j]);
	    H_YvsY[i][j] = dH2(Form("MUSEtele/%sy_vs_%sy", GEMname[i], GEMname[j]),
				  Form("%s y vs. %s y", GEMname[i], GEMname[j]),
				  nbins_vs, -0.5, 249.5, nbins_vs, -0.5, 249.5);
	    printf("    [%d][%d] %p\n", i, j, H_YvsY[i][j]);

	    ////////
	    
	 //    for (int k=0; k<GEM_NUM; k++)
	 //      {
		// H_XvsX_cut[i][j]=NULL;
		// H_YvsY_cut[i][j]=NULL;
		// H_XvsX_cut[i][k]=NULL;
		// H_YvsY_cut[i][k]=NULL;
		// H_XvsX_cut[j][k]=NULL;
		// H_YvsY_cut[j][k]=NULL;
		
		// //	if (k==j) continue;
		// H_XvsX_cut[i][j] = dH2(Form("MUSEtele/mapping/%sx_vs_%sx", GEMname[i], GEMname[j]),  
		// 	           Form("%s x vs. %s x", GEMname[i], GEMname[j]),
		// 		       nbins_vs, -0.5, 249.5, nbins_vs, -0.5, 249.5);
		// //	printf("    [%d][%d][%d] %p\n", t, i, j, H_XvsX[t][i][j]);
		// H_YvsY_cut[i][j] = dH2(Form("MUSEtele/mapping/%sy_vs_%sy", GEMname[i], GEMname[j]),
		// 		      Form("%s y vs. %s y", GEMname[i], GEMname[j]),
		// 		      nbins_vs, -0.5, 249.5, nbins_vs, -0.5, 249.5);
		// //printf("    [%d][%d][%d] %p\n", t, i, j, H_YvsY[t][i][j]);


		// H_XvsX_cut[i][k] = dH2(Form("MUSEtele/mapping/%sx_vs_%sx", GEMname[i], GEMname[k]),
		// 		      Form("%s x vs. %s x", GEMname[i], GEMname[k]),
		// 		      nbins_vs, -0.5, 249.5, nbins_vs, -0.5, 249.5);
		// //	printf("    [%d][%d][%d] %p\n", t, i, j, H_XvsX[t][i][j]);
		// H_YvsY_cut[i][k] = dH2(Form("MUSEtele/mapping/%sy_vs_%sy", GEMname[i], GEMname[k]),
		// 		      Form("%s y vs. %s y",GEMname[i], GEMname[k]),
		// 		      nbins_vs, -0.5, 249.5, nbins_vs, -0.5, 249.5);

		// H_XvsX_cut[j][k] = dH2(Form("MUSEtele/mapping/%sx_vs_%sx", GEMname[j], GEMname[k]),
		// 		      Form("%s x vs. %s x", GEMname[j], GEMname[k]),
		// 		      nbins_vs, -0.5, 249.5, nbins_vs, -0.5, 249.5);
		// //	printf("    [%d][%d][%d] %p\n", t, i, j, H_XvsX[t][i][j]);
		// H_YvsY_cut[j][k] = dH2(Form("MUSEtele/mapping/%sy_vs_%sy", GEMname[j], GEMname[k]),
		// 		      Form("%s y vs. %s y", GEMname[j], GEMname[k]),
		// 		      nbins_vs, -0.5, 249.5, nbins_vs, -0.5, 249.5);

	 //      }; // k

	    ///////
	  }; // j

      }; //i
  //LumiGUI->defineHistograms();

  //return ok;
  return 0;
}

Long_t Det_LumiGEM::defineHistogramsSpec()
{
  someSpecHist=true;
    for (int i=0; i<GEM_NUM; i++)
      {
	deadchmap[i]=dH2(Form("MUSEtele/%s/deadchmap%s",GEMname[i],GEMname[i]), Form("%d GEM Dead Channel Map",i), 250, -0.5, 249.5,250, -0.5, 249.5);
	// cmode related................ 
	h1adcspec_cmode_x_apv1[i]=dH1(Form("MUSEtele/%s/getmean_cmode_x_apv1",GEMname[i]), Form("%d GEM X APV 1 - peak excluded average ADCs for many events",i),100,0.0,3000.0);
	h1adcspec_cmode_y_apv1[i]=dH1(Form("MUSEtele/%s/getmean_cmode_y_apv1",GEMname[i]), Form("%d GEM Y APV 1- peak excluded average ADCs for many events",i),100,0.0,3000.0);
	h1adcspec_cmode_x_apv2[i]=dH1(Form("MUSEtele/%s/getmean_cmode_x_apv2",GEMname[i]), Form("%d GEM X APV 2- peak excluded average ADCs for many events",i),100,0.0,3000.0);
	h1adcspec_cmode_y_apv2[i]=dH1(Form("MUSEtele/%s/getmean_cmode_y_apv2",GEMname[i]), Form("%d GEM Y APV 2- peak excluded average ADCs for many events",i),100,0.0,3000.0);

	//cmode and pedestal subtraction related
	h2adcspec_cmode_nopk_x[i]=dH2(Form("MUSEtele/%s/adcspectra_cmode_nopk_x",GEMname[i]), Form("%d GEM X APV ADCs - nopk (excluding peak)-c mode subtracted",i), 250,-0.5, 249.5,500,-500.5,4499.5);
	h2adcspec_cmode_nopk_y[i]=dH2(Form("MUSEtele/%s/adcspectra_cmode_nopk_y",GEMname[i]), Form("%d GEM Y APV ADCs - nopk (excluding peak)-c mode subtracted",i), 250,-0.5, 249.5,500,-500.5,4499.5);

	h2adcspec_cm_pedsub_nopk_x[i]=dH2(Form("MUSEtele/%s/adcspectra_cm_pedsub_nopk_x",GEMname[i]), Form("%d GEM X APV ADCs after both cm and ped subtraction (excluding peak)",i), 250,-0.5, 249.5,500,-500.5,4499.5);
	h2adcspec_cm_pedsub_nopk_y[i]=dH2(Form("MUSEtele/%s/adcspectra_cm_pedsub_nopk_y",GEMname[i]), Form("%d GEM Y APV ADCs after both cm and ped subtraction (excluding peak)",i), 250,-0.5, 249.5,500,-500.5,4499.5);
    
	h2adcspec_cm_pedsub_peak_x[i]=dH2(Form("MUSEtele/%s/adcspectra_cm_pedsub_peak_x",GEMname[i]), Form("%d GEM X APV Signal ADCs after both cm and ped subtraction",i), 250,-0.5, 249.5,500,-500.5,4499.5);
	h2adcspec_cm_pedsub_peak_y[i]=dH2(Form("MUSEtele/%s/adcspectra_cm_pedsub_peak_y",GEMname[i]), Form("%d GEM Y APV Signal ADCs after both cm and ped subtraction",i), 250,-0.5, 249.5,500,-500.5,4499.5);
	
	if (onlybin2binsub){
	  // h2adcspec_onlyped_nopk_x[t][i]=dH2(Form("MUSEtele/%s/%s/adcspectra_pedsub_nopk_x",leftright[t],GEMname[i]), Form("%d GEM X APV ADCs only ped subtraction (excluding peak)",i), 250,-0.5, 249.5,500,-500.5,4499.5);
	  // h2adcspec_onlyped_nopk_y[t][i]=dH2(Form("MUSEtele/%s/%s/adcspectra_pedsub_nopk_y",leftright[t],GEMname[i]), Form("%d GEM Y APV ADCs only ped subtraction (excluding peak)",i), 250,-0.5, 249.5,500,-500.5,4499.5);
	   h2adcspec_pedsub_nopk_x[i]=dH2(Form("MUSEtele/%s/adcspectra_pedsub_nopk_x",GEMname[i]), Form("%d GEM X APV ADCs only ped subtraction (excluding peak)",i), 250,-0.5, 249.5,500,-500.5,4499.5);
	   h2adcspec_pedsub_nopk_y[i]=dH2(Form("MUSEtele/%s/adcspectra_pedsub_nopk_y",GEMname[i]), Form("%d GEM Y APV ADCs only ped subtraction (excluding peak)",i), 250,-0.5, 249.5,500,-500.5,4499.5);
        
	};

      };
  return ok;
}


Long_t Det_LumiGEM::defineHistogramsSpecAll()
{
  allSpecHist=true;
  defineHistograms(); 
 defineHistogramsSpec();
    for (int i=0; i<GEM_NUM; i++)
      {
	h1adcspec_x[i]=dH1(Form("MUSEtele/%s/adc1dspectra_x",GEMname[i]), Form("%d GEM X APV ADCs",i), 500,-500.5,4499.5);
 	h1adcspec_y[i]=dH1(Form("MUSEtele/%s/adc1dspectra_y",GEMname[i]), Form("%d GEM Y APV ADCs",i), 500,-500.5,4499.5);
 	
 	h2thresholdx[i]=dH2(Form("MUSEtele/%s/2D ADC - Threshold X GEM %i",GEMname[i],i),"ADC - Threshold X",250,-0.5,249.5,500,-500,4500);
 	h2thresholdy[i]=dH2(Form("MUSEtele/%s/2D ADC - Threshold Y GEM %i",GEMname[i],i),"ADC - Threshold Y",250,-0.5,249.5,500,-500,4500);
 	
 	h1thresholdx[i]=dH1(Form("MUSEtele/%s/1D ADC - Threshold X GEM %i",GEMname[i],i),"ADC - Threshold X",500,-500.5,4499.5);
 	h1thresholdy[i]=dH1(Form("MUSEtele/%s/1D ADC - Threshold Y GEM %i",GEMname[i],i),"ADC - Threshold Y",500,-500.5,4499.5);
	
	h2adcspec_x[i]=dH2(Form("MUSEtele/%s/adcspectra_x",GEMname[i]), Form("%d GEM X APV ADCs",i), 250,-0.5, 249.5,500,-500.5,4499.5); 
	h2adcspec_y[i]=dH2(Form("MUSEtele/%s/adcspectra_y",GEMname[i]), Form("%d GEM Y APV ADCs",i), 250,-0.5, 249.5,500,-500.5,4499.5); 
	h2adcspec_nopk_x[i]=dH2(Form("MUSEtele/%s/adcspectra_nopk_x",GEMname[i]), Form("%d GEM X APV ADCs - nopk (excluding peak)",i), 250,-0.5, 249.5,500,-500.5,4499.5);
	h2adcspec_nopk_y[i]=dH2(Form("MUSEtele/%s/adcspectra_nopk_y",GEMname[i]), Form("%d GEM Y APV ADCs - nopk (excluding peak)",i), 250,-0.5, 249.5,500,-500.5,4499.5);
	h2evespec_nopk_x[i]=dH2(Form("MUSEtele/%s/evespectra_nopk_x",GEMname[i]), Form("%d GEM X APV entries - nopk (excluding peak)",i), 250,-0.5, 249.5,100,0.0,100000);
	h2evespec_nopk_y[i]=dH2(Form("MUSEtele/%s/evespectra_nopk_y",GEMname[i]), Form("%d GEM Y APV entries - nopk (excluding peak)",i), 250,-0.5, 249.5,100,0.0,100000);
	
	//	bckrms_nopk_x[t][i]==dH2(Form("MUSEtele/%s/%s/bckrms_nopk_x",leftright[t],GEMname[i]), Form("%d GEM X background RMS",i),250,-0.5, 249.5,100,-100.5,399.5);
	//bckrms_nopk_y[t][i]==dH2(Form("MUSEtele/%s/%s/bckrms_nopk_y",leftright[t],GEMname[i]), Form("%d GEM Y background RMS",i),250,-0.5, 249.5,100,-100.5,399.5);

	//cmode and pedestal subtraction related
	h2adcspec_cmode_x[i]=dH2(Form("MUSEtele/%s/adcspectra_cmode_x",GEMname[i]), Form("%d GEM X APV ADCs - c mode subtracted",i), 250,-0.5, 249.5,4096,-500.5,4499.5);
	h2adcspec_cmode_y[i]=dH2(Form("MUSEtele/%s/adcspectra_cmode_y",GEMname[i]), Form("%d GEM Y APV ADCs - c mode subtracted",i), 250,-0.5, 249.5,4096,-500.5,4499.5);
	h2adcspec_cm_pedsub_x[i]=dH2(Form("MUSEtele/%s/adcspectra_cm_pedsub_x",GEMname[i]), Form("%d GEM X APV ADCs after both cm and ped subtraction",i), 250,-0.5, 249.5,500,-500.5,4499.5);
	h2adcspec_cm_pedsub_y[i]=dH2(Form("MUSEtele/%s/adcspectra_cm_pedsub_y",GEMname[i]), Form("%d GEM Y APV ADCs after both cm and ped subtraction",i), 250,-0.5, 249.5,500,-500.5,4499.5);

	// hit amplitude related
	H_amplmap2d[i]=dH2(Form("MUSEtele/%s/amplmaps%s_aftclust",GEMname[i],GEMname[i]), Form("%d GEM Amplitude Map",i), 250, -0.5, 249.5,250, -0.5, 249.5);
	h2hitmaps[i]=dH2(Form("MUSEtele/%s/hitmaps%s_b4clust",GEMname[i],GEMname[i]), Form("%d GEM Hit Map - Before cluster finds",i), 250, -0.5, 249.5,250, -0.5, 249.5);
	H_hitampx[i]=dH1(Form("MUSEtele/%s/xhitamps%s_b4clust",GEMname[i],GEMname[i]), Form("%d GEM X Hit Amplitudes - Before cluster finds",i), 100, -500.0, 2500);
	H_hitampy[i]=dH1(Form("MUSEtele/%s/yhitamps%s_b4clust",GEMname[i],GEMname[i]), Form("%d GEM Y Hit Amplitudes - Before cluster finds",i), 100, -500.0, 2500);
	h2amplmapsx[i]=dH2(Form("MUSEtele/%s/h2amplmapsx%s_b4clust",GEMname[i],GEMname[i]), Form("%d GEM X Amplitude Map - Before cluster finds; x chan; y chan",i), 250, -0.5, 249.5,250, -0.5, 249.5);
	h2amplmapsy[i]=dH2(Form("MUSEtele/%s/h2amplmapsy%s_b4clust",GEMname[i],GEMname[i]), Form("%d GEM Y Amplitude Map - Before cluster finds; x chan; y chan",i), 250, -0.5, 249.5,250, -0.5, 249.5);
	//totentries[i]=dH1(Form("MUSEtele/%s/totentries%s_b4clust",GEMname[i],GEMname[i]), Form("%d GEM Total Entries - Before cluster finds",i), 250, -0.5, 249.5);

	if (doSubtractbaseline){
	  //h2adcspec_base_x[i]=dH2(Form("MUSEtele/%s/adcspectra_base_x",GEMname[i]), Form("%d GEM X APV ADCs - baseline subtracted",i), 250,-0.5, 249.5,4096,-500.5,4499.5);
	  //h2adcspec_base_y[i]=dH2(Form("MUSEtele/%s/adcspectra_base_y",GEMname[i]), Form("%d GEM Y APV ADCs - baseline subtracted",i), 250,-0.5, 249.5,4096,-500.5,4499.5);
	  //h2adcspec_base_nopk_x[i]=dH2(Form("MUSEtele/%s/adcspectra_base_nopk_x",GEMname[i]), Form("%d GEM X APV ADCs - baseline subtracted (excluding peak)",i), 250,-0.5, 249.5,249.5,-500.5,4499.5);
	  //h2adcspec_base_nopk_y[i]=dH2(Form("MUSEtele/%s/adcspectra_base_nopk_y",GEMname[i]), Form("%d GEM Y APV ADCs - baseline subtracted (excluding peak)",i), 250,-0.5, 249.5,249.5,-500.5,4499.5);
	};

      }
    return ok;
}


void Det_LumiGEM::InitHitFinder()
{
  hitfinder = new cHitFinder(250, 250);
  hitfinder->SetClusterSigmaRange(valCFSigmaMin, valCFSigmaMax);
  // allocate memory for processed and raw data (for one GEM at a time only):
  //Only have 250 channels so why use more mem than you need?
  //Ethan
  xdat    = new int[250];
  ydat    = new int[250];
  //xdatraw = new int[250];
  //ydatraw = new int[250];
}

////////////////////////////////////////////////////////////////////////////////////////////////
//These were all ints
//Probably shouldn't globally define a whole bunch of vectors in a random spot between functions
//Ethan
std::vector <double> dattotx(250,0);
std::vector <double> dattoty(250,0);
std::vector <std::vector <double> > dattotx_nopk_ent(GEM_NUM,dattoty);
std::vector <std::vector <double> > dattoty_nopk_ent(GEM_NUM,dattoty);

std::vector <std::vector <double> > gettotpedx(GEM_NUM,dattotx);
std::vector <std::vector <double> > gettotpedy(GEM_NUM,dattoty);
std::vector <std::vector <double> > gettotpedxentries(GEM_NUM,dattotx);
std::vector <std::vector <double> > gettotpedyentries(GEM_NUM,dattoty);

std::vector <std::vector <double> > gettotonlypedx(GEM_NUM,dattotx);
std::vector <std::vector <double> > gettotonlypedy(GEM_NUM,dattoty);
std::vector <std::vector <double> > gettotonlypedxentries(GEM_NUM,dattotx);
std::vector <std::vector <double> > gettotonlypedyentries(GEM_NUM,dattoty);


std::vector <std::vector <double> > gettotx_adc(GEM_NUM,dattotx);
std::vector <std::vector <double> > gettoty_adc(GEM_NUM,dattotx);
std::vector <std::vector <double> > gettotx_adc_e(GEM_NUM,dattotx);
std::vector <std::vector <double> > gettoty_adc_e(GEM_NUM,dattotx);
std::vector <std::vector <double> > totent(GEM_NUM,dattoty);

//std::vector <int> oneclusteve(3,0);

int totampx[GEM_NUM][250];
int totampy[GEM_NUM][250];
int totampentx[GEM_NUM][250];
int totampenty[GEM_NUM][250];

//std::vector <std::vector <int> > avgampx(3,dattotx);

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

// Startup function for event display (currently not in use)
Long_t Det_LumiGEM::startup_vis()
{
  vis = true; // Flag that this is event display mode
  startup(); // Call the usual startup
  return ok;
}

////////////////////////////////////////////////////////////////////////////////////////////////


Long_t Det_LumiGEM::startup()
{ 
  cout << "LumiGEM: Startup called" << endl;
  // get the SiPM TDC/ADC output tree:
 
  InitSC();
  InitLumiBranches_in();
  InitLumiBranches_out();
  
  //  getBranchObject("EventInfo", (TObject**)&eventinfo);
  runinfo = (MRTRunInfo*)getFileObject("RunInfo");

  // printf("*** eventinfo is %p\n", eventinfo);
  printf("*** runinfo is %p\n", runinfo);

  runstarttime   = runinfo->startTime;
  runstoptime    = runinfo->stopTime;
  nextsctime     = runstarttime+1.0;
  beamcharge     = 0.;
  estlumicounter = 0.;

  blobreader = new cLumiGEMBinaryBlobRead();

  InitHitFinder();

  tmpdat = new int[1024];

  // Do any initializations, e.g. creating histograms, that need the
  // ROOT files already opened:
  //InitRMShistos();

  // print some config information:
  ShowBasicConfig();

  // In case we're called from within the Visual Cooker (visco), we should
  // try to add a tab here:
  viscotab    = addTab("LumiGEM");
  if (viscotab && !vis)
    {
      //gStyle->SetOptStat(1111001);
      printf("LumiGEM: ** VisCo mode **\n");
      LumiGUI->InitGUI(this, viscotab);
      LumiGUI->defineHistograms();
    }
  else if (vis)
    {
      printf("LumiGEM: ** Event Display mode **\n");
      LumiGUI->InitGUI(this, viscotab);
      LumiGUI->defineHistograms();
    }
  else
    printf("LumiGEM: ** non-VisCo mode **\n");

  // count triggers:
  lumitriggercounter = 1;
  lumibankcounter = 0;

 
  dead_ch_masking=true;/// Keep this always "true" if you want to mask the dead channels defined in /init/LumiGEM.xml file.
  use_gainmatch=false;  /// Keep this always "true" if you want to use channel-to-channel gain match for all APV channels.

  doFindClusters=true; //Need always true
  doSubtractped=true; //Need always true
  doSubtractcmode=true; //Need always true
  doAPVcalib=false;
  doSubtractbaseline=false;
  onlybin2binsub=true; //Need always true
  doSubtractonlyped=true;
 

 //////////////////////////////////Read the common mode mean values at startup/////////////////////////////////////
 printf("LumiGEM: ** read mean cmode at startup **\n");
  std::ifstream cmfile("event_avg_cmode.dat");
  std::string line;
  int tel,gem;
  double avgx_cmode_apv1,avgx_cmode_apv2,avgy_cmode_apv1,avgy_cmode_apv2;
  while (std::getline(cmfile, line)) 
    {
      std::istringstream ss(line);
      ss >> gem >> avgx_cmode_apv1>> avgx_cmode_apv2 >> avgy_cmode_apv1>> avgy_cmode_apv2;
      cmode_mean_x_apv1[gem]=avgx_cmode_apv1;
      cmode_mean_y_apv1[gem]=avgy_cmode_apv1;
      cmode_mean_x_apv2[gem]=avgx_cmode_apv2;
      cmode_mean_y_apv2[gem]=avgy_cmode_apv2;
       printf("read mean cmode at startup %d %5.2lf %5.2lf %5.2lf %5.2lf\n ",gem,cmode_mean_x_apv1[gem],cmode_mean_x_apv2[gem],cmode_mean_y_apv1[gem],cmode_mean_y_apv2[gem]);
    };
  cmfile.close();
  ////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////Read the average pedestalonly values at startup/////////////////////////////////////
printf("LumiGEM: ** read average (only)pedestal at startup **\n");
  std::ifstream pedonlyfile("only_bin_2_bin_pedestals.dat");
  std::string pedonlyline;
  int onlygem,onlystrip;
  double pedonlyx,pedonlyy;
  while (std::getline(pedonlyfile, pedonlyline)) 
    {
      std::istringstream onlypss(pedonlyline);
      onlypss >> onlygem >> onlystrip >>pedonlyx >> pedonlyy;
      avg_pedonly_x[onlygem][onlystrip]=pedonlyx;
      avg_pedonly_y[onlygem][onlystrip]=pedonlyy;
      //printf("read average (only)ped at startup %d %d %d %d\n ",onlygem,onlystrip,avg_pedonly_x[onlygem][onlystrip],avg_pedonly_y[onlygem][onlystrip]);
    };
  pedonlyfile.close();
 
  ////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////Read the average pedestal values at startup/////////////////////////////////////
printf("LumiGEM: ** read average cmode subtracted pedestal at startup **\n");
  std::ifstream pedfile("bin_2_bin_pedestals.dat");
  std::string pedline;
  int pedgem,pedstrip;
  double pedx,pedy;
  while (std::getline(pedfile, pedline)) 
    {
      std::istringstream pss(pedline);
      pss >> pedgem >> pedstrip >>pedx >> pedy;
      avg_ped_x[pedgem][pedstrip]=pedx;
      avg_ped_y[pedgem][pedstrip]=pedy;
      
      // printf("read average ped at startup %d %d %d %d\n ",pedgem,pedstrip,avg_ped_x[pedgem][pedstrip],avg_ped_y[pedgem][pedstrip]);
    };
  pedfile.close();
  // printf("finished dead \n"); 
  ////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////Read the background values (after both cmode and pedestal subtract) at startup/////////////////////////////////////
printf("LumiGEM: ** read RMS and Mean of the cmode and pedestal subtracted backgrounds at startup **\n");
  std::ifstream bkgfile("bin_2_bin_bkg.dat");
  std::string bkgline;
  int bkggem,bkgstrip;
  double xrms,xmean,yrms,ymean;
  while (std::getline(bkgfile, bkgline)) 
    {
      std::istringstream pss(bkgline);
      pss >> bkggem >> bkgstrip >>xrms>>xmean>>yrms>>ymean;
      bkgmean_x[bkggem][bkgstrip]=xmean;
      bkgmean_y[bkggem][bkgstrip]=ymean;
      bkgrms_x[bkggem][bkgstrip]=xrms;
      bkgrms_y[bkggem][bkgstrip]=yrms;

      
      // printf("read average ped at startup %d %d %d %d\n ",pedgem,pedstrip,avg_ped_x[pedgem][pedstrip],avg_ped_y[pedgem][pedstrip]);
    };
  bkgfile.close();
 
  ////////////////////////////////////////////////////////////////////////////////////////////////////
   
  // Process the background histograms after subtracting both common mode and pedestals
  preproc = true;
  /*
  (unsigned int j=0; j<in->GetEntries(); j++)
    //   for (unsigned int j=0; j<1000; j++)  
 {
     // if (!(j%50000))
     //   cout<<"Pre-processing event "<<j<<"/"<<in->GetEntries()<<" ("<<(((double)j)/((double)(in->GetEntries()))*100)<<"%)\n";      
      in->GetEntry(j);
      process();
   } 
*/

  // Set-up the common mode and pedestal subtracted threshold containers

  std::vector<double> temp(250,0);
//std::vector< std::vector<double> > cmcthx(3,temp);
   //std::vector< std::vector<double> > cmcthy(3,temp);
  for (unsigned int j=0; j<GEM_NUM; j++) cmcthx.push_back(temp);
  for (unsigned int j=0; j<GEM_NUM; j++) cmcthy.push_back(temp);

  for (unsigned int i=0; i<GEM_NUM; i++)
    {
      for (unsigned int q=0; q<250; q++)
	{	  
	  // TH1D * xcor = xcm_ped[t][i]->ProjectionY("xcor",q+1,q+1);
	   // TH1D * ycor = ycm_ped[t][i]->ProjectionY("ycor",q+1,q+1);

	  //  cmcthx[i][q] = 2.5*xcor->GetRMS();
	  //   cmcthy[i][q] = 2.5*ycor->GetRMS();

	  //cmcthx[i][q]   = 100;
	  //cmcthy[i][q]   = 100;	 	
	  // printf("local maxima %d %d %d %5.3lf %5.3lf \n",t,i,q,cmcthx[i][q],cmcthy[i][q]);

	  //cmcthx[i][q]   = 2.5*bkgrms_x[i][q];
	  //cmcthy[i][q]   = 2.5*bkgrms_y[i][q];
		if(i==4||i==5)//IFP GEMs are noisy so we use a higher threshold for them.
		{
	  		cmcthx[i][q]   = 6.0*bkgrms_x[i][q];
	  		cmcthy[i][q]   = 6.0*bkgrms_y[i][q];
	  	}
	  	else
	  	{
	  		cmcthx[i][q]   = 3.0*bkgrms_x[i][q];
	  		cmcthy[i][q]   = 3.0*bkgrms_y[i][q];
	  	}

	  //delete xcor;
	  //delete ycor;
	};    
    };
  //   in->GetEntry(0);
 //printf("Startup Done \n");
 //////////////////////////////////////////////////////////////////////////////////////////////////

  return ok;
}


Long_t Det_LumiGEM::startup_cmode()
{

  startup();
  //startup_ASCII();
  //  startup();
  //  getmeancmode=true;
  // disable subtracting existing cmode
  doSubtractcmode=false;
  onlybin2binsub=false;
  // disable subtracting existing pedestals
  doSubtractped=false;
 // disable subtracting the baseline
 doSubtractbaseline=false;
  // disable the slow stuff
  doFindClusters=false;
  return ok;
}

Long_t Det_LumiGEM::startup_findPedestal()
{

  startup();
  //  startup();

 // disable subtracting cmode, ONLY if you want to check the bin-2-bin pedestal effects alone.//////////////////
 doSubtractcmode=true;
 onlybin2binsub=false;
  // disable subtracting existing pedestals
  doSubtractped=false;
 // disable subtracting the baseline
 doSubtractbaseline=false;
  // disable the slow stuff
  doFindClusters=false;
  return ok;
}

Long_t Det_LumiGEM::startup_background_sub()
{
  startup();
  //  startup();

 // disable subtracting cmode, ONLY if you want to check the bin-2-bin pedestal effects alone.//////////////////
 doSubtractcmode=true;
 onlybin2binsub=false;
  // disable subtracting existing pedestals
 //This is disabled in find pedestals above but enabled here even though both functions are called for bin_2_bin subtraction recipe
 //Ethan
  doSubtractped=true;
 // disable subtracting the baseline
 doSubtractbaseline=false;
  // disable the slow stuff
  doFindClusters=false;
  return ok;
}


Long_t Det_LumiGEM::startup_onlyPedestal()
{
   startup();
// startup();
 // disable subtracting cmode, ONLY if you want to check the bin-2-bin pedestal effects alone.//////////////////
 doSubtractcmode=false;
  // disable subtracting existing pedestals
  doSubtractped=false;
 // disable subtracting the baseline
 doSubtractbaseline=false;
  // disable the slow stuff
  doFindClusters=false;
  doSubtractonlyped=true;
  onlybin2binsub=true;
  return ok;
}


Long_t Det_LumiGEM::startup_APVcalib()
{
  startup();
  doAPVcalib=true; // Only to do the APV gain calibration
  use_gainmatch=false; // Use APV gain maching, TRUE only if you want to check your calib constants. Otherwise always FALSE here
 // Use common mode subtraction
 doSubtractcmode=true;
  // Use pedestal subtraction
  doSubtractped=true;
 // disable the baseline subtraction (the polynomial fit method)
 doSubtractbaseline=false;
  // disable the cluster finding
  doFindClusters=false;
  return ok;
}

int Det_LumiGEM::ReadTelescopeData()
{
  int cnt=0;

  if (blobreader->ReadBank(LUMI)==0)
    return 0;
  if (blobreader->GetNAPVs()!=4*GEM_NUM)
    printf(" bank LUMI has %d APVs\n", blobreader->GetNAPVs());

  for (int i=0; i<GEM_NUM; i++)// Six GEMs per telescope
    {
      int napvs = blobreader->GetNAPVs();
      for (int apv=0; apv<napvs; apv++)
	cnt += GEM[i]->AddAPVData(apv, blobreader->GetBankBuffer(apv));
    };

  return cnt;
};


int Det_LumiGEM::CheckRange(int n, int *dat, int low, int high)
// Returns the number of samples in 'dat' outside range [low,high]
//  Return values:
//     0:  everything inside range
//    >0:  number of samples outside range
//    -1:  everything inside range AND all samples are equal to zero! (missing data?)
{
  // counter for number of events outside range; flag to signal if all samples are =0
  int cnt=0, nullflag=1;
  for (int i=0; i<n; i++)
    if ((dat[i]<low)||(dat[i]>high)) cnt++;
    else if (dat[i]!=0) nullflag=0;
  if (cnt==0)
    if (nullflag)
      return -1;
  return cnt;
}

Long_t Det_LumiGEM::common_process()
{
  gemo->hits.clear();
  gemo->eval.clear();
  //  int eventid=eventinfo->eventNumber;
  //  int triggerpattern=eventinfo->trigFired;
   int triggerpattern=0xffff;

  // count the number of lumi triggers:
  //if (triggerpattern&0x02) {lumitriggercounter[0]++;}// cout<<"Lumi left\n\n";}
   // if (triggerpattern&0x04) {lumitriggercounter[1]++;}// cout<<"Lumi right\n\n";}

  if (viscotab)
    {
      LumiGUI->SetTriggerPatternOI();
      if (LumiGUI->CheckTriggerPattern()!=ok)
	{
	  debug(2, " LumiGEM::process(): skip!\n");
	  return maySkip;
	};
    };

  // Check trigger pattern:
  // if (triggerpatternOI<0xffff) // 0xffff is to override _everything_, even broken runs were trigFrired is always zero!
       //   if ((triggerpattern&triggerpatternOI)==0)
  //   return maySkip;

  // if (verbose)
  //  printf(" Event %6d has lumi trigger: trigFired = 0x%04x\n",  eventid, triggerpattern);

  // flags to remember data quality problems for this event:
  zeroFLAG=false;
  outofsyncFLAG=false;

  return ok;
}
  

bool Det_LumiGEM::common_process_inner(int i)
{
  if (verbose>2)
    {
      printf("=======================================================\n");
      printf("===================== G E M  %d ========================\n", i);
      printf("=======================================================\n");
    };
  nx = GEM[i]->PushXdata();
  ny = GEM[i]->PushYdata();
  GEM[i]->GetXdata(xdat);
  GEM[i]->GetYdata(ydat);
    
  //  printf(" nx and ny inner  %d %d %d %d \n",t,i,nx,ny);
  // printf(" nx and ny inner  %d %d %d %d \n",t,i,nx,xdat);

//We don't really need this so commenting it out for now
  //Ethan
  // histogram mean rms's:
  // double rmsxval=TMath::RMS(nx, xdat);
  // double rmsyval=TMath::RMS(ny, ydat);
  // if (rmshisto[i])
  //   {
  //     rmshisto[i]->Fill(rmsxval);
  //     rmshisto[i]->Fill(rmsyval);
  //   };
  // LumiGEMeval eval;
  // eval.GEMid = GEM_NUM+i;
  // eval.rmsx = rmsxval;
  // eval.rmsy = rmsyval;
  // gemo->eval.push_back(eval);
    
  //
  // Check raw data for sync losses:
  //   Sync losses occur when an APV chip "forgets" its configuration
  //   and sends invalid data to the VME module or when the VME module
  //   ADC chip configuration breaks during a run (actually I don't know
  //   if the latter happens, but I'm pretty sure that the APV go crazy
  //   every once in a while.
  //   Such sync losses can be detected by checking the raw data for
  //   invalid values, i.e. beyond the range of the 12 bit ADCs like
  //   negative values or values >4095 counts.
  //   In order to be not too conservative and accept small hickups in
  //   the data stream we can specify a range inside the data is "valid"
  //   and a number of samples that need to be outside that range in
  //   order to raise the "bad_daqsync" flag.

  int outofsynccnt=0;
  int outofsynccntx = CheckRange(nx, xdat, valAPVsyncDataRangeLow, valAPVsyncDataRangeHigh);
  outofsynccnt += outofsynccntx;
  if (outofsynccnt==-1)
    {
      outofsynccnt = 0;
      // DetectorStatus |= LumiGEM::bad | LumiGEM::bad_zerodata;
      debug(0, " ----*****---- ZERO APV FRAME! GEM=%d X\n", i);
      zeroFLAG=true;
    };
  int outofsynccnty = CheckRange(ny, ydat, valAPVsyncDataRangeLow, valAPVsyncDataRangeHigh);
  //  if ( ((t==0)&&(i==1)) ) // left middle GEM has NO Y READOUT! Skip those channels here!
  //  outofsynccnty=0;
  outofsynccnt += outofsynccnty;
  //H2((t*GEM_NUM+i)*2,   outofsynccntx, "APVsOutOfSync", "APVs out of sync", 12, -0.5, 11.5, 252, -1.5, 250.5);
  //H2((t*GEM_NUM+i)*2+1, outofsynccnty, "APVsOutOfSync", "APVs out of sync", 12, -0.5, 11.5, 252, -1.5, 250.5);
  if (outofsynccnt==-1)
    {
      outofsynccnt = 0;
      // DetectorStatus |= LumiGEM::bad | LumiGEM::bad_zerodata;
      debug(0, " ----*****---- ZERO APV FRAME! GEM=%d Y\n", i);
      zeroFLAG=true;
    };
  if (outofsynccnt > valAPVoutOfSyncSampleThreshold)
    {
      //DetectorStatus |= LumiGEM::bad | LumiGEM::bad_daqsync;
      debug(0, " ----******---- OUT OF SYNC: %d samples in GEM=%d\n", outofsynccnt, i);
      outofsyncFLAG=true;
    };
   
   //All GEMs have 122 chans on first APVS
    //Ethan
 /*if ((i==0)||(i==1)) {
    apvx1=122;
    apvy1=122;
  };
 if ((i==2)||(i==3)) {
*/   
    apvx1=122;
    apvy1=122;
  //};
  //  printf("check after %d %d %d  %d\n ",t,i,apvx1,apvy1);  

  //
  // If we should only do all the DetectorFlag stuff we can stop here:
  if (ProcessDetectorFlagOnly)
    return true;
  return false;
}
 

/////////////////////////////////////////////////////

void Det_LumiGEM::callib_apvchannels(int i)
{
  int maxxadc,maxxch,maxyadc,maxych;  

  maxxadc = -10000;
  maxxch = 0;
  maxyadc = -10000;
  maxych = 0;
  for (int k=0; k<250; k++)		      
    {
      if(xdat[k]>maxxadc) //THis gets the maximum ADC value and it's strip # for xdat
	{
	  maxxadc=xdat[k];
	  maxxch =k;
	};
      if(ydat[k]>maxyadc) //THis gets the maximum ADC value and it's strip # for ydat
	{
	  maxyadc=ydat[k];
	  maxych =k;
	};
    };

  for (int k=0; k<250; k++)		      
    {
      //  if ((k==maxxch)&&(xdat[k]>500)&&(xdat[k]<1800)) //THis adds the maximum ADCs on each strip for # of "eve" events  
   if ((k==maxxch)) //THis adds the maximum ADCs on each strip for # of "eve" events 
	{
	  gettotx_adc[i][k] +=xdat[k];
	  gettotx_adc_e[i][k] = gettotx_adc_e[i][k]+1;
	   // printf("2 check after %d %d %d %d \n ",k,gettotx_adc[i][k],xdat[k],gettotx_adc_e[i][k]);
	};
  
      //  if ((k==maxych)&&(ydat[k]>500)&&(ydat[k]<1800)) //THis adds the maximum ADCs on each strip for # of "eve" events
     if ((k==maxych)) //THis adds the maximum ADCs on each strip for # of "eve" events
	{
	  gettoty_adc[i][k] +=ydat[k];
	  gettoty_adc_e[i][k] = gettoty_adc_e[i][k]+1;
	  //if (k==100)	  printf("check after %d %d %d %d\n ",i,k,gettoty_adc[i][k],gettoty_adc_e[i][k]);  
	};
    };
}

cmodeinfo_apv Det_LumiGEM::findCMode_apv(int i)
{
  double maxxadc_apv1,maxxch_apv1,maxxadc_apv2,maxxch_apv2,maxyadc_apv1,maxych_apv1,maxyadc_apv2,maxych_apv2;  
  double gettoty_cmode_apv1,gettoty_cmode_e_apv1,gettoty_cmode_apv2,gettoty_cmode_e_apv2,gettotx_cmode_apv1,gettotx_cmode_e_apv1,gettotx_cmode_apv2,gettotx_cmode_e_apv2;

  maxxadc_apv1 = 0;
  maxxch_apv1 = 1000;
  maxyadc_apv1 = 0;
  maxych_apv1 = 1000;
  maxxadc_apv2 = 0;
  maxxch_apv2 = 1000;
  maxyadc_apv2 = 0;
  maxych_apv2 = 1000;

 for (int k=0; k<apvx1; k++)		      
    {
      if(xdat[k]>maxxadc_apv1) //THis gets the maximum ADC value and it's strip # for xdat
	{
	  maxxadc_apv1 =xdat[k];
	  maxxch_apv1 =k;
	};
    };
 for (int k=apvx1; k<250; k++)		      
   {
     if(xdat[k]>maxxadc_apv2) //THis gets the maximum ADC value and it's strip # for xdat
       {
	 maxxadc_apv2 =xdat[k];
	 maxxch_apv2 =k;
       };
   };

 for (int k=0; k<apvy1; k++)		      
   {
     if(ydat[k]>maxyadc_apv1) //THis gets the maximum ADC value and it's strip # for xdat
       {
	 maxyadc_apv1 =ydat[k];
	 maxych_apv1 =k;
       };
   };
for (int k=apvy1; k<250; k++)		      
   {
     if(ydat[k]>maxyadc_apv2) //THis gets the maximum ADC value and it's strip # for xdat
       {
	 maxyadc_apv2 =ydat[k];
	 maxych_apv2 =k;
       };
   };

  gettotx_cmode_e_apv1=0;
  gettoty_cmode_e_apv1=0;
  gettotx_cmode_apv1=0;
  gettoty_cmode_apv1=0;

  gettotx_cmode_e_apv2=0;
  gettoty_cmode_e_apv2=0;
  gettotx_cmode_apv2=0;
  gettoty_cmode_apv2=0;

/// for the first APV on both X and Y ////////////////////

  for (int k=0; k<apvx1; k++)		      
    {
      if ((k<=maxxch_apv1-csize)||(k>=maxxch_apv1+csize)) //THis adds ADCs for each strip without the maximum ADC strip # +/- 2 on each event
	{
	  gettotx_cmode_apv1 +=xdat[k];
	  gettotx_cmode_e_apv1 = gettotx_cmode_e_apv1+1;
	};
    };
 for (int k=apvx1; k<250; k++)		      
    {
      if ((k<=maxxch_apv2-csize)||(k>=maxxch_apv2+csize)) //THis adds ADCs for each strip without the maximum ADC strip # +/- 2 on each event
	{
	  gettotx_cmode_apv2 +=xdat[k];
	  gettotx_cmode_e_apv2 = gettotx_cmode_e_apv2+1;
	};
    };

 for (int k=0; k<apvy1; k++)		      
   {
      if ((k<=maxych_apv1-csize)||(k>=maxych_apv1+csize)) //THis adds ADCs for each strip without the maximum ADC strip # +/- 2 on each event
	{
	  gettoty_cmode_apv1 +=ydat[k];
	  gettoty_cmode_e_apv1 = gettoty_cmode_e_apv1+1;
	};
    };

 for (int k=apvy1; k<250; k++)		      
   {
     if ((k<=maxych_apv2-csize)||(k>=maxych_apv2+csize)) //THis adds ADCs for each strip without the maximum ADC strip # +/- 2 on each event
       {
	 gettoty_cmode_apv2 +=ydat[k];
	 gettoty_cmode_e_apv2 = gettoty_cmode_e_apv2+1;
       };
   };
 
  cmodeinfo_apv cm_apv;
  //This is dividing integers...
  //Isn't that bad?
  //Changing above to doubles
  //Ethan
  cm_apv.x_event_apv1=(int)gettotx_cmode_apv1/gettotx_cmode_e_apv1; // gives the average non-cluster ADC on each event on first X apv
  cm_apv.y_event_apv1=(int)gettoty_cmode_apv1/gettoty_cmode_e_apv1; // gives the average non-cluster ADC on each event on first Y apv
  cm_apv.x_event_apv2=(int)gettotx_cmode_apv2/gettotx_cmode_e_apv2; // gives the average non-cluster ADC on each event on second X apv
  cm_apv.y_event_apv2=(int)gettoty_cmode_apv2/gettoty_cmode_e_apv2; // gives the average non-cluster ADC on each event on second Y apv
  
  cm_apv.x_apv1=(int)(gettotx_cmode_apv1/gettotx_cmode_e_apv1)-cmode_mean_x_apv1[i]; //cm for first X apv
  cm_apv.y_apv1=(int)(gettoty_cmode_apv1/gettoty_cmode_e_apv1)-cmode_mean_y_apv1[i]; //cm for first Y apv
  cm_apv.x_apv2=(int)(gettotx_cmode_apv2/gettotx_cmode_e_apv2)-cmode_mean_x_apv2[i]; //cm for second X apv
  cm_apv.y_apv2=(int)(gettoty_cmode_apv2/gettoty_cmode_e_apv2)-cmode_mean_y_apv2[i]; //cm for second Y apv

  cm_apv.maxx_apv1=maxxadc_apv1; //maximum X ADC on first APV
  cm_apv.maxy_apv1=maxyadc_apv1; //maximum Y ADC on first APV
  cm_apv.maxchx_apv1=maxxch_apv1; //THe channel which gives maximum X ADC on first APV
  cm_apv.maxchy_apv1=maxych_apv1;//THe channel which gives maximum X ADC on first APV

  cm_apv.maxx_apv2=maxxadc_apv2;
  cm_apv.maxy_apv2=maxyadc_apv2;
  cm_apv.maxchx_apv2=maxxch_apv2;
  cm_apv.maxchy_apv2=maxych_apv2;
 
  //  printf("check befre %d %d %5.2lf %5.2lf\n ",t,i,cmode_mean_x[t][i],cmode_mean_y[t][i]);
  //  printf("check after %d %d  %d %d\n ",cm.x_event,cm.y_event,cm.x,cm.y);  

  return cm_apv;
}

//////////////////////////////////////////////////////////

void Det_LumiGEM::subtractCMode(int i,cmodeinfo_apv &info)
{
  for (int k=0; k<250; k++)		      
    {
      if ((i==0)||(i==1)) {
	if (k<122) {
	  xdat[k]-=info.x_apv1;
	  ydat[k]-=info.y_apv1;
	};
	if (k>121) {
	  xdat[k]-=info.x_apv2;
	  ydat[k]-=info.y_apv2;
	};
      };
        if ((i==2)||(i==3)) {
      	if (k<122) xdat[k]-=info.x_apv1;
	if (k>121) xdat[k]-=info.x_apv2;
	//Changing to reflect 6 APV channels not connected in all GEMs
	//Was k<128, k>127
	//ETHAN
	if (k<122) ydat[k]-=info.y_apv1;
	if (k>121) ydat[k]-=info.y_apv2;
      };
	//Added by Ethan and Tanvi
	//Probably should subtract CMode right?
	if((i==4)||(i==5))
	{
      	  if (k<122) xdat[k]-=info.x_apv1;
	  if (k>121) xdat[k]-=info.x_apv2;
	
	  if (k<122) ydat[k]-=info.y_apv1;
	  if (k>121) ydat[k]-=info.y_apv2;
	}
    };
}
void Det_LumiGEM::subtractBin2Bin(int i)
{
  for (int k=0;k<250;k++)
    {

      xdat[k]-=avg_ped_x[i][k];
      ydat[k]-=avg_ped_y[i][k];
      //  xdat[k]-=perchannelped[k*2+2*250*i+2*250*3*t];
      //  ydat[k]-=perchannelped[1+k*2+2*250*i+2*250*3*t];
      //      debug(0,"%i %i %i %i %i\n",k,i,t,1+k*2+2*250*i+2*250*3*t,perchannelped[1+k*2+2*250*i+2*250*3*t]);
      // printf ("channels per ped %i %i %i %i %i\n",t,i,k,k*2+2*250*i+2*250*3*t,perchannelped[k*2+2*250*i+2*250*3*t]);
    }
}

void Det_LumiGEM::subtractonlyBin2Bin(int i)
{
  for (int k=0;k<250;k++)
    {
      //printf("read before average (only)ped at startup %d %d %d %d %d\n ",t,i,k,xdat[k],ydat[k]);
      xdat[k]-=avg_pedonly_x[i][k];
      ydat[k]-=avg_pedonly_y[i][k];
    
      //printf("read after average (only)ped at startup %d %d %d %d %d\n ",t,i,k,xdat[k],ydat[k]);
      //      printf("read average (only)ped at startup %d %d %d %d %d\n ",t,i,k,avg_pedonly_x[i][k],avg_pedonly_y[i][k]);
// printf ("channels per ped %i %i %i %i %i\n",t,i,k,k*2+2*250*i+2*250*3*t,perchannelped[k*2+2*250*i+2*250*3*t]);
    }
}

void Det_LumiGEM::dobaselinesub(int i)
{
  if (verbose>1 && nx!=250 &&ny!=250)
	    printf(" %d x samples, %d y samples\n", nx, ny);
	  
	    hitfinder->SetPreProcessMethod(cHitFinder::ppRemoveLinearBaseline);
	   // //hitfinder->SetPreProcessMethod(cHitFinder::ppDoNothing);	    
	   hitfinder->RemoveLinearBaseline(GEM[i]->GetNXsamplesList(), xdat);
	   hitfinder->RemoveLinearBaseline(GEM[i]->GetNYsamplesList(), ydat);
}

Long_t Det_LumiGEM::Channel_2_channel_gain_calib(int id, int gem, int strip, int dir, double calibc)
{
  CalibConsts[500*gem+2*strip+dir]=calibc;
  //printf(" Calibration Constants %d %d %d %d %8.5lf\n",tele,gem,strip,dir,CalibConsts[500*gem+2*strip+dir]);
  return ok;
}

//////////////////////////////////////////////////////////////



//////////////////////////////
void Det_LumiGEM::findClusters(int i)
{
  double offsetx[4]={-0.75,0.25,0.0,0.0}; // Setup GEM offsets in x CHANNEL NUMBER so divide mm offset by 0.4 to get channel! 
  double offsety[4]={0.797,-1.179,0.149,0.35}; // Setup GEM offsets in y CHANNEL NUMBER so divide mm offset by 0.4 to get channel!
// used y offset = 0.58 before and the mean of y (horrizontal) residual was 0.1508. Changed the offset untill MS GEM Y residual closer to 0
  // double MSoffset[2]={0.0,0.0}; // Setup GEM offsets {x,y} 0.23 mm ....0.23/0.4=0.58
  // find clusters:
  //printf("go through findclusters \n");   

  if (nx!=250 || ny!=250)
    printf(" %d x samples, %d y samples\n", nx, ny);
	  


  // copy data to use  later:
  //memcpy(xdatraw, xdat, nx*sizeof(xdat[0]));
  //memcpy(ydatraw, ydat, ny*sizeof(ydat[0]));
    //ETHAN
    //Instead lets use a loop, memcpy makes me nervous
//Nevermind, xdatraw and ydatraw are NEVER USED IN THIS CODE
//They are used in TAB_LUMIGEM!
  //   for(int i=0;i<250;i++)
  //   {
  //   	xdatraw[i] = xdat[i];
  //   	ydatraw[i] = ydat[i];
  //   }  
  // /////////////////////////////////////////////////////////////////////////////////////
  
  ////////////////////////Start the new hitfinder using newhitfinder.cpp//////////////
  //printf("Startig new hit finder \n");    
  std::vector<double> bxdat, bydat;
  for (int k=0;k<250;k++) bxdat.push_back(xdat[k]);
  for (int k=0;k<250;k++) bydat.push_back(ydat[k]);

     //for (int k=0;k<250;k++) {
       //  printf("x/y data GEM %d Strip %d X data & thre %5.2lf %5.2lf Y data %5.2lf %5.2lf\n",i,k,bxdat[k],cmcthx[i],bydat[k],cmcthy[i]);
       //printf("x/y data GEM %5.2lf %5.2lf\n",cmcthx[i],cmcthy[i]);
     //};
    
  
//xmax and ymax are vectors of the channels of the local maxima ADC values
  std::vector<unsigned int> xmax = localMax(bxdat, cmcthx[i]); 
  std::vector<unsigned int> ymax = localMax(bydat, cmcthy[i]);

  for(int k = 0; k < xmax.size(); k++)
  {
  	int xdiff = bxdat[xmax[k]] - cmcthx[i][xmax[k]];
  	h2thresholdx[i]->Fill(xmax[k],xdiff);
  	h1thresholdx[i]->Fill(xdiff);
  }
  for(int k = 0; k < ymax.size(); k++)
  {
  	int ydiff = bydat[ymax[k]] - cmcthy[i][ymax[k]];
  	h2thresholdy[i]->Fill(ymax[k],ydiff);
  	h1thresholdy[i]->Fill(ydiff);
  }

  // //This tells you the characteristic size, in number of channels, of an event
  int xover = 0;
  int yover = 0;
  for(int k = 0; k <250; k++)
  {
  	if(bxdat[k]>cmcthx[i][k])
  		xover++;
  	if(bydat[k]>cmcthy[i][k])
  		yover++;
  }
  H1(xover,TString::Format("MUSEtele/Channels over threshold X GEM %i",i),"Channels over Threshold X",20,-0.5,19.5);
  H1(yover,TString::Format("MUSEtele/Channels over threshold Y GEM %i",i),"Channels over Threshold Y",20,-0.5,19.5);


  
   for (int k=0;k<250;k++) {
      if (bxdat[k]==-100000) bxdat[k]=0;
      if (bydat[k]==-100000) bydat[k]=0;
    };

  // Create a padded image of the summed ADC data
  std::vector<double> row(260,0);
  std::vector< std::vector<double> > immat(260,row);
  for (unsigned int r=5; r<255; r++)
    for (unsigned int c=5; c<255; c++){
      // int k=c-5;
      // int q=r-5;
      //  if (deadchannels[k*2+2*250*i+2*250*3*t]>0) bxdat[deadchannels[k*2+2*250*i+2*250*3*t]]=0;
      //  if (deadchannels[q*2+2*250*i+2*250*3*t+1]>0) bydat[deadchannels[q*2+2*250*i+2*250*3*t+1]]=0;
      immat[r][c] += (bxdat[c-5]+bydat[r-5]);
    };

  //std::vector<unsigned int> cx, cy;
  //std::vector<double> cp;
  double hitpos[xmax.size()][ymax.size()];
  
  // Loop over the combinations of maxima candidates
  for (unsigned int q=0; q<xmax.size(); q++)
    {
      // x coordinate maximum candidate (shifted for the image padding)
      unsigned int xm = xmax[q]+5;
      for (unsigned int k=0; k<ymax.size(); k++)
	  {
	  // y coordinate maximum candidate (shifted for the image padding)
	  unsigned int ym = ymax[k]+5;
	  //cx and cy are the same size, and if laid out next to each other, provide every possible pair combination
	  //of xmax and ymax values
	  //xmax and ymax are the channels that formed a local maximum
	  //cx.push_back(xmax[q]);//this is the channel of the local maximum
	  //cy.push_back(ymax[k]);

	  //printf("test this %d %d %d %d\n",q,k,xmax[q],ymax[k]);
	  //printf("test this %d %d %d %d\n",i,k,xmax[q],ymax[k]);
	  // Compute the candidate score for this hit
	  double prom = pow(csize,4)*immat[ym][xm];
	  // for (int ps=1; ps<4; ps++){
	  //   prom -= ((immat[ym-ps][xm-ps]+immat[ym+ps][xm-ps]+immat[ym-ps][xm+ps]+immat[ym+ps][xm+ps]));
	  //   //printf("prom %d %5.2lf \n",ps,prom);
	  // };
	  //ETHAN
	  //New scoring routine, not sure what the justification was for the last one
	  //Just subtracting diagonals seemed strange to me.
	  for(int x = -csize; x < csize+1; x++)
	  {
	  	for(int y = -csize; y < csize + 1; y++)
	  	{
	  		if(x==0 || y==0)
	  			continue;
	  		// if( (ymax[k]+y) < 0 || (ymax[k]+y) >249)
	  		// 	continue;
	  		// if( (xmax[q]+x) < 0 || (xmax[q]+x) > 249)
	  		// 	continue;
	  		// prom -= (avg_ped_y[i][ymax[k]+y] + avg_ped_x[i][xmax[q]+x] );
	  		prom -= immat[xm+x][ym+y];
	  	}	
	  }	    
	  //ETHAN
	  //Vector of summed ADC values of all local maxima
	  //cp.push_back(prom); 
	  hitpos[q][k]=prom;
	  }
    } 
       // Special handling for the 1D hit case to prevent rejection of multiple hits
	 //   if (i==1)
	 //     for (unsigned int q=0; q<cy.size(); q++)
	 //       cy[q] = q;
	 
   // printf("cp.size is %d\n",cp.size());

         // Evaluate the different hits
 //  std::vector<bool> valid(cp.size(),true);
 //  std::vector<unsigned int> hitind;

 //  //This returns the vector hitind, which is a vector of the indices of cp ordered from largest value of cp to smallest.
 //  while (*max_element(valid.begin(),valid.end()))
 //    {	 
 //      unsigned int mind = maxind(cp,valid);      

 //      hitind.push_back(mind);
 //      //printf("mind is %d\n",mind);
      
 //      for (unsigned int q=0; q<cp.size(); q++)
	// {
	//    //printf("q, cp.size, mind is %d %d %d \n",q,cp.size(),mind);
	//   //	   printf("q, mind,cp.size, cp(q, mind) %d  %d %d %d %d \n",q,mind,cp.size(),cp[q],cp[mind]);
	//   if (q==mind){  
	//     valid[q] = false;
	//     //printf("q, cp.size, mind is %d %d %d \n",q,cp.size(),mind);
	//   }
	//   //If any other hit is < 75% of of maximum, all of those hits are considered invalid
	//   else if ((cx[q]==cx[mind] || cy[q]==cy[mind]) && cp[q]<(0.75*cp[mind])){
	//     valid[q] = false;
	//     //	    printf("q, cp.size, mind is %d %d %d \n",q,cp.size(),mind);
	//     //printf("q,mind, cx(q,mind), cy(q,mind) cp(q,mind) %d %d %d %d %d %d %d %d \n",q, mind, cx[q], cx[mind], cy[q], cy[mind],cp[q], cp[mind]);	 
	//   }
	// }
 //      //printf("begins hitind.size is %d\n",hitind.size()); 
 //   }

 //  //printf("hitind.size is %d\n",hitind.size()); 

 //  // Reject nutzo noise events
 //       if (hitind.size()>50){
	//  //printf("larger hitind.size is %d\n",hitind.size()); 
	//  hitind.clear();
 //       };





 	//Noah Steinberg and Ethan Cline Hit Finder
       //Isn't this easier to read and just as good as the one below?
      	bool notdone = true;
      int last;
      int numloops =0;
      while(notdone)
      {
      	
      	int xpos=-1;
      	int ypos=-1;
        double temp=-1;
      	int dindx;
      	int uindx;
      	int dindy;
      	int uindy;
		double chargex=0;
		double numx = 0;
		double denx = 0;
		double chargey=0;
		double numy = 0;
		double deny = 0;
		last = temp;
      	for(int q=0; q<xmax.size();q++)
      	{
      		for(int k=0; k<ymax.size();k++)
      		{
      			if(hitpos[q][k]>temp)
      			{
       					//Find the largest maximum
      				temp = hitpos[q][k];
      				xpos = q;
      				ypos = k;
       					// Set that maximum to 0 so we don't use it again in the next loop iteration
      				hitpos[q][k]=-1;

      			}

      		}
      	}
      	//If the current maximum is < 90% of previous maximum then we skip it
      	//and leave the loop
      	if(temp <0.9*last)
      	{
      		notdone=false;
      		break;
      	}
       //Take care of edge channels that get a hit so we don't go off the end of the array
	   if (xmax[xpos]<(csize+1))
	      dindx = 0;
	   else
	      dindx = xmax[xpos]-csize;

	   if (xmax[xpos]>(bxdat.size()-(csize+1)) )
	      uindx = bxdat.size()-1;
	   else
	      uindx = xmax[xpos]+csize;

	  	//Adds up charge on +/- 2 channels of hit position
	  //Finds weighted mean of charge distribution and assigns that to position below
		for(int j=dindx;j<=uindx;j++)
		{
			chargex += bxdat[j];
			numx+= ((double)j)*bxdat[j];
			denx += bxdat[j];
		}

     	
	    if (ymax[ypos]<(csize+1))
	      dindy = 0;
	   else
	      dindy = ymax[ypos]-csize;

	   if (ymax[ypos]>(bydat.size()-(csize+1)) )
	      uindy = bydat.size()-1;
	   else
	      uindy = ymax[ypos]+csize;


		for(int j=dindy;j<=uindy;j++)
		{
			chargey += bydat[j];
			numy+= ((double)j)*bydat[j];
			deny += bydat[j];
		}
       //std::cout << "x position " << numx/denx << " y position " << numy/deny << std::endl;
		//now put your hit in the output tree
		LumiGEMhit ahit;
		ahit.GEMid = i;
		if(i<4)
			ahit.xl = numx/denx+offsetx[i];
		else
			ahit.xl = numx/denx;
		//Takes care of offset found by survey group
		if(i<4)
			ahit.yl = numy/deny+offsety[i];
		else
			ahit.yl = numy/deny;

		if(ahit.xl < 0 || ahit.xl > 249 || ahit.yl < 0 || ahit.yl > 249)//this also should not happen but does, skip them
			continue;

		ahit.ampl = (bxdat[numx/denx]+bydat[numy/deny])/2;
		ahit.sigma = 0;
		ahit.charge = (chargex+chargey)/2;
		ahit.quality = temp;
		if(ahit.ampl<0 || ahit.charge<0)//This shouldn't happen but does, skip these wackos
			continue;
		gemo->hits.push_back(ahit);
		numloops++;
		if(numloops >4)
			notdone = false;
	}













  // Decide which hits will be hits and which will be rejected
 //  unsigned int hpushed = 0;
 //  if (hitind.size())
 //    {
 //      bool done = false;
 //      double lastscore = 0;
 //      std::vector<bool> hmask(cp.size(),false);

 //      // printf("hitind.size is %d\n",hitind.size());

 //      for (unsigned int q=0; q<hitind.size(); q++) hmask[hitind[q]] = true;	 

      
     


 //      //  printf("cluster finder ",hpushed);
 //      	//This loop finds the x coord of the cluster and stores in hitlist
 //      	//Then finds the y coord and stores in hitlist
 //      	//Then the charge sharing funciton takes those pairs and makes them a single coordinate pair
 //    while (!done)
	// {
	//   // Get the remaining hit with the maximum score	  
	//   unsigned int hcind = maxind(cp,hmask);//finds index of maximum ADC value in cp	  
	//   hmask[hcind] = false;//masks the last max value of cp that was used so it doesn't get reused
	//   double cscore = cp[hcind];//score of hit, largest ADC is largest score
	//   hitlist.clear();//clear previous pair of hits
	//   if (hpushed<7 || (cscore>0.9*lastscore))//only store 7 hits, does run through if new score is less than 90% of last score	       	     
	//     {	
	//       LumiGEMhit hit;	
	//       // hit.xl = meanHit(cx[hcind],bxdat,&hit)>0;
	//       //mean finds the mean ADC value between the max value in x and its two neighbors
	//       if (meanHit(cx[hcind],bxdat,&hit)>0) {
	// 	hit.GEMid = i+GEM_NUM;
	// 	//	printf("Here GEMid %d \n",i);
	// 	hit.quality = cp[hcind];
	// 	if ((hit.GEMid==7)||(hit.GEMid==11)) hit.xl = hit.xl+MSoffset[0];
	// 	else hit.xl = hit.xl;		

	// 	hitlist.push_back(hit);
	// 	H1(hit.ampl, Form("GEM%dAmplX", i), Form("GEM%dAmplX", i),
	// 	   250, 0., 2500.);
	// 	H1(hit.sigma, Form("GEM%dSigmaX", i), Form("GEM%dSigmaX", i),
	// 	   250, 0., 3.);
	// 	H2(hit.ampl, hit.sigma, Form("GEM%dSigmaVsAmplX", i),
	// 	   Form("GEM%dSigmaVsAmplX", i),
	// 	   250, 0., 2500., 250, 0., 3.);
	//       };	
	//       //mean finds the mean ADC value between the max value in y and its two neighbors
	//       if (meanHit(cy[hcind],bydat,&hit)>0) {                  
	// 	hit.GEMid = i+GEM_NUM;
	// 	hit.quality = cp[hcind];
	// 	if ((hit.GEMid==7)||(hit.GEMid==11)) hit.yl = hit.xl+MSoffset[1];
	// 	else hit.yl = hit.xl;//This is actually correct. Check function in newhitfinder.cpp
	// 						//Not the best way to do this I think, naming convention can confuse people
	// 						//ETHAN
	// 	hit.ylerr = hit.xlerr;
	// 	hit.xl = -1;     hit.xlerr = 0.;

	// 	hitlist.push_back(hit);
	// 	H1(hit.ampl, Form("GEM%dAmplY", i), Form("GEM%dAmplY", i),
	// 	   250, 0., 2500.);
	// 	H1(hit.sigma, Form("GEM%dSigmaY", i), Form("GEM%dSigmaY", i),
	// 	   250, 0., 3.);
	// 	H2(hit.ampl, hit.sigma, Form("GEM%dSigmaVsAmplY", i),
	// 	   Form("GEM%dSigmaVsAmplY", i),
	// 	   250, 0., 2500., 250, 0., 3.);
	//       }
       	
 //       		// if(hit.xl==-1||hit.yl==-1)
 //       		// 	std::cout << "missing x or y coordinate" << std::endl;

	//       hpushed++;
 //       		//std::cout << "hits found " <<hpushed << std::endl;
	//       lastscore=cscore;
	//       hitfinder->ChargeSharing(hitlist, &(gemo->hits));
	//     }
	//   else
	//     done = true;
	//   if (!(*max_element(hmask.begin(),hmask.end())))
	//     done = true;
	// }
 //    }
 //  //ETHAN
 //   cp.clear();
 //   cx.clear();
 //   cy.clear();
 //   xmax.clear();
 //   ymax.clear();
 //   bxdat.clear();
 //   bydat.clear();
 //   hitlist.clear();
 //   hitind.clear();
          	 
  /*        
 /////////////////////////////////// old hit finder/////////////////////////////////
  /////////////////////////////////////////////////
        
  //  hitfinder->GaussFilter(nx, xdat, valLMGaussFilterSigma, tmpdat); // i did commented this out
      //hitfinder->GaussFilter(ny, ydat, valLMGaussFilterSigma, tmpdat); // i did commented this out
  //printf("Go through OLD hit finder \n");	 
  int xlist[16], ylist[16];
  //printf("x local maxima \n");
  int nlmx = hitfinder->FindLocalMaxima(nx, xdat, xlist, valLMMaximumNLM, valLMMinimumDistance, valLMMinimumAmplitude);
  // printf("y local maxima \n");
  int nlmy = hitfinder->FindLocalMaxima(ny, ydat, ylist, valLMMaximumNLM, valLMMinimumDistance, valLMMinimumAmplitude);
  
  // for (int i=0; i<250;i++) printf ("xdat, ydat %d %d %d \n",i,xdat[i],ydat[i]);	   	   	  
  // go through the list of local maxima and check if they belong to clusters:
  LumiGEMhit  hit;
  hitlist.clear();
  xclustercnt[i]=0;
  for (int j=0; xlist[j]>=0; j++)
    {
      if (hitfinder->CheckClusterCandidate(xlist[j], valCFMinimumAmplitude, xdat, xdatraw, nx, &hit)>0)
	{	 
	  if (verbose>1)
	    printf(" ====> X CLUSTER: @%d <====\n", xlist[j]);
	  // push hit into vector:

	  hit.GEMid = i+t*6;
	  if (valmake1dhits) 
	    gemo->hits.push_back(hit);
	  else
	    hitlist.push_back(hit);
	  H1(hit.ampl, Form("%sGEM%dAmplX", leftright[t], i), Form("%sGEM%dAmplX", leftright[t], i),
	     250, 0., 2500.);
	  H1(hit.sigma, Form("%sGEM%dSigmaX", leftright[t], i), Form("%sGEM%dSigmaX", leftright[t], i),
	     250, 0., 3.);
	  H2(hit.ampl, hit.sigma, Form("%sGEM%dSigmaVsAmplX", leftright[t], i),
	     Form("%sGEM%dSigmaVsAmplX", leftright[t], i),
	     250, 0., 2500., 250, 0., 3.);
	  xclustercnt[i]++;
	};
    };
 
  estlumicounter += xclustercnt[i];
  //  hitlist.clear();
  yclustercnt[i]=0;
  for (int j=0; ylist[j]>=0; j++)
    {
      if (hitfinder->CheckClusterCandidate(ylist[j], valCFMinimumAmplitude, ydat, ydatraw, ny, &hit)>0)
	{
	  if (verbose>1)
	    printf(" ====> Y CLUSTER: @%d <====\n", ylist[j]);
	  // push hit into vector:

	  hit.GEMid = i+t*6;
	  if ((hit.GEMid==1)||(hit.GEMid==5)) hit.yl = hit.xl+MSoffset[1]; /// INtroduced Offset to make the vertical residuals of tracks on MS GEM centered to 0
	  else hit.yl = hit.xl;

	  // hit.yl = hit.xl; 
	  hit.ylerr = hit.xlerr;
	  hit.xl = -1;     hit.xlerr = 0.;
	  if (valmake1dhits) {
	    gemo->hits.push_back(hit); }
	  else {
	    hitlist.push_back(hit);
	    H1(hit.ampl, Form("%sGEM%dAmplY", leftright[t], i), Form("%sGEM%dAmplY", leftright[t], i),
	       250, 0., 2500.);
	    H1(hit.sigma, Form("%sGEM%dSigmaY", leftright[t], i), Form("%sGEM%dSigmaY", leftright[t], i),
	       250, 0., 3.);
	    H2(hit.ampl, hit.sigma, Form("%sGEM%dSigmaVsAmplY", leftright[t], i),
	       Form("%sGEM%dSigmaVsAmplY", leftright[t], i),
	       250, 0., 2500., 250, 0., 3.);
	    yclustercnt[i]++; }
	};
    };
  estlumicounter += yclustercnt[i];

  // Charge sharing (if wanted by user):
  if (valmake1dhits==0)
    {
      // we need to treat the left sector middle GEM separately:
      if ( (t==0)&&(i==1) && false) //this is disabled because they seem to work!
	// 'ChargeFaking' will produce a "fake" cluster in y with sigma=99.9 
	//  in the center of the detector. In track fitting the yerr of this GEM
	//  should be set to something like ±100mm or so to make sure it doesn't
	//  affect tracking at all.
	hitfinder->ChargeFaking(hitlist, &(gemo->hits));
      else
	//printf("go through charge sharing\n");
	hitfinder->ChargeSharing(hitlist, &(gemo->hits));

    };	 
  /////////////////////////////////// old  hit finder/////////////////////////////////
*/

}


Long_t Det_LumiGEM::process()
{

  // printf("LumiGEM: process called\n");
  if (Long_t r=common_process()!=ok) return r;//Just clears gemo

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
      //  printf("LumiGEM: process called next\n");
      if (LUMI==NULL){
      	printf("LUMI is NULL\n");
      	return 0;
      }
      lumibankcounter++;
      if (verbose>1) printf(" processing LUMI:\n");
      for (int i=0; i<GEM_NUM; i++)
	GEM[i]->ClearAPVs();

      //      printf(" ReadTelescopeData %d:\n", t);
      if (ReadTelescopeData()==0) { // is there any lumi data for this event?
		printf("No Data in Telescope\n");
		return 0;
	  }
      for (int i=0; i<GEM_NUM; i++)// Six GEMs per telescope
	{ 
	  if (common_process_inner(i)){//Just checks data is in range and no sync errors
	  	printf("common process loop error \n");
	   return 0;//Potential errors here 
	  }

	  //ETHAN
	  //printf("LumiGEM: process called next\n");
	  int xraw[250],yraw[250];	 
	  for (int k=0; k<250; k++) 		      
	    {
	      xraw[k]=xdat[k];
	      yraw[k]=ydat[k];
	      //printf("strip #3 %d x data %d y data %d \n",k, xdat[k], ydat[k]);
	   /*
	      LMhitmapX[t][i]->Fill(k,xdat[k]);
	      LMhitmapY[t][i]->Fill(k,ydat[k]);
	      h2adcspec_x[t][i]->Fill(k,xdat[k]);
	      h2adcspec_y[t][i]->Fill(k,ydat[k]);
	  */
	    };       
	  //printf("LumiGEM: process called next\n");
	 
	  cmodeinfo_apv cminfo_apv=findCMode_apv(i);
	  if (someSpecHist)
	  {
	    h1adcspec_cmode_x_apv1[i]->Fill(cminfo_apv.x_event_apv1);
	    h1adcspec_cmode_y_apv1[i]->Fill(cminfo_apv.y_event_apv1);	  
	    h1adcspec_cmode_x_apv2[i]->Fill(cminfo_apv.x_event_apv2);
	    h1adcspec_cmode_y_apv2[i]->Fill(cminfo_apv.y_event_apv2);	  
	  };

	  //printf("I am here");	 
	  /////////////////// Total "raw" ADC vs strip #.  (before applying common-mode noise, bin-2-bin pedestals or polynomial fit baseline subtractions.////////////////////////////

	  if (allSpecHist)
	    for (int k=0; k<250; k++)		      
	      {
		h2adcspec_x[i]->Fill(k,xdat[k]);
		h2adcspec_y[i]->Fill(k,ydat[k]);
		//Get the total X/Y ADCs and # of events on each strip excluding the maximum ADC strip # +/- 2 for each event
		if (((k<apvx1)&&((k<cminfo_apv.maxchx_apv1-csize)||(k>cminfo_apv.maxchx_apv1+csize)))||((k>apvx1-1)&&((k<cminfo_apv.maxchx_apv2-csize)||(k>cminfo_apv.maxchx_apv2+csize)))) {		       
		  h2adcspec_nopk_x[i]->Fill(k,xdat[k]);
		  dattotx_nopk_ent[i][k]=dattotx_nopk_ent[i][k]+1;
		};
		  h1adcspec_x[i]->Fill(xdat[k]);
		if (((k<apvy1)&&((k<cminfo_apv.maxchy_apv1-csize)||(k>cminfo_apv.maxchy_apv1+csize)))||((k>apvy1-1)&&((k<cminfo_apv.maxchy_apv2-csize)||(k>cminfo_apv.maxchy_apv2+csize)))) {
		  h2adcspec_nopk_y[i]->Fill(k,ydat[k]);
		  dattoty_nopk_ent[i][k]=dattoty_nopk_ent[i][k]+1;
		};
		   h1adcspec_y[i]->Fill(ydat[k]);

		h2evespec_nopk_x[i]->Fill(k,dattotx_nopk_ent[i][k]);
		h2evespec_nopk_y[i]->Fill(k,dattoty_nopk_ent[i][k]);
	      };

	  ////////////////////////// Only bin-2-bin pedestal subtraction//////////////////////////////////////
	  ////If you need to check the effects only when the bin to bin subtractions, you should turn ON the flag "onlybin2binsub". 
	  ////However, because all these bin-2-bin pedestals are determined only after common mode subtraction, then you have to 
	  ////turn OFF the "doSubtractcmode" flag on Long_t Det_LumiGEM::startup_findPedestal() doSubtractcmode=false; or just commented out the flag.
	  //cmodeinfo_apv cminfo_apv_onlyb2b=findCMode_apv(t,i);
	  //printf("before subtractBin2Bin  \n");

	  if (onlybin2binsub){
 
	    if (someSpecHist)
	      for (int k=0; k<250; k++)		      
		{
		  if (((k<apvx1)&&((k<cminfo_apv.maxchx_apv1-csize)||(k>cminfo_apv.maxchx_apv1+csize)))||((k>apvx1-1)&&((k<cminfo_apv.maxchx_apv2-csize)||(k>cminfo_apv.maxchx_apv2+csize)))) {
		    // h2adcspec_pedsub_nopk_x[t][i]->Fill(k,xdat[k]);
		    gettotonlypedx[i][k] +=xdat[k];
		    gettotonlypedxentries[i][k]=gettotonlypedxentries[i][k]+1;
		  };
		  if (((k<apvy1)&&((k<cminfo_apv.maxchy_apv1-csize)||(k>cminfo_apv.maxchy_apv1+csize)))||((k>apvy1-1)&&((k<cminfo_apv.maxchy_apv2-csize)||(k>cminfo_apv.maxchy_apv2+csize)))) {
		    // h2adcspec_pedsub_nopk_y[t][i]->Fill(k,ydat[k]);
		    gettotonlypedy[i][k] +=ydat[k];
		    gettotonlypedyentries[i][k]=gettotonlypedyentries[i][k]+1;
		  };

		  //	printf("get ped %d %d %d %d %d \n",k,gettotonlypedx[i][k],gettotonlypedxentries[i][k],gettotonlypedy[i][k],gettotonlypedyentries[i][k]);
		};



	    if (doSubtractonlyped) subtractonlyBin2Bin(i);
	    cmodeinfo_apv cminfo_apv_onlyb2b=findCMode_apv(i);
	    if (allSpecHist) {
	      for (int k=0; k<250; k++)		      
	  	{
		  if (((k<apvx1)&&((k<cminfo_apv_onlyb2b.maxchx_apv1-csize)||(k>cminfo_apv_onlyb2b.maxchx_apv1+csize)))||((k>apvx1-1)&&((k<cminfo_apv_onlyb2b.maxchx_apv2-csize)||(k>cminfo_apv_onlyb2b.maxchx_apv2+csize)))) 
		  h2adcspec_pedsub_nopk_x[i]->Fill(k,xdat[k]);

		  if (((k<apvy1)&&((k<cminfo_apv_onlyb2b.maxchy_apv1-csize)||(k>cminfo_apv_onlyb2b.maxchy_apv1+csize)))||((k>apvy1-1)&&((k<cminfo_apv_onlyb2b.maxchy_apv2-csize)||(k>cminfo_apv_onlyb2b.maxchy_apv2+csize)))) 
		   h2adcspec_pedsub_nopk_y[i]->Fill(k,ydat[k]);

		  // printf("get ped %d %d %d %d %d \n",k,cminfo_apv_onlyb2b.maxchx_apv1,cminfo_apv_onlyb2b.maxchx_apv2,cminfo_apv_onlyb2b.maxchy_apv1,cminfo_apv_onlyb2b.maxchy_apv2);
	  	};
	     
	    };
	  };  

	  for (int k=0; k<250; k++)		      
	    {
	      xdat[k]=xraw[k];
	      ydat[k]=yraw[k];
	    };

	  //////////////////////////////////////////////////////////////////////////////////////////
	
	  if (doSubtractcmode) subtractCMode(i,cminfo_apv); //////Subtract common mode
	  cmodeinfo_apv cminfo_apv_cm=findCMode_apv(i);
	  if (someSpecHist)
	    for (int k=0; k<250; k++)		      
	      {
		if (allSpecHist)
		  {
		    h2adcspec_cmode_x[i]->Fill(k,xdat[k]);
		    h2adcspec_cmode_y[i]->Fill(k,ydat[k]);
		  }
		if (((k<apvx1)&&((k<cminfo_apv_cm.maxchx_apv1-csize)||(k>cminfo_apv_cm.maxchx_apv1+csize)))||((k>apvx1-1)&&((k<cminfo_apv_cm.maxchx_apv2-csize)||(k>cminfo_apv_cm.maxchx_apv2+csize)))) {
		  h2adcspec_cmode_nopk_x[i]->Fill(k,xdat[k]);
		  gettotpedx[i][k] +=xdat[k];
		  gettotpedxentries[i][k]=gettotpedxentries[i][k]+1;
		  //if(i==4)
		  //std::cout << i << " " << k << " max chan " << cminfo_apv_cm.maxchx_apv1 << " totpedx per chan " << gettotpedx[i][k]/gettotpedxentries[i][k] << std::endl;
		};
		if (((k<apvy1)&&((k<cminfo_apv_cm.maxchy_apv1-csize)||(k>cminfo_apv_cm.maxchy_apv1+csize)))||((k>apvy1-1)&&((k<cminfo_apv_cm.maxchy_apv2-csize)||(k>cminfo_apv_cm.maxchy_apv2+csize)))) {
		  h2adcspec_cmode_nopk_y[i]->Fill(k,ydat[k]);
		  gettotpedy[i][k] +=ydat[k];
		  gettotpedyentries[i][k]=gettotpedyentries[i][k]+1;
		};
		//	printf("get ped %d %d %d %d %d \n",k,gettotpedx[i][k],gettotpedxentries[i][k],gettotpedy[i][k],gettotpedyentries[i][k]);
	      };	    
	  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	  if (doSubtractped) subtractBin2Bin(i);  //////Subtract bin-to-bin pedestals (after common mode)
	  // if (doSubtractped) printf("get ped \n");
	  cmodeinfo_apv cminfo_apv_b2b=findCMode_apv(i);
	  if (someSpecHist) {
	    for (int k=0; k<250; k++)		      
	      {

		if (allSpecHist)
		  {		  
		    h2adcspec_cm_pedsub_x[i]->Fill(k,xdat[k]);
		    h2adcspec_cm_pedsub_y[i]->Fill(k,ydat[k]);
		  }
		//	if (someSpecHist) {
		if (((k<apvx1)&&((k<cminfo_apv_b2b.maxchx_apv1-csize)||(k>cminfo_apv_b2b.maxchx_apv1+csize)))||((k>apvx1-1)&&((k<cminfo_apv_b2b.maxchx_apv2-csize)||(k>cminfo_apv_b2b.maxchx_apv2+csize))))
		  h2adcspec_cm_pedsub_nopk_x[i]->Fill(k,xdat[k]);
	
		if (((k<apvx1)&&((k>=cminfo_apv_b2b.maxchx_apv1-csize)&&(k<=cminfo_apv_b2b.maxchx_apv1+csize)))||((k>apvx1-1)&&((k>=cminfo_apv_b2b.maxchx_apv2-csize)&&(k<=cminfo_apv_b2b.maxchx_apv2+csize)))&&(xdat[k]>0)) 
		  h2adcspec_cm_pedsub_peak_x[i]->Fill(k,xdat[k]);

		if (((k<apvy1)&&((k<cminfo_apv_b2b.maxchy_apv1-csize)||(k>cminfo_apv_b2b.maxchy_apv1+csize)))||((k>apvy1-1)&&((k<cminfo_apv_b2b.maxchy_apv2-csize)||(k>cminfo_apv_b2b.maxchy_apv2+csize))))
		  h2adcspec_cm_pedsub_nopk_y[i]->Fill(k,ydat[k]);

		if (((k<apvy1)&&((k>=cminfo_apv_b2b.maxchy_apv1-csize)&&(k<=cminfo_apv_b2b.maxchy_apv1+csize)))||((k>apvy1-1)&&((k>=cminfo_apv_b2b.maxchy_apv2-csize)&&(k<=cminfo_apv_b2b.maxchy_apv2+csize)))&&(ydat[k]>0)) 
		  h2adcspec_cm_pedsub_peak_y[i]->Fill(k,ydat[k]);


		//printf("get ped %d %d %d %d %d \n",k,cminfo_apv_b2b.maxchx_apv1,cminfo_apv_b2b.maxchx_apv2,cminfo_apv_b2b.maxchy_apv1,cminfo_apv_b2b.maxchy_apv2);

	      };
	  };
	   

	  ///////////////////// This is only to check the baseline subtraction effets without common-mode or bin-2-bin pedestal subtractions. So, the raw data is using.////////////////////////////////////////////////
	  if (doSubtractbaseline) {	 
	    
	 //    for (int k=0; k<250; k++)		      
	 //      {
		// xdat[k]=xraw[k];
		// ydat[k]=yraw[k];
	 //      };
	    
	 //    dobaselinesub(t,i);
	 //    cmodeinfo cminfo_base=findCMode(t,i);
	 //    if (allSpecHist)
	 //      for (int k=0; k<250; k++)		      
		// {
		//   h2adcspec_base_x[t][i]->Fill(k,xdat[k]);
		//   h2adcspec_base_y[t][i]->Fill(k,ydat[k]);
		  
		//   if ((k<=cminfo_base.maxchx-2)||(k>=cminfo_base.maxchx+2)) 
		//     h2adcspec_base_nopk_x[t][i]->Fill(k,xdat[k]);
		//   if ((k<=cminfo_base.maxchy-2)||(k>=cminfo_base.maxchy+2)) 
		//     h2adcspec_base_nopk_y[t][i]->Fill(k,ydat[k]);
		// };
		debug(0,"This is old dead code using findCMode which no longer exists as a function\n");
		exit(-1);

	  }
	  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	  //totent[1][i]=totent[1][i]+1;
	  //if (allSpecHist) totentries[i]->Fill(totent[1][i]);
	  for (int ky=0; ky<250; ky++)		      
	   {
	      int amply  = ydat[ky];
	      if (amply>300) {
			totampy[i][ky]=totampy[i][ky]+amply;
			totampenty[i][ky]=totampenty[i][ky]+1;
			for (int kx=0; kx<250; kx++)		      
			{
			    int amplx  = xdat[kx];	   
			    if (amplx>300){
			    	//Why is this here? You have it correct and commented out directly above!
			    	//Putting this here keeps adding it over and over for every single x amplitude entry that is greater than 300
			    	//ETHAN
			      //totampy[i][ky]=totampy[i][ky]+amply;
			      //totampenty[i][ky]=totampenty[i][ky]+1;

			      totampx[i][kx]=totampx[i][kx]+amplx;
			      totampentx[i][kx]=totampentx[i][kx]+1;
		     
			      if (allSpecHist) {
					h2amplmapsx[i]->Fill(kx,ky,amplx);
					h2amplmapsy[i]->Fill(kx,ky,amply);   		
					h2hitmaps[i]->Fill(kx,ky);
					H_hitampx[i]->Fill(amplx);
					H_hitampy[i]->Fill(amply);
			      };
			      //printf("process  xdat ydat %d %d %d %d %d %d \n",kx,ky,totampx[kx][ky],amplx,totampy[kx][ky],amply);
			    }; 
		    };
	      };
	   };
	  
 /////////////////////////////////////////Dead Channels Masking /////////////////////////////////////////
	  if (dead_ch_masking) {   
	    for (int k=0; k<250; k++)
	      {
		if (deadchannels[k*2+2*250*i]>0) {
		  //	printf ("X channels per ped %i %i %i %i %i %i\n",t,i,k,k*2+2*250*i+2*250*3*t,deadchannels[k*2+2*250*i+2*250*3*t],xdat[deadchannels[k*2+2*250*i+2*250*3*t]]);   
		  xdat[deadchannels[k*2+2*250*i]]=-100000;
		};
		if (deadchannels[k*2+2*250*i]>0) {
		  //	printf ("Y channels per ped %i %i %i %i %i %i\n",t,i,k,k*2+2*250*i+2*250*3*t,deadchannels[k*2+2*250*i+2*250*3*t],xdat[deadchannels[k*2+2*250*i+2*250*3*t]]);   
		  ydat[deadchannels[k*2+2*250*i]]=-100000;	
		};

		for (int ky=0; ky<250; ky++)
		  {
		    if ((deadchannels[k*2+2*250*i]>0)||(deadchannels[ky*2+2*250*i+1]>0)) {
		      if (someSpecHist) {
			deadchmap[i]->Fill(k,ky);
		      };
		    };
		  };
	      };
	  };
    
 ///////////////////////Do APV Channel-2-channel gain calibration//////////////////////////////////////
	 
	  if (use_gainmatch) { /// Use APV channel-to-channel gain match//////
	  	debug(0,"If not all the GEMs have calibration constants defined in LumiGEM.xml this segfaults\n");
	    for (int k=0; k<250; k++)
	      {    
		//	if ((k==34)||(k==42)||(k==66)) printf ("Calibration Constants before %i %i %i %i %i \n",t,i,k,xdat[k],ydat[k]);


		//	if (deadchannels[k*2+2*250*i+2*250*3*t]>0) xdat[deadchannels[k*2+2*250*i+2*250*3*t]]=-100000;
		//	else {
		  if (xdat[k]>0) xdat[k]=xdat[k]*CalibConsts[500*i+2*k];
		  else  xdat[k]=xdat[k]-xdat[k]*CalibConsts[500*i+2*k];
		  //	};

		  //	if (deadchannels[k*2+2*250*i+2*250*3*t+1]>0) ydat[deadchannels[k*2+2*250*i+2*250*3*t+1]]=-100000; 
		  //	else {
		  if (ydat[k]>0) ydat[k]=ydat[k]*CalibConsts[500*i+2*k+1];
		  else ydat[k]=ydat[k]-ydat[k]*CalibConsts[500*i+2*k+1];
		  //	};
		//  printf ("Calibration Constants after %i %i %i %i %i \n",t,i,k,xdat[k],ydat[k]); 

		//	if ((k==34)||(k==42)||(k==66)) printf ("Calibration Constants after %i %i %i %i %i \n",t,i,k,xdat[k],ydat[k]); 
	      };
	  };
       
/////////////////////////////////// APV channel-to-channel gain calibration ////////////////////////
	   if (doAPVcalib) callib_apvchannels(i); /// Determine the  APV channel-to-channel gain calibration constants//////
//////////////////////////////////////////////////////////////////////////////////////////////////////

	  if (doFindClusters) findClusters(i);


	  //delete[] xraw;
	  //delete[] yraw;

	  //	  printf("done \n");	
	}; // for i (GEMs)
  //printf("done telescope, GEMs \n");
  // check if any GEM had bad raw data:
  if (zeroFLAG)
    APVzeroSamplesCNT++; // count this as a zerosamples event
  if (outofsyncFLAG)
    APVoutOfSyncCNT++; // count this as an outofsync event



  // loop over all hits found in all GEMs to make
  // correlation plots like x vs x and y vs y:

  std::vector <LumiGEMhit> xyhits[GEM_NUM]; // need xy cluster list for each GEM element!
  for (unsigned int j=0; j<gemo->hits.size(); j++)
    {
      int GEMid = gemo->hits[j].GEMid;
      //int g = GEMid%GEM_NUM;
      // create temporary GEMhit to push it to vector of hits:
      LumiGEMhit ahit;
      ahit.xl = gemo->hits[j].xl;
      ahit.yl = gemo->hits[j].yl;
      xyhits[GEMid].push_back(ahit);
      // fill 2d hitmaps:

      //printf("after charge sharing %d %5.2lf %5.2lf %5.2lf %5.2lf\n",g,gemo->hits[j].xl,gemo->hits[j].yl,gemo->hits[j].ampl,gemo->hits[j].charge);
      //printf("old clusters %d %5.2lf %5.2lf\n",gemo->hits[j].GEMid,0.4*gemo->hits[j].xl-50,0.4*gemo->hits[j].yl-50);

      // H_hitmap2d[t][g]->Fill(gemo->hits[j].xl, gemo->hits[j].yl);
      //H_hitmap2d[t][g]->Fill(0.4*gemo->hits[j].xl-50,0.4*gemo->hits[j].yl-50);
      //  hm2d[t][g]->Fill(0.4*gemo->hits[j].xl-50,0.4*gemo->hits[j].yl-50);
      if (allSpecHist) H_amplmap2d[GEMid]->Fill(gemo->hits[j].xl, gemo->hits[j].yl, gemo->hits[j].ampl);
    };

  /*
  // cut on MI GEM:
  bool good=true;
  for (unsigned int i=0; i<xyhits[0][0].size(); i++)
  {
  // double yl=xyhits[0][1][i].yl;
  // if ( (yl<120.)||(yl>130.) )
  // 	{
  // 	  good=false;
  // 	  break;
  // 	};
  double yl=xyhits[0][0][i].yl;
  if ( (yl<125.)||(yl>135.) )
  {
  good=false;
  break;
  };
  };
  */
  // now we have all clusters in xyhits:
  //Fill 2d hit map for each GEM
    for (int i=0; i<GEM_NUM; i++) {
      for (unsigned int ihit=0; ihit<xyhits[i].size(); ihit++) {
	H_hitmap2d[i]->Fill(0.4*xyhits[i][ihit].xl-50.0,
			       0.4*xyhits[i][ihit].yl-50.0);
	//printf("t=%d i=%d hit=%d x=%5.2lf y=%5.2lf\n",t,i,ihit,
	//     0.4*xyhits[t][i][ihit].xl-50.0,0.4*xyhits[t][i][ihit].yl-50.0);
      }
    };
// nested loop over all GEMs to fill correlation plots:
  //    if (good)

    for (int i=0; i<GEM_NUM; i++)
      for (int j=0; j<GEM_NUM; j++)
	{
	  if (i==j) continue;
	  for (unsigned int ihit=0; ihit<xyhits[i].size(); ihit++)
	    for (unsigned int jhit=0; jhit<xyhits[j].size(); jhit++)
	      {

		H_XvsX[i][j]->Fill(xyhits[i][ihit].xl,
				      xyhits[j][jhit].xl);
		H_YvsY[i][j]->Fill(xyhits[i][ihit].yl,
				      xyhits[j][jhit].yl);
	      };
	};

 

  if (viscotab)
    {
      //printf(" %8d: triggerpattern = 0x%x\n", eventid, triggerpattern);
      //      LumiGUI->FillHitMaps();
      LumiGUI->DrawRawData();
      LumiGUI->SimpleTracking();
    };
  //   };// we need the SiPM TDC

  return ok;
}


////////////////////////////////////////////////////////////////////////////////////////////////




Long_t Det_LumiGEM::showTraces()
{
  if (viscotab==NULL)
    return ok;
  return LumiGUI->showTraces();
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
Long_t Det_LumiGEM::test()
{
	printf("made it to test\n");
	return 0;
}
Long_t Det_LumiGEM::finaltest()
{
	printf("made it to finalize test\n");
	return 0;
}
Long_t Det_LumiGEM::done()
{
  /*  if (SCManager)
      {
      beamcharge   += (SC_BeamCurrent->value)*1e-3*(runstoptime-lasteventtime);
      printf("total integrated beam charge = %eC in %.3lfs\n", beamcharge, runstoptime-runstarttime);
      };
  */
  // show info about trigger counts:
  printf("\n LumiGEM: trigger summary follows...\n ===== TRIGGER COUNTS =====\n");
  if (lumitriggercounter)
    printf(" lumi left:  %7d banks in %7d triggers (%3.2lf%%)\n",
	   lumibankcounter, lumitriggercounter, (double)lumibankcounter/(double)lumitriggercounter*100.);
  else
    printf(" lumi left: no triggers!!\n");
  if (lumitriggercounter)
    printf(" lumi right: %7d banks in %7d triggers (%3.2lf%%)\n",
	   lumibankcounter, lumitriggercounter, (double)lumibankcounter/(double)lumitriggercounter*100.);
  else
    printf(" lumi right: no triggers!!\n");

  // check monitored slow control channels:

  //printf("\nMonitored Slow Control Channels:\n");
  int anythingbad=0;
  /*
    for (int t=0; t<2; t++)
    for (unsigned int i=0; i<SCMonitor[t].size(); i++)
    {
    printf(" %s\t%s\n", SCMonitor[t][i].name, goodbad[SCMonitor[t][i].wasbad]);
    anythingbad += SCMonitor[t][i].wasbad;
    };
  */
  int oldstatus=DetectorStatus;
  // Check for HV problems:
  if (anythingbad) // some HV problem?
    DetectorStatus |= LumiGEM::bad | LumiGEM::bad_hv;
  // Check for raw data quality problems:
  if (APVzeroSamplesCNT > valAPVzeroSamplesEventThreshold) // too many bad events?
    DetectorStatus |= LumiGEM::bad | LumiGEM::bad_zerodata;
  if (APVoutOfSyncCNT > valAPVoutOfSyncEventThreshold) // too many bad events?
    DetectorStatus |= LumiGEM::bad | LumiGEM::bad_daqsync;
  // if something was bad mask out "good" (should not be on anyway, but who knows?)
  if (DetectorStatus & LumiGEM::bad)
    DetectorStatus &= (~LumiGEM::good);
  else // otherwise the status is "good"
    DetectorStatus |= LumiGEM::good;
  // write detector status to database:
  setDetectorFlag("LumiGEM", DetectorStatus);
  printf(" DetectorStatus = %d\n", DetectorStatus);
  if (strlen(WriteDetectorFlagFilename)!=0)
    {
      FILE *file=NULL;
      int fopencnt=0;
      while (file==NULL)
	{
	  file = fopen(WriteDetectorFlagFilename, "a");
	  fopencnt++;
	  usleep(10000);
	  if (fopencnt>25) break;
	};
      if (file)
	{
	  fprintf(file, "%d\t%d APVzeroCNT:\t%d APVoutofsyncCNT:\t%d\n",
		  runinfo->runNumber, DetectorStatus, APVzeroSamplesCNT, APVoutOfSyncCNT);
	  fclose(file);
	}
      else
	printf(" ***** CANNOT OPEN TEXT FILE FOR DETECTOR FLAG OUTPUT *****\n");
    };

    //ETHAN
    //This was listed in a segfault
  //out->Draw("LumiGEMhits.hits.sigma>>hCFsigma(300,0.0,3.0)");

    //Unused for now
    //Ethan
 //    for (int i=0;i<GEM_NUM;i++)
 //      {
	// //	int tot_ent = totentries[t][i]->GetEntries();
	// for (int x=0;x<250;x++){
	//   double avgampx=(double)totampx[i][x]/totampentx[i][x];
	//   double avgampy=(double)totampy[i][x]/totampenty[i][x];
		   
	//   //  if (totampx[t][i][x]>0)  printf("process  xdat ydat %d %d %d %d %d %g\n",t,i,x,totampx[t][i][x],totampentx[t][i][x],avgampx);
	//   //	  if (allSpecHist) normamplmapsx[t][i]->Fill(x,avgampx);
	//   //	  if (allSpecHist) normamplmapsy[t][i]->Fill(x,avgampy);
	// };
 //      };

  ////////////////////////////////////////////////////////////////////////////
  cleanup();
  return 0;
}


Long_t Det_LumiGEM::getmean_cmode()
{
  if (doSubtractcmode)
    debug(0,"***************\nWarning: common mode has been subtracted! Please deactivate the 'doSubtractcmode' \n**************\n");

   ofstream outf("event_avg_cmode.dat");
  //  int id=0;

    for (int i=0;i<GEM_NUM;i++)
      {
	//Get the mean of common mode spectra histogram:

	double meancm_x_apv1=h1adcspec_cmode_x_apv1[i]->GetMean();
	double meancm_y_apv1=h1adcspec_cmode_y_apv1[i]->GetMean();

	double meancm_x_apv2=h1adcspec_cmode_x_apv2[i]->GetMean();
	double meancm_y_apv2=h1adcspec_cmode_y_apv2[i]->GetMean();

		printf(" get cmode mean %d %5.2lf %5.2lf %5.2lf %5.2lf\n", i, meancm_x_apv1, meancm_x_apv2, meancm_y_apv1, meancm_y_apv2);
	
		outf <<i<<"\t"<<meancm_x_apv1<<"\t"<<meancm_x_apv2<<"\t"<<meancm_y_apv1<<"\t"<<meancm_y_apv2<<std::endl;    
      }    
   outf.close();

   debug(0,"****************\nCreated avg_cmode.dat in current directory for further use\n**************");

  return ok;
}


///////////
Long_t Det_LumiGEM::dump_background_sub()
{
  if (doSubtractped)
    debug(0,"***************\nWarning: Pedestals have been subtracted: Exported Pedestals are difference only!\n**************\n");

  ofstream myfile;
  myfile.open("bin_2_bin_bkg.dat");

  /*
 for (int t=1;t<2;t++)
    for (int i=0;i<GEM_NUM;i++)
      {
   bckrms_nopk_x[t][i]==dH2(Form("MUSEtele/%s/%s/bckrms_nopk_x",leftright[t],GEMname[i]), Form("%d GEM X background RMS",i),250,-0.5, 249.5,100,-100.5,399.5);
  bckrms_nopk_y[t][i]==dH2(Form("MUSEtele/%s/%s/bckrms_nopk_y",leftright[t],GEMname[i]), Form("%d GEM Y background RMS",i),250,-0.5, 249.5,100,-100.5,399.5);
      }
*/

  // ofstream outf("ped.xml");
  //int id=0;
    for (int i=0;i<GEM_NUM;i++)
      {
	//	bckrms_nopk_x[t][i]==dH2(Form("MUSEtele/%s/%s/bckrms_nopk_x",leftright[t],GEMname[i]), Form("%d GEM X background RMS",i),250,-0.5, 249.5,100,-100.5,399.5);
	//bckrms_nopk_y[t][i]==dH2(Form("MUSEtele/%s/%s/bckrms_nopk_y",leftright[t],GEMname[i]), Form("%d GEM Y background RMS",i),250,-0.5, 249.5,100,-100.5,399.5);

	//	TProfile  *xprof=h2adcspec_cmode_nopk_x[t][i]->ProfileX();
	//	TProfile  *yprof=h2adcspec_cmode_nopk_y[t][i]->ProfileX();
	for (int k=0;k<250;k++)
	  {
	    
	    TH1D * xcor = h2adcspec_cm_pedsub_nopk_x[i]->ProjectionY("xcor",k+1,k+1);
	    TH1D * ycor = h2adcspec_cm_pedsub_nopk_y[i]->ProjectionY("ycor",k+1,k+1);
	  
	  xrms[i][k] = xcor->GetRMS();
	  yrms[i][k] = ycor->GetRMS();

	  xmean[i][k] = xcor->GetMean();
	  ymean[i][k] = ycor->GetMean();

	  //	  bckrms_nopk_x[t][i]->Fill(k,xrms[i][k]);
	  //	  bckrms_nopk_y[t][i]->Fill(k,yrms[i][k]);

	  myfile <<i<<"\t"<<k<<"\t"<<xrms[i][k]<<"\t"<<xmean[i][k]<<"\t" <<yrms[i][k]<<"\t" <<ymean[i][k] << endl;
	  //   printf("process  xdat ydat %d %d %d %d %d %d \n",i,k,getavgpedx[i][k],getavgpedy[i][k]);
	  }    
      }
  myfile.close();
  //      outf.close();

  //  debug(0,"****************\nCreated bin_2_bin_pedestals.txt in current directory. Please paste contents into gem init xml with appropriate time tag\n**************");
  debug(0,"****************\nCreated bin_2_bin_bkg.dat in current directory. \n**************");
  return ok;
}
//////////

Long_t Det_LumiGEM::dumpPedestals()
{
  if (doSubtractped)
    debug(0,"***************\nWarning: Pedestals have been subtracted: Exported Pedestals are difference only!\n**************\n");

  std::ofstream myfile;
  myfile.open("bin_2_bin_pedestals.dat");

  // ofstream outf("ped.xml");
  //int id=0;

    for (int i=0;i<GEM_NUM;i++)
      {
	//	TProfile  *xprof=h2adcspec_cmode_nopk_x[t][i]->ProfileX();
	//	TProfile  *yprof=h2adcspec_cmode_nopk_y[t][i]->ProfileX();
	for (int k=0;k<250;k++)
	  {

	   
	    // get the average pedestals on each bin
	    std::cout << "GEM CHAN "<<i << " "<<k << " tot ped x " << gettotpedx[i][k] << " tot ped  x entries " << gettotpedxentries[i][k] << std::endl; 
	    if (gettotpedxentries[i][k]>0)
	    {
	      getavgpedx[i][k]=gettotpedx[i][k]/gettotpedxentries[i][k];
	      std::cout << " avg ped " << getavgpedx[i][k] <<  " tot ped x " << gettotpedx[i][k] << " tot ped  x entries " << gettotpedxentries[i][k] << std::endl; 
	    }
	    else getavgpedx[i][k]=10000;
	    
	    if ((getavgpedx[i][k]<=1)||(getavgpedx[i][k]>2000)||(getavgpedx[i][k]!=getavgpedx[i][k])) getavgpedx[i][k]=1100;

	    if (gettotpedyentries[i][k]>0) getavgpedy[i][k]=gettotpedy[i][k]/gettotpedyentries[i][k];
	    else getavgpedy[i][k]=10000;
	    
	    if ((getavgpedy[i][k]<=1)||(getavgpedy[i][k]>2000)||(getavgpedy[i][k]!=getavgpedy[i][k])) getavgpedy[i][k]=1100;
	   
	    std::cout << "GEM chan" << i<< " "<< k <<" "<< " " << getavgpedx[i][k]<< " " << getavgpedy[i][k] << std::endl;

	    myfile <<i<<"\t"<<k<<"\t"<<getavgpedx[i][k]<<"\t"<<getavgpedy[i][k]<< endl;

	    std::cout << "GEM chan" << i<< " "<< k <<" "<< " " << getavgpedx[i][k]<< " " << getavgpedy[i][k] << std::endl;
	  }    
      }
  myfile.close();
  //      outf.close();

  //  debug(0,"****************\nCreated bin_2_bin_pedestals.txt in current directory. Please paste contents into gem init xml with appropriate time tag\n**************");
  debug(0,"****************\nCreated bin_2_bin_pedestals.dat in current directory. \n**************");

  return ok;
}

Long_t Det_LumiGEM::dumponlyPedestals()
{
   if (onlybin2binsub)
    debug(0,"***************\nWarning: Only Pedestals have been subtracted: Exported Pedestals are difference only!\n**************\n");

  ofstream onlypedfile;
  onlypedfile.open("only_bin_2_bin_pedestals.dat");

    for (int i=0;i<GEM_NUM;i++)
      {
	for (int k=0;k<250;k++)
	  {  
	    // get the average pedestals on each bin
	    if (gettotonlypedxentries[i][k]>0) getavgonlypedx[i][k]=gettotonlypedx[i][k]/gettotonlypedxentries[i][k];
	    else getavgonlypedx[i][k]=10000;
	    if ((getavgonlypedx[i][k]==1)||(getavgonlypedx[i][k]==0)) getavgonlypedx[i][k]=10000;
	    if (gettotonlypedyentries[i][k]>0) getavgonlypedy[i][k]=gettotonlypedy[i][k]/gettotonlypedyentries[i][k];
	    else getavgonlypedy[i][k]=10000;
	    if ((getavgonlypedy[i][k]==1)||(getavgonlypedy[i][k]==0)) getavgonlypedy[i][k]=10000;

	    onlypedfile <<i<<"\t"<<k<<"\t"<<getavgonlypedx[i][k]<<"\t"<<getavgonlypedy[i][k]<< endl;
	  }    
      }
  onlypedfile.close();
debug(0,"****************\nCreated only_bin_2_bin_pedestals.dat in current directory. \n**************");

  return ok;
}

//////////////////////////////////////////////////////////////////////////////////////////

Long_t Det_LumiGEM::done_APVcalib()
{
   if (doAPVcalib)
     debug(0,"***************\nWarning: If you really don't want to perform APV channel-to-channel gain calibrations, then please deactivate the 'doAPVcalib' \n**************\n");

   ofstream myxcalibfile,myycalibfile;
  myxcalibfile.open("do_APV_Xcalib.dat");
 myycalibfile.open("do_APV_Ycalib.dat");

double avgmaxADC_x[GEM_NUM][250],avgmaxADC_y[GEM_NUM][250];
double xmaxdata[GEM_NUM][250],ymaxdata[GEM_NUM][250];


    for (int i=0;i<GEM_NUM;i++)
      {


 int count_x,count_y;
 count_x=0;
 count_y=0;
 int index_x[250],index_y[250];

 for (int k=0;k<250;k++)
   {
	//Get the mean of maximum ADCs on each APV channel (After common mode and Pedestal subtraction)	   
     if ((gettotx_adc_e[i][k]>0)){
       avgmaxADC_x[i][k]=gettotx_adc[i][k]/gettotx_adc_e[i][k];
       count_x=count_x+1;
       index_x[count_x]=k;
       xmaxdata[i][k]=avgmaxADC_x[i][k];

       //printf(" get avg. max. X ADC %d %d %d %d %d %d %10.2lf\n", t,i, k,count_x, gettotx_adc[i][k],gettotx_adc_e[i][k], avgmaxADC_x[i][k]);
     }
	    //  else avgmaxADC_x[i][k]=-10000;
     else xmaxdata[i][k]=-100000;

     if ((gettoty_adc_e[i][k]>0)) {
       avgmaxADC_y[i][k]=gettoty_adc[i][k]/gettoty_adc_e[i][k];
	      //  if (avgmaxADC_y[i][k]>1000) {
       count_y=count_y+1;
       index_y[count_y]=k;
		  //  ymaxdata[i][k]=avgmaxADC_y[i][k];
       ymaxdata[i][k]=avgmaxADC_y[i][k];
		  //	};
     }
     else ymaxdata[i][k]=-100000;
	    //	printf(" get avg. max. X ADC %d %d %d %d %10.2lf %d %10.2lf  \n", t,i, k,count_x,gettotx_adc[i][k],gettotx_adc_e[i][k],avgmaxADC_x[i][k]);

	    //printf(" get avg. max. X ADC %d %d %d %d %d  %10.2lf %d %d %d %10.2lf %d\n", t,i, k, gettotx_adc[i][k],gettotx_adc_e[i][k],avgmaxADC_x[i][k],count_x[i], gettoty_adc[i][k],gettoty_adc_e[i][k],avgmaxADC_y[i][k],count_y[i]);
	   
   }; 

	double index[count_x];
	double xdata[count_x];
	for (int n=1;n<count_x+1;n++)
	  {
	    index[n] = index_x[n];
	    xdata[n]=avgmaxADC_x[i][index_x[n]];
	    //	    xdata[n]=avgmaxADC_x[i][index_x[n]];	
	    //printf(" get avg. max. X ADC %d %d %d %d %10.2lf %10.2lf %10.2lf\n", t,i,n,index_x[n],index[n],avgmaxADC_x[i][index_x[n]],xdata[n]);
	    myxcalibfile <<"\t"<<i<<"\t"<<n<<"\t"<<index[n]<<"\t"<<xdata[n]<< endl;
	  };

	double yindex[count_y];
	double ydata[count_y];
	for (int ny=1;ny<count_y+1;ny++)
	  {
	    yindex[ny] = index_y[ny];
	    ydata[ny]=avgmaxADC_y[i][index_y[ny]];
	    //	    ydata[ny]=avgmaxADC_y[i][index_y[ny]];	
		//	printf(" get avg. max. X ADC %d %d %d %d %10.2lf %10.2lf %10.2lf\n", t,i,n,index_x[n],index[n],avgmaxADC_x[i][index_x[n]],xdata[n]);
	    myycalibfile <<"\t"<<i<<"\t"<<ny<<"\t"<<yindex[ny]<<"\t"<<ydata[ny]<< endl;
	  };


	//	TGraph *gtmp  = new TGraph(count_x, index, xdata);
      }    
    myxcalibfile.close();
    myycalibfile.close();

gettotx_adc.clear();
gettoty_adc.clear();
gettotx_adc_e.clear();
gettoty_adc_e.clear();
    debug(0,"****************\nCreated do_APV_X(Y)calib.dat in the current directory. Use it for linear fit and do the gain calibrations accordingly.\n**************");

  return ok;
}
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


Long_t Det_LumiGEM::cmdline(char *cmd)
//Long_t Det_LumiGEM::cmdline(cmd)
{
  cout << "LumiGEM: Got commandline: >" << cmd << "<" << endl;
  ///////////////////////////////////////////////////////
  // check for verbosity:
  if (strstr(cmd, "-v"))
    verbose=1;
  if (strstr(cmd, "-vv"))
    verbose=2;
  if (strstr(cmd, "-vvv"))
    verbose=3;
  printf(" verbosity %d\n", verbose);
  ///////////////////////////////////////////////////////
  // check if only DetectorFlag should be processed:
  if (strstr(cmd, "--detectorflagonly"))
    ProcessDetectorFlagOnly = true;
  ///////////////////////////////////////////////////////

  
  
  return 0;
}

void Det_LumiGEM::cleanup()
{
  cout << "LumiGEM: deleting GEM objects..." << endl;
  
  delete[] xdat;
  delete[] ydat;

  //delete[] xdatraw;

  //delete[] ydatraw;

  delete gemo;
  //  delete[] hitfinder;
  //  delete[] LumiGUI;
  //delete[] tmpdat;


    for (int i=0; i<GEM_NUM; i++)// Four GEMs per telescope
      {
	delete GEM[i];
      };

dattotx_nopk_ent.clear();
dattoty_nopk_ent.clear();

gettotpedx.clear();
gettotpedy.clear();
gettotpedxentries.clear();
gettotpedyentries.clear();

gettotonlypedx.clear();
gettotonlypedy.clear();
gettotonlypedxentries.clear();
gettotonlypedyentries.clear();

totent.clear();
//totampx.clear();
//totampy.clear();
//avgmaxADC_x.clear();
//avgmaxADC_y.clear();

}


extern "C"{
  Plugin *factory(TTree *in, TTree *out, TFile *inf_, TFile *outf_, TObject *p)
  {
    return (Plugin *) new Det_LumiGEM(in, out, inf_, outf_, p);
  }
}

ClassImp(Det_LumiGEM);
