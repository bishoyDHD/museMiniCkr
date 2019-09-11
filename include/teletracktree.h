
/**
 ** This file contains the definition of MUSE Telescope Straight Tracks branch objects
 **/

#ifndef __TELETRACKTREE_H__
#define __TELETRACKTREE_H__

#include "muserawtree.h"

class StraightTrack : public MRTBase
{
 public:
  Int_t telescope;
  Double_t x0, y0, z0; // 'origin' of track (in mm)
  Double_t x1, y1, z1;
  Double_t x2, y2, z2;
  Double_t x3, y3, z3;
  Double_t x4, y4, z4;//IFP GEMs
  Double_t x5, y5, z5;
  Double_t mxifp, myifp;
  Double_t bx,by,bxifp,byifp;
  
  Double_t xchi2;//X and Y chi-sq stored separately because the tracking is independent
  Double_t ychi2;//So a problem could exist in one coordinate and not the other


  Double_t mx, my;     // x and y slopes of track

  std::vector <Double_t> xresidua, yresidua, z; // distances of MI GEM clusters from
                                                // lines through US+DS clusters

  StraightTrack();
  virtual ~StraightTrack();

  ClassDef(StraightTrack,6);
};

class TeleTracks : public MRTBase
{
 public:

  std::vector <StraightTrack> tracks; // vector (!) of straight tracks 
  TeleTracks();
  virtual ~TeleTracks();

  ClassDef(TeleTracks,1);
};

#endif
