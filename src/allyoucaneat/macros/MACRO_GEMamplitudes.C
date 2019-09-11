#include "TSystem.h"
#include "TROOT.h"
#include "TGNumberEntry.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH2.h"
#include "TCut.h"

#include "include/tracktree.h"

#include "src/allyoucaneat/TSuperClasses.cc"
#include "src/allyoucaneat/helper.cc"

void MACRO_GEMamplitudes(int runnumber)
{
  printf("****************************\n");
  printf(" GEM cluster amplitudes for run #%d\n", runnumber);
  printf("****************************\n");
  
  TFile *fgem     = new TFile(Form("/data/MUSE/GEMtestMay2013/data/run%d.root", runnumber), "READ");
  TTree *gemtree  = (TTree*)(fgem->Get("lumigemcooked"));

  TSuperCanvas *c = new TSuperCanvas("GEMamplitudes",
				     Form("LumiGEM Cluster Amplitudes for run #%d", runnumber), 600, 800);
  c->Divide(2, 3);
  c->SetQuitOnClose();

  for (int i=0; i<3; i++)
    {
      if (gemtree)
	{
	  c->cd(i*2+1); // left sector
	  gemtree->Draw(Form("LumiGEMhits.hits.ampl>>hgem%d(125,0,2500.)", i),
			Form("LumiGEMhits.hits.GEMid==%d", i));
	  c->cd(i*2+2); // right sector
	  gemtree->Draw(Form("LumiGEMhits.hits.ampl>>hgem%d(125,0.,2500.)", i+3),
			Form("LumiGEMhits.hits.GEMid==%d", i+3));
	  c->Update();
	};
    };
}
