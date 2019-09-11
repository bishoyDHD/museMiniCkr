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

void MACRO_LeadGlassSpectra(int runnumber)
{
  printf("****************************\n");
  printf(" Lead Glass Spectra for run #%d\n", runnumber);
  printf("****************************\n");
  
  char filename[256];
  FILE *pipe = gSystem->OpenPipe(Form("ls %s/ROOTfiles/run_%d_*.root",
				      GetPathToRun(runnumber, "run"), runnumber), "r");
  int retval=fscanf(pipe, "%s", filename);
  if (retval<1)
    {
      printf(" Cannot find ROOTfile for run %d - bailing out!\n", runnumber);
      return;
    };
  printf(">>>%s<<<\n", filename);
  gSystem->ClosePipe(pipe);
  TFile *f=new TFile(filename, "READ");
  TTree *tree = (TTree*)(f->Get("ORT"));

  TSuperCanvas *c = new TSuperCanvas("LeadGlasses",
				     Form("Lead Glass Spectra for run #%d", runnumber), 800, 600);
  c->SetQuitOnClose();
  c->Divide(3, 2);
  int lg_hi[] = { 10, 11, 12, 13, 14, 15 };
  int nbins = 256;
  for (int i=0; i<6; i++)
    {
      c->cd(i+1);
      tree->Draw(Form("LumiADC.highRange[%d]>>hlg%d(%d,-0.5,2047.5)", lg_hi[i], i, nbins),
		 "EventInfo.trigFired&0xffff");
      tree->Draw(Form("LumiADC.highRange[%d]>>hlg_lt%d(%d,-0.5,2047.5)", lg_hi[i], i, nbins),
		 "EventInfo.trigFired&0x04", "same");
      gPad->SetLogy();
      TH1* h=(TH1*)(gDirectory->FindObject(Form("hlg_lt%d", i)));
      h->SetLineColor(kRed);
      c->Update();
    };
}
