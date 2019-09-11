
/**
 ** This file contains the definition of MUSE TRB3 branch objects
 **/

#ifndef __MUSETRB3TREE_H__
#define __MUSETRB3TREE_H__

#include "muserawtree.h"
#define NCH_TRB 33
class MUSETRB3 : public MRTBase
{
 public:
  Int_t    evnum;
  Int_t    ctsaddr;
  Int_t    tdccode;
  Int_t    trignum;
  Int_t    nhits[NCH_TRB];
  Int_t    epoch[NCH_TRB];
  Int_t    finetime[NCH_TRB];
  Int_t    coarsetime[NCH_TRB];
  Double_t calibratedtime[NCH_TRB];
  Double_t calibratedtrailingtime[NCH_TRB];

  Double_t sau;
  Double_t sad;
  Double_t sadiff;
  Double_t scmt;
  Double_t bc1;
  Double_t rawrftime;
  Double_t rftime;

  MUSETRB3();
  virtual ~MUSETRB3();

  ClassDef(MUSETRB3,9);
};

#endif
