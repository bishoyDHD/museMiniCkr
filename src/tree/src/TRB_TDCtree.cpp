#include "TRB_TDCtree.h"

TRB_TDC_Board::TRB_TDC_Board()
{
	TDC[0].resize(48);
	TDC[1].resize(48);
	TDC[2].resize(48);
	TDC[3].resize(48);
}

TRB_TDC_Board::~TRB_TDC_Board()
{}

void TRB_TDC_Board::clear()
{
	for(int i=0; i<4; i++)
	{
		for(int j=0; j<TDC[i].size(); j++)
			TDC[i][j].clear();
	}
//	reftime.clear();
//	trb_reftime.clear();
//	trig.clear();
}



TRB_TDC_Channel::TRB_TDC_Channel()
{}

TRB_TDC_Channel::~TRB_TDC_Channel()
{}

void TRB_TDC_Channel::clear()
{
   trb_tdc.clear();
}

int TRB_TDC_logic_to_internal(int tdc_num, int tdc_ch)
{
   return tdc_ch+tdc_num*48;//48 channels, 4 tdc
}

void TRB_TDC_internal_to_logic(int internal, int *tdc_num, int *tdc_ch)
{
   *tdc_ch = internal%48;
   *tdc_num = (internal/48)%4;
}

ClassImp(TRB_TDC_Board);
ClassImp(TRB_TDC_Channel);
