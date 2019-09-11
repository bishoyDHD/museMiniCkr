#ifndef __SIPMTIME_H_
#define __SIPMTIME_H_
#include "muserawtree.h"
#include "v1190tree.h"
#include "v1290tree.h"
#include "trb3tree.h"
#include "v792tree.h"
#include "mqdctree.h"
#include <map>
#include <vector>

int BH_logic_to_internal(int plane, int bar, int side);

void BH_internal_to_logic(int internal, int * plane, int *bar, int * side);

class BHbar: public MRTBase
{
  public:
    std::multimap <unsigned int, v1290hit> tdc_v1290[2];
    std::multimap <unsigned int, trbhit> tdc_trb[2];
    short adc_v792[2];
    std::vector <short> adc_mqdc[2];

    void clear();
    BHbar();
    virtual ~BHbar();

    ClassDef(BHbar,2);

};


class BHraw: public MRTBase
{
 public:
  std::vector<BHbar> plane[4];
  std::vector<v1290hit> reftime;
  std::vector<trbhit> trb_reftime;
  std::vector<trbhit> trig;
  void clear();
  BHraw();
  virtual ~BHraw();

  ClassDef(BHraw,3);

};

#endif 
