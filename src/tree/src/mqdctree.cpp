#include "mqdctree.h"

mqdc::mqdc()
{}

mqdc::~mqdc()
{}

void mqdc::clear()
{
  unmapped_channels.clear();
  extraHits=0;
  eventCounter=0xffffffff;
}

ClassImp(mqdc);




