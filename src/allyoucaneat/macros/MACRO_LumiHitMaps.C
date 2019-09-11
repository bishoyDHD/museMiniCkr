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

void MACRO_LumiHitMaps(int runnumber)
{
  printf("****************************\n");
  printf(" Lumi Hit Maps for run #%d\n", runnumber);
  printf("****************************\n");
  
  TFile *f = new TFile(Form("%s/lumitracks.root",
			    GetPathToRun(runnumber, "lumitracks")), "READ");

  TSuperCanvas *c = new TSuperCanvas("LumiHitMaps",
				     Form("Lumi Hit Maps for run #%d", runnumber), 600, 800);
  c->Divide(4, 5);
  c->SetQuitOnClose();

  TCut chi_Cut   = TCut("Tracks.tracks.chi<10.");
  TCut iter_Cut  = TCut("Tracks.tracks.iterations>10");
  TCut theta_Cut = TCut("(Tracks.tracks.momentum.Theta()*180./3.1416>8.)&&(Tracks.tracks.momentum.Theta()*180./3.1416<16.)");
  TCut y_Cut     = TCut("(Tracks.tracks.position.y()>-20.)&&(Tracks.tracks.position.y()<+20.)");
  TCut set_Cut0  = TCut("Tracks.tracks.set==0");
  TCut set_Cut1  = TCut("Tracks.tracks.set==1");

  TTree *tree = (TTree*)(f->Get("lumitracks"));
  Tracks *tracks=NULL;
  tree->SetBranchAddress("Tracks", &tracks);
  Traces *traces=NULL;
  tree->SetBranchAddress("LumiTraces", &traces);

  c->cd(1);
  tree->Draw("LumiTraces.position.z():LumiTraces.position.x()");
  c->Update();

  TFile *f2 = new TFile(Form("%s/lumihitmaps.root",
			     GetPathToRun(runnumber, "lumihitmaps")), "READ");
  TH2 *h=NULL;
  char leftright[2][8]={"left", "right"};
  for (int i=0; i<16; i++)
    {
      c->cd(i+2);
      f2->GetObject(Form("LumiTrackOverview/cut/hitmaps/%s_plane%d", leftright[i/8], i%8), h);
      if (h)
	h->Draw("");
    };
}
