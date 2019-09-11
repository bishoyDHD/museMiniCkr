#include "TGNumberEntry.h" // One dirty way to make "Form()" available...
#include "src/allyoucaneat/helper.cc"

void MACRO_GEMrawdata(int runnumber)
{
  printf("****************************\n");
  printf(" GEM Raw Data for run #%d\n", runnumber);
  printf("****************************\n");
  printf(" starting visco...\n");
  int retval=system(Form("bin/visco recipes/GEMlumi_XY_tofs.xml %s/ROOTfiles/run_%d_*.root &",
			 GetPathToRun(runnumber, "run"), runnumber));
}
