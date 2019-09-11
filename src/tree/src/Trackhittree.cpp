
#include "Trackhittree.h"

TrackHit::TrackHit()
{}

TrackHit::~TrackHit()
{} 

ClassImp(TrackHit);

TrackHits::TrackHits()
{}

TrackHits::~TrackHits()
{}

void TrackHits::clear()
{
  tracks.clear();

}  

ClassImp(TrackHits);