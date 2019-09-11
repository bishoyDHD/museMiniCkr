#include "trb3tree.h"

TRB3::TRB3()
{}

TRB3::~TRB3()
{}

void TRB3::clear()
{
   errors.clear();
   unmapped_channels.clear();
}

ClassImp(TRB3);

trbhit::trbhit():time(0xffffffff),rising(0) {};

trbhit::~trbhit()
{};

ClassImp(trbhit);
