#include "cTelescope.h"

cTelescope::cTelescope(int numberofGEMs)
  : nGEMs(numberofGEMs)
{
  GEM = new cGEM*[nGEMs];
  for (int i=0; i<nGEMs; i++)
    GEM[i] = NULL;
}

cTelescope::~cTelescope()
{
  for (int i=0; i<nGEMs; i++)
    if (GEM[i]) delete GEM[i];
  delete[] GEM;
}
