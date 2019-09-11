#include "TSystem.h"
#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TGNumberEntry.h"
#include "TCut.h"
#include "TH1.h"
#include "TLegend.h"
#include "include/orawtree.h"

#include "src/allyoucaneat/TSuperClasses.cc"
#include "src/allyoucaneat/helper.cc"

#include <stdio.h>

void MACRO_SiPMTDCs(int runnumber, int howmanyevents)
{
  printf("****************************\n");
  printf(" SiPM TDCs for run #%d\n", runnumber);
  printf("****************************\n");
  printf(" showing plots...\n");

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

  TSuperCanvas *c = new TSuperCanvas("SiPMTDCs", Form("SiPM TDCs Run #%d", runnumber), 1000, 400);
  c->Divide(2, 1);
  c->SetQuitOnClose();
  gStyle->SetOptStat(1001111);

  TCut TDC_Cut[4];
  for (int i=0; i<4; i++)
    TDC_Cut[i] = TCut(Form("(LumiTrigger.tdc[%d]>0)&&(LumiTrigger.tdc[%d]<4095)", i, i));
  TCut Trigger_Cut = TCut("(EventInfo.trigFired&0x6)");

  TLegend *leg=NULL;

  c->cd(1);
  ORT->Draw("0.05*(LumiTrigger.tdc[1]-LumiTrigger.tdc[0])>>L_SiPMtof(100,-25.,+25.)", TDC_Cut[0]+TDC_Cut[1], "", howmanyevents);
  ((TH1*)gDirectory->FindObject("L_SiPMtof"))->GetXaxis()->SetTitle("L_SiPM t.o.f. / ns");
  ((TH1*)gDirectory->FindObject("L_SiPMtof"))->SetTitle("Time of flight in left lumi telescope");
  ORT->Draw("0.05*(LumiTrigger.tdc[1]-LumiTrigger.tdc[0])>>L_SiPMtof_t(100,-25.,+25.)", TDC_Cut[0]+TDC_Cut[1]+Trigger_Cut, "same", howmanyevents);
  ((TH1*)gDirectory->FindObject("L_SiPMtof_t"))->SetLineColor(kBlue);
  leg = new TLegend(0.0, 0.80, 0.43, 0.91);
  leg->AddEntry(((TH1*)gDirectory->FindObject("L_SiPMtof")), "left SiPMs, all triggers");
  leg->AddEntry(((TH1*)gDirectory->FindObject("L_SiPMtof_t")), "left SiPMs, lumi trigger only");
  leg->Draw();
  c->Update();
  c->cd(2);
  ORT->Draw("0.05*(LumiTrigger.tdc[3]-LumiTrigger.tdc[2])>>R_SiPMtof(100,-25.,+25.)", TDC_Cut[2]+TDC_Cut[3], "", howmanyevents);
  ((TH1*)gDirectory->FindObject("R_SiPMtof"))->GetXaxis()->SetTitle("R_SiPM t.o.f. / ns");
  ((TH1*)gDirectory->FindObject("R_SiPMtof"))->SetTitle("Time of flight in right lumi telescope");
  ORT->Draw("0.05*(LumiTrigger.tdc[3]-LumiTrigger.tdc[2])>>R_SiPMtof_t(100,-25.,+25.)", TDC_Cut[2]+TDC_Cut[3]+Trigger_Cut, "same", howmanyevents);
  ((TH1*)gDirectory->FindObject("R_SiPMtof_t"))->SetLineColor(kBlue);
  leg = new TLegend(0.0, 0.80, 0.43, 0.91);
  leg->AddEntry(((TH1*)gDirectory->FindObject("R_SiPMtof")), "right SiPMs, all triggers");
  leg->AddEntry(((TH1*)gDirectory->FindObject("R_SiPMtof_t")), "right SiPMs, lumi trigger only");
  leg->Draw();
  
  return;
}
