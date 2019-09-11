#include "lumigemtree.h"

LumiGEMhit::LumiGEMhit()
{
}

LumiGEMhit::~LumiGEMhit()
{
}

ClassImp(LumiGEMhit);


LumiGEMeval::LumiGEMeval()
{
}

LumiGEMeval::~LumiGEMeval()
{
}

ClassImp(LumiGEMeval);


LumiGEM::LumiGEM()
{
}

LumiGEM::~LumiGEM()
{
}

int LumiGEM::GetNClusters(int gemid)
{
  int cnt=0;
  for (unsigned int i=0; i<hits.size(); i++)
    if (hits[i].GEMid==gemid)
      cnt++;
  return cnt;
}

int LumiGEM::GetNXClusters(int gemid)
{
  int cnt=0;
  for (unsigned int i=0; i<hits.size(); i++)
    if (hits[i].GEMid==gemid)
      if (hits[i].xl>=0.)
	cnt++;
  return cnt;
}

int LumiGEM::GetNYClusters(int gemid)
{
  int cnt=0;
  for (unsigned int i=0; i<hits.size(); i++)
    if (hits[i].GEMid==gemid)
      if (hits[i].yl>=0.)
	cnt++;
  return cnt;
}

Bool_t LumiGEM::IsXCluster(unsigned int index)
{
  if (index<hits.size())
    if (hits[index].xl>=0.)
      return kTRUE;
  return kFALSE;
}

Bool_t LumiGEM::IsYCluster(unsigned int index)
{
  if (index<hits.size())
    if (hits[index].yl>=0.)
      return kTRUE;
  return kFALSE;
}

ClassImp(LumiGEM);
