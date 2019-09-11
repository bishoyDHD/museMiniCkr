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

void MACRO_LumiWiremaps(int runnumber)
{
  printf("****************************\n");
  printf(" Lumi Wire Maps for run #%d\n", runnumber);
  printf("****************************\n");
  
  TFile *fgem     = new TFile(Form("%s/gem.root",
				   GetPathToRun(runnumber, "gem")), "READ");
  TTree *gemtree  = (TTree*)(fgem->Get("lumigemcooked"));
  TFile *fmwpc    = new TFile(Form("%s/mwpc.root",
				   GetPathToRun(runnumber, "mwpc")), "READ");
  TTree *mwpctree = (TTree*)(fmwpc->Get("ODT"));

  TSuperCanvas *cgem = new TSuperCanvas("LumiGEMRawWireMaps",
					Form("LumiGEM Raw Wire Maps for run #%d", runnumber), 600, 800);
  cgem->Divide(2, 3);
  cgem->SetQuitOnClose();

  TSuperCanvas *cmwpc = new TSuperCanvas("LumiMWPCRawWireMaps",
					Form("LumiMWPC Raw Wire Maps for run #%d", runnumber), 600, 800);
  cmwpc->Divide(2, 3);
  cmwpc->SetQuitOnClose();

  for (int i=0; i<3; i++)
    {
      if (gemtree)
	{
	  cgem->cd(i*2+1); // left sector
	  gemtree->Draw(Form("LumiGEMhits.hits.yl:LumiGEMhits.hits.xl>>hgem%d", i),
			Form("LumiGEMhits.hits.GEMid==%d", i));
	  cgem->cd(i*2+2); // right sector
	  gemtree->Draw(Form("LumiGEMhits.hits.yl:LumiGEMhits.hits.xl>>hgem%d", i+3),
			Form("LumiGEMhits.hits.GEMid==%d", i+3));
	  cgem->Update();
	};
      if (mwpctree)
	{
	  cmwpc->cd(i*2+1); // left sector
	  mwpctree->Draw(Form("MWPChits.hits.yy:MWPChits.hits.xx>>hmwpc%d", i),
			Form("MWPChits.hits.cham==%d", i));
	  cmwpc->cd(i*2+2); // right sector
	  mwpctree->Draw(Form("MWPChits.hits.yy:MWPChits.hits.xx>>hmwpc%d", i+3),
			Form("MWPChits.hits.cham==%d", i+3));
	  cmwpc->Update();
	};
    };
}
