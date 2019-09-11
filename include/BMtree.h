#ifndef __BMTIME_H_
#define __BMTIME_H_
#include "muserawtree.h"
#include "v1190tree.h"
#include "v1290tree.h"
#include "trb3tree.h"
#include "v792tree.h"
#include "mqdctree.h"
#include <map>
#include <vector>

int BM_logic_to_internal(int plane, int bar, int side);

void BM_internal_to_logic(int internal, int *plane, int *bar, int *side);


class BMbar: public MRTBase
{
  public:
    std::multimap <unsigned int, trbhit> tdc_trb[2];
    std::vector <short> adc_mqdc[2];

    void clear();
    BMbar();
    virtual ~BMbar();

    ClassDef(BMbar,1);

};


class BMraw: public MRTBase
{
 public:
  std::vector<BMbar> plane[2];
  std::vector<trbhit> trb_reftime;
  std::vector<trbhit> trig;
  void clear();
  BMraw();
  virtual ~BMraw();

  ClassDef(BMraw,1);

};

#endif 
