#include "v1190tree.h"

v1190::v1190()
{}

v1190::~v1190()
{}

void v1190::clear()
{
  unmapped_channels.clear();
  extraHits=0;
  extendedTriggerTimeTag=0;
  status=0;
  eventCounter=0xffffffff;
}

ClassImp(v1190);



v1190hit::v1190hit():error(0),time(0xffffffff),trailing(false){ }

v1190hit::v1190hit(unsigned int t):error(0){
  time=t &0x7ffff;
  trailing=(t &0x4000000)>0;
}

v1190hit::~v1190hit()
{};

ClassImp(v1190hit);
