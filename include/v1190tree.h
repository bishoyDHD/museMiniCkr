#ifndef __V1190_H_
#define __V1190_H_
#include "muserawtree.h"

#include <map>


class v1190hit
{
 public:
  unsigned int time;
  unsigned int error;
  bool trailing;

  v1190hit();
  v1190hit(unsigned int);
  virtual ~v1190hit();
  ClassDef(v1190hit,1);   
};

class v1190: public MRTBase
{
 public:
  std::multimap<unsigned int,v1190hit> unmapped_channels;
  unsigned int extraHits;
  unsigned int extendedTriggerTimeTag;
  unsigned int status;
  unsigned int eventCounter;
  void clear();
  v1190();

  virtual ~v1190();

  ClassDef(v1190,1);

};

#endif 
