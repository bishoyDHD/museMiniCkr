#include "v1290tree.h"

v1290::v1290()
{}

v1290::~v1290()
{}

void v1290::clear()
{
  unmapped_channels.clear();
  extraHits=0;
  extendedTriggerTimeTag=0;
  status=0;
  eventCounter=0xffffffff;
}

ClassImp(v1290);



v1290hit::v1290hit():error(0),time(0xffffffff),trailing(false){ }

v1290hit::v1290hit(unsigned int t):error(0){
  time=t &0x1fffff;
  trailing=(t &0x4000000)>0;
}

v1290hit::~v1290hit()
{};

ClassImp(v1290hit);
