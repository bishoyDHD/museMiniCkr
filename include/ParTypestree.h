#ifndef __PARTYPESTREE_H_
#define __PARTYPESTREE_H_
#include "muserawtree.h"

#include <vector>

class ParType:public MRTBase
{
	public:
		int id;//11 electrons, 13 muon, 211 pion. add minus sign for anti particles (+ charge)

	ParType();
	virtual ~ParType();

	ClassDef(ParType,1);
};

class ParTypes:public MRTBase
{
 public:
  std::vector<ParType> particles;  

  void clear();
  ParTypes();
  virtual ~ParTypes();

  ClassDef(ParTypes,1);
  
};



#endif
