#ifndef __TRB_TDC_TREE_H_
#define __TRB_TDC_TREE_H_
#include "muserawtree.h"
#include "trb3tree.h"
#include <map>

//Ievgen 03/06/2018: Added class for TRB_TDC hits for debuging trb3 TDCs:

int TRB_TDC_logic_to_internal(int tdc_num, int tdc_ch);
void TRB_TDC_internal_to_logic(int internal, int *tdc_num, int *tdc_ch);


class TRB_TDC_Channel: public MRTBase
{
  public:
    std::multimap <unsigned int, trbhit> trb_tdc;

    void clear();
    TRB_TDC_Channel();
    virtual ~TRB_TDC_Channel();

    ClassDef(TRB_TDC_Channel,1);

};

class TRB_TDC_Board: public MRTBase
{
 public:
  std::vector<TRB_TDC_Channel> TDC[4]; //There are 4 TDCs on TRB3 board
  void clear();
  TRB_TDC_Board();
  virtual ~TRB_TDC_Board();

  ClassDef(TRB_TDC_Board,2);

};




#endif
