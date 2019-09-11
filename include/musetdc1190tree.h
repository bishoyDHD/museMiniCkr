
/**
 ** This file contains the definition of MUSETDC v1190 branch objects
 **/

#ifndef __MUSETDC1190TREE_H__
#define __MUSETDC1190TREE_H__

#include "muserawtree.h"

class MUSETDC1190 : public MRTBase
{
 public:
  Int_t    evnum;
  Double_t time[128];
  Int_t    nhits[128];
  Double_t rawrftime;
  Double_t sau;
  Double_t sad;
  Double_t sadiff;
  Double_t scmt;
  Double_t bc1;
  Double_t gem1;
  Double_t gem2;
  Double_t rftime;

  MUSETDC1190();
  virtual ~MUSETDC1190();

  ClassDef(MUSETDC1190,7);
};

#endif
