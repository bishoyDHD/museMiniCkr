#include <cstdlib>

const char pathlist[][32] = { "/data/fall2012/NearOnline", "/data/fall2012", "/data/tests", "/data/scratch/nearonline", "/data/cosmics2013", "" };

const char *GetPathToRun(int runnumber, const char *filetype)
{
  int pl=-1, retval=255;
  while (retval)
    {
      pl++;
      //printf(" ===> trying >%s<\n", Form("ls %s/ROOTfiles/%s_%d_*.root", pathlist[pl], filetype, runnumber));
      retval=system(Form("ls %s/ROOTfiles/%s_%d_*.root", pathlist[pl], filetype, runnumber));
      if (retval==0) return (pathlist[pl]);
      if (retval) // also try with runnumber subdir...
	{
	  //printf(" ===> trying >%s<\n", Form("ls %s/%d/%s.root", pathlist[pl], runnumber, filetype));
	  retval=system(Form("ls %s/%d/%s.root", pathlist[pl], runnumber, filetype));
	  if (retval==0) return Form("%s/%d", pathlist[pl], runnumber);
	};
      if (strlen(pathlist[pl])==0)
	{
	  printf(" *** ERROR *** - Cannot find ROOT file!\n");
	  return (const char*)("");
	};
    };
  return (const char*)(pathlist[pl]);
}
