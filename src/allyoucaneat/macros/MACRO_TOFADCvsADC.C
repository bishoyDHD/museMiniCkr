#include "TSystem.h"
#include "TROOT.h"
#include "TGNumberEntry.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH2.h"

#include "src/allyoucaneat/TSuperClasses.cc"
#include "src/allyoucaneat/helper.cc"

void MACRO_TOFADCvsADC(int runnumber, int howmanyevents)
{
  printf("****************************\n");
  printf(" TOF ADC vs ADC for run #%d\n", runnumber);
  printf("****************************\n");
  printf(" processing raw data...\n");
  
  int retval=system(Form("bin/cooker recipes/demo.xml %s/ROOTfiles/run_%d_*.root /tmp/allyoucaneat.root -c macro:filename:\\\"root/plugin_macros/tofadc.C\\\" -n %d",
			 GetPathToRun(runnumber, "run"), runnumber, howmanyevents));

  printf(" generating plots... this can take some time, be patient!\n");

  TFile *f = new TFile("/tmp/allyoucaneat.root");
  char name[256];
  TH2I *a, *t, *b;
  TSuperCanvas *c1=new TSuperCanvas("LeftTOFs", Form("Left TOFs Run #%d", runnumber), 900, 800);
  c1->Divide(6,3);
  c1->SetQuitOnClose();
  TSuperCanvas *c2=new TSuperCanvas("RightTOFs", Form("Right TOFs Run #%d", runnumber), 900, 800);
  c2->Divide(6,3);
  c2->SetQuitOnClose();
  for (int bar = 0; bar < 36; bar++)
    {
      if (bar<18)
	c1->cd(bar+1);
      else
	c2->cd(bar+1-18);

      gPad->SetBorderMode(-1);

      sprintf(name, "not_both_%i",bar);
      a = (TH2I*) f->Get(name);
      sprintf(name, "top_%i",bar);
      t = (TH2I*) f->Get(name);
      sprintf(name, "bot_%i",bar);
      b = (TH2I*) f->Get(name);

      b->SetMarkerColor(kGreen);
      //b->SetMarkerStyle(6);
      t->SetMarkerColor(kRed);
      //t->SetMarkerStyle(6);
      a->SetMarkerStyle(4);

      if (bar<18)
	sprintf(name, "Bar L%i", bar);
      else
	sprintf(name, "Bar R%i", bar-18);
      a->SetTitle(name);
      a->Draw();
      a->GetXaxis()->SetTitle("top PMT");
      a->GetYaxis()->SetTitle("bottom PMT");
      b->Draw("same");
      t->Draw("same");

      if (bar<18)
	c1->Update();
      else
	c2->Update();
      printf(" [%d]", bar); fflush(stdout);
    };
  printf(" done!\n");
}
