#include "SciFitree.h"

SciFi::SciFi()
{}

SciFi::~SciFi()
{}

void SciFi::clear()
{
  for (int i=0;i<2;i++)
    for (int j=0;j<2;j++)
      hits[i][j].clear();
  reference_time.clear();
}  

ClassImp(SciFi);
