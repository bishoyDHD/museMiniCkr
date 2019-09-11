/*
 * This is the definition of the olympus raw data structures
 * MRT = Muse Root Tree
 */

#ifndef __MUSERAWTREE_H__
#define __MUSERAWTREE_H__


#include <TObject.h>
#include <TTimeStamp.h>
#include <vector>
#include <map>

/**
 * Base class for all components. We may use this for function prototypes.
 */

class MRTBase: public TObject
{
 public:
  MRTBase();
  virtual ~MRTBase();
  ClassDef(MRTBase,2);
};

/**
 * A class to store information of the current run
 */

class MRTRunInfo:public MRTBase
{
	public:
		UInt_t runNumber;
		TTimeStamp startTime;
		TTimeStamp stopTime;
		UInt_t nrOfEvents;
		// currently unused values
		MRTRunInfo();
		virtual ~MRTRunInfo();
		ClassDef(MRTRunInfo,2);
};

// Per event info 

class MRTEventInfo:public MRTBase
{
	public:
		UInt_t eventNumber;
		UInt_t eventType;
		UInt_t daqmismatch;
		TTimeStamp eventTime;
		UInt_t trigFired;
		UInt_t liveTimeCounter;
		UInt_t freeClock;
		UInt_t deadTimeCounter;
		double weight;
		MRTEventInfo();
		virtual ~MRTEventInfo();
		ClassDef(MRTEventInfo,4);
};


/**
 * Classes for multihit tdcs
 */

class MRTxDCHit:public MRTBase
{
 public:
  UInt_t channel;
  UInt_t time;
  MRTxDCHit();
  virtual ~MRTxDCHit();
  ClassDef(MRTxDCHit,1);
};

class MRTxDC:public MRTBase
{
 public:
  UInt_t events;
  std::vector<MRTxDCHit> hits;
  MRTxDC();
  virtual ~MRTxDC();
  ClassDef(MRTxDC,1);
};

/**
 * Classes for the TRB3
 */

class MRTTRB3Hit:public MRTBase
{
 public:
  UInt_t channel;
  UInt_t epoch;
  UInt_t finetime;
  UInt_t coarsetime;
  MRTTRB3Hit();
  virtual ~MRTTRB3Hit();
  ClassDef(MRTTRB3Hit,1);
};

class MRTTRB3:public MRTBase
{
 public:
  UInt_t events;
  UInt_t ctsaddr;
  UInt_t tdccode;
  UInt_t trigger;
  std::vector<MRTTRB3Hit> hits;
  MRTTRB3();
  virtual ~MRTTRB3();
  ClassDef(MRTTRB3,2);
};

class MRTSCAHit:public MRTBase
{
  public:
    UInt_t channel;
    UInt_t counts;
    UInt_t timestamp;
    MRTSCAHit();
    virtual ~MRTSCAHit();
    ClassDef(MRTSCAHit,1);
};

class MRTSCA:public MRTBase
{
  public:
    std::vector<MRTSCAHit> hits;
    MRTSCA();
    virtual ~MRTSCA();
    ClassDef(MRTSCA,1);
};
/**
 * A Class saving a binary blob, with a pointer and a size
 */

class MRTBinaryBlob:public MRTBase
{
 public:
  Int_t size; 
  UChar_t *data; //[size]
  MRTBinaryBlob();
  virtual ~MRTBinaryBlob();
  ClassDef(MRTBinaryBlob,1);
};

/**
 * A class for the slow control in band data
 */
class MRTSlowData:public MRTBase
{
public:
  struct slowStruct{
    unsigned short index;
    unsigned long long time;
    double value;
    short status;
  };
  std::vector<MRTSlowData::slowStruct> slowData;
  MRTSlowData();
  virtual ~MRTSlowData();
  ClassDef(MRTSlowData,1);
};

/**
 * A wrapper class for the slowcontrol index to name mapping
 */
 
class MRTIndex2Name: public MRTBase
{
public:
  std::map<unsigned short, std::string> index2name;
  MRTIndex2Name();
  virtual ~MRTIndex2Name();
  ClassDef(MRTIndex2Name,1);
}; 





#endif

