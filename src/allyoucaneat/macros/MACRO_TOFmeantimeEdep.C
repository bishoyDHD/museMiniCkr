#include "TSystem.h"
#include "TROOT.h"
#include "TGNumberEntry.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH2.h"

#include "src/allyoucaneat/TSuperClasses.cc"
#include "src/allyoucaneat/helper.cc"

void MACRO_TOFmeantimeEdep(int runnumber, int howmanyevents, int triggerpattern)
{
  printf("****************************\n");
  printf(" TOF Edep vs meantime for run #%d\n", runnumber);
  printf("****************************\n");

  printf(" processing raw data...\n");
  int retval=system(Form("bin/cooker recipes/tof.xml %s/ROOTfiles/run_%d_*.root /tmp/allyoucaneat.root -n %d", 
			 GetPathToRun(runnumber, "run"), runnumber, howmanyevents));

  // access temporary ROOT file:
  TFile *f=new TFile("/tmp/allyoucaneat.root", "READ");
  TTree *tree = (TTree*)f->Get("ToFhits");
  
  // plot histograms:
  TSuperCanvas *c1 = new TSuperCanvas("LeftTOFs", Form("Left TOFs Edep vs meantime run #%d", runnumber), 800,600);
  c1->Divide(6,3);
  c1->SetQuitOnClose();
  TSuperCanvas *c2 = new TSuperCanvas("RightTOFs", Form("Right TOFsEdep vs meantime run #%d", runnumber), 800,600);
  c2->Divide(6,3);
  c2->SetQuitOnClose();
  double from=-25., to=+50.;
  int nbins = to-from;
  for (int bar=0; bar<36; bar++)
    {
      if (bar<18)
	c1->cd(bar+1);
      else
	c2->cd(bar+1-18);

      gPad->SetBorderMode(-1);
      gPad->SetLogz();
      tree->Draw(Form("ToFhits.hits.qsum:ToFhits.hits.meantime>>EdepMeantime%d(%d,%.1lf,%.1lf,100,0.,10000.)", bar, nbins, from, to),
		 Form("(bar==%d)&&(EventInfo.trigFired&%d)", bar, triggerpattern), "colz", howmanyevents);

      if (bar<18)
	c1->Update();
      else
	c2->Update();
    };
}
