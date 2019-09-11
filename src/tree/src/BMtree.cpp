#include "BMtree.h"

BMraw::BMraw()
{
	plane[0].resize(32);
	plane[1].resize(4);

}

BMraw::~BMraw()
{}

void BMraw::clear()
{
  for(int i=0; i<2; i++)
  {
	for(int j=0; j<plane[i].size(); j++)
		plane[i][j].clear();
  }
	trb_reftime.clear();
	trig.clear();
}

BMbar::BMbar()
{}

BMbar::~BMbar()
{}

void BMbar::clear()
{
	for(int i=0; i<2; i++)
	{
  		tdc_trb[i].clear();
  		adc_mqdc[i].clear();
  	}
}


int BM_logic_to_internal(int plane, int bar, int up)
{
   return bar+plane*32+up*32*2;;//20 bars, 2 sides (top and bottom)
}

void BM_internal_to_logic(int internal, int *plane, int *bar, int *up)
{
   *bar = internal%32;
   *plane = (internal/32)%2;
   *up = (internal/(32*2));
}

ClassImp(BMraw);
ClassImp(BMbar);
