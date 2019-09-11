
/**
 ** This file contains the definition of MUSETDC v1290 branch objects
 **/

#ifndef __MUSETDC1290TREE_H__
#define __MUSETDC1290TREE_H__

#include "muserawtree.h"

class MUSETDC1290 : public MRTBase
{
 public:
  Int_t    evnum;
  Double_t time[16];
  Int_t    nhits[16];
  Double_t rawrftime;
  Double_t sau;
  Double_t sad;
  Double_t sadiff;
  Double_t scmt;
  Double_t bc1;
  Double_t rftime;

  MUSETDC1290();
  virtual ~MUSETDC1290();

  ClassDef(MUSETDC1290,5);

};

#endif
