#include "Physics_LumiGEM.h"

#include "cTrack.h"

#include<iostream>
#include<cmath>
#include "unistd.h"

using namespace std;

const char GEMname[4][8]   = {"US", "MI", "DS", ""};
const char leftright[3][8] = {"left", "right", ""};
const char xystr[3][8]     = {"x", "y", ""};

Physics_LumiGEM::Physics_LumiGEM(TTree *in, TTree *out, TFile *inf_, TFile *outf_, TObject *p)
  : Plugin(in, out, inf_, outf_, p)
{
  cout << "LumiGEM: Physics Plugin constructor called" << endl;
}

Physics_LumiGEM::~Physics_LumiGEM()
{
}

Long_t Physics_LumiGEM::defineHistograms()
{
  cout << "LumiGEM: Physics Plugin defineHistograms called" << endl;
  T1hitmap = new TH2I("LumiGEM/T1hitmap", "T1 hitmap", 240, -60., 180., 240, -60., 180.);
  T2hitmap = new TH2I("LumiGEM/T2hitmap", "T2 hitmap", 240, -60., 180., 240, -60., 180.);
  T1T2acceptcnt  = 0;
  SiPMtriggercnt = 0;

  return 0;
}

Long_t Physics_LumiGEM::startup()
{
  cout << "LumiGEM: Physics Plugin startup called" << endl;
  //////////////////////////////////////////////////////////
  // get access to slow control data:
  /*  SCManager = (slowctrl::manager*)getMemoryObject("SlowCtrl Manager");
  if (SCManager==NULL)
    {
      printf(" * WARNING * - LumiGEM::InitSC(): SlowControl not available for this run!\n");
    }
  else
    {
      SC_BeamCurrent       = SCManager->getCurrentByName("DORIS:Current");
      SC_EDipole           = SCManager->getCurrentByName("DORIS:EDipole");
      SC_ToroidCurrentSet  = SCManager->getCurrentByName("TOR:CurrentSet");
      SC_ToroidCurrentMeas = SCManager->getCurrentByName("TOR:CurrentIn");
      SC_TargetFlowSet     = SCManager->getCurrentByName("TGT:MFC:LOWER:set");
      SC_TargetFlowMeas    = SCManager->getCurrentByName("TGT:MFC:LOWER:in");
      // get access to slowcontrol lumi info:
      SClumi = (SCLumiInfo*)getMemoryObject("SlowCtrl Lumi");
    };
  */
  //////////////////////////////////////////////////////////
  getBranchObject("LumiGEM", (TObject**)&lumigem);
  //  getBranchObject("ToF", (TObject**)&tof);
  getBranchObject("EventInfo", (TObject**)&eventinfo);
  runinfo = (MRTRunInfo*)getFileObject("RunInfo");
  runstarttime   = runinfo->startTime;
  runstoptime    = runinfo->stopTime;

  printf(" EventInfo: %p\n", eventinfo);
  printf(" RunInfo:   %p\n", runinfo);
  //////////////////////////////////////////////////////////
  lumitriggercounter[0]=0;
  lumitriggercounter[1]=0;

  lumitrackcnt[0]=0;
  lumitrackcnt[1]=0;
  protoncnt[0]=0;
  protoncnt[1]=0;

  return 0;
}

Long_t Physics_LumiGEM::process()
{
  int lumigemsize    = lumigem->hits.size();
  int triggerpattern = eventinfo->trigFired & 0x0000ffff;
  if (triggerpattern&0x02) lumitriggercounter[0]++; // left sector
  if (triggerpattern&0x04) lumitriggercounter[1]++; // right sector
  // Check trigger scintillator efficiencies:
  //if (triggerpattern&0x10) SimpleTracking();
  SimpleTracking();
  if ((triggerpattern&0x06)==0) return 0;
  if (lumigemsize==0) return 0;
  //////////////////////////////////////////////////////////
  //  int tofsize     = tof->hits.size() ;
  // count clusters on the GEMs in x/y and per GEM separately:
  int gemclustercnt[6][2];
  for (int i=0; i<6; i++)
    for (int j=0; j<2; j++)
      gemclustercnt[i][j]=0;
  for (int i=0; i<lumigemsize; i++)
    {
      int gemid = lumigem->hits[i].GEMid;
      int xy    = 0; // assume cluster in x
      if (lumigem->hits[i].xl<0.) xy = 1; // or was it in y?
      gemclustercnt[gemid][xy]++; // count it!
    };
  int goodgem[2];
  goodgem[0]=0;
  goodgem[1]=0;
  // count good GEMs per telescope:
  for (int gemid=0; gemid<6; gemid++)
    {
      if ((gemclustercnt[gemid][0]>0)&&(gemclustercnt[gemid][1]>0))
	goodgem[gemid/3]++;
    };
  //printf(" good GEMs: %d // %d\n", goodgem[0], goodgem[1]);
  //////////////////////////////////////////////////////////
  for (int sector=0; sector<2; sector++)
    {
      /*
      for (int th=0; th<tofsize; th++)			// Taken out by Alexander 2/10/12
	{
	  int barid = tof->hits[th].bar;
	  int frombar = 11 -1+18*(1-sector);
	  int tobar   = 19 -1+18*(1-sector);
	  if ((barid>frombar)&&(barid<tobar)) // bars are 0-based
	    {
	      double edep     = tof->hits[th].qsum;
	      double meantime = tof->hits[th].meantime;
	      // protons have edep>1500 and meantime >10ns
	      if ((edep>1500.)&&(meantime>10.))
		{
		  protoncnt[sector]++;
		  // The following is obsolete as we count track candidates in
		  // the SimpleTracking() method from now on instead:
		  // if (goodgem[sector])
		  //   lumitrackcnt[sector]++;
		};
	    };
	};
      */
    };

  return 0;
}

