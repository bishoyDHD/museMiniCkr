/** 
 * This file has the implementations of the ORT 
 */

#include "../../../include/muserawtree.h"

MRTBase::MRTBase() {};
MRTBase::~MRTBase() {};
ClassImp(MRTBase);


MRTRunInfo::MRTRunInfo() {};
MRTRunInfo::~MRTRunInfo() {};
ClassImp(MRTRunInfo);

MRTEventInfo::MRTEventInfo() {};
MRTEventInfo::~MRTEventInfo() {};
ClassImp(MRTEventInfo);


MRTBinaryBlob::MRTBinaryBlob() {
data=NULL;
};
MRTBinaryBlob::~MRTBinaryBlob() {};
ClassImp(MRTBinaryBlob);


MRTxDCHit::MRTxDCHit(){};
MRTxDCHit::~MRTxDCHit(){};
ClassImp(MRTxDCHit);

MRTxDC::MRTxDC(){};
MRTxDC::~MRTxDC(){};
ClassImp(MRTxDC);

MRTTRB3Hit::MRTTRB3Hit(){};
MRTTRB3Hit::~MRTTRB3Hit(){};
ClassImp(MRTTRB3Hit);

MRTTRB3::MRTTRB3(){};
MRTTRB3::~MRTTRB3(){};
ClassImp(MRTTRB3);

MRTSCAHit::MRTSCAHit(){};
MRTSCAHit::~MRTSCAHit(){};
ClassImp(MRTSCAHit);

MRTSCA::MRTSCA(){};
MRTSCA::~MRTSCA(){};
ClassImp(MRTSCA);

MRTSlowData::MRTSlowData(){};
MRTSlowData::~MRTSlowData(){};
ClassImp(MRTSlowData);

MRTIndex2Name::MRTIndex2Name() {};
MRTIndex2Name::~MRTIndex2Name() {};
ClassImp(MRTIndex2Name);
