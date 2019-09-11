#ifndef __SCIFI_H_
#define __SCIFI_H_
#include "muserawtree.h"

#include "trb3tree.h"


#include <map>
#include <vector>

class SciFi:public MRTBase
{
 public:
  std::multimap<unsigned int, trbhit> hits[2][2];
  std::vector<trbhit> reference_time;
  void clear();
  SciFi();
  virtual ~SciFi();

  ClassDef(SciFi,2);
  
};


#endif