Long_t Physics_LumiGEM::done()
{
  cout << "LumiGEM: Physics Plugin done called" << endl;
  printf(" lumitriggers:  %d // %d\n", lumitriggercounter[0], lumitriggercounter[1]);
  printf(" lumitracks:    %d // %d\n", lumitrackcnt[0], lumitrackcnt[1]);
  /*  printf(" dtcBeamcharge: %e\n", SClumi->dtcBeamcharge);
      printf(" dtcLuminosity: %e\n", SClumi->dtcLuminosity);*/
  printf(" run duration:  %e\n", runstoptime-runstarttime);
  printf("   lumitrack rate at nominal luminosity (2*10^33/(cm²s)):\n");
  /*  printf("      %.2lf // %.2lf\n",
	 double(lumitrackcnt[0])*2e33/SClumi->dtcLuminosity,
	 double(lumitrackcnt[1])*2e33/SClumi->dtcLuminosity);
  */
  //
  // write results to disk:
  resultfile=NULL;
  while(resultfile==NULL)
    {
      resultfile = fopen("LumiGEMphysics.result", "a");
      usleep(10000);
    };
  fprintf(resultfile, "%d", runinfo->runNumber);
  fprintf(resultfile, " %d %d", (int)runinfo->startTime, (int)runinfo->stopTime); // 2,3

  fprintf(resultfile, " %d %d", lumitriggercounter[0], lumitriggercounter[1]); // 4,5
  fprintf(resultfile, " %d %d", lumitrackcnt[0], lumitrackcnt[1]);             // 6,7
  /*  fprintf(resultfile, " %e %e %e", SClumi->dtcBeamcharge, SClumi->dtcLuminosity, runstoptime-runstarttime); // 8,9,10
  fprintf(resultfile, " %e %e", double(lumitrackcnt[0])*2e33/SClumi->dtcLuminosity, // 11,12
	  double(lumitrackcnt[1])*2e33/SClumi->dtcLuminosity);
  fprintf(resultfile, " %d %d", protoncnt[0], protoncnt[1]); // 13,14
  fprintf(resultfile, " %e %e", double(protoncnt[0])*2e33/SClumi->dtcLuminosity, // 15,16
	  double(protoncnt[1])*2e33/SClumi->dtcLuminosity);
  fprintf(resultfile, " %d %d", T1T2acceptcnt, SiPMtriggercnt); // 17,18
  fprintf(resultfile, " %.4lf %.2lf", SC_EDipole->value, SC_ToroidCurrentMeas->value); // 19,20*/
  fprintf(resultfile, "\n");
  // that's it!
  fclose(resultfile);

  return 0;
}

