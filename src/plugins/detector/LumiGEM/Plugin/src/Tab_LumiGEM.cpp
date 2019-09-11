#include "Tab_LumiGEM.h"
#include "Det_LumiGEM.h"
#include "cTrack.h"

#include "TGTableLayout.h"

#include "TGLabel.h"

#include "TROOT.h"
#include "TRandom.h"
#include "TMarker.h"
#include "TGClient.h"

#include<iostream>
#include<cmath>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

Tab_LumiGEM::Tab_LumiGEM()
{
  cout << "Tab_LumiGEM: Tab constructor called" << endl;

  lumiplugin = NULL;
  myframe    = NULL;

  /*  tof        = NULL;
  tofraw     = NULL;
  */
  HistogramsDefined = 0;
}

Long_t Tab_LumiGEM::InitGUI(Det_LumiGEM *plugin, TGCompositeFrame *tgframe)
{
  cout << "Tab_LumiGEM: InitGUI called" << endl;

  lumiplugin = plugin;

  // try to get the TOF output tree:
  /*  lumiplugin->getOutBranchObject("ToFhits", (TObject**)&tof);
  lumiplugin->getBranchObject("ToF", (TObject**)&tofraw);
  printf(" tofhits     = %p\n", tof);
  printf(" tofraw      = %p\n", tofraw);
  */
  // try to get the LumiTrigger raw tree:
  //  lumiplugin->getBranchObject("LumiTrigger", (TObject**)&lumitrigger);
  //  printf(" lumitrigger = %p\n", lumitrigger);

  canvas = new TCanvas("lumicanvas", "LumiGEM", 800, 600);
  canvas->Divide(4, 2);

    for (int i=0; i<4; i++)// four GEMs per telescope
      {
	rawhistoX[i] = NULL;
	rawhistoY[i] = NULL;
	hitmapX[i]   = NULL;
	hitmapY[i]   = NULL;
	hitmap2d[i]  = NULL;
      };
  
  // colors for GUI elements:
  gClient->GetColorByName("red",   red);
  gClient->GetColorByName("forestgreen", green);

  // Need to create yet another frame inside the one we get from visco!
  // This is needed since visco changed to TGDockableFrames - well, Fons only knows...
  myframe = new TGCompositeFrame(tgframe);

  myframe->SetLayoutManager(new TGTableLayout(myframe, 25, 12, kFALSE, 2));
  tgframe->AddFrame(myframe, new TGLayoutHints(kLHintsExpandX));
  int cx=0, cy=0;
  { // MAIN LAYOUT
    TGGroupFrame *gframe;
    gframe = new TGGroupFrame(myframe, "General Settings", kVerticalFrame);
    {
      CBGraphicsOutput = new TGCheckButton(gframe, "enable graphical output");
      gframe->AddFrame(CBGraphicsOutput);
      //CBGraphicsOutput->SetState(kButtonDown);
      CBFastProcessing = new TGCheckButton(gframe, "fast processing (GUI off)");
      gframe->AddFrame(CBFastProcessing);
      CB1dClusters = new TGCheckButton(gframe, "1d clusters");
      gframe->AddFrame(CB1dClusters);
      if (plugin->valmake1dhits)
	CB1dClusters->SetState(kButtonDown);
      gframe->AddFrame(new TGLabel(gframe, "verbosity level:"));
      NEVerbosityLevel = new TGNumberEntry(gframe, 0, 3, -1, TGNumberFormat::kNESInteger);
      gframe->AddFrame(NEVerbosityLevel);
      TGTextButton *TBEditor = new TGTextButton(gframe, "Editor");
      //TBEditor->SetCommand("((TCanvas*)gROOT->FindObject(\"Histo Canvas\"))->ToggleEditor();");
      //TBEditor->SetCommand("new TGedEditor()->ConnectToCanvas((TCanvas*)gROOT->FindObject(\"Histo Canvas\"));");
      gframe->AddFrame(TBEditor, new TGLayoutHints(kLHintsExpandX));
      TGTextButton *TBBrowser = new TGTextButton(gframe, "Browser");
      TBBrowser->SetCommand("new TBrowser()");
      gframe->AddFrame(TBBrowser);
      gframe->AddFrame(TBBrowser, new TGLayoutHints(kLHintsExpandX));
    }
    myframe->AddFrame(gframe, new TGTableLayoutHints(cx, cx+1, cy, cy+6));
    cx++; cy=0;

    gframe = new TGGroupFrame(myframe, "Data Breakpoint");
    {
      gframe->SetLayoutManager(new TGTableLayout(gframe, 10, 10, kFALSE, 2));

      int cx=0, cy=0;
      CBStopOnGoodEvent = new TGCheckButton(gframe, "stop on good event");
      gframe->AddFrame(CBStopOnGoodEvent, new TGTableLayoutHints(cx, cx+2, cy, cy+1)); cx+=2;
      CBSkipOtherEvents = new TGCheckButton(gframe, "skip other events");
      gframe->AddFrame(CBSkipOtherEvents, new TGTableLayoutHints(cx, cx+1, cy, cy+1));
      cx=0; cy++;

      gframe->AddFrame(new TGLabel(gframe, "triggerpatternOI: "), new TGTableLayoutHints(cx, cx+1, cy, cy+1)); cx++;
      NETriggerPatternOI = new TGNumberEntry(gframe, lumiplugin->triggerpatternOI, 8, -1, TGNumberFormat::kNESHex);
      gframe->AddFrame(NETriggerPatternOI, new TGTableLayoutHints(cx, cx+2, cy, cy+1)); cx+=2;
      //NETriggerPatternOI->Connect("ValueChanged(Long_t)", "Tab_LumiGEM", this, "SetTriggerPatternOI()");
      NETriggerOICounter = new TGNumberEntry(gframe, 0, 5, -1, TGNumberFormat::kNESInteger);
      gframe->AddFrame(NETriggerOICounter, new TGTableLayoutHints(cx, cx+1, cy, cy+1)); cx++;
      NETriggerOICounter->SetEditable(kFALSE);
      cx=0; cy++;

      gframe->AddFrame(new TGLabel(gframe, "trigger under test: "), new TGTableLayoutHints(cx, cx+1, cy, cy+1)); cx++;
      NETriggerUnderTest = new TGNumberEntry(gframe, 4, 8, -1, TGNumberFormat::kNESHex);
      gframe->AddFrame(NETriggerUnderTest, new TGTableLayoutHints(cx, cx+2, cy, cy+1)); cx+=2;
      PBTrigEffBar = new TGHProgressBar(gframe, 150, 20, red, green);
      gframe->AddFrame(PBTrigEffBar, new TGTableLayoutHints(cx, cx+2, cy, cy+1));
      PBTrigEffBar->SetRange(0.0, 1.0);
      PBTrigEffBar->SetPosition(0.0);
      PBTrigEffBar->Reset();
      PBTrigEffBar->ShowPosition(true, true);
      cx=0; cy++;

      CBRearTOFs = new TGCheckButton(gframe, "rear TOFs    tof>");
      gframe->AddFrame(CBRearTOFs, new TGTableLayoutHints(cx, cx+1, cy, cy+1)); cx++;
      //CBRearTOFs->SetState(kButtonDown);
      NEReadTOFtof = new TGNumberEntry(gframe, 10, 5, -1, TGNumberFormat::kNESRealOne);
      gframe->AddFrame(NEReadTOFtof, new TGTableLayoutHints(cx, cx+1, cy, cy+1)); cx++;
      gframe->AddFrame(new TGLabel(gframe, " ns    Edep>"), new TGTableLayoutHints(cx, cx+1, cy, cy+1)); cx++;
      NEReadTOFedep = new TGNumberEntry(gframe, 1500, 5, -1, TGNumberFormat::kNESInteger);
      gframe->AddFrame(NEReadTOFedep, new TGTableLayoutHints(cx, cx+1, cy, cy+1)); cx++;
      cx=0; cy++;      

      gframe->AddFrame(new TGLabel(gframe, "GEMidOI: "), new TGTableLayoutHints(cx, cx+1, cy, cy+1)); cx++;
      NEGEMidOI = new TGNumberEntry(gframe, 0, 8, -1, TGNumberFormat::kNESInteger);
      gframe->AddFrame(NEGEMidOI, new TGTableLayoutHints(cx, cx+2, cy, cy+1));
      cx=0; cy++;

      gframe->AddFrame(new TGLabel(gframe, "TOF multi: "), new TGTableLayoutHints(cx, cx+1, cy, cy+1)); cx++;
      NETOFmulti = new TGNumberEntry(gframe, 0, 8, -1, TGNumberFormat::kNESInteger);
      gframe->AddFrame(NETOFmulti, new TGTableLayoutHints(cx, cx+2, cy, cy+1));
      cx=0; cy++;

      CBGEMtrackcandidate = new TGCheckButton(gframe, "GEM track candidate");
      gframe->AddFrame(CBGEMtrackcandidate, new TGTableLayoutHints(cx, cx+3, cy, cy+1));
      cx=0; cy++;
    }
    myframe->AddFrame(gframe, new TGTableLayoutHints(cx, cx+2, cy, cy+3));
    cx+=2; cy=0;

    gframe = new TGGroupFrame(myframe, "vs. Plot");
    {
      gframe->SetLayoutManager(new TGTableLayout(gframe, 10, 10, kFALSE, 2));

      int cx=0, cy=0;
      CBvsPlotX = new TGComboBox(gframe);
      CBvsPlotY = new TGComboBox(gframe);
      int cnt=0;
      TObjArray *inarray = lumiplugin->in->GetListOfBranches();
      for (int i=0; inarray->At(i); i++)
        {
          printf(" in[%d]:  %s\n", i, inarray->At(i)->GetName());
	  CBvsPlotX->AddEntry(inarray->At(i)->GetName(), cnt);
	  CBvsPlotY->AddEntry(inarray->At(i)->GetName(), cnt);
	  cnt++;
        };
      TObjArray *outarray = lumiplugin->out->GetListOfBranches();
      for (int i=0; outarray->At(i); i++)
        {
          printf(" out[%d]: %s\n", i, outarray->At(i)->GetName());
	  CBvsPlotX->AddEntry(outarray->At(i)->GetName(), cnt);
	  CBvsPlotY->AddEntry(outarray->At(i)->GetName(), cnt);
	  cnt++;
        };
      gframe->AddFrame(CBvsPlotX, new TGTableLayoutHints(cx, cx+1, cy, cy+1)); cy++;
      gframe->AddFrame(CBvsPlotY, new TGTableLayoutHints(cx, cx+1, cy, cy+1));
      CBvsPlotX->Resize(150, 20);
      CBvsPlotY->Resize(150, 20);
      CBvsPlotX->GetListBox()->Resize(150, 200);
      CBvsPlotY->GetListBox()->Resize(150, 200);

      TClass *someclass = new TClass("LumiGEMhit");
      someclass->GetListOfDataMembers()->Print();
    }
    myframe->AddFrame(gframe, new TGTableLayoutHints(cx, cx+3, cy, cy+3));
    cx=0; cy+=3;

    gframe = new TGGroupFrame(myframe, "Trigger Monitor");
    {
      int cx=0, cy=0;
      char CTLBname[16][24] =
	{
	  "main",
	  "lumiL (L_SiPM & R_TOF)", "lumiR (R_SiPM & L_TOF)",
	  "L_LGC & R_backTOF", "R_LGC & L_backTOF", "L_SiPM", "R_SiPM",
	  "L_TOF", "R_TOF", "L_3fold", "R_3fold",
	  "", "", "", "", ""
	};
      gframe->SetLayoutManager(new TGTableLayout(gframe, 3, 20, kFALSE, 2));
      char label[8]; label[0]='\0';
      gframe->AddFrame(new TGLabel(gframe, "     free CTLB0 "), new TGTableLayoutHints(cx, cx+1, cy, cy+1));
      gframe->AddFrame(new TGLabel(gframe, "prescld CTLB0 "), new TGTableLayoutHints(cx, cx+1, cy+1, cy+2));
      cx++;
      for (int i=0; i<16; i++)
	{
	  if (i==15) sprintf(label, "CTLB16");
	  CBfreerunCTLB[i]   = new TGCheckButton(gframe, label); gframe->AddFrame(CBfreerunCTLB[i], new TGTableLayoutHints(cx, cx+1, cy, cy+1));
	  CBfreerunCTLB[i]->SetToolTipText(CTLBname[i]);
	  CBprescaledCTLB[i] = new TGCheckButton(gframe, label); gframe->AddFrame(CBprescaledCTLB[i], new TGTableLayoutHints(cx, cx+1, cy+1, cy+2));
	  CBprescaledCTLB[i]->SetToolTipText(Form("%s prescaled", CTLBname[i]));
	  if ((i>0)&&(i<7))
	    {
	      Pixel_t col = green;
	      if (i%2) col=red;
	      CBfreerunCTLB[i]->SetBackgroundColor(col);
	      CBprescaledCTLB[i]->SetBackgroundColor(col);
	    };
	  cx++;
	};
      TEFreePattHex  = new TGTextEntry(gframe, "0x0000"); gframe->AddFrame(TEFreePattHex, new TGTableLayoutHints(cx, cx+1, cy, cy+1));
      TEPrescPattHex = new TGTextEntry(gframe, "0x0000"); gframe->AddFrame(TEPrescPattHex, new TGTableLayoutHints(cx, cx+1, cy+1, cy+2));
    }
    myframe->AddFrame(gframe, new TGTableLayoutHints(cx, cx+5, cy, cy+5));

    gframe = new TGGroupFrame(myframe, "TOF monitor");
    {
      gframe->SetLayoutManager(new TGTableLayout(gframe, 20, 3, kFALSE, 2));
      int cx=0, cy=0;
      for (int i=0; i<18; i++)
	{
	  CBTOFmon[i]    = new TGCheckButton(gframe, Form("%d", i+1));
	  CBTOFmon[i+18] = new TGCheckButton(gframe, "");
	  gframe->AddFrame(CBTOFmon[i], new TGTableLayoutHints(cx, cx+1, cy, cy+1));
	  gframe->AddFrame(CBTOFmon[i+18], new TGTableLayoutHints(cx+1, cx+2, cy, cy+1));
	  cy++;
	};
    }
    myframe->AddFrame(gframe, new TGTableLayoutHints(10, 12, 0, 20));

    myframe->Resize(500, 200);

  } // MAIN LAYOUT

  fillhistosTab = false;
  protoncandidate = false;

  verbose=0;

  // temporarily: open logfile for "good" tracks and trigger efficiencies:
  triggerfile = fopen("track_trigger.log", "w");

  return 0;
}

