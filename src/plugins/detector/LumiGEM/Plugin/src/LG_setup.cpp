#include <Det_LumiGEM.h>
#include "cHitFinder.h"

const char GEMname[GEM_NUM][8]   = {"US", "4TH", "MI","DS"};
const char leftright[3][8] = {"left", "right", ""};
const char xystr[3][8]     = {"x", "y", ""};
const char goodbad[2][8]   = {"GOOD", "BAD"};


Long_t Det_LumiGEM::AddAPVtoGEM(int lAPVnum, const char *gem, const char *xy, int skipfirst)
{
  int gemid = StringToValue(gem,    GEMname);
  int xyval = StringToValue(xy,     xystr);
  if (verbose>1)
    {
      printf("Det_LumiGEM::AddAPVtoGEM(): Trying to create GEM from\n\tgemid=%d\n\txy=%d\n",
	     gemid, xyval);
      printf("\tlAPVnum=%d\n\tskipfirst=%d\n", lAPVnum, skipfirst);
    };
  int retval=-1;
  if (xyval==cGEM::xaxis)
    retval = GEM[gemid]->AddAPVx(lAPVnum, skipfirst);
  else  if (xyval==cGEM::yaxis)
    retval = GEM[gemid]->AddAPVy(lAPVnum, skipfirst);
  if (retval>=0)
    {
      printf("  added AVP lAPVid=%2d to %s GEM %s axis (skipping %d first samples)!\n",
	     lAPVnum, gem, xy, skipfirst);
      return retval;
    };
  printf("Det_LumiGEM::AddAPVtoGEM(): Couldn't create GEM from\n\tgemid=%d\n\txy=%d\n",
	 gemid, xyval);
  return -2;
}
Long_t Det_LumiGEM::AddAPVtoGEMright(int id, int lAPVnum, const char *gem, const char *xy, int skipfirst)
{
  return AddAPVtoGEM(lAPVnum, gem, xy, skipfirst);
}
Long_t Det_LumiGEM::AddAPVtoGEMleft(int id, int lAPVnum, const char *gem, const char *xy, int skipfirst)
{
  return AddAPVtoGEM(lAPVnum, gem, xy, skipfirst);
}
/*
Long_t Det_LumiGEM::SkipAPVChannelsright(int id, int lAPVnum, const char *channelstoskip, int size)
{
  int retval=0, t=1;
   for (int i=0; i<3; i++)// three GEMs per telescope
    retval += GEM[t][i]->SkipAPVChannels(lAPVnum, channelstoskip, size);
 printf(" Skipped %d channels on lAPVid=%d, %s sector\n", retval, lAPVnum, leftright[t]);

   if (retval)
      return 0;
    return -1;
}
*/
Long_t Det_LumiGEM::ModifyAPVright(int id, int lAPVnum, const char *chlist, int size, cAPV::eSampleModification m)
{
  int retval=0;

  for (int i=0; i<6; i++)// four GEMs per telescope
    retval += GEM[i]->ModifyAPVChannels(lAPVnum, chlist, size, m);
  printf(" modified %d channels on lAPVid=%d\n", retval, lAPVnum);
  if (retval)
    return 0;
  return -1;
}
Long_t Det_LumiGEM::ModifyAPVleft(int id, int lAPVnum, const char *chlist, int size, cAPV::eSampleModification m)
{
  int retval=0;

  for (int i=0; i<6; i++)// four GEMs per telescope
    retval += GEM[i]->ModifyAPVChannels(lAPVnum, chlist, size, m);
  printf(" modified %d channels on lAPVid=%d\n", retval, lAPVnum);
  if (retval)
    return 0;
  return -1;
}

Long_t Det_LumiGEM::SwapGEMaxis(int id, const char *sector, const char *gem, const char *xy)
{
  int telescope = StringToValue(sector, leftright);
  int gemid     = StringToValue(gem,    GEMname);
  int xyval     = StringToValue(xy,     xystr);
  printf(" swapping %s axis of %s sector %s GEM\n", xy, sector, gem);
  switch (xyval)
    {
    case cGEM::xaxis:
      GEM[gemid]->SwapXaxis();
      return 0;
    case cGEM::yaxis:
      GEM[gemid]->SwapYaxis();
      return 0;
    };
  return -1;
}

