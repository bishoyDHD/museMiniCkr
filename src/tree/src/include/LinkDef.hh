#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class std::vector<unsigned short>+;
#pragma link C++ class std::pair<unsigned short, std::string>+;
#pragma link C++ class std::map<unsigned short, std::string>+;
#pragma link C++ class std::vector<MRTSlowData::slowStruct>+;


#pragma link C++ class MRTBase+;
#pragma link C++ class MRTRunInfo+;

#pragma link C++ class MRTxDCHit+;
#pragma link C++ class std::vector<MRTxDCHit>+;
#pragma link C++ class MRTxDC+;

#pragma link C++ class MRTTRB3Hit+;
#pragma link C++ class std::vector<MRTTRB3Hit>+;
#pragma link C++ class MRTTRB3+;

#pragma link C++ class MRTSCAHit+;
#pragma link C++ class std::vector<MRTSCAHit>+;
#pragma link C++ class MRTSCA+;

#pragma link C++ class MRTEventInfo+;
#pragma link C++ class MRTBinaryBlob+;
#pragma link C++ class LumiGEM+;
#pragma link C++ class LumiGEMhit+;
#pragma link C++ class LumiGEMeval+;

#pragma link C++ class StraightTrack+;
#pragma link C++ class std::vector<StraightTrack>+;
#pragma link C++ class TeleTracks+;
#pragma link C++ class std::vector<double>+;

#pragma link C++ class MUSEADC+;
#pragma link C++ class MUSETDC1290+;
#pragma link C++ class MUSETDC1190+;
#pragma link C++ class MUSETRB3+;
#pragma link C++ class MUSESCALER+;

#pragma link C++ class GeneratorEvent+;
#pragma link C++ class GeneratorParticle+;

#pragma link C++ class SciFi+;
#pragma link C++ class std::multimap<unsigned int,double>+;
#pragma link C++ class std::pair<unsigned int,double>+;


#pragma link C++ class SciFiOutput+;

#pragma link C++ class std::multimap<unsigned int,trbhit>+;
#pragma link C++ class std::vector<trbhit>+;

#pragma link C++ class TrackHits+;
#pragma link C++ class TrackHit+;
#pragma link C++ class std::vector<TrackHit>+;

#pragma link C++ class ParTypes+;
#pragma link C++ class ParType+;
#pragma link C++ class std::vector<ParType>+;

#pragma link C++ class std::multimap<unsigned int,trbhit>+;
#pragma link C++ class std::pair<unsigned int,trbhit>+;
#pragma link C++ class TRB3+;
#pragma link C++ class trbhit+;


#pragma link C++ class mappedchannels+;
#pragma link C++ class std::multimap<std::string,v1190hit>+;
#pragma link C++ class std::map<std::string,short>+;



#pragma link C++ class v1190+;
#pragma link C++ class v1190hit+;
#pragma link C++ class std::multimap<unsigned int,v1190hit>+;

#pragma link C++ class v1290+;
#pragma link C++ class v1290hit+;
#pragma link C++ class std::multimap<unsigned int,v1290hit>+;

#pragma link C++ class v792+;
#pragma link C++ class std::multimap<unsigned int,unsigned short>+;

#pragma link C++ class mqdc+;
#pragma link C++ class std::multimap<unsigned int,unsigned short>+;

#pragma link C++ class std::pair<unsigned int,v1190hit>+;

#pragma link C++ class std::pair<std::string,v1190hit>+;
#pragma link C++ class std::pair<unsigned int,v1290hit>+;

#pragma link C++ class std::pair<std::string,v1290hit>+;
#pragma link C++ class std::pair<std::string,short>+;

#pragma link C++ class gemControltree+;
#pragma link C++ class BHraw+;
#pragma link C++ class BHbar+;
#pragma link C++ class BMraw+;
#pragma link C++ class BMbar+;

//Ievgen 03/07/2017:
#pragma link C++ class TRB_TDC_Board+;
#pragma link C++ class TRB_TDC_Channel+;

#pragma link C++ class std::pair<std::string,v1190hit>+;
#pragma link C++ class std::pair<std::string,v1290hit>+;
#pragma link C++ class std::pair<std::string,short>+;

#pragma link C++ class Hitclass+;
#pragma link C++ class TrackerHit+;
//#pragma link C++ class BH+;

#pragma link C++ class MRTSlowData+;
#pragma link C++ class MRTIndex2Name+;
#pragma link C++ struct MRTSlowData::slowStruct+;


#endif