Tab_LumiGEM::~Tab_LumiGEM()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

Long_t Tab_LumiGEM::defineHistograms()
{
  if (HistogramsDefined) return 0;
  HistogramsDefined = 1;
  printf(" Tab_LumiGEM::defineHistograms() called\n");
  PrepareHitMaps();

  int nbins_T1T2hitmap = 240;
  T1hitmap = lumiplugin->dH2("LumiGEM/T1hitmap", "T1 hitmap", nbins_T1T2hitmap, -60., 180., nbins_T1T2hitmap, -60., 180.);
  T2hitmap = lumiplugin->dH2("LumiGEM/T2hitmap", "T2 hitmap", nbins_T1T2hitmap, -60., 180., nbins_T1T2hitmap, -60., 180.);
  T1hitmapTUT = lumiplugin->dH2("LumiGEM/T1hitmapTUT", "T1 hitmap trigger under test", nbins_T1T2hitmap, -60., 180., nbins_T1T2hitmap, -60., 180.);
  T2hitmapTUT = lumiplugin->dH2("LumiGEM/T2hitmapTUT", "T2 hitmap trigger under test", nbins_T1T2hitmap, -60., 180., nbins_T1T2hitmap, -60., 180.);
  T1TUTeffmap = lumiplugin->dH2("LumiGEM/T1TUTeffmap", "T1 trigger under test efficiency map", nbins_T1T2hitmap, -60., 180., nbins_T1T2hitmap, -60., 180.);
  T2TUTeffmap = lumiplugin->dH2("LumiGEM/T2TUTeffmap", "T2 trigger under test efficiency map", nbins_T1T2hitmap, -60., 180., nbins_T1T2hitmap, -60., 180.);
  T1T2acceptcnt = 0;
  TUTtriggercnt = 0;

  MIGEMresX = lumiplugin->dH1("LumiGEM/MiGEMsagittaX", "MiGEMsagittaX", 128, -15., +15.);
  MIGEMresY = lumiplugin->dH1("LumiGEM/MiGEMsagittaY", "MiGEMsagittaY", 128, -15., +15.);
  return 0;
}

