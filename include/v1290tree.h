#ifndef __V1290_H_
#define __V1290_H_
#include "muserawtree.h"

#include <map>


class v1290hit
{
 public:
  unsigned int time;
  unsigned int error;
  bool trailing;

  v1290hit();
  v1290hit(unsigned int);
  virtual ~v1290hit();
  ClassDef(v1290hit,1);   
};

class v1290: public MRTBase
{
 public:
  std::multimap<unsigned int,v1290hit> unmapped_channels;
  unsigned int extraHits;
  unsigned int extendedTriggerTimeTag;
  unsigned int status;
  unsigned int eventCounter;
  void clear();
  v1290();

  virtual ~v1290();

  ClassDef(v1290,1);

};

#endif 
