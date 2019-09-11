#include "TrackerTree.h"

TrackerHit::TrackerHit()
{}

TrackerHit::~TrackerHit()
{}

void TrackerHit::clear()
{
	for(int i=0; i<2; i++)
	{
  		//tdc_v1290[i].clear();
  		//tdc_trb[i].clear();
  		//adc_mqdc[i].clear();
  	}
}

ClassImp(TrackerHit);
