#include "TSystem.h"
#include "TROOT.h"
#include "TGNumberEntry.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH2.h"
#include "TCut.h"

#include "src/allyoucaneat/TSuperClasses.cc"
#include "src/allyoucaneat/helper.cc"

void MACRO_LumiTargetShape(int runnumber)
{
  printf("****************************\n");
  printf(" Target Shape from Lumi Tracks for run #%d\n", runnumber);
  printf("****************************\n");
  
  TFile *f = new TFile(Form("%s/gemtracks.root",
			    GetPathToRun(runnumber, "gemtracks")), "READ");

  TSuperCanvas *c = new TSuperCanvas("TargetShape",
				     Form("Target Shape from Lumi Tracks for run #%d", runnumber), 800, 600);
  c->Divide(1, 2);
  c->SetQuitOnClose();

  TCut chi_Cut   = TCut("Tracks.tracks.chi<10.");
  TCut iter_Cut  = TCut("Tracks.tracks.iterations>10");
  TCut theta_Cut = TCut("(Tracks.tracks.momentum.Theta()*180./3.1416>8.)&&(Tracks.tracks.momentum.Theta()*180./3.1416<16.)");
  TCut y_Cut     = TCut("(Tracks.tracks.position.y()>-20.)&&(Tracks.tracks.position.y()<+20.)");
  TCut set_Cut0  = TCut("Tracks.tracks.set==0");
  TCut set_Cut1  = TCut("Tracks.tracks.set==1");
  TCut sector_Cut = TCut("LumiTraces.traces.position.x()<0.");

  c->cd(1);
  TH2 *shape_uncut=NULL;
  f->GetObject("TrackFit/yzPlane", shape_uncut);
  //shape_uncut->Draw("colz");

  c->cd(2);
  TTree *tree = (TTree*)(f->Get("lumitracks"));
  tree->Draw("Tracks.tracks.position.y():Tracks.tracks.position.z()>>h(125,-500.,+500.,50,-25.,+25)",
	     iter_Cut+y_Cut+chi_Cut+sector_Cut, "colz");
}
