#ifndef __V792_H_
#define __V792_H_
#include "muserawtree.h"

#include <map>



class v792: public MRTBase
{
 public:
  std::map<unsigned int,short> unmapped_channels;
  unsigned int extraHits;
  unsigned int eventCounter;
  void clear();
  v792();

  virtual ~v792();

  ClassDef(v792,1);

};

#endif 
