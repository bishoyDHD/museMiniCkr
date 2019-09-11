
/**
 ** This file contains the definition of MUSE v792 ADC branch objects
 **/

#ifndef __MUSEADCTREE_H__
#define __MUSEADCTREE_H__

#include "muserawtree.h"

class MUSEADC : public MRTBase
{
 public:
  Int_t    evnum;
  Double_t adcvalue[32];

  MUSEADC();
  virtual ~MUSEADC();

  ClassDef(MUSEADC,1);
};

#endif
