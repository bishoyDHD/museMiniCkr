
/**
 ** This file contains the definition of MUSE SCALER branch objects
 **/

#ifndef __MUSESCALERTREE_H__
#define __MUSESCALERTREE_H__

#include "muserawtree.h"

class MUSESCALER : public MRTBase
{
 public:
  Int_t    counts[32];
  Double_t time[32];
  Double_t rate[32];

  MUSESCALER();
  virtual ~MUSESCALER();

  ClassDef(MUSESCALER,1);
};

#endif
