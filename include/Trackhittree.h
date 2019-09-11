#ifndef __TrackHitTree_H_
#define __TrackHitTree_H_
#include "muserawtree.h"

#include <vector>

class TrackHit:public MRTBase
{
	public:
		int id;
		double chisq;
		double my;//
		double mx;//
		double x0;
		double y0;
		double z0;
		double interceptzy;//in mm
		double interceptzx;//in mm

	TrackHit();
	virtual ~TrackHit();

	ClassDef(TrackHit,1);
};

class TrackHits:public MRTBase
{
 public:
  std::vector<TrackHit> tracks;  

  void clear();
  TrackHits();
  virtual ~TrackHits();

  ClassDef(TrackHits,1);
  
};



#endif