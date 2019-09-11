#ifndef __mappedchannels_H_
#define __mappedchannels_H_
#include "muserawtree.h"
#include "v1190tree.h"
#include "v1290tree.h"
#include "trb3tree.h"
#include "v792tree.h"
#include "mqdctree.h"
#include <map>



class mappedchannels: public MRTBase
{
 public:
   
  // enum channel {trig=0,trig2,rf,sau,sad,sipm1,sipm2,bc,twobytwo,sbr,sbl,nai1,nai2};

  
  std::multimap<std::string,v1190hit> tdc_v1190;
  std::multimap<std::string,v1290hit> tdc_v1290;
  std::multimap<std::string,trbhit> tdc_trb;
  std::map<std::string,short > adc_v792; // Things why I love root: short works, unsigned short does not...
  std::map<std::string,short > adc_mqdc;
  
  void clear();
  mappedchannels();
  virtual ~mappedchannels();

  ClassDef(mappedchannels,1);

};

#endif 