int Det_LumiGEM::StringToValue(const char *needle, const char haystack[][8])
{
  for (int i=0; strlen(haystack[i]); i++)
    if (!strcmp(needle, haystack[i]))
      return i;
  return -1;
}

void Det_LumiGEM::ShowBasicConfig()
{
  printf("\n***\n*** Cluster Finder configuration:\n***\n");
  printf(" valLMGaussFilterSigma = %4.1lf\n", valLMGaussFilterSigma);
  printf(" valLMMinimumAmplitude = %4d\n",    valLMMinimumAmplitude);
  printf(" valLMMinimumDistance  = %4d\n",    valLMMinimumDistance);
  printf(" valLMMaximumNLM       = %4d\n",    valLMMaximumNLM);
  printf(" valCFMinimumAmplitude = %4d\n",    valCFMinimumAmplitude);
  printf("    CFSigmaRange       = [ %.2lf ; %.2lf ]\n", valCFSigmaMin, valCFSigmaMax);
  printf(" valmake1dhits         = %d\n",     valmake1dhits);
  printf("\n***\n*** Trigger Pattern of Interest: \n***");
  printf(" triggerpattern        = 0x%02x\n", triggerpatternOI);
  printf("\n***\n*** Data Quality Checking: \n");
  printf(" valAPVsyncDataRange   = [ %+6d ; %+6d ]\n", valAPVsyncDataRangeLow, valAPVsyncDataRangeHigh);
  printf(" valAPVOOSyncSampleThr = %d samples\n", valAPVoutOfSyncSampleThreshold);
  printf("\n");
}


Long_t Det_LumiGEM::SetPerChannelPedestal(int id, int tele, int packet, int ch, int dir, int ped)
{
  perchannelped[ch*2+2*250*packet+2*250*GEM_NUM+dir]=ped;
  return ok;
}
Long_t Det_LumiGEM::Maskdeadchannels(int id, int packet, int ch, int dir, int dead)
{
  deadchannels[ch*2+2*250*packet+2*250*GEM_NUM+dir]=dead;
  //printf(" dead channel %d %d %d %d\n", tele,packet,dir,dead);
  //  printf(" dead channel %d\n",deadchannels[1761]);
  return ok;
}


////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

Long_t Det_LumiGEM::watchSCchannel(int id, const char *name, int sector, double low, double high)
{
  return ok;
}

Long_t Det_LumiGEM::watchSCHVchannel(int id, const char *name, int sector, double setpoint, double tolerance)
{
    return watchSCchannel(id, name, sector, setpoint-tolerance, setpoint+tolerance);
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

void Det_LumiGEM::InitSC()
{
}

void Det_LumiGEM::InitLumiBranches_in()
{
  LUMI=NULL;
  //LUMI[1]=NULL;
  // LUMI[2]=NULL;
  // LUMI[3]=NULL;

  getBranchObject("GEM1",(TObject**)&LUMI);
  //  getBranchObject("GEM1",(TObject**)&LUMI[1]);

  // getBranchObject("G_US",(TObject**)&LUMI[0]);
  // getBranchObject("G_4t",(TObject**)&LUMI[1]);
  // // getBranchObject("G_MI",(TObject**)&LUMI[2]);
  // // getBranchObject("G_DS",(TObject**)&LUMI[3]);

  // printf("LUMI[0] is %p\n", LUMI[0]);
  // printf("LUMI[1] is %p\n", LUMI[1]);
  // // printf("LUMI[2] is %p\n", LUMI[2]);
  // // printf("LUMI[3] is %p\n", LUMI[3]);

}

void Det_LumiGEM::InitLumiBranches_out()
{
  //make new branch on output tree
  gemo = new LumiGEM();
  makeBranch("LumiGEMhits", (TObject **)&gemo);
  //  copyBranch("LumiTrigger");
}

void Det_LumiGEM::InitRMShistos()
{

    for (int i=0; i<GEM_NUM; i++)// three GEMs per telescope
      {
	// rmshisto[t][i] = new TH1I(Form("LumiGEM/DataQuality/rmshisto_%d_%d", t, i), 
	// 			  Form("RMS histogram %s %s GEM", leftright[t], GEMname[i]),
	// 			  128, 0., 256.);
	rmshisto[i] = dH1(Form("LumiGEM/DataQuality/rmshisto_%d",  i), 
				  Form("RMS histogram %s GEM", GEMname[i]),
				  128, 0., 256.);
      };
}