void Tab_LumiGEM::SetTriggerPatternOI()
{
  //printf("changed TPOI to 0x%02x\n", (unsigned int)NETriggerPatternOI->GetNumber());
  lumiplugin->SetTriggerPattern(0, (int)NETriggerPatternOI->GetNumber());
}

void Tab_LumiGEM::Set1dClusterOnOff()
{
  if (CB1dClusters->GetState()==kButtonDown)
    lumiplugin->valmake1dhits = 1;
  else
    lumiplugin->valmake1dhits = 0;
}

Long_t Tab_LumiGEM::ProcessSClumi()
{
  /*  if (lumiplugin->SClumi)
    printf(" dtcBeamcharge = %e  dtcLuminosity = %e\n", lumiplugin->SClumi->dtcBeamcharge, lumiplugin->SClumi->dtcLuminosity);
  */
  return 0;
}

Long_t Tab_LumiGEM::ProcessTOFs()
{
  return 0;
}

Long_t Tab_LumiGEM::ProcessSiPMs()
{
  if (fillhistosTab==false) return 0;
  if (protoncandidate==false) return 0;
  fillhistosTab=false;
  return 0;
}


Long_t Tab_LumiGEM::CheckTriggerPattern()
{
  int triggerpattern=0;
  //if (lumiplugin->eventinfo)
  //  triggerpattern=lumiplugin->eventinfo->trigFired;
  // else
    triggerpattern=0xffff;
  if (lumiplugin->triggerpatternOI<0xffff) // 0xffff is to override _everything_, even broken runs were trigFrired is always zero!
    if ((triggerpattern&lumiplugin->triggerpatternOI)==0)
      return Plugin::maySkip;
  return Plugin::ok;
}


