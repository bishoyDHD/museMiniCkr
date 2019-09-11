#include "TSystem.h"
#include "TROOT.h"
#include "TGNumberEntry.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TH2.h"

#include "src/allyoucaneat/TSuperClasses.cc"
#include "src/allyoucaneat/helper.cc"

void MACRO_TOFSiPMefficiency(int runnumber, int firstbar)
{
  printf("****************************\n");
  printf(" TOF SiPM Efficiencies for run #%d\n", runnumber);
  printf("****************************\n");
  printf(" processing raw data...\n");

  //int retval=system(Form("bin/visco recipes/tofsipm.xml %s/ROOTfiles/run_%d_*.root -c ToF:setSiPMToF:%d",
  //			 GetPathToRun(runnumber, "run"), runnumber, firstbar));
  int retval=system(Form("bin/visco recipes/tofsipm.xml %s/ROOTfiles/run_%d_*.root",
			 GetPathToRun(runnumber, "run"), runnumber));
  return;
}
