#include "Hittree.h"

Hitclass::Hitclass()
{}

Hitclass::~Hitclass()
{}

void Hitclass::clear()
{
	for(int i=0; i<2; i++)
	{
  		//tdc_v1290[i].clear();
  		//tdc_trb[i].clear();
  		//adc_mqdc[i].clear();
  	}
}

ClassImp(Hitclass);
