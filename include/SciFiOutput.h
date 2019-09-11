#ifndef __scifioutput_h_
#define __scifioutput_h_
#include "muserawtree.h"
class SciFiOutput:public MRTBase
{
public:
    double SciFi_Time[2][16];               // average time of a fiber           [2 planes][16 fibers]
    double SciFi_ModTime[2][16];            // wrapped in 19.7 ns time window    [2 planes][16 fibers]
    double UV_Coincidence_Time[16][16];     // average coincidence time          [16 fibers][16 fibers]
    double UV_ModCoin_Time[16][16];         // average coincidence time          [16 fibers][16 fibers]

    size_t  NSciFiHits;                     // number of scifi real hits
    double  SciFiActualHits[10][3];         // up to 10 hits per event,          [3 components] : x,y,z
    
    size_t  Ntracks;                        // number of GEM tracks
    double  SciFiPredictedHits[10][3];      // up to 10 hits (from 10 tracks),   [3 components] : x,y,z
    
    double  SciFiHitsDifference[10][10][2]; // [NSciFiHits]x[Ntracks] Actual-Predicted hits differences: x,y

    SciFiOutput();
    virtual ~SciFiOutput();
    
    ClassDef(SciFiOutput,1);
};

#endif 
