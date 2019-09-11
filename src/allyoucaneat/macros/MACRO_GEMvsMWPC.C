#include "TSystem.h"
#include "TROOT.h"
#include "TGNumberEntry.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH2.h"

#include "src/allyoucaneat/TSuperClasses.cc"
#include "src/allyoucaneat/helper.cc"

void MACRO_GEMvsMWPC(int runnumber)
{
  printf("****************************\n");
  printf(" GEM vs MWPC vetex distributions for run #%d\n", runnumber);
  printf("****************************\n");

  // access near online ROOT file:
  TFile *fmwpc = new TFile(Form("%s/mwpctracks.root",
				GetPathToRun(runnumber, "mwpctracks")), "READ");
  if (!fmwpc)
    {
      printf(" * ERROR * - Cannot open ROOT file with MWPC tracks...\n");
      return;
    };
  TFile *fgem  = new TFile(Form("%s/gemtracks.root",
				GetPathToRun(runnumber, "gemtracks")), "READ");
  if (!fgem)
    {
      printf(" * ERROR * - Cannot open ROOT file with GEM tracks...\n");
      return;
    };
  TTree *gemtree = (TTree*)fmwpc->Get("lumitracks");
  gemtree->SetName("gemtracks");
  TTree *mwpctree = (TTree*)fgem->Get("lumitracks");
  mwpctree->SetName("mwpctracks");
  mwpctree->AddFriend(gemtree);
  
  TString stdcut = "(gemtracks.Tracks.tracks.chi<25.)&&(mwpctracks.Tracks.tracks.chi<25.)";

  TSuperCanvas *canvas = new TSuperCanvas("c", "c", 1500, 500);
  canvas->Divide(3, 1);
  canvas->SetQuitOnClose();
  canvas->cd(1);
  mwpctree->Draw("gemtracks.Tracks.tracks.momentum.Energy():mwpctracks.Tracks.tracks.momentum.Energy()>>hE(40,0,4000,40,0,4000)", stdcut, "colz");
  TH2 *hE = (TH2*)gDirectory->FindObject("hE");
  hE->GetXaxis()->SetTitle("E_MWPC");
  hE->GetYaxis()->SetTitle("E_GEM");
  hE->SetTitle("E_{GEM} vs E_{MWPC}");
  canvas->Update();
  canvas->cd(2);
  mwpctree->Draw("gemtracks.Tracks.tracks.position.z():mwpctracks.Tracks.tracks.position.z()>>hz(100,-1000,1000,100,-1000,1000)", stdcut, "colz");
  TH2 *hz = (TH2*)gDirectory->FindObject("hz");
  hz->GetXaxis()->SetTitle("z_MWPC");
  hz->GetYaxis()->SetTitle("z_GEM");
  hz->SetTitle("z_{GEM} vs z_{MWPC}");
  canvas->Update();
  canvas->cd(3);
  mwpctree->Draw("gemtracks.Tracks.tracks.position.y():mwpctracks.Tracks.tracks.position.y()>>hy(100,-50,50,100,-50,50;x;y)", stdcut, "colz");
  TH2 *hy = (TH2*)gDirectory->FindObject("hy");
  hy->GetXaxis()->SetTitle("y_MWPC");
  hy->GetYaxis()->SetTitle("y_GEM");
  hy->SetTitle("y_{GEM} vs y_{MWPC}");
}
