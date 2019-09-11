#include "TSystem.h"
#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TGNumberEntry.h"
#include "TCut.h"
#include "TH1.h"
#include "include/orawtree.h"
#include "include/wc2tree.h"

#include "src/allyoucaneat/TSuperClasses.cc"
#include "src/allyoucaneat/helper.cc"

#include <stdio.h>

void MACRO_WCoverview(int runnumber)
{
  printf("****************************\n");
  printf(" WC Overview run #%d\n", runnumber);
  printf("****************************\n");
  printf(" showing plots...\n");

  TFile *f = new TFile(Form("%s/wc.root", GetPathToRun(runnumber, "wc")), "READ");

  TSuperCanvas *c = new TSuperCanvas("WCoverview", Form("WC Overview for run #%d", runnumber), 800, 600);
  c->Divide(2,2);
  c->SetQuitOnClose();

  c->cd(1);
  TH2 *BendingVsPos=NULL;
  f->GetObject("WC/TrackCands/Bending/VsPos", BendingVsPos);
  BendingVsPos->Draw("colz");

  c->cd(2);
  TH2 *LinearityVsX=NULL;
  f->GetObject("WC/Cell/Linearity/VsX", LinearityVsX);
  LinearityVsX->Draw("colz");

  c->cd(3);
  TH1 *layersPerTrack=NULL;
  f->GetObject("WC/TrackCands/layersPerTrack", layersPerTrack);
  layersPerTrack->Draw();

  c->cd(4);
  TH1 *BendingX=NULL, *BendingY=NULL;
  f->GetObject("WC/TrackCands/Bending/X", BendingX);
  f->GetObject("WC/TrackCands/Bending/Y", BendingY);
  BendingX->Draw();
  BendingY->Draw("same");
  BendingY->SetLineColor(kRed);
}
