#ifndef __TRB3TREE_H_
#define __TRB3TREE_H_
#include "muserawtree.h"

#include <map>

class trbhit
{
public:
  double time;
  bool rising;
  
  trbhit();
  virtual ~trbhit();
  ClassDef(trbhit,1);
};

class TRB3:public MRTBase
{
 public:
  std::vector<unsigned int> errors;
  std::multimap<unsigned int,trbhit> unmapped_channels;
  void clear();
  TRB3();

  virtual ~TRB3();

  ClassDef(TRB3,2);
};



#endif

