#ifndef __MQDC_H_
#define __MQDC_H_
#include "muserawtree.h"

#include <map>



class mqdc: public MRTBase
{
 public:
  std::map<unsigned int,short> unmapped_channels;
  unsigned int extraHits;
  unsigned int eventCounter;
  void clear();
  mqdc();

  virtual ~mqdc();

  ClassDef(mqdc,1);

};

#endif 
