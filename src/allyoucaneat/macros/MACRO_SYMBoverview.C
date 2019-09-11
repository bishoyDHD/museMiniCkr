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

void MACRO_SYMBoverview(int runnumber)
{
  printf("****************************\n");
  printf(" SYMB Overview run #%d\n", runnumber);
  printf("****************************\n");
  printf(" showing plots...\n");

  char filename[256];
  FILE *pipe = gSystem->OpenPipe(Form("ls %s/ROOTfiles/run_%d_*.root", GetPathToRun(runnumber, "run"), runnumber), "r");
  int retval=fscanf(pipe, "%s", filename);
  if (retval<1)
    {
      printf(" Cannot find ROOTfile for run %d - bailing out!\n", runnumber);
      return;
    };
  printf(">>>%s<<<\n", filename);
  gSystem->ClosePipe(pipe);
  TFile *f=new TFile(filename, "READ");
  if (f==NULL)
    {
      printf(" Cannot open ROOT file for run %d!\n", runnumber);
      return;
    };
  TTree *ORT = (TTree*)f->Get("ORT");
  if (ORT==NULL)
    {
      printf(" Cannot find ORT in ROOTfile!\n");
      return;
    };

  TSuperCanvas *c1 = new TSuperCanvas("SYMBoverviewleft", Form("SYMB Overview Left Run #%d", runnumber), 600, 600);
  c1->Divide(3, 3);
  c1->SetQuitOnClose();
  TSuperCanvas *c2 = new TSuperCanvas("SYMBoverviewright", Form("SYB Overview Right Run #%d", runnumber), 600, 600);
  c2->Divide(3, 3);
  c2->SetQuitOnClose();

  for (int i=0; i<9; i++)
    {
      c1->cd(i+1);
      gPad->SetLogy();
      ORT->Draw(Form("SYMB_ADC.values[%d]>>QDCleft%d(256,-0.5,4095.5)", i, i));
      c2->cd(i+1);
      gPad->SetLogy();
      ORT->Draw(Form("SYMB_ADC.values[%d]>>QDCright%d(256,-0.5,4095.5)", i+9, i));
      c1->Update();
      c2->Update();
    };

  return;
}
