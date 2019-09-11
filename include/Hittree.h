#ifndef __HITTREE_H_
#define __HITTREE_H_
#include "muserawtree.h"
#include "v1190tree.h"
#include "v1290tree.h"
#include "trb3tree.h"
#include "v792tree.h"
#include "mqdctree.h"
#include <map>

class Hitclass: public MRTBase
{
  public:
    //std::vector <v1290hit> tdc_v1290[2];
    //std::vector <trbhit> tdc_trb[2];
    //short adc_v792[2];
    //std::vector <short> adc_mqdc[2];
    //enum clustertype = {};

    void clear();
    Hitclass();
    virtual ~Hitclass();

    ClassDef(Hitclass,1);

};

#endif 