Long_t Physics_LumiGEM::SimpleTracking(int verbose)
{
  //verbose=2;
  double minampl = 100.;
  double minsigma = 0.4, maxsigma = 1.65;
  std::vector <LumiGEMhit> xyhits[6];
  std::vector <LumiGEMhit> allhits = lumigem->hits;
  int oldGEMid=-1;
  for (unsigned int j=0; j<lumigem->hits.size(); j++)
    {
      int GEMid   = allhits[j].GEMid;
      if (GEMid!=oldGEMid)
	{
	  if (verbose>1) printf(" ******** %s %s GEM ******** \n", leftright[GEMid/3], GEMname[GEMid%3]);
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
	  printf(" %d hits left in list\n", (int)lumigem->hits.size()-j);
	};
      for (unsigned int k=j+1; k<lumigem->hits.size(); k++)
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
	  double ampldiff = amplx-amply;
	  if ((ampldiff<-550.)||(ampldiff>350.)) { if (verbose>1) printf("         NO! ampldiff = %.0lf\n", ampldiff);
	    double ampl1 = allhits[k].ampl;
	    double ampl2 = allhits[j].ampl;
	    if ((ampl1/ampl2<0.8)||(ampl1/ampl2>1.2))
	      continue; };
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
	  xyhits[allhits[j].GEMid].push_back(ahit);
	};
    };
  if (verbose>1)
    printf("\n");

  // try to fit tracks in telescopes:
  int lverb=0;
  double zgem[6] = { 1846.2, 2180.3, 2600.7, 1840.7, 2176.5, 2597.2 };
  double xpitch = 0.4, ypitch = 0.4;
  for (int t=0; t<2; t++)
    {
      // accept only events with a hit in all three GEMs:
      if ((xyhits[0+3*t].size()==0)|(xyhits[1+3*t].size()==0)|(xyhits[2+3*t].size()==0)) continue;
      if (lverb)
	printf(" Track fitting...\n");
      if (verbose>1)
	for (int i=0; i<6; i++)
	  printf(" %s %s GEM has %d hits\n", leftright[i/3], GEMname[i%3], (int)xyhits[i].size());
      cTrack *track = new cTrack(2);
      // loop over all possible combinations:
      for (unsigned int g1=0; g1<xyhits[0+3*t].size(); g1++)
	for (unsigned int g2=0; g2<xyhits[1+3*t].size(); g2++)
	  for (unsigned int g3=0; g3<xyhits[2+3*t].size(); g3++)
	    {
	      // for this specific set of spacepoints in the three GEMs
	      // hand the coordinates from US and DS GEM over to the track object:
	      track->SetPoint(0, xyhits[0+3*t][g1].xl*xpitch, xyhits[0+3*t][g1].yl*ypitch, zgem[0+3*t]);
	      track->SetPoint(1, xyhits[2+3*t][g3].xl*xpitch, xyhits[2+3*t][g3].yl*ypitch, zgem[2+3*t]);
	      // and do the "fit":
	      track->CalculateTrajectory(cTrack::nofield);

	      // check if cluster in MI GEM is close enough to assumed track:
	      double dx, dy;
	      track->DistanceInXYPlane(xyhits[1+3*t][g2].xl*xpitch,
	      			       xyhits[1+3*t][g2].yl*ypitch,
	      			       zgem[1+3*t],
	      			       &dx, &dy);
	      if (lverb)
		printf("    distance: %2.2lf %2.2lf\n", dx, dy);
	      // in x we should be off by at least 1mm an maximum 4.5mm:
	      if ((dx<1.0)||(dx>4.5))
		continue;
	      // in y we should not be off by more than 1.25mm:
	      if (dy>1.25)
		continue;
	      if (lverb)
		printf(" ===== GOOD TRACK ===== ");
	      lumitrackcnt[t]++;
	      // check if track could have hit US and DS trigger scintillators:
	      double dxT1, dxT2, dyT1, dyT2;
	      track->DistanceInXYPlane(0., 0., 1700., &dxT1, &dyT1);
	      track->DistanceInXYPlane(0., 0., 2650., &dxT2, &dyT2);
	      T1hitmap->Fill(dxT1, dyT1);
	      T2hitmap->Fill(dxT2, dyT2);
	      int T1acceptance=0, T2acceptance=0;
	      double margin=0.0;
	      if ( (dxT1>margin) && (dxT1<120.0-margin) && (dyT1>margin) && (dyT1<120.0-margin) )
		T1acceptance=1;
	      if ( (dxT2>margin) && (dxT2<120.0-margin) && (dyT2>margin) && (dyT2<120.0-margin) )
		T2acceptance=1;
	      int SiPMtrigger=0;
	      int triggerpattern = eventinfo->trigFired;
	      int freepatt  = (triggerpattern&0xffff0000)>>16;
	      //int prescpatt = triggerpattern&0x0000ffff;
	      //if (CBfreerunCTLB[6]->GetState())
	      if ((freepatt>>6)&1)
		SiPMtrigger=1;
	      if (lverb)
		printf(" T1: %d  ---  T2: %d  ---  SiPMtrigger: %d  \n", T1acceptance, T2acceptance, SiPMtrigger);
	      if (T1acceptance&&T2acceptance)
		{
		  //int eventid = eventinfo->eventNumber;
		  //fprintf(triggerfile, "%d %d\n", eventid, SiPMtrigger);
		  //fflush(triggerfile);
		  //printf("\nTrack found: triggerpattern = 0x%x (0x%x ->6-> 0x%x --> %d, %d)\n",
		  //	 triggerpattern&0x0000ffff, freepatt, freepatt>>6, (freepatt>>6)&1, SiPMtrigger);
		  // we can only check the efficiency using the lumi efficiency (lead glass) trigger:
		  if (triggerpattern&0x10)
		    {
		      T1T2acceptcnt++;
		      if (SiPMtrigger)
			SiPMtriggercnt++;
		      else
			{
			  //gROOT->SetInterrupt();
			};
		    };
		};
	      if (lverb)
		printf(" T1T2: %d   +SiPM: %d\n", T1T2acceptcnt, SiPMtriggercnt);
	    };
    };

  return 0;
}


/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

Long_t Physics_LumiGEM::cmdline(char *cmd)
{
  return 0;
}

extern "C"{
  Plugin *factory(TTree *in, TTree *out, TFile *inf_, TFile *outf_, TObject *p)
  {
    return (Plugin *) new Physics_LumiGEM(in, out, inf_, outf_, p);
  }
}


ClassImp(Physics_LumiGEM);
