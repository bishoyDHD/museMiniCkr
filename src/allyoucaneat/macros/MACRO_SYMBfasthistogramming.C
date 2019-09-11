#include "TSystem.h"
#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TGNumberEntry.h"
#include "TCut.h"
#include "TH1.h"
#include "include/orawtree.h"

#include "src/allyoucaneat/TSuperClasses.cc"
#include "src/allyoucaneat/helper.cc"

#include <stdio.h>

void MACRO_SYMBfasthistogramming(int runnumber)
{
  printf("****************************\n");
  printf(" SYMB Overview run #%d\n", runnumber);
  printf("****************************\n");
  printf(" showing plots...\n");

  TFile *f = new TFile(Form("%s/symb.root",
			    GetPathToRun(runnumber, "symb")), "READ");

  TSuperCanvas *c = new TSuperCanvas("SYMBfasthisto",
				     Form("SYMB Fast Histogramming Overview for run #%d", runnumber), 800, 400);
  c->Divide(2, 1);
  c->SetQuitOnClose();

  c->cd(1);
  TH2 *hnorm = (TH2*)(f->Get("SM/COINCIDENCE/NORMALIZE"));
  hnorm->Draw("colz");

  c->cd(2);
  TH1 *hep_left  = (TH1*)(f->Get("SM/eP/eP_LeftMaster")); printf("%p\n", hep_left);
  TH1 *hep_right = (TH1*)(f->Get("SM/eP/eP_RightMaster")); printf("%p\n", hep_right);
  double max = hep_left->GetMaximum();
  hep_left->GetXaxis()->SetRange(0., 1e6);
  hep_left->Draw();
  hep_left->GetYaxis()->SetRangeUser(0., max*15.);
  hep_right->GetXaxis()->SetRange(0., 1e6);
  hep_right->Draw("same");
  hep_right->SetLineColor(kRed);
}
