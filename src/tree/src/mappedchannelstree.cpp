#include "mappedchannelstree.h"

mappedchannels::mappedchannels()
{}

mappedchannels::~mappedchannels()
{}

void mappedchannels::clear()
{
  tdc_v1190.clear();
  tdc_v1290.clear();
  tdc_trb.clear();
  adc_v792.clear();
  adc_mqdc.clear();
}

ClassImp(mappedchannels);