Long_t Tab_LumiGEM::DrawRawData()
{
  verbose = (int)NEVerbosityLevel->GetNumber();
  lumiplugin->verbose = verbose;
  // check triggerpattern:
  SetTriggerPatternOI();
  Set1dClusterOnOff();

  // int triggerpattern=0;
  // if (lumiplugin->eventinfo)
  //  triggerpattern=lumiplugin->eventinfo->trigFired;
  // else
    int triggerpattern=0xffff;
  int freepatt  = (triggerpattern&0xffff0000)>>16;
  int prescpatt = triggerpattern&0x0000ffff;
  if (prescpatt==0)
    printf(" *** BAD TRIGGER PATTERN ***\n");
  // if (lumiplugin->triggerpatternOI<0xffff) // 0xffff is to override _everything_, even broken runs were trigFrired is always zero!
  /*
    if ((triggerpattern&lumiplugin->triggerpatternOI)==0)
      {
	if (CBSkipOtherEvents->GetState()==kButtonDown)
	  return Plugin::skip;
	else
	  return Plugin::maySkip;
      };
  */
  NETriggerOICounter->SetNumber(NETriggerOICounter->GetNumber()+1);
  // trigger monitor:
  TEFreePattHex->SetText(Form("0x%04x", freepatt));
  TEPrescPattHex->SetText(Form("0x%04x", prescpatt));
  if (CBFastProcessing->GetState()==kButtonUp)
    for (int i=0; i<16; i++)
      {
	CBfreerunCTLB[i]->SetState( (EButtonState)((freepatt>>i)&1) );
	CBprescaledCTLB[i]->SetState( (EButtonState)((prescpatt>>i)&1) );
	//printf("%d", (freepatt>>i)&1);
      };
  //printf("\n");
  // check rear TOFs:
  protoncandidate=false; // note that this flag will be set in a different place only!
  // graphics output:
  if (CBGraphicsOutput->GetState()==kTRUE)
    {
      canvas->Clear();
      canvas->Divide(2, 4);
    };
  // prepare to count clusters:
  int GEMidOI = (int)NEGEMidOI->GetNumber();
  int GEMOIclusters=0;
  double clusterminampl=200;

  //////////////////////////////////////////////////////////////////////////
  int maxxadc_apv1,maxxch_apv1,maxyadc_apv1,maxych_apv1;
  int maxxadc_apv2,maxxch_apv2,maxyadc_apv2,maxych_apv2;
  int gettotx_cmode_apv1,gettotx_cmode_e_apv1,gettoty_cmode_apv1,gettoty_cmode_e_apv1;
  int gettotx_cmode_apv2,gettotx_cmode_e_apv2,gettoty_cmode_apv2,gettoty_cmode_e_apv2;
 int apvx1,apvy1;

std::vector <int> dattotx(250,0);
std::vector <int> dattoty(250,0);
std::vector <std::vector <int> > readavgx(4,dattotx);
std::vector <std::vector <int> > readavgy(4,dattoty);
//////////////////////////////////////////// read the average common mode file //////////////////////////////////////////////
    std::ifstream cmfile("event_avg_cmode.dat");
    std::string cmline;
     int cmgem;
      double avgx_cmode_apv1,avgx_cmode_apv2,avgy_cmode_apv1,avgy_cmode_apv2;
    while (std::getline(cmfile, cmline)) 
      {
	std::istringstream ss(cmline);
	ss >> cmgem >> avgx_cmode_apv1 >> avgx_cmode_apv2 >> avgy_cmode_apv1 >> avgy_cmode_apv2;
	cmode_mean_x_apv1[cmgem]=avgx_cmode_apv1;
	cmode_mean_x_apv2[cmgem]=avgx_cmode_apv2;
	cmode_mean_y_apv1[cmgem]=avgy_cmode_apv1;
	cmode_mean_y_apv2[cmgem]=avgy_cmode_apv2;
	//	printf("read cmode %d %d %5.2lf %5.2lf\n ",cmtel,cmgem,cmode_mean_x[cmtel][cmgem],cmode_mean_y[cmtel][cmgem]);
      };
    cmfile.close();
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////// read the average pedestal file //////////////////////////////////////////////
    std::ifstream pedfile("bin_2_bin_pedestals.dat");
    std::string pedline;
    int pedgem,pedstrip,pedx,pedy;
    while (std::getline(pedfile, pedline)) 
      {
	std::istringstream pss(pedline);
	pss >> pedgem >> pedstrip >> pedx >> pedy;
		avg_ped_x[pedgem][pedstrip]=pedx;
		avg_ped_y[pedgem][pedstrip]=pedy;
	//  printf("read average ped  %d %d %d %d\n ",pedgem,pedstrip,avg_ped_x[pedgem][pedstrip],avg_ped_y[pedgem][pedstrip]);
      };
    pedfile.close();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // loop over all GEMs:
    for (int i=0; i<4; i++)// four GEMs per telescope
      {
	if (lumiplugin->LUMI==NULL) continue;
	int nx = lumiplugin->GEM[i]->PushXdata();
	int ny = lumiplugin->GEM[i]->PushYdata();
	if ((nx==0)&&(ny==0)) continue;
	lumiplugin->GEM[i]->GetXdata(lumiplugin->xdatraw);
	lumiplugin->GEM[i]->GetYdata(lumiplugin->ydatraw);
		lumiplugin->GEM[i]->GetXdata(lumiplugin->xdatraw);
		lumiplugin->GEM[i]->GetYdata(lumiplugin->ydatraw);
	// mask "bad" bits in left sector APVs:
	// if ((t==0))
	//   {
	//     if( i!=2)	
	//       for (int s=0; s<nx; s++)
	// 	lumiplugin->xdatraw[s] = lumiplugin->xdatraw[s] & 0xff3f;
	//     if( i==0)	
	//     for (int s=0; s<ny; s++)
	//       lumiplugin->ydatraw[s] = lumiplugin->ydatraw[s] & 0xff3f;
	//   };
	
		  apvx1=122;
		  apvy1=122;

  maxxadc_apv1 = 0;
    maxxch_apv1 = 1000;
   maxxadc_apv2 = 0;
    maxxch_apv2 = 1000;
    maxyadc_apv1 = 0;
    maxych_apv1 = 1000;
   maxyadc_apv2 = 0;
    maxych_apv2 = 1000;

    for (int k=0; k<apvx1; k++)		      
      {
	if(lumiplugin->xdatraw[k]>maxxadc_apv1) //THis gets the maximum ADC value and it's strip # for xdat
	  {
	    maxxadc_apv1=lumiplugin->xdatraw[k];
	    maxxch_apv1 =k;
	  };
      };
    for (int k=apvx1; k<250; k++)		      
      {
	if(lumiplugin->xdatraw[k]>maxxadc_apv2) //THis gets the maximum ADC value and it's strip # for ydat
	  {
	    maxxadc_apv2=lumiplugin->xdatraw[k];
	    maxxch_apv2 =k;
	  };
      };
    for (int k=0; k<apvy1; k++)		      
      {
	if(lumiplugin->ydatraw[k]>maxyadc_apv1) //THis gets the maximum ADC value and it's strip # for xdat
	  {
	    maxyadc_apv1=lumiplugin->ydatraw[k];
	    maxych_apv1 =k;
	  };
      };
    for (int k=apvy1; k<250; k++)		      
      {
	if(lumiplugin->ydatraw[k]>maxyadc_apv2) //THis gets the maximum ADC value and it's strip # for xdat
	  {
	    maxyadc_apv2=lumiplugin->ydatraw[k];
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

	    for (int k=0; k<apvx1; k++)		      
	      {
		if ((k<=maxxch_apv1-2)||(k>=maxxch_apv1+2)) //THis adds ADCs for # of "eve" events for each strip without the maximum ADC strip # +/- 2 on each event
		  {
		    gettotx_cmode_apv1 +=lumiplugin->xdatraw[k];
		    gettotx_cmode_e_apv1 = gettotx_cmode_e_apv1+1;
		  };
	      };
	    for (int k=apvx1; k<250; k++)		      
	      {
		if ((k<=maxxch_apv2-2)||(k>=maxxch_apv2+2)) //THis adds ADCs for # of "eve" events for each strip without the maximum ADC strip # +/- 2 on each event
		  {
		    gettotx_cmode_apv2 +=lumiplugin->xdatraw[k];
		    gettotx_cmode_e_apv2 = gettotx_cmode_e_apv2+1;
		  };
	      };

	    for (int k=0; k<apvy1; k++)		      
	      {
		if ((k<=maxych_apv1-2)||(k>=maxych_apv1+2)) //THis adds ADCs for # of "eve" events for each strip without the maximum ADC strip # +/- 2 on each event
		  {
		    gettoty_cmode_apv1 +=lumiplugin->ydatraw[k];
		    gettoty_cmode_e_apv1 = gettoty_cmode_e_apv1+1;
		  };
	      };
	    for (int k=apvy1; k<250; k++)		      
	      {
		if ((k<=maxych_apv2-2)||(k>=maxych_apv2+2)) //THis adds ADCs for # of "eve" events for each strip without the maximum ADC strip # +/- 2 on each event
		  {
		    gettoty_cmode_apv2 +=lumiplugin->ydatraw[k];
		    gettoty_cmode_e_apv2 = gettoty_cmode_e_apv2+1;
		  };
	      };
	    int cmode_x_apv1,cmode_x_apv2,cmode_y_apv1,cmode_y_apv2;

	    cmode_x_apv1=gettotx_cmode_apv1/gettotx_cmode_e_apv1-cmode_mean_x_apv1[i];
	    cmode_x_apv2=gettotx_cmode_apv2/gettotx_cmode_e_apv2-cmode_mean_x_apv2[i];

	    cmode_y_apv1=gettoty_cmode_apv1/gettoty_cmode_e_apv1-cmode_mean_y_apv1[i];
	    cmode_y_apv2=gettoty_cmode_apv2/gettoty_cmode_e_apv2-cmode_mean_y_apv2[i];


	     for (int k=0; k<250; k++)		    /////////  subtract common mode
	      {
		//	printf("before cmode %d %d %d %d %d %d\n ",i,k,lumiplugin->xdatraw[k],cmode_x,lumiplugin->ydatraw[k],cmode_y);
		//	lumiplugin->xdatraw[k]-=cmode_x;
		//	lumiplugin->ydatraw[k]-=cmode_y;

		if ((i==0)||(i==1)) {
		  if (k<122) {
		    lumiplugin->xdatraw[k]-=cmode_x_apv1;
		    lumiplugin->ydatraw[k]-=cmode_y_apv1;
		  };
		  if (k>121) {
		    lumiplugin->xdatraw[k]-=cmode_x_apv2;
		    lumiplugin->ydatraw[k]-=cmode_y_apv2;
		  };
		};
		if ((i==2)||(i==3)) {
		  if (k<122) lumiplugin->xdatraw[k]-=cmode_x_apv1;
		  if (k>121) lumiplugin->xdatraw[k]-=cmode_x_apv2;
		  if (k<128) lumiplugin->ydatraw[k]-=cmode_y_apv1;
		  if (k>127) lumiplugin->ydatraw[k]-=cmode_y_apv2;
		};
	      };
	    //    	lumiplugin->hitfinder->RemoveLinearBaseline(lumiplugin->GEM[t][i]->GetNXsamplesList(), lumiplugin->xdatraw);
	    //   	lumiplugin->hitfinder->RemoveLinearBaseline(lumiplugin->GEM[t][i]->GetNYsamplesList(), lumiplugin->ydatraw);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////subtract the bin-2-bin pedestals //////////////////////////////////////////////////////////////////////////
    for (int k=0; k<250; k++) 		      
      {
	//	printf("before read average ped  %d %d %d %d\n ",i,k,avg_ped_x[i][k],avg_ped_y[i][k]);
	//printf("before ped %d %d %d %d %d %d\n ",i,k,lumiplugin->xdatraw[k],avg_ped_x[i][k],lumiplugin->ydatraw[k],avg_ped_y[i][k]);
	lumiplugin->xdatraw[k]-=avg_ped_x[i][k]; //Subtract the bin-to-bin pedestals (previously determined for the total events = tote) from each bin
	 lumiplugin->ydatraw[k]-=avg_ped_y[i][k];
	  //	printf("after ped %d %d %d %d\n ",i,k,lumiplugin->xdatraw[k],lumiplugin->ydatraw[k]);
      };
	
    //	lumiplugin->hitfinder->RemoveLinearBaseline(lumiplugin->GEM[t][i]->GetNXsamplesList(), lumiplugin->xdatraw);
    //	lumiplugin->hitfinder->RemoveLinearBaseline(lumiplugin->GEM[t][i]->GetNYsamplesList(), lumiplugin->ydatraw);
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if (rawhistoX[i]==NULL)
	  rawhistoX[i] = new TH1I(Form("LumiGEM/%sGEMX",  GEMname[i]),
				     Form("%s sector %s GEM X;strip #;ADC value", GEMname[i]),
				     250, -0.5, 249.5);
	if (rawhistoY[i]==NULL)
	  rawhistoY[i] = new TH1I(Form("LumiGEM/%sGEMY", GEMname[i]),
				     Form("%s GEM Y;strip #;ADC value",  GEMname[i]),
				     250, -0.5, 249.5);
	// fill raw data histos:
	for (int j=0; j<nx; j++)
	  rawhistoX[i]->SetBinContent(j+1, lumiplugin->xdatraw[j]);
	for (int j=0; j<ny; j++)
	  rawhistoY[i]->SetBinContent(j+1, lumiplugin->ydatraw[j]);
	// plot raw data:
	if (verbose>1)
	  printf(" ====>> Telescope - GEM %d <<====\n", i);
	if (CBGraphicsOutput->GetState()==kTRUE)
	  {
	    canvas->cd( (2-i)*4 + 2+1 );
	    rawhistoX[i]->Draw();
	    rawhistoX[i]->GetYaxis()->SetRangeUser(-250., 2000.);
	    canvas->cd( (2-i)*4 + 2+2 );
	    rawhistoY[i]->Draw();
	    rawhistoY[i]->GetYaxis()->SetRangeUser(-250., 2000.);
	  };
	if (CBStopOnGoodEvent->GetState()==kTRUE)
	  gROOT->SetInterrupt();
      };
  
  // create markers:
  if (CBGraphicsOutput->GetState()==kButtonDown)
    {
      for (unsigned int j=0; j<lumiplugin->gemo->hits.size(); j++)
	{
	  int GEMid = lumiplugin->gemo->hits[j].GEMid;
	  double x  = lumiplugin->gemo->hits[j].xl;
	  double y  = lumiplugin->gemo->hits[j].yl;
	  TMarker *m;
	  int i=GEMid%4;
	  int twoDclusters=0;
	  if ((x>=0.)&&(y>=0.))
	    twoDclusters=1;
	  if (y>=0.)
	    {
	      canvas->cd( (i)*2 + 2+2 ); // y hit
	      m = new TMarker(y, 1000., 23);
	      m->Draw();
	      if (twoDclusters)
		m->SetMarkerColor(kGreen); // matched 2d clusters
	      else
		m->SetMarkerColor(kRed);   // ordinary 1d clusters
	    };
	  if (x>=0.)
	    {
	      canvas->cd( (i)*2 +2+1 ); // x hit
	      m = new TMarker(x, 1000., 23);
	      m->Draw();
	      if (twoDclusters)
		m->SetMarkerColor(kGreen); // matched 2d clusters
	      else
		m->SetMarkerColor(kRed);   // ordinary 1d clusters
	    };

	  if (GEMid==GEMidOI)
	    if (lumiplugin->gemo->hits[j].ampl>clusterminampl)
	      GEMOIclusters++;
	};
      canvas->Update();
    };
  // stop on events of interest:
  //if (GEMOIclusters>=2)
  //  gROOT->SetInterrupt();
  return Plugin::ok;
}

Long_t Tab_LumiGEM::SimpleTracking()
{
  verbose=(int)NEVerbosityLevel->GetNumber();
  // limits on amplitude and width of clusters:
  double minampl = 50.;
  double minsigma = 0.4, maxsigma = 2.5;
  std::vector <LumiGEMhit> xyhits[8];
  std::vector <LumiGEMhit> allhits = lumiplugin->gemo->hits;
  int oldGEMid=-1;
  for (unsigned int j=0; j<lumiplugin->gemo->hits.size(); j++)
    {
      int GEMid   = allhits[j].GEMid;
      if (GEMid!=oldGEMid)
	{
	  if (verbose>1) printf(" ******** %s GEM ******** \n", GEMname[GEMid%4]);
	  oldGEMid = GEMid;
	};
      // don't accept too small clusters:
      double ampl = allhits[j].charge;//ampl;
      if (ampl<minampl) continue;
      if (verbose>1) printf(" hit @ %.0lf;%.0lf : \n", allhits[j].xl, allhits[j].yl);
      // dont't accept too narrow or too wide clusters:
      double sigma = allhits[j].sigma;
      if ((sigma<minsigma)||(sigma>maxsigma)) {if (verbose>1) printf("wrong sigma=%.1lf\n", sigma); continue; };
      // we need to treat amplitudes in x and y differently as we'll
      // do charge sharing later on:
      double amplx=0., amply=0.;
      bool   isx  = (allhits[j].yl<0.);
      if (isx)
	amplx = allhits[j].charge;//ampl;
      else
	amply = allhits[j].charge;//ampl;
      // Loop over the rest of hits in the list to figure out which
      // combinations might be useful for simple tracking:
      if (verbose>1)
	{
	  if (isx)
	    printf("I have a hit in x with ampl = %.0lf\n", amplx);
	  else
	    printf("I have a hit in y with ampl = %.0lf\n", amply);
	  printf(" %d hits left in list\n", (int)lumiplugin->gemo->hits.size()-j);
	};
      for (unsigned int k=j+1; k<lumiplugin->gemo->hits.size(); k++)
	{
	  // don't accept too small clusters:
	  //if (allhits[k].ampl<minampl) continue;
	  if (allhits[k].charge<minampl) continue;
	  if (verbose>1) printf("     partner candidate @ %.0lf;%.0lf\n", allhits[k].xl, allhits[k].yl);
	  if (allhits[k].GEMid!=GEMid) continue; // hit needs to be on the same GEM
	  if ((allhits[k].yl<0.)==isx) continue; // hit needs to be on the opposite readout axis
	  if ((allhits[k].sigma<minsigma)||(allhits[k].sigma>maxsigma)) {if (verbose>1) printf("wrong sigma=%.1lf\n", allhits[k].sigma); continue; };
	  // sort amplitudes:
	  if (isx)
	    amply = allhits[k].charge;//ampl;
	  else
	    amplx = allhits[k].charge;//ampl;
	  if (verbose>1) printf("    there is a partner candidate on the other axis with ampl = %.0lf\n", allhits[k].charge);//ampl);
	  // now check if charges are similar:
	  int METHOD_chargematching = 1;

	  double ampldiff = amplx-amply;
	  if (amplx+amply>5000) ampldiff=0;
	  switch (METHOD_chargematching)
	    {
	    case 1: //////////////////////////////////////////////////////
		  if ((ampldiff<-550.)||(ampldiff>350.)) { 
		    if (verbose>1) printf("         NO! ampldiff = %.0lf\n", ampldiff);
		    double ampl1 = allhits[k].ampl;
		    double ampl2 = allhits[j].ampl;
		    if ((ampl1/ampl2<0.8)||(ampl1/ampl2>1.2))
		      continue; };
	      break;
	    case 2: //////////////////////////////////////////////////////
	      if (amplx/amply<0.7) continue;
	      if (amply/amplx<0.7) continue;
	      break;
	    };
	  if (verbose>1) printf("        %.0lf-%.0lf=%.0lf YES!\n", amplx, amply, ampldiff);
	  // it seems we found a possible partner for our hit/cluster:
	  LumiGEMhit ahit;
	  if (isx)
	    {
	      ahit.xl = allhits[j].xl;
	      ahit.yl = allhits[k].yl;
	    }
	  else
	    {
	      ahit.xl = allhits[k].xl;
	      ahit.yl = allhits[j].yl;
	    };
	  ahit.ampl   = (allhits[k].ampl+allhits[j].ampl)/2.;
	  ahit.charge = (allhits[k].charge+allhits[j].charge)/2.;
	  xyhits[allhits[j].GEMid].push_back(ahit);
	};
    };
  if (verbose>1)
    printf("\n");

  // try to fit tracks in telescopes:
  int lverb=((int)NEVerbosityLevel->GetNumber())-2;
  double zgem[6] = { 1846.2, 2180.3, 2600.7, 1840.7, 2176.5, 2597.2 };
  double xpitch = 0.4, ypitch = 0.4;
  for (int t=1; t<2; t++)
    {
      // accept only events with a hit in all three GEMs:
      if ((xyhits[0+3*t].size()==0)|(xyhits[1+3*t].size()==0)|(xyhits[2+3*t].size()==0)) continue;
      if (lverb)
	printf(" Track fitting...\n");
      if (verbose>1)
	for (int i=0; i<8; i++)
	  {
	    printf(" %s GEM has %d hits\n", GEMname[i%3], (int)xyhits[i].size());
	    if ((int)xyhits[i].size()>1)
	      for (int j=0; j<(int)xyhits[i].size(); j++)
		printf("          %4.2lf %4.2lf\n", xyhits[i][j].xl, xyhits[i][j].yl);
	  };
      cTrack *track = new cTrack(3);
      // loop over all possible combinations:
      bool foundtrack=false;
      for (unsigned int g1=0; g1<xyhits[0+3].size(); g1++)
	{
	  for (unsigned int g2=0; g2<xyhits[1+3].size(); g2++)
	    {
	      for (unsigned int g3=0; g3<xyhits[2+3].size(); g3++)
		{
		  // for this specific set of spacepoints in the three GEMs
		  // hand the coordinates from US and DS GEM over to the track object:
		  track->SetPoint(0, xyhits[0+3][g1].xl*xpitch, xyhits[0+3][g1].yl*ypitch, zgem[0+3]);
		  track->SetPoint(1, xyhits[2+3][g3].xl*xpitch, xyhits[2+3][g3].yl*ypitch, zgem[2+3]);
		  // and do the "fit":
		  track->CalculateTrajectory(cTrack::nofield);

		  // check if cluster in MI GEM is close enough to assumed track:
		  double dx, dy;
		  track->DistanceInXYPlane(xyhits[1+3][g2].xl*xpitch,
					   xyhits[1+3][g2].yl*ypitch,
					   zgem[1+3],
					   &dx, &dy);
		  if (lverb)
		    printf("    distance: %2.2lf %2.2lf\n", dx, dy);
		  // in x we should be off by at least 1mm an maximum 4.5mm:
		    if ((dx<1.0)||(dx>40.5))
		      continue;
		  // in y we should not be off by more than 1.25mm:
		    if (dy>1.25)
		      continue;
		  printf(" ===== GOOD TRACK t=%d ===== ", t);
		  /*		  if (lumitrigger)
		    {
		      int tt=0; if (t==0) tt=1;
		      SiPMQDCscatttrk[t]->Fill((int)lumitrigger->adc[tt*2+4], (int)lumitrigger->adc[tt*2+1+4]);
		    };
		  */
		  MIGEMresX->Fill(dx);
		  MIGEMresY->Fill(dy);
		  FillHitMaps();
		  fillhistosTab = true;
		  foundtrack = true;
		  /*		  if (tof)
		    {
		      for (unsigned int th=0; th<tof->hits.size(); th++)
			if (tof->hits[th].qsum>2000.)
			  TOFtrackhisto->Fill(tof->hits[th].bar+1);
		    };
		  */
		  // histogram cluster amplitudes:
		  clusterampl[0]->Fill(xyhits[0+3][g1].ampl);
		  clusterampl[1]->Fill(xyhits[1+3][g2].ampl);
		  clusterampl[2]->Fill(xyhits[2+3][g3].ampl);
		  clustercharge[0]->Fill(xyhits[0+3][g1].charge);
		  clustercharge[1]->Fill(xyhits[1+3][g2].charge);
		  clustercharge[2]->Fill(xyhits[2+3][g3].charge);
		  // check if track could have hit US and DS trigger scintillators:
		  double dxT1, dxT2, dyT1, dyT2;
		  double zT1 = 1700.;
		  double zT2 = 2650.;
		  track->DistanceInXYPlane(0., 0., zT1, &dxT1, &dyT1);
		  track->DistanceInXYPlane(0., 0., zT2, &dxT2, &dyT2);
		  T1hitmap->Fill(dxT1, dyT1);
		  T2hitmap->Fill(dxT2, dyT2);
		  int T1acceptance=0, T2acceptance=0;
		  double margin=0.0;
		  if ( (dxT1>margin) && (dxT1<120.0-margin) && (dyT1>margin) && (dyT1<120.0-margin) )
		    T1acceptance=1;
		  if ( (dxT2>margin) && (dxT2<120.0-margin) && (dyT2>margin) && (dyT2<120.0-margin) )
		    T2acceptance=1;
		  // check TUT (Trigger Under Test):
		  int TUTtrigger=0;
		  //if (CBfreerunCTLB[6]->GetState())
		  int freepatt;
		  //  if (lumiplugin->eventinfo)
		  //   freepatt = (lumiplugin->eventinfo->trigFired&0xffff0000)>>16;
		  // else
		    freepatt = 0xffff;
		  if (freepatt&((int)NETriggerUnderTest->GetNumber()))
		    TUTtrigger=1;
		  if (TUTtrigger)
		    {
		      T1hitmapTUT->Fill(dxT1, dyT1);
		      T2hitmapTUT->Fill(dxT2, dyT2);
		      T1TUTeffmap->Clear();
		      T1TUTeffmap->Divide(T1hitmapTUT, T1hitmap, 1, 1, "B");
		      T2TUTeffmap->Clear();
		      T2TUTeffmap->Divide(T2hitmapTUT, T2hitmap, 1, 1, "B");
		    };
		  printf(" T1: %d  ---  T2: %d  ---  TUTtrigger: %d (0x%x) \n", T1acceptance, T2acceptance, TUTtrigger, freepatt);
		  if (T1acceptance&&T2acceptance)
		    {
		      //int eventid = lumiplugin->eventinfo->eventNumber;
		      //fprintf(triggerfile, "%d %d\n", eventid, TUTtrigger);
		      //fflush(triggerfile);
		      T1T2acceptcnt++;
		      if (TUTtrigger)
			TUTtriggercnt++;
		      else
			{
			  //gROOT->SetInterrupt();
			};
		      if (CBGEMtrackcandidate->GetState()==kButtonDown)
			gROOT->SetInterrupt();
		    };
		  printf(" T1T2: %d   +TUT: %d\n", T1T2acceptcnt, TUTtriggercnt);
		  if (T1T2acceptcnt)
		    {
		      PBTrigEffBar->SetPosition(double(TUTtriggercnt)/double(T1T2acceptcnt));
		      PBTrigEffBar->Paint();
		    };
		  if (foundtrack) break;
		};
	      if (foundtrack) break;
	    };
	  if (foundtrack) break;
	};
    };

  return Plugin::ok;
}

Long_t Tab_LumiGEM::showTraces()
{
  //canvas->cd();
  return Plugin::ok;
}

Long_t Tab_LumiGEM::PrepareHitMaps()
{
  // loop over all GEMs to prepare hitmaps:
  for (int t=1; t<2; t++)  // two telescopes
    for (int i=0; i<4; i++)// three GEMs per telescope
      {
	int hitmap_posnbins;  hitmap_posnbins=250;
	int hitmap_amplnbins; hitmap_amplnbins=125;
	hitmapX[i] = new TH1I(Form("LumiGEM/HitMaps/HM%sGEMX",  GEMname[i]),
				 Form("Hitmap %s GEM X",  GEMname[i]),
				 hitmap_posnbins, -0.5, 249.5);//, hitmap_amplnbins, 0., 2000.);
	hitmapY[i] = new TH1I(Form("LumiGEM/HitMaps//HM%sGEMY", GEMname[i]),
				 Form("Hitmap %s GEM Y", GEMname[i]),
				 hitmap_posnbins, -0.5, 249.5);//, hitmap_amplnbins, 0., 2000.);
	// if (hitmap2d[t][i]==NULL)
	//   hitmap2d[t][i] = new TH1I(Form("LumiGEM/HM%ssector%sGEM", leftright[t], GEMname[i]),
	// 			     Form("Hitmap %s sector %s GEM", leftright[t], GEMname[i]),
	// 			     250, -0.5, 249.5);
	clusterampl[i] = new TH1I(Form("LumiGEM/Clusters/Amlitudes%sGEM", GEMname[i]),
				     Form("Amlitudes %s GEM", GEMname[i]),
				     96, 0., 2500.);
	clustercharge[i] = new TH1I(Form("LumiGEM/Clusters/Charges%sGEM", GEMname[i]),
				       Form("Charges %s GEM", GEMname[i]),
				       96, 0., 6000.);
      };
  return Plugin::ok;
}

Long_t Tab_LumiGEM::DrawHitMaps()
{
  if (canvas==NULL)
    canvas = new TCanvas("lumicanvas", "LumiGEM", 800, 600);
  canvas->Clear();
  canvas->Divide(2, 4);
  // loop over all GEMs to draw hitmaps:
    for (int i=0; i<4; i++)// three GEMs per telescope
      {
	if (hitmapY[i])
	  {
	    canvas->cd( (i)*2 + 2+2 );
	    hitmapY[i]->Draw();
	  };
	if (hitmapX[i])
	  {
	    canvas->cd( (i)*2 + 2+1 );
	    hitmapX[i]->Draw();
	  };
      };
  canvas->Update();
  return Plugin::ok;
}

Long_t Tab_LumiGEM::FillHitMaps()
{
  //int eventid = lumiplugin->eventinfo->eventNumber;
  int triggerpattern=0;
  //  if (lumiplugin->eventinfo)
  //  triggerpattern=lumiplugin->eventinfo->trigFired;
  //  else
      triggerpattern=0xffff;
  // if (lumiplugin->triggerpatternOI<0xffff) // 0xffff is to override _everything_, even broken runs were trigFrired is always zero!
       //   if ((triggerpattern&lumiplugin->triggerpatternOI)==0)
  //   return Plugin::maySkip;
  // fill the hitmaps:
  for (unsigned int j=0; j<lumiplugin->gemo->hits.size(); j++)
    {
      int GEMid   = lumiplugin->gemo->hits[j].GEMid;
      double ampl = lumiplugin->gemo->hits[j].ampl;
      double x    = lumiplugin->gemo->hits[j].xl;
      double y    = lumiplugin->gemo->hits[j].yl;
      int i=GEMid%4;
      if (ampl<100.)  continue;
      if (ampl>1800.) continue;
      if (x<0.)
	hitmapY[i]->Fill(y);
      else
	hitmapX[i]->Fill(x);
    };
  /*
  for (int t=0; t<2; t++)  // two telescopes
    for (int i=0; i<3; i++)// three GEMs per telescope
      {
	if (hitmapY[t][i])
	  {
	    canvas->cd( (2-i)*4 + t*2+2 );
	    hitmapY[t][i]->Draw();
	  };
	if (hitmapX[t][i])
	  {
	    canvas->cd( (2-i)*4 + t*2+1 );
	    hitmapX[t][i]->Draw();
	  };	
      };
  canvas->Update();
  */
  return Plugin::ok;
}


////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

ClassImp(Tab_LumiGEM);
