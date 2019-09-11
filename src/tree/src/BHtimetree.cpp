#include "BHtimetree.h"

BHraw::BHraw()
{
	plane[0].resize(16);
	plane[1].resize(16);
	plane[2].resize(16);
	plane[3].resize(16);
}

BHraw::~BHraw()
{}

void BHraw::clear()
{
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<plane[i].size(); j++)
			plane[i][j].clear();
	}
	reftime.clear();
	trb_reftime.clear();
	trig.clear();
}

BHbar::BHbar()
{}

BHbar::~BHbar()
{}

void BHbar::clear()
{
	for(int i=0; i<2; i++)
	{
  		tdc_v1290[i].clear();
  		tdc_trb[i].clear();
  		adc_mqdc[i].clear();
  	}
}

int BH_logic_to_internal(int plane, int bar, int side)
{
   return bar+plane*16 + side*16*4;//4 planes, 16 paddles
}

void BH_internal_to_logic(int internal, int *plane, int *bar,int *side)
{
   *bar = internal%16;
   *plane = (internal/16)%4;
   *side = (internal/(16*4));
}

ClassImp(BHraw);
ClassImp(BHbar);
