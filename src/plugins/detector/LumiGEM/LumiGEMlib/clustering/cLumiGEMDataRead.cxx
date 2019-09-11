#include "cLumiGEMDataRead.h"

cLumiGEMDataRead::cLumiGEMDataRead()
{
}

cLumiGEMDataRead::~cLumiGEMDataRead()
{
}

int cLumiGEMDataRead::GetNAPVs()
{
  return GEMrawdata.numapvs;
}

int* cLumiGEMDataRead::GetBankBuffer(int lAPVnum)
{
  return GEMrawdata.apvdata[lAPVnum];
}
